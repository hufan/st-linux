// SPDX-License-Identifier: GPL-2.0
/*
 * Driver for STM32 Digital Camera Memory Interface Pixel Processor
 *
 * Copyright (C) STMicroelectronics SA 2021
 * Authors: Hugues Fruchet <hugues.fruchet@foss.st.com>
 *          Alain Volmat <alain.volmat@foss.st.com>
 *          for STMicroelectronics.
 */

#include <linux/init.h>
#include <linux/module.h>

#include "dcmipp-common.h"

u32 _reg_read(void __iomem *base, u32 reg)
{
	return readl_relaxed(base + reg);
}
EXPORT_SYMBOL_GPL(_reg_read);

void _reg_write(void __iomem *base, u32 reg, u32 val)
{
	writel_relaxed(val, base + reg);
}
EXPORT_SYMBOL_GPL(_reg_write);

#define IS_SINK(sd, pad) ((sd)->entity.pads[(pad)].flags & MEDIA_PAD_FL_SINK)
#define IS_SRC(sd, pad)  ((sd)->entity.pads[(pad)].flags & MEDIA_PAD_FL_SOURCE)
#define PAD_STR(sd, pad) (IS_SRC((sd), (pad))) ? "src" : "sink"

#define DCMIPP_FORMAT_NONE	0x00

#define DCMIPP_FMT(mbus_code, pixfmt, size, avail)	\
	{ .code = MEDIA_BUS_FMT_##mbus_code, \
	  .pixelformat = V4L2_PIX_FMT_##pixfmt, \
	  .bpp = size, .availability = DCMIPP_##avail }
#define DCMIPP_INPUT_FMT(mbus_code, pixfmt, size, avail) \
		DCMIPP_FMT(mbus_code, pixfmt, size, avail)
#define DCMIPP_OUTPUT_FMT(mbus_code, pixfmt, size, avail) \
		DCMIPP_FMT(mbus_code, pixfmt, size, avail)
#define DCMIPP_INTERNAL_FMT(mbus_code, pixfmt, size, avail) \
		DCMIPP_FMT(mbus_code, pixfmt, size, avail)

static const struct dcmipp_pix_map dcmipp_pix_map_list[] = {
	/* RGB formats */
	DCMIPP_INPUT_FMT(RGB565_2X8_LE, RGB565, 2, PIPE0_POSTPROC_INPUT),
	DCMIPP_OUTPUT_FMT(RGB565_2X8_LE, RGB565, 2, PIPE0_POSTPROC_OUTPUT),

	/* YUV formats */
	DCMIPP_INPUT_FMT(YUYV8_2X8, YUYV, 2, PIPE0_POSTPROC_INPUT),
	DCMIPP_OUTPUT_FMT(YUYV8_2X8, YUYV, 2, PIPE0_POSTPROC_OUTPUT),
	DCMIPP_INPUT_FMT(UYVY8_2X8, UYVY, 2, PIPE0_POSTPROC_INPUT),
	DCMIPP_OUTPUT_FMT(UYVY8_2X8, UYVY, 2, PIPE0_POSTPROC_OUTPUT),
	DCMIPP_INPUT_FMT(Y8_1X8, GREY, 1, PIPE0_POSTPROC_INPUT),
	DCMIPP_OUTPUT_FMT(Y8_1X8, GREY, 1, PIPE0_POSTPROC_OUTPUT),

	/* Bayer formats - 8bits */
	DCMIPP_INPUT_FMT(SBGGR8_1X8, SBGGR8, 1, PIPE0_POSTPROC_INPUT),
	DCMIPP_OUTPUT_FMT(SBGGR8_1X8, SBGGR8, 1, PIPE0_POSTPROC_OUTPUT),
	DCMIPP_INPUT_FMT(SGBRG8_1X8, SGBRG8, 1, PIPE0_POSTPROC_INPUT),
	DCMIPP_OUTPUT_FMT(SGBRG8_1X8, SGBRG8, 1, PIPE0_POSTPROC_OUTPUT),
	DCMIPP_INPUT_FMT(SGRBG8_1X8, SGRBG8, 1, PIPE0_POSTPROC_INPUT),
	DCMIPP_OUTPUT_FMT(SGRBG8_1X8, SGRBG8, 1, PIPE0_POSTPROC_OUTPUT),
	DCMIPP_INPUT_FMT(SRGGB8_1X8, SRGGB8, 1, PIPE0_POSTPROC_INPUT),
	DCMIPP_OUTPUT_FMT(SRGGB8_1X8, SRGGB8, 1, PIPE0_POSTPROC_OUTPUT),

	/* JPEG format */
	DCMIPP_INPUT_FMT(JPEG_1X8, JPEG, 1, PIPE0_POSTPROC_INPUT),
	DCMIPP_OUTPUT_FMT(JPEG_1X8, JPEG, 1, PIPE0_POSTPROC_OUTPUT),
};

const struct dcmipp_pix_map *dcmipp_pix_map_by_index(unsigned int i, u32 stage)
{
	const struct dcmipp_pix_map *fmt = NULL;
	int j = 0;

	while (j < ARRAY_SIZE(dcmipp_pix_map_list)) {
		if (dcmipp_pix_map_list[j].availability & stage) {
			if (i == 0) {
				fmt = &dcmipp_pix_map_list[j];
				break;
			}
			i--;
		}
		j++;
	}

	return fmt;
}
EXPORT_SYMBOL_GPL(dcmipp_pix_map_by_index);

const struct dcmipp_pix_map *dcmipp_pix_map_by_code(u32 code, u32 stage)
{
	unsigned int i;

	for (i = 0; i < ARRAY_SIZE(dcmipp_pix_map_list); i++) {
		if (dcmipp_pix_map_list[i].code == code &&
		    dcmipp_pix_map_list[i].availability & stage)
			return &dcmipp_pix_map_list[i];
	}
	return NULL;
}
EXPORT_SYMBOL_GPL(dcmipp_pix_map_by_code);

const struct dcmipp_pix_map *dcmipp_pix_map_by_pixelformat(u32 pixelformat,
							   u32 stage)
{
	unsigned int i;

	for (i = 0; i < ARRAY_SIZE(dcmipp_pix_map_list); i++) {
		if (dcmipp_pix_map_list[i].pixelformat == pixelformat)
			return &dcmipp_pix_map_list[i];
	}
	return NULL;
}
EXPORT_SYMBOL_GPL(dcmipp_pix_map_by_pixelformat);

const struct dcmipp_pix_map *_dcmipp_pix_map_by_index
						(unsigned int i,
						 const struct dcmipp_pix_map *l,
						 unsigned int size)
{
	if (i >= size)
		return NULL;

	return &l[i];
}
EXPORT_SYMBOL_GPL(_dcmipp_pix_map_by_index);

const struct dcmipp_pix_map *_dcmipp_pix_map_by_code
					(u32 code,
					 const struct dcmipp_pix_map *l,
					 unsigned int size)
{
	unsigned int i;

	for (i = 0; i < size; i++) {
		if (l[i].code == code)
			return &l[i];
	}
	return NULL;
}
EXPORT_SYMBOL_GPL(_dcmipp_pix_map_by_code);

const struct dcmipp_pix_map *_dcmipp_pix_map_by_pixelformat
						(u32 pixelformat,
						 const struct dcmipp_pix_map *l,
						 unsigned int size)
{
	unsigned int i;

	for (i = 0; i < size; i++) {
		if (l[i].pixelformat == pixelformat)
			return &l[i];
	}
	return NULL;
}
EXPORT_SYMBOL_GPL(_dcmipp_pix_map_by_pixelformat);

int dcmipp_name_to_pipe_id(const char *name)
{
	if (strstr(name, "dump"))
		return 0;
	else
		return -EINVAL;
}
EXPORT_SYMBOL_GPL(dcmipp_name_to_pipe_id);

/* Helper function to allocate and initialize pads */
struct media_pad *dcmipp_pads_init(u16 num_pads, const unsigned long *pads_flag)
{
	struct media_pad *pads;
	unsigned int i;

	/* Allocate memory for the pads */
	pads = kcalloc(num_pads, sizeof(*pads), GFP_KERNEL);
	if (!pads)
		return ERR_PTR(-ENOMEM);

	/* Initialize the pads */
	for (i = 0; i < num_pads; i++) {
		pads[i].index = i;
		pads[i].flags = pads_flag[i];
	}

	return pads;
}
EXPORT_SYMBOL_GPL(dcmipp_pads_init);

int dcmipp_link_validate(struct media_link *link)
{
	/* TODO */
	return 0;
}
EXPORT_SYMBOL_GPL(dcmipp_link_validate);

static const struct media_entity_operations dcmipp_ent_sd_mops = {
	.link_validate = dcmipp_link_validate,
};

int dcmipp_ent_sd_register(struct dcmipp_ent_device *ved,
			   struct v4l2_subdev *sd,
			   struct v4l2_device *v4l2_dev,
			   const char *const name,
			   u32 function,
			   u16 num_pads,
			   const unsigned long *pads_flag,
			   const struct v4l2_subdev_internal_ops *sd_int_ops,
			   const struct v4l2_subdev_ops *sd_ops,
			   irq_handler_t handler,
			   irq_handler_t thread_fn,
			   void __iomem **regs)
{
	int ret;

	/* Allocate the pads. Should be released from the sd_int_op release */
	ved->pads = dcmipp_pads_init(num_pads, pads_flag);
	if (IS_ERR(ved->pads))
		return PTR_ERR(ved->pads);

	/* Fill the dcmipp_ent_device struct */
	ved->ent = &sd->entity;

	/* Initialize the subdev */
	v4l2_subdev_init(sd, sd_ops);
	sd->internal_ops = sd_int_ops;
	sd->entity.function = function;
	sd->entity.ops = &dcmipp_ent_sd_mops;
	sd->owner = THIS_MODULE;
	strscpy(sd->name, name, sizeof(sd->name));
	v4l2_set_subdevdata(sd, ved);

	/* Expose this subdev to user space */
	sd->flags |= V4L2_SUBDEV_FL_HAS_DEVNODE;
	if (sd->ctrl_handler)
		sd->flags |= V4L2_SUBDEV_FL_HAS_EVENTS;

	/* Initialize the media entity */
	ret = media_entity_pads_init(&sd->entity, num_pads, ved->pads);
	if (ret)
		goto err_clean_pads;

	/* Register the subdev with the v4l2 and the media framework */
	ret = v4l2_device_register_subdev(v4l2_dev, sd);
	if (ret) {
		dev_err(v4l2_dev->dev,
			"%s: subdev register failed (err=%d)\n",
			name, ret);
		goto err_clean_m_ent;
	}

	ved->handler = handler;
	ved->thread_fn = thread_fn;
	ved->regs = regs;

	return 0;

err_clean_m_ent:
	media_entity_cleanup(&sd->entity);
err_clean_pads:
	dcmipp_pads_cleanup(ved->pads);
	return ret;
}
EXPORT_SYMBOL_GPL(dcmipp_ent_sd_register);

void dcmipp_ent_sd_unregister(struct dcmipp_ent_device *ved, struct v4l2_subdev *sd)
{
	media_entity_cleanup(ved->ent);
	v4l2_device_unregister_subdev(sd);
}
EXPORT_SYMBOL_GPL(dcmipp_ent_sd_unregister);

/*
 * Helpers for format related callbacks
 */
static const struct v4l2_mbus_framefmt fmt_default = {
	.width = 640,
	.height = 480,
	.code = MEDIA_BUS_FMT_SRGGB8_1X8,
	.field = V4L2_FIELD_NONE,
	.colorspace = V4L2_COLORSPACE_DEFAULT,
};

int dcmipp_init_cfg(struct v4l2_subdev *sd,
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
EXPORT_SYMBOL_GPL(dcmipp_init_cfg);

int dcmipp_enum_mbus_code(struct v4l2_subdev *sd,
			  struct v4l2_subdev_pad_config *cfg,
			  struct v4l2_subdev_mbus_code_enum *code)
{
	struct dcmipp_common_device *ent = v4l2_get_subdevdata(sd);
	const struct dcmipp_pix_map *vpix;
	unsigned int inout;

	/* TODO - do we need to check that pad value is not >= num_pads ? */
	if (IS_SINK(sd, code->pad))
		inout = INPUT;
	else
		inout = OUTPUT;

	vpix = dcmipp_pix_map_by_index(code->index,
				       STAGE(ent->pipe_id, ent->type, inout));

	if (!vpix)
		return -EINVAL;

	code->code = vpix->code;

	return 0;
}
EXPORT_SYMBOL_GPL(dcmipp_enum_mbus_code);

int dcmipp_enum_frame_size(struct v4l2_subdev *sd,
			   struct v4l2_subdev_pad_config *cfg,
			   struct v4l2_subdev_frame_size_enum *fse)
{
	struct dcmipp_common_device *ent = v4l2_get_subdevdata(sd);
	const struct dcmipp_pix_map *vpix;
	unsigned int inout;

	if (fse->index)
		return -EINVAL;

	/* TODO - do we need to check that pad value is not >= num_pads ? */
	if (IS_SINK(sd, fse->pad))
		inout = INPUT;
	else
		inout = OUTPUT;

	/* Only accept code in the pix map table */
	vpix = dcmipp_pix_map_by_code(fse->code,
				      STAGE(ent->pipe_id, ent->type, inout));
	if (!vpix)
		return -EINVAL;

	fse->min_width = DCMIPP_FRAME_MIN_WIDTH;
	fse->max_width = DCMIPP_FRAME_MAX_WIDTH;
	fse->min_height = DCMIPP_FRAME_MIN_HEIGHT;
	fse->max_height = DCMIPP_FRAME_MAX_HEIGHT;

	return 0;
}
EXPORT_SYMBOL_GPL(dcmipp_enum_frame_size);

int dcmipp_get_fmt(struct v4l2_subdev *sd,
		   struct v4l2_subdev_pad_config *cfg,
		   struct v4l2_subdev_format *fmt)
{
	struct dcmipp_common_device *ent = v4l2_get_subdevdata(sd);

	mutex_lock(&ent->lock);

	if (IS_SINK(sd, fmt->pad))
		fmt->format = fmt->which == V4L2_SUBDEV_FORMAT_TRY ?
			      *v4l2_subdev_get_try_format(sd, cfg, 0) :
			      ent->sink_fmt;
	else
		fmt->format = fmt->which == V4L2_SUBDEV_FORMAT_TRY ?
			      *v4l2_subdev_get_try_format(sd, cfg, 0) :
			      ent->src_fmt;

	mutex_unlock(&ent->lock);

	return 0;
}
EXPORT_SYMBOL_GPL(dcmipp_get_fmt);

static void dcmipp_adjust_fmt(struct dcmipp_common_device *ent,
			      struct v4l2_mbus_framefmt *fmt, u32 inout)
{
	const struct dcmipp_pix_map *vpix;

	/* Only accept code in the pix map table */
	vpix = dcmipp_pix_map_by_code(fmt->code,
				      STAGE(ent->pipe_id, ent->type, inout));
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

int dcmipp_set_fmt(struct v4l2_subdev *sd,
		   struct v4l2_subdev_pad_config *cfg,
		   struct v4l2_subdev_format *fmt)
{
	struct dcmipp_common_device *ent = v4l2_get_subdevdata(sd);
	struct v4l2_mbus_framefmt *pad_fmt;
	int ret = 0;
	unsigned int inout;

	mutex_lock(&ent->lock);

	if (fmt->which == V4L2_SUBDEV_FORMAT_ACTIVE) {
		if (ent->streaming) {
			ret = -EBUSY;
			goto out;
		}

		if (IS_SINK(sd, fmt->pad))
			pad_fmt = &ent->sink_fmt;
		else
			pad_fmt = &ent->src_fmt;
	} else {
		pad_fmt = v4l2_subdev_get_try_format(sd, cfg, 0);
	}

	if (IS_SINK(sd, fmt->pad))
		inout = INPUT;
	else
		inout = OUTPUT;

	dcmipp_adjust_fmt(ent, &fmt->format, inout);

	dev_dbg(ent->dev, "%s: %s format update: old:%dx%d (0x%x, %d, %d, %d, %d) new:%dx%d (0x%x, %d, %d, %d, %d)\n",
		ent->sd.name,
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
	mutex_unlock(&ent->lock);

	return ret;
}
EXPORT_SYMBOL_GPL(dcmipp_set_fmt);
