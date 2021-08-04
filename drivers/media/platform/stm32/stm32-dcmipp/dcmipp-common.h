/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Driver for STM32 Digital Camera Memory Interface Pixel Processor
 *
 * Copyright (C) STMicroelectronics SA 2021
 * Authors: Hugues Fruchet <hugues.fruchet@foss.st.com>
 *          Alain Volmat <alain.volmat@foss.st.com>
 *          for STMicroelectronics.
 */

#ifndef _DCMIPP_COMMON_H_
#define _DCMIPP_COMMON_H_

#include <linux/interrupt.h>
#include <linux/slab.h>
#include <media/media-device.h>
#include <media/v4l2-device.h>
#include <media/v4l2-fwnode.h>

#define DCMIPP_PDEV_NAME "dcmipp"

/* DCMIPP-specific controls */
#define DCMIPP_CID_DCMIPP_BASE		(0x00f00000 | 0xf000)
#define DCMIPP_CID_DCMIPP_CLASS		(0x00f00000 | 1)
#define DCMIPP_CID_TEST_PATTERN		(DCMIPP_CID_DCMIPP_BASE + 0)

#define DCMIPP_FRAME_MAX_WIDTH 4096
#define DCMIPP_FRAME_MAX_HEIGHT 2160
#define DCMIPP_FRAME_MIN_WIDTH 16
#define DCMIPP_FRAME_MIN_HEIGHT 16

/* Pipe identifier */
#define DCMIPP_DUMP_PIPE 0

#define DCMIPP_FRAME_INDEX(lin, col, width, bpp) \
	(((lin) * (width) + (col)) * (bpp))

/**
 * struct dcmipp_colorimetry_clamp - Adjust colorimetry parameters
 *
 * @fmt:		the pointer to struct v4l2_pix_format or
 *			struct v4l2_mbus_framefmt
 *
 * Entities must check if colorimetry given by the userspace is valid, if not
 * then set them as DEFAULT
 */
#define dcmipp_colorimetry_clamp(fmt)					\
do {									\
	if ((fmt)->colorspace == V4L2_COLORSPACE_DEFAULT ||		\
	    (fmt)->colorspace > V4L2_COLORSPACE_DCI_P3) {		\
		(fmt)->colorspace = V4L2_COLORSPACE_DEFAULT;		\
		(fmt)->ycbcr_enc = V4L2_YCBCR_ENC_DEFAULT;		\
		(fmt)->quantization = V4L2_QUANTIZATION_DEFAULT;	\
		(fmt)->xfer_func = V4L2_XFER_FUNC_DEFAULT;		\
	}								\
	if ((fmt)->ycbcr_enc > V4L2_YCBCR_ENC_SMPTE240M)		\
		(fmt)->ycbcr_enc = V4L2_YCBCR_ENC_DEFAULT;		\
	if ((fmt)->quantization > V4L2_QUANTIZATION_LIM_RANGE)		\
		(fmt)->quantization = V4L2_QUANTIZATION_DEFAULT;	\
	if ((fmt)->xfer_func > V4L2_XFER_FUNC_SMPTE2084)		\
		(fmt)->xfer_func = V4L2_XFER_FUNC_DEFAULT;		\
} while (0)

/**
 * struct dcmipp_platform_data - platform data to components
 *
 * @entity_name:	The name of the entity to be created
 *
 * Board setup code will often provide additional information using the device's
 * platform_data field to hold additional information.
 * When injecting a new platform_device in the component system the core needs
 * to provide to the corresponding submodules the name of the entity that should
 * be used when registering the subdevice in the Media Controller system.
 */
struct dcmipp_platform_data {
	char entity_name[32];
};

/*
 * Enum pointing pipeline stages
 */
#define DCMIPP_ISP	0
#define	DCMIPP_POSTPROC	2
#define INPUT	1
#define OUTPUT	2
#define STAGE(pipe, subdev, pad)	(((pad) << (subdev)) << (8 * (pipe)))
enum dcmipp_pipeline_stage {
	DCMIPP_PIPE0_POSTPROC_INPUT	= STAGE(0, DCMIPP_POSTPROC, INPUT),
	DCMIPP_PIPE0_POSTPROC_OUTPUT	= STAGE(0, DCMIPP_POSTPROC, OUTPUT),
};

/**
 * struct dcmipp_pix_map - maps media bus code with v4l2 pixel format
 *
 * @code:		media bus format code defined by MEDIA_BUS_FMT_* macros
 * @bbp:		number of bytes each pixel occupies
 * @pixelformat:	pixel format devined by V4L2_PIX_FMT_* macros
 *
 * Struct which matches the MEDIA_BUS_FMT_* codes with the corresponding
 * V4L2_PIX_FMT_* fourcc pixelformat and its bytes per pixel (bpp)
 */
struct dcmipp_pix_map {
	unsigned int code;
	unsigned int bpp;
	u32 pixelformat;
	u32 csi2_dt;
	u32 availability;
	u32 nb_planes;
	bool bayer;
	u8 prcr_format;
	u8 prcr_swapbits;
	u8 prcr_swaprb;
	u8 prcr_swapcycles;
};

/**
 * struct dcmipp_ent_device - core struct that represents a node in the topology
 *
 * @ent:		the pointer to struct media_entity for the node
 * @pads:		the list of pads of the node
 * @process_frame:	callback send a frame to that node
 * @vdev_get_format:	callback that returns the current format a pad, used
 *			only when is_media_entity_v4l2_video_device(ent) returns
 *			true
 *
 * Each node of the topology must create a dcmipp_ent_device struct. Depending on
 * the node it will be of an instance of v4l2_subdev or video_device struct
 * where both contains a struct media_entity.
 * Those structures should embedded the dcmipp_ent_device struct through
 * v4l2_set_subdevdata() and video_set_drvdata() respectivaly, allowing the
 * dcmipp_ent_device struct to be retrieved from the corresponding struct
 * media_entity
 */
struct dcmipp_ent_device {
	struct media_entity *ent;
	struct media_pad *pads;
	void * (*process_frame)(struct dcmipp_ent_device *ved,
				const void *frame);
	void (*vdev_get_format)(struct dcmipp_ent_device *ved,
				struct v4l2_pix_format *fmt);

	/* Parallel input device */
	struct v4l2_fwnode_bus_parallel	bus;
	irq_handler_t handler;
	irq_handler_t thread_fn;
	void __iomem **regs;
	struct reset_control **rstc;
};

struct dcmipp_common_device {
	struct dcmipp_ent_device ved;
	struct v4l2_subdev sd;
	struct device *dev;
	struct v4l2_mbus_framefmt sink_fmt;
	struct v4l2_mbus_framefmt src_fmt;
	bool streaming;
	/* Protect this data structure */
	struct mutex lock;

	unsigned int type;

	unsigned int pipe_id;

	void __iomem *regs;
};

int dcmipp_init_cfg(struct v4l2_subdev *sd,
		    struct v4l2_subdev_pad_config *cfg);
int dcmipp_enum_mbus_code(struct v4l2_subdev *sd,
			  struct v4l2_subdev_pad_config *cfg,
			  struct v4l2_subdev_mbus_code_enum *code);
int dcmipp_enum_frame_size(struct v4l2_subdev *sd,
			   struct v4l2_subdev_pad_config *cfg,
			   struct v4l2_subdev_frame_size_enum *fse);
int dcmipp_get_fmt(struct v4l2_subdev *sd,
		   struct v4l2_subdev_pad_config *cfg,
		   struct v4l2_subdev_format *fmt);
int dcmipp_set_fmt(struct v4l2_subdev *sd,
		   struct v4l2_subdev_pad_config *cfg,
		   struct v4l2_subdev_format *fmt);
/**
 * dcmipp_pads_init - initialize pads
 *
 * @num_pads:	number of pads to initialize
 * @pads_flags:	flags to use in each pad
 *
 * Helper functions to allocate/initialize pads
 */
struct media_pad *dcmipp_pads_init(u16 num_pads,
				   const unsigned long *pads_flag);

/**
 * dcmipp_pads_cleanup - free pads
 *
 * @pads: pointer to the pads
 *
 * Helper function to free the pads initialized with dcmipp_pads_init
 */
static inline void dcmipp_pads_cleanup(struct media_pad *pads)
{
	kfree(pads);
}

/**
 * dcmipp_pix_map_by_index - get dcmipp_pix_map struct by its index
 *
 * @i:			index of the dcmipp_pix_map struct in dcmipp_pix_map_list
 */
const struct dcmipp_pix_map *dcmipp_pix_map_by_index(unsigned int i, u32 stage);
const struct dcmipp_pix_map *_dcmipp_pix_map_by_index(unsigned int i,
						      const struct dcmipp_pix_map *l,
						      unsigned int size);

/**
 * dcmipp_pix_map_by_code - get dcmipp_pix_map struct by media bus code
 *
 * @code:		media bus format code defined by MEDIA_BUS_FMT_* macros
 */
const struct dcmipp_pix_map *dcmipp_pix_map_by_code(u32 code, u32 stage);
const struct dcmipp_pix_map *_dcmipp_pix_map_by_code(u32 code,
						     const struct dcmipp_pix_map *l,
						     unsigned int size);

/**
 * dcmipp_pix_map_by_pixelformat - get dcmipp_pix_map struct by v4l2 pixel format
 *
 * @pixelformat:	pixel format devined by V4L2_PIX_FMT_* macros
 */
const struct dcmipp_pix_map *dcmipp_pix_map_by_pixelformat(u32 pixelformat, u32 stage);
const struct dcmipp_pix_map *_dcmipp_pix_map_by_pixelformat(u32 pixelformat,
							    const struct dcmipp_pix_map *l,
							    unsigned int size);

int dcmipp_name_to_pipe_id(const char *name);

/**
 * dcmipp_ent_sd_register - initialize and register a subdev node
 *
 * @ved:	the dcmipp_ent_device struct to be initialize
 * @sd:		the v4l2_subdev struct to be initialize and registered
 * @v4l2_dev:	the v4l2 device to register the v4l2_subdev
 * @name:	name of the sub-device. Please notice that the name must be
 *		unique.
 * @function:	media entity function defined by MEDIA_ENT_F_* macros
 * @num_pads:	number of pads to initialize
 * @pads_flag:	flags to use in each pad
 * @sd_int_ops:	pointer to &struct v4l2_subdev_internal_ops
 * @sd_ops:	pointer to &struct v4l2_subdev_ops.
 *
 * Helper function initialize and register the struct dcmipp_ent_device and struct
 * v4l2_subdev which represents a subdev node in the topology
 */
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
			   void __iomem **regs);

/**
 * dcmipp_ent_sd_unregister - cleanup and unregister a subdev node
 *
 * @ved:	the dcmipp_ent_device struct to be cleaned up
 * @sd:		the v4l2_subdev struct to be unregistered
 *
 * Helper function cleanup and unregister the struct dcmipp_ent_device and struct
 * v4l2_subdev which represents a subdev node in the topology
 */
void dcmipp_ent_sd_unregister(struct dcmipp_ent_device *ved,
			      struct v4l2_subdev *sd);

/**
 * dcmipp_link_validate - validates a media link
 *
 * @link: pointer to &struct media_link
 *
 * This function call validates if a media link is valid for streaming.
 */
int dcmipp_link_validate(struct media_link *link);

#define reg_write(device, reg, val) \
	(reg_write_dbg((device)->dev, #reg, (device)->regs, (reg), (val)))
#define reg_read(device, reg) \
	 (reg_read_dbg((device)->dev, #reg, (device)->regs, (reg)))
#define reg_set(device, reg, mask) \
	 (reg_set_dbg((device)->dev, #reg, (device)->regs, (reg), (mask)))
#define reg_clear(device, reg, mask) \
	 (reg_clear_dbg((device)->dev, #reg, (device)->regs, (reg), (mask)))

static inline u32 reg_read_dbg(struct device *dev, const char *regname,
			       void __iomem *base, u32 reg)
{
	u32 val = readl_relaxed(base + reg);

	dev_dbg(dev, "RD  %s %#10.8x\n", regname, val);
	return val;
}

static inline void reg_write_dbg(struct device *dev, const char *regname,
				 void __iomem *base, u32 reg, u32 val)
{
	dev_dbg(dev, "WR  %s %#10.8x\n", regname, val);
	writel_relaxed(val, base + reg);
}

static inline void reg_set_dbg(struct device *dev, const char *regname,
			       void __iomem *base, u32 reg, u32 mask)
{
	dev_dbg(dev, "SET %s %#10.8x\n", regname, mask);
	reg_write_dbg(dev, regname, base, reg, readl_relaxed(base + reg) | mask);
}

static inline void reg_clear_dbg(struct device *dev, const char *regname,
				 void __iomem *base, u32 reg, u32 mask)
{
	dev_dbg(dev, "CLR %s %#10.8x\n", regname, mask);
	reg_write_dbg(dev, regname, base, reg, readl_relaxed(base + reg) & ~mask);
}

#endif

