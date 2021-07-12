// SPDX-License-Identifier: GPL-2.0
/*
 * Driver for STM32 Digital Camera Memory Interface Pixel Processor
 *
 * Copyright (C) STMicroelectronics SA 2021
 * Authors: Hugues Fruchet <hugues.fruchet@foss.st.com>
 *          Alain Volmat <alain.volmat@foss.st.com>
 *          for STMicroelectronics.
 */

#include <linux/component.h>
#include <linux/delay.h>
#include <linux/module.h>
#include <linux/mod_devicetable.h>
#include <linux/platform_device.h>
#include <linux/vmalloc.h>
#include <linux/v4l2-mediabus.h>
#include <media/v4l2-rect.h>
#include <media/v4l2-subdev.h>

#include "dcmipp-common.h"

#define DCMIPP_POSTPROC_DRV_NAME "dcmipp-byteproc"

#define DCMIPP_FMT_WIDTH_DEFAULT  640
#define DCMIPP_FMT_HEIGHT_DEFAULT 480

#define IS_SINK(sd, pad) ((sd)->entity.pads[(pad)].flags & MEDIA_PAD_FL_SINK)
#define IS_SRC(sd, pad)  ((sd)->entity.pads[(pad)].flags & MEDIA_PAD_FL_SOURCE)
#define PAD_STR(sd, pad) (IS_SRC((sd), (pad))) ? "src" : "sink"

#define to_dcmipp_common(a)	((struct dcmipp_common_device *)(&((a)->common)))

struct dcmipp_postproc_device {
	struct dcmipp_common_device common;
	struct v4l2_fract sink_interval;
	unsigned int frate;
	u32 src_code;
	struct v4l2_rect crop;
	bool do_crop;
};

static const struct v4l2_mbus_framefmt fmt_default = {
	.width = DCMIPP_FMT_WIDTH_DEFAULT,
	.height = DCMIPP_FMT_HEIGHT_DEFAULT,
	.code = MEDIA_BUS_FMT_SBGGR8_1X8,
	.field = V4L2_FIELD_NONE,
	.colorspace = V4L2_COLORSPACE_DEFAULT,
};

static const struct v4l2_rect crop_default = {
	.top = 0,
	.left = 0,
	.width = DCMIPP_FMT_WIDTH_DEFAULT,
	.height = DCMIPP_FMT_HEIGHT_DEFAULT,
};

static const struct v4l2_rect crop_min = {
	.width = DCMIPP_FRAME_MIN_WIDTH,
	.height = DCMIPP_FRAME_MIN_HEIGHT,
	.top = 0,
	.left = 0,
};

static struct v4l2_rect
dcmipp_postproc_get_crop_bound(const struct v4l2_mbus_framefmt *fmt)
{
	/* Get the crop bounds to clamp the crop rectangle correctly */
	struct v4l2_rect r = {
		.left = 0,
		.top = 0,
		.width = fmt->width,
		.height = fmt->height,
	};
	return r;
}

static void dcmipp_postproc_adjust_crop(struct v4l2_rect *r,
					const struct v4l2_mbus_framefmt *fmt)
{
	const struct v4l2_rect src_rect =
		dcmipp_postproc_get_crop_bound(fmt);

	/* Disallow rectangles smaller than the minimal one. */
	v4l2_rect_set_min_size(r, &crop_min);
	v4l2_rect_map_inside(r, &src_rect);
}

static void dcmipp_postproc_adjust_src_fmt(struct v4l2_mbus_framefmt *src_fmt,
					   struct v4l2_mbus_framefmt *sink_fmt)
{
	/* Can only divide by 2 in width or height */
	if (src_fmt->width <= sink_fmt->width / 2)
		src_fmt->width = sink_fmt->width / 2;
	else
		src_fmt->width = sink_fmt->width;

	if (src_fmt->height <= sink_fmt->height / 2)
		src_fmt->height = sink_fmt->height / 2;
	else
		src_fmt->height = sink_fmt->height;
}

static void dcmipp_postproc_adjust_fmt(struct v4l2_mbus_framefmt *fmt, u32 type,
				       unsigned int pipe_id)
{
	const struct dcmipp_pix_map *vpix;

	/* Only accept code in the pix map table */
	vpix = dcmipp_pix_map_by_code(fmt->code,
				      STAGE(pipe_id, DCMIPP_POSTPROC, type));
	if (!vpix)
		fmt->code = fmt_default.code;

	fmt->width = clamp_t(u32, fmt->width, DCMIPP_FRAME_MIN_WIDTH,
			     DCMIPP_FRAME_MAX_WIDTH) & ~1;
	fmt->height = clamp_t(u32, fmt->height, DCMIPP_FRAME_MIN_HEIGHT,
			      DCMIPP_FRAME_MAX_HEIGHT) & ~1;

	if (fmt->field == V4L2_FIELD_ANY || fmt->field == V4L2_FIELD_ALTERNATE)
		fmt->field = fmt_default.field;

	dcmipp_colorimetry_clamp(fmt);
}

static int dcmipp_postproc_set_fmt(struct v4l2_subdev *sd,
				   struct v4l2_subdev_pad_config *cfg,
				   struct v4l2_subdev_format *fmt)
{
	struct dcmipp_postproc_device *postproc = v4l2_get_subdevdata(sd);
	struct v4l2_mbus_framefmt *pad_fmt;
	int ret = 0;
	unsigned int type;

	mutex_lock(&postproc->common.lock);

	if (fmt->which == V4L2_SUBDEV_FORMAT_ACTIVE) {
		if (postproc->common.streaming) {
			ret = -EBUSY;
			goto out;
		}

		if (IS_SINK(sd, fmt->pad))
			pad_fmt = &postproc->common.sink_fmt;
		else
			pad_fmt = &postproc->common.src_fmt;

	} else {
		pad_fmt = v4l2_subdev_get_try_format(sd, cfg, 0);
	}

	if (IS_SINK(sd, fmt->pad))
		type = INPUT;
	else
		type = OUTPUT;

	dcmipp_postproc_adjust_fmt(&fmt->format, type, postproc->common.pipe_id);

	if (IS_SRC(sd, fmt->pad))
		dcmipp_postproc_adjust_src_fmt(&fmt->format,
					       &postproc->common.sink_fmt);

	dev_dbg(postproc->common.dev, "%s: %s format update: old:%dx%d (0x%x, %d, %d, %d, %d) new:%dx%d (0x%x, %d, %d, %d, %d)\n",
		postproc->common.sd.name,
		PAD_STR(sd, fmt->pad),
		/* old */
		pad_fmt->width, pad_fmt->height, pad_fmt->code,
		pad_fmt->colorspace, pad_fmt->quantization,
		pad_fmt->xfer_func, pad_fmt->ycbcr_enc,
		/* new */
		fmt->format.width, fmt->format.height, fmt->format.code,
		fmt->format.colorspace, fmt->format.quantization,
		fmt->format.xfer_func, fmt->format.ycbcr_enc);

	*pad_fmt = fmt->format;

out:
	mutex_unlock(&postproc->common.lock);

	return ret;
}

static int dcmipp_postproc_get_selection(struct v4l2_subdev *sd,
					 struct v4l2_subdev_pad_config *cfg,
					 struct v4l2_subdev_selection *s)
{
	struct dcmipp_postproc_device *postproc = v4l2_get_subdevdata(sd);
	struct v4l2_mbus_framefmt *sink_fmt;
	struct v4l2_rect *crop;

	if (IS_SINK(sd, s->pad))
		return -EINVAL;

	if (s->which == V4L2_SUBDEV_FORMAT_ACTIVE) {
		sink_fmt = &postproc->common.sink_fmt;
		crop = &postproc->crop;
	} else {
		sink_fmt = v4l2_subdev_get_try_format(sd, cfg, 0);
		crop = v4l2_subdev_get_try_crop(sd, cfg, 0);
	}

	switch (s->target) {
	case V4L2_SEL_TGT_CROP:
		s->r = *crop;
		break;
	case V4L2_SEL_TGT_CROP_BOUNDS:
	case V4L2_SEL_TGT_CROP_DEFAULT:
		s->r = dcmipp_postproc_get_crop_bound(sink_fmt);
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

static int dcmipp_postproc_set_selection(struct v4l2_subdev *sd,
					 struct v4l2_subdev_pad_config *cfg,
					 struct v4l2_subdev_selection *s)
{
	struct dcmipp_postproc_device *postproc = v4l2_get_subdevdata(sd);
	struct v4l2_mbus_framefmt *sink_fmt;
	struct v4l2_rect *crop;
	bool _do_crop;
	bool *do_crop;

	if (IS_SINK(sd, s->pad))
		return -EINVAL;

	if (s->which == V4L2_SUBDEV_FORMAT_ACTIVE) {
		sink_fmt = &postproc->common.sink_fmt;
		crop = &postproc->crop;
		do_crop = &postproc->do_crop;
	} else {
		sink_fmt = v4l2_subdev_get_try_format(sd, cfg, 0);
		crop = v4l2_subdev_get_try_crop(sd, cfg, 0);
		do_crop = &_do_crop;
	}

	switch (s->target) {
	case V4L2_SEL_TGT_CROP:
		dcmipp_postproc_adjust_crop(&s->r, sink_fmt);

		*crop = s->r;
		*do_crop = true;

		dev_dbg(postproc->common.dev, "s_selection: crop %ux%u@(%u,%u)\n",
			crop->width, crop->height, crop->left, crop->top);
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

static const struct v4l2_subdev_pad_ops dcmipp_postproc_pad_ops = {
	.init_cfg		= dcmipp_init_cfg,
	.enum_mbus_code		= dcmipp_enum_mbus_code,
	.enum_frame_size	= dcmipp_enum_frame_size,
	.get_fmt		= dcmipp_get_fmt,
	.set_fmt		= dcmipp_postproc_set_fmt,
	.get_selection		= dcmipp_postproc_get_selection,
	.set_selection		= dcmipp_postproc_set_selection,
};

static int dcmipp_postproc_configure_scale_crop
			(struct dcmipp_postproc_device *postproc)
{
	struct dcmipp_common_device *ent = to_dcmipp_common(postproc);
	const struct dcmipp_pix_map *vpix;
	u32 hprediv, vprediv;
	struct v4l2_rect crop;
	bool do_crop = false;
	u32 val = 0;

	/* find bpp from format */
	vpix = dcmipp_pix_map_by_code(ent->src_fmt.code,
				      STAGE(ent->pipe_id,
					    DCMIPP_POSTPROC, OUTPUT));
	if (!vpix)
		return -EINVAL;

	/* clear decimation/crop */
	reg_clear(ent, DCMIPP_P0PPCR, DCMIPP_P0PPCR_BSM_MASK);
	reg_clear(ent, DCMIPP_P0PPCR, DCMIPP_P0PPCR_LSM);
	reg_write(ent, DCMIPP_P0SCSTR, 0);
	reg_write(ent, DCMIPP_P0SCSZR, 0);

	if (vpix->pixelformat == V4L2_PIX_FMT_JPEG)
		/* Ignore scale/crop with JPEG */
		return 0;

	/* decimation */
	hprediv = postproc->common.sink_fmt.width /
		  postproc->common.src_fmt.width;
	if (hprediv == 2)
		val |= DCMIPP_P0PPCR_BSM_2_4 << 7;/* 2 bytes out of 4 */

	vprediv = postproc->common.sink_fmt.height /
		  postproc->common.src_fmt.height;
	if (vprediv == 2)
		val |= DCMIPP_P0PPCR_LSM;/* one line out of two */

	if (val) {
		/* decimate using bytes and lines skipping */
		reg_set(ent, DCMIPP_P0PPCR, val);

		/* crop to decimated resolution */
		crop.top = 0;
		crop.left = 0;
		crop.width = postproc->common.src_fmt.width;
		crop.height = postproc->common.src_fmt.height;
		do_crop = true;

		dev_dbg(postproc->common.dev, "decimate to %dx%d [prediv=%dx%d]\n",
			crop.width, crop.height, hprediv, vprediv);
	}

	if (postproc->do_crop) {
		/* explicit crop superseed default crop */
		crop = postproc->crop;
		do_crop = true;
	}

	if (do_crop) {
		dev_dbg(postproc->common.dev, "crop to %dx%d\n",
			crop.width, crop.height);

		/* expressed in 32-bits words on X axis, lines on Y axis */
		reg_write(ent, DCMIPP_P0SCSTR,
			  ((crop.left * vpix->bpp) / 4) | crop.top << 16);
		reg_write(ent, DCMIPP_P0SCSZR,
			  DCMIPP_P0SCSZR_ENABLE |
			  ((crop.width * vpix->bpp) / 4) | crop.height << 16);
	}

	return 0;
}

static void dcmipp_postproc_configure_framerate
			(struct dcmipp_postproc_device *postproc)
{
	struct dcmipp_common_device *ent = to_dcmipp_common(postproc);

	/* Frame skipping */
	reg_clear(ent, DCMIPP_PXFCTCR(postproc->pipe_id),
		  DCMIPP_P0FCTCR_FRATE_MASK);
	reg_set(ent, DCMIPP_PXFCTCR(postproc->pipe_id), postproc->frate);
}

static int dcmipp_postproc_g_frame_interval(struct v4l2_subdev *sd,
					    struct v4l2_subdev_frame_interval *fi)
{
	struct dcmipp_postproc_device *postproc = v4l2_get_subdevdata(sd);

	if (IS_SINK(sd, fi->pad)) {
		fi->interval = postproc->sink_interval;
	} else {
		fi->interval.numerator = postproc->sink_interval.numerator;
		fi->interval.denominator =
			postproc->sink_interval.denominator /
			(1 << postproc->frate);
	}

	return 0;
}

static int dcmipp_postproc_s_frame_interval(struct v4l2_subdev *sd,
					    struct v4l2_subdev_frame_interval *fi)
{
	struct dcmipp_postproc_device *postproc = v4l2_get_subdevdata(sd);

	mutex_lock(&postproc->common.lock);

	if (postproc->common.streaming) {
		mutex_unlock(&postproc->common.lock);
		return -EBUSY;
	}

	if (IS_SINK(sd, fi->pad)) {
		postproc->sink_interval = fi->interval;
	} else {
		if (fi->interval.denominator >=
		    postproc->sink_interval.denominator) {
			postproc->frate = 0;
		} else {
			if (fi->interval.denominator <=
			   (postproc->sink_interval.denominator / 8))
				postproc->frate = 3;
			else if (fi->interval.denominator <=
			   (postproc->sink_interval.denominator / 4))
				postproc->frate = 2;
			else if (fi->interval.denominator <=
			   (postproc->sink_interval.denominator / 2))
				postproc->frate = 1;
		}
	}

	mutex_unlock(&postproc->common.lock);

	return 0;
}

#define STOP_TIMEOUT_US 1000
#define POLL_INTERVAL_US  50
static int dcmipp_postproc_s_stream(struct v4l2_subdev *sd, int enable)
{
	struct dcmipp_postproc_device *postproc = v4l2_get_subdevdata(sd);
	int ret = 0;

	mutex_lock(&postproc->common.lock);

	if (enable) {
		dcmipp_postproc_configure_framerate(postproc);

		ret = dcmipp_postproc_configure_scale_crop(postproc);
		if (ret)
			goto err;
	}

err:
	mutex_unlock(&postproc->common.lock);

	return ret;
}

static const struct v4l2_subdev_video_ops dcmipp_postproc_video_ops = {
	.g_frame_interval = dcmipp_postproc_g_frame_interval,
	.s_frame_interval = dcmipp_postproc_s_frame_interval,
	.s_stream = dcmipp_postproc_s_stream,
};

static const struct v4l2_subdev_ops dcmipp_postproc_ops = {
	.pad = &dcmipp_postproc_pad_ops,
	.video = &dcmipp_postproc_video_ops,
};

static void dcmipp_postproc_release(struct v4l2_subdev *sd)
{
	struct dcmipp_common_device *ent =
				container_of(sd, struct dcmipp_common_device, sd);

	kfree(ent);
}

static const struct v4l2_subdev_internal_ops dcmipp_postproc_int_ops = {
	.release = dcmipp_postproc_release,
};

static void dcmipp_postproc_comp_unbind(struct device *comp, struct device *master,
					void *master_data)
{
	struct dcmipp_ent_device *ved = dev_get_drvdata(comp);
	struct dcmipp_common_device *ent = container_of(ved, struct dcmipp_common_device,
						    ved);

	dcmipp_ent_sd_unregister(ved, &ent->sd);
}

static int dcmipp_postproc_comp_bind(struct device *comp, struct device *master,
				     void *master_data)
{
	struct v4l2_device *v4l2_dev = master_data;
	struct dcmipp_platform_data *pdata = comp->platform_data;
	struct dcmipp_postproc_device *postproc;
	int ret;

	/* Allocate the postproc struct */
	postproc = kzalloc(sizeof(*postproc), GFP_KERNEL);
	if (!postproc)
		return -ENOMEM;

	/* Initialize ved and sd */
	ret = dcmipp_ent_sd_register(&postproc->common.ved, &postproc->common.sd,
				     v4l2_dev,
				     pdata->entity_name,
				     MEDIA_ENT_F_PROC_VIDEO_PIXEL_FORMATTER, 2,
				     (const unsigned long[2]) {
				     MEDIA_PAD_FL_SINK,
				     MEDIA_PAD_FL_SOURCE,
				     },
				     &dcmipp_postproc_int_ops, &dcmipp_postproc_ops,
				     NULL, NULL,
				     &postproc->common.regs);
	if (ret) {
		kfree(postproc);
		return ret;
	}

	dev_set_drvdata(comp, &postproc->common.ved);
	postproc->common.dev = comp;
	postproc->common.type = DCMIPP_POSTPROC;

	/* Pipe identifier */
	postproc->common.pipe_id = dcmipp_name_to_pipe_id(pdata->entity_name);

	return 0;
}

static const struct component_ops dcmipp_postproc_comp_ops = {
	.bind = dcmipp_postproc_comp_bind,
	.unbind = dcmipp_postproc_comp_unbind,
};

static int dcmipp_postproc_probe(struct platform_device *pdev)
{
	return component_add(&pdev->dev, &dcmipp_postproc_comp_ops);
}

static int dcmipp_postproc_remove(struct platform_device *pdev)
{
	component_del(&pdev->dev, &dcmipp_postproc_comp_ops);

	return 0;
}

static const struct platform_device_id dcmipp_postproc_driver_ids[] = {
	{
		.name           = DCMIPP_POSTPROC_DRV_NAME,
	},
	{ }
};

static struct platform_driver dcmipp_postproc_pdrv = {
	.probe		= dcmipp_postproc_probe,
	.remove		= dcmipp_postproc_remove,
	.id_table	= dcmipp_postproc_driver_ids,
	.driver		= {
		.name	= DCMIPP_POSTPROC_DRV_NAME,
	},
};

module_platform_driver(dcmipp_postproc_pdrv);

MODULE_DEVICE_TABLE(platform, dcmipp_postproc_driver_ids);

MODULE_AUTHOR("Hugues Fruchet <hugues.fruchet@st.com>");
MODULE_DESCRIPTION("STMicroelectronics STM32 Digital Camera Memory Interface with Pixel Processor driver");
MODULE_LICENSE("GPL");
