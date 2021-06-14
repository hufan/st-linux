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
#include <linux/module.h>
#include <linux/mod_devicetable.h>
#include <linux/platform_device.h>
#include <linux/v4l2-mediabus.h>
#include <linux/vmalloc.h>
#include <media/v4l2-event.h>
#include <media/v4l2-subdev.h>

#include "dcmipp-common.h"

#define DCMIPP_PAR_DRV_NAME "dcmipp-parallel"

#define IS_SINK(pad) (!(pad))
#define IS_SRC(pad)  ((pad))

#define PAR_MEDIA_BUS_FMT_DEFAULT MEDIA_BUS_FMT_RGB565_2X8_LE

static const struct dcmipp_pix_map dcmipp_par_sink_pix_map_list[] = {
	{
		.code = MEDIA_BUS_FMT_RGB565_2X8_LE,
		.pixelformat = V4L2_PIX_FMT_RGB565,
		.bpp = 2,
		.prcr_format = DCMIPP_PRCR_FORMAT_RGB565,
	},
	{
		.code = MEDIA_BUS_FMT_YUYV8_2X8,
		.pixelformat = V4L2_PIX_FMT_YUYV,
		.bpp = 2,
		.prcr_format = DCMIPP_PRCR_FORMAT_YUV422,
	},
	{
		.code = MEDIA_BUS_FMT_JPEG_1X8,
		.pixelformat = V4L2_PIX_FMT_JPEG,
		.bpp = 1,
		.prcr_format = 0xff,
	},
};

static const struct dcmipp_pix_map dcmipp_par_src_pix_map_list[] = {
	{
		.code = MEDIA_BUS_FMT_RGB565_2X8_LE,
		.pixelformat = V4L2_PIX_FMT_RGB565,
		.bpp = 2,
		.prcr_format = 0x22,
		.prcr_swapcycles = 1,
	},
	{
		.code = MEDIA_BUS_FMT_YUYV8_2X8,
		.pixelformat = V4L2_PIX_FMT_YUYV,
		.bpp = 2,
		.prcr_format = DCMIPP_PRCR_FORMAT_YUV422,
	},
	{
		.code = MEDIA_BUS_FMT_JPEG_1X8,
		.pixelformat = V4L2_PIX_FMT_JPEG,
		.bpp = 2,
	},
};

const struct dcmipp_pix_map *__dcmipp_pix_map_by_index(unsigned int i,
						       __u32 pad)
{
	if (IS_SINK(pad))
		return _dcmipp_pix_map_by_index
				(i, dcmipp_par_sink_pix_map_list,
				 ARRAY_SIZE(dcmipp_par_sink_pix_map_list));
	else if (IS_SRC(pad))
		return _dcmipp_pix_map_by_index
				(i, dcmipp_par_src_pix_map_list,
				 ARRAY_SIZE(dcmipp_par_src_pix_map_list));

	return NULL;
}

const struct dcmipp_pix_map *__dcmipp_pix_map_by_code(u32 code,
						      __u32 pad)
{
	if (IS_SINK(pad))
		return _dcmipp_pix_map_by_code
				(code, dcmipp_par_sink_pix_map_list,
				 ARRAY_SIZE(dcmipp_par_sink_pix_map_list));
	else if (IS_SRC(pad))
		return _dcmipp_pix_map_by_code
				(code, dcmipp_par_src_pix_map_list,
				 ARRAY_SIZE(dcmipp_par_src_pix_map_list));

	return NULL;
}

struct dcmipp_par_device {
	struct dcmipp_ent_device ved;
	struct v4l2_subdev sd;
	struct device *dev;
	/* The active format */
	struct v4l2_mbus_framefmt mbus_format;
	bool streaming;
	void __iomem			*regs;
	u32				prsr;
	int				errors_count;
	int				buffers_count;
};

static const struct v4l2_mbus_framefmt fmt_default = {
	.width = 640,
	.height = 480,
	.code = PAR_MEDIA_BUS_FMT_DEFAULT,
	.field = V4L2_FIELD_NONE,
	.colorspace = V4L2_COLORSPACE_DEFAULT,
};

static int dcmipp_par_init_cfg(struct v4l2_subdev *sd,
			       struct v4l2_subdev_pad_config *cfg)
{
	unsigned int i;

	for (i = 0; i < sd->entity.num_pads; i++) {
		struct v4l2_mbus_framefmt *mf;

		mf = v4l2_subdev_get_try_format(sd, cfg, i);
		*mf = fmt_default;
	}

	return 0;
}

static int dcmipp_par_enum_mbus_code(struct v4l2_subdev *sd,
				     struct v4l2_subdev_pad_config *cfg,
				     struct v4l2_subdev_mbus_code_enum *code)
{
	const struct dcmipp_pix_map *vpix =
		__dcmipp_pix_map_by_index(code->index, code->pad);

	if (!vpix)
		return -EINVAL;

	code->code = vpix->code;

	return 0;
}

static int dcmipp_par_enum_frame_size(struct v4l2_subdev *sd,
				      struct v4l2_subdev_pad_config *cfg,
				      struct v4l2_subdev_frame_size_enum *fse)
{
	const struct dcmipp_pix_map *vpix;

	if (fse->index)
		return -EINVAL;

	/* Only accept code in the pix map table */
	vpix = __dcmipp_pix_map_by_code(fse->code, fse->pad);
	if (!vpix)
		return -EINVAL;

	fse->min_width = DCMIPP_FRAME_MIN_WIDTH;
	fse->max_width = DCMIPP_FRAME_MAX_WIDTH;
	fse->min_height = DCMIPP_FRAME_MIN_HEIGHT;
	fse->max_height = DCMIPP_FRAME_MAX_HEIGHT;

	return 0;
}

static int dcmipp_par_get_fmt(struct v4l2_subdev *sd,
			      struct v4l2_subdev_pad_config *cfg,
			      struct v4l2_subdev_format *fmt)
{
	struct dcmipp_par_device *par =
				container_of(sd, struct dcmipp_par_device, sd);

	fmt->format = fmt->which == V4L2_SUBDEV_FORMAT_TRY ?
		      *v4l2_subdev_get_try_format(sd, cfg, fmt->pad) :
		      par->mbus_format;

	return 0;
}

static void dcmipp_par_adjust_fmt(struct v4l2_mbus_framefmt *fmt, __u32 pad)
{
	const struct dcmipp_pix_map *vpix;

	/* Only accept code in the pix map table */
	vpix = __dcmipp_pix_map_by_code(fmt->code, pad);
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

static int dcmipp_par_set_fmt(struct v4l2_subdev *sd,
			      struct v4l2_subdev_pad_config *cfg,
			      struct v4l2_subdev_format *fmt)
{
	struct dcmipp_par_device *par = v4l2_get_subdevdata(sd);
	struct v4l2_mbus_framefmt *mf;

	if (fmt->which == V4L2_SUBDEV_FORMAT_ACTIVE) {
		/* Do not change the format while stream is on */
		if (par->streaming)
			return -EBUSY;

		mf = &par->mbus_format;
	} else {
		mf = v4l2_subdev_get_try_format(sd, cfg, fmt->pad);
	}

	/* Set the new format */
	dcmipp_par_adjust_fmt(&fmt->format, fmt->pad);

	dev_dbg(par->dev, "%s: format update: old:%dx%d (0x%x, %d, %d, %d, %d) new:%dx%d (0x%x, %d, %d, %d, %d)\n",
		par->sd.name,
		/* old */
		mf->width, mf->height, mf->code,
		mf->colorspace,	mf->quantization,
		mf->xfer_func, mf->ycbcr_enc,
		/* new */
		fmt->format.width, fmt->format.height, fmt->format.code,
		fmt->format.colorspace, fmt->format.quantization,
		fmt->format.xfer_func, fmt->format.ycbcr_enc);

	*mf = fmt->format;

	return 0;
}

static const struct v4l2_subdev_pad_ops dcmipp_par_pad_ops = {
	.init_cfg		= dcmipp_par_init_cfg,
	.enum_mbus_code		= dcmipp_par_enum_mbus_code,
	.enum_frame_size	= dcmipp_par_enum_frame_size,
	.get_fmt		= dcmipp_par_get_fmt,
	.set_fmt		= dcmipp_par_set_fmt,
};

static int dcmipp_par_configure(struct dcmipp_par_device *par)
{
	u32 val = 0;
	const struct dcmipp_pix_map *vpix;

	/* Set vertical synchronization polarity */
	if (par->ved.bus.flags & V4L2_MBUS_VSYNC_ACTIVE_HIGH)
		val |= DCMIPP_PRCR_VSPOL;

	/* Set horizontal synchronization polarity */
	if (par->ved.bus.flags & V4L2_MBUS_HSYNC_ACTIVE_HIGH)
		val |= DCMIPP_PRCR_HSPOL;

	/* Set pixel clock polarity */
	if (par->ved.bus.flags & V4L2_MBUS_PCLK_SAMPLE_RISING)
		val |= DCMIPP_PRCR_PCKPOL;

	/* Set format */
	vpix = __dcmipp_pix_map_by_code(par->mbus_format.code, 1);
	val |= vpix->prcr_format << 16;

	/* swap LSB vs MSB within one cycle */
	if (vpix->prcr_swapbits)
		val |= DCMIPP_PRCR_SWAPBITS;

	/* swap cycles */
	if (vpix->prcr_swapcycles)
		val |= DCMIPP_PRCR_SWAPCYCLES;

	reg_write(par, DCMIPP_PRCR, val);

	return 0;
}

static int dcmipp_par_s_stream(struct v4l2_subdev *sd, int enable)
{
	struct dcmipp_par_device *par =
				container_of(sd, struct dcmipp_par_device, sd);
	int ret = 0;

	if (enable) {
		ret = dcmipp_par_configure(par);

		par->errors_count = 0;
		par->buffers_count = 0;

		/* Enable error interruptions */
		reg_write(par, DCMIPP_PRIER, DCMIPP_PRIER_ERRIE);

		/* Enable parallel interface */
		reg_set(par, DCMIPP_PRCR, DCMIPP_PRCR_ENABLE);
	} else {
		/* Disable parallel interface */
		reg_clear(par, DCMIPP_PRCR, DCMIPP_PRCR_ENABLE);

		if (par->errors_count)
			dev_warn(par->dev, "Some errors found while streaming: errors=%d, buffers=%d\n",
				 par->errors_count, par->buffers_count);
		dev_dbg(par->dev, "Stop streaming, errors=%d, buffers=%d\n",
			par->errors_count, par->buffers_count);
	}

	par->streaming = enable;
	return ret;
}

static const struct v4l2_subdev_video_ops dcmipp_par_video_ops = {
	.s_stream = dcmipp_par_s_stream,
};

static const struct v4l2_subdev_ops dcmipp_par_ops = {
	.pad = &dcmipp_par_pad_ops,
	.video = &dcmipp_par_video_ops,
};

static void dcmipp_par_release(struct v4l2_subdev *sd)
{
	struct dcmipp_par_device *par =
				container_of(sd, struct dcmipp_par_device, sd);

	kfree(par);
}

static const struct v4l2_subdev_internal_ops dcmipp_par_int_ops = {
	.release = dcmipp_par_release,
};

static void dcmipp_par_comp_unbind(struct device *comp, struct device *master,
				   void *master_data)
{
	struct dcmipp_ent_device *ved = dev_get_drvdata(comp);
	struct dcmipp_par_device *par =
			container_of(ved, struct dcmipp_par_device, ved);

	dcmipp_ent_sd_unregister(ved, &par->sd);
}

static irqreturn_t dcmipp_par_irq_thread(int irq, void *arg)
{
	struct dcmipp_par_device *par =
			container_of(arg, struct dcmipp_par_device, ved);

	if (par->prsr & DCMIPP_PRSR_ERRF)
		par->errors_count++;

	return IRQ_HANDLED;
}

static int dcmipp_par_comp_bind(struct device *comp, struct device *master,
				void *master_data)
{
	struct v4l2_device *v4l2_dev = master_data;
	struct dcmipp_platform_data *pdata = comp->platform_data;
	struct dcmipp_par_device *par;
	int ret;

	/* Allocate the par struct */
	par = kzalloc(sizeof(*par), GFP_KERNEL);
	if (!par)
		return -ENOMEM;

	/* Initialize ved and sd */
	ret = dcmipp_ent_sd_register
		(&par->ved, &par->sd, v4l2_dev,
		 pdata->entity_name,
		 MEDIA_ENT_F_VID_IF_BRIDGE, 2,
		 (const unsigned long[2]) {
		  MEDIA_PAD_FL_SINK,
		  MEDIA_PAD_FL_SOURCE,
		  },
		 &dcmipp_par_int_ops, &dcmipp_par_ops,
		 NULL,
		 dcmipp_par_irq_thread,
		 &par->regs);
	if (ret)
		goto err_free_hdl;

	dev_set_drvdata(comp, &par->ved);
	par->dev = comp;

	/* Initialize the frame format */
	par->mbus_format = fmt_default;

	return 0;

err_free_hdl:
	kfree(par);

	return ret;
}

static const struct component_ops dcmipp_par_comp_ops = {
	.bind = dcmipp_par_comp_bind,
	.unbind = dcmipp_par_comp_unbind,
};

static int dcmipp_par_probe(struct platform_device *pdev)
{
	return component_add(&pdev->dev, &dcmipp_par_comp_ops);
}

static int dcmipp_par_remove(struct platform_device *pdev)
{
	component_del(&pdev->dev, &dcmipp_par_comp_ops);

	return 0;
}

static const struct platform_device_id dcmipp_par_driver_ids[] = {
	{
		.name           = DCMIPP_PAR_DRV_NAME,
	},
	{ }
};

static struct platform_driver dcmipp_par_pdrv = {
	.probe		= dcmipp_par_probe,
	.remove		= dcmipp_par_remove,
	.id_table	= dcmipp_par_driver_ids,
	.driver		= {
		.name	= DCMIPP_PAR_DRV_NAME,
	},
};

module_platform_driver(dcmipp_par_pdrv);

MODULE_DEVICE_TABLE(platform, dcmipp_par_driver_ids);

MODULE_AUTHOR("Hugues Fruchet <hugues.fruchet@st.com>");
MODULE_DESCRIPTION("STMicroelectronics STM32 Digital Camera Memory Interface with Pixel Processor driver");
MODULE_LICENSE("GPL");
