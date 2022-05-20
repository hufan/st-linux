/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright (C) 2015 Atmel
 *
 * Boris Brezillon <boris.brezillon@free-electrons.com
 */

#ifndef __AT91_SOC_H
#define __AT91_SOC_H

#include <linux/sys_soc.h>

struct at91_soc {
	u32 cidr_match;
	u32 cidr_mask;
	u32 version_mask;
	u32 exid_match;
	const char *name;
	const char *family;
};

#define AT91_SOC(__cidr, __cidr_mask, __version_mask, __exid,	\
		 __name, __family)				\
	{							\
		.cidr_match = (__cidr),				\
		.cidr_mask = (__cidr_mask),			\
		.version_mask = (__version_mask),		\
		.exid_match = (__exid),				\
		.name = (__name),				\
		.family = (__family),				\
	}

struct soc_device * __init
at91_soc_init(const struct at91_soc *socs);

#define AT91RM9200_CIDR_MATCH		0x09290780

#define AT91SAM9260_CIDR_MATCH		0x019803a0
#define AT91SAM9261_CIDR_MATCH		0x019703a0
#define AT91SAM9263_CIDR_MATCH		0x019607a0
#define AT91SAM9G20_CIDR_MATCH		0x019905a0
#define AT91SAM9RL64_CIDR_MATCH		0x019b03a0
#define AT91SAM9G45_CIDR_MATCH		0x019b05a0
#define AT91SAM9X5_CIDR_MATCH		0x019a05a0
#define AT91SAM9N12_CIDR_MATCH		0x019a07a0
#define SAM9X60_CIDR_MATCH		0x019b35a0
#define SAMA7G5_CIDR_MATCH		0x00162100

#define AT91SAM9M11_EXID_MATCH		0x00000001
#define AT91SAM9M10_EXID_MATCH		0x00000002
#define AT91SAM9G46_EXID_MATCH		0x00000003
#define AT91SAM9G45_EXID_MATCH		0x00000004

#define AT91SAM9G15_EXID_MATCH		0x00000000
#define AT91SAM9G35_EXID_MATCH		0x00000001
#define AT91SAM9X35_EXID_MATCH		0x00000002
#define AT91SAM9G25_EXID_MATCH		0x00000003
#define AT91SAM9X25_EXID_MATCH		0x00000004

#define AT91SAM9CN12_EXID_MATCH		0x00000005
#define AT91SAM9N12_EXID_MATCH		0x00000006
#define AT91SAM9CN11_EXID_MATCH		0x00000009

#define SAM9X60_EXID_MATCH		0x00000000
#define SAM9X60_D5M_EXID_MATCH		0x00000001
#define SAM9X60_D1G_EXID_MATCH		0x00000010
#define SAM9X60_D6K_EXID_MATCH		0x00000011

#define SAMA7G51_EXID_MATCH		0x3
#define SAMA7G52_EXID_MATCH		0x2
#define SAMA7G53_EXID_MATCH		0x1
#define SAMA7G54_EXID_MATCH		0x0

#define AT91SAM9XE128_CIDR_MATCH	0x329973a0
#define AT91SAM9XE256_CIDR_MATCH	0x329a93a0
#define AT91SAM9XE512_CIDR_MATCH	0x329aa3a0

#define SAMA5D2_CIDR_MATCH		0x0a5c08c0
#define SAMA5D21CU_EXID_MATCH		0x0000005a
#define SAMA5D225C_D1M_EXID_MATCH	0x00000053
#define SAMA5D22CU_EXID_MATCH		0x00000059
#define SAMA5D22CN_EXID_MATCH		0x00000069
#define SAMA5D23CU_EXID_MATCH		0x00000058
#define SAMA5D24CX_EXID_MATCH		0x00000004
#define SAMA5D24CU_EXID_MATCH		0x00000014
#define SAMA5D26CU_EXID_MATCH		0x00000012
#define SAMA5D27C_D1G_EXID_MATCH	0x00000033
#define SAMA5D27C_D5M_EXID_MATCH	0x00000032
#define SAMA5D27C_LD1G_EXID_MATCH	0x00000061
#define SAMA5D27C_LD2G_EXID_MATCH	0x00000062
#define SAMA5D27CU_EXID_MATCH		0x00000011
#define SAMA5D27CN_EXID_MATCH		0x00000021
#define SAMA5D28C_D1G_EXID_MATCH	0x00000013
#define SAMA5D28C_LD1G_EXID_MATCH	0x00000071
#define SAMA5D28C_LD2G_EXID_MATCH	0x00000072
#define SAMA5D28CU_EXID_MATCH		0x00000010
#define SAMA5D28CN_EXID_MATCH		0x00000020

#define SAMA5D3_CIDR_MATCH		0x0a5c07c0
#define SAMA5D31_EXID_MATCH		0x00444300
#define SAMA5D33_EXID_MATCH		0x00414300
#define SAMA5D34_EXID_MATCH		0x00414301
#define SAMA5D35_EXID_MATCH		0x00584300
#define SAMA5D36_EXID_MATCH		0x00004301

#define SAMA5D4_CIDR_MATCH		0x0a5c07c0
#define SAMA5D41_EXID_MATCH		0x00000001
#define SAMA5D42_EXID_MATCH		0x00000002
#define SAMA5D43_EXID_MATCH		0x00000003
#define SAMA5D44_EXID_MATCH		0x00000004

#define SAME70Q21_CIDR_MATCH		0x21020e00
#define SAME70Q21_EXID_MATCH		0x00000002
#define SAME70Q20_CIDR_MATCH		0x21020c00
#define SAME70Q20_EXID_MATCH		0x00000002
#define SAME70Q19_CIDR_MATCH		0x210d0a00
#define SAME70Q19_EXID_MATCH		0x00000002

#define SAMS70Q21_CIDR_MATCH		0x21120e00
#define SAMS70Q21_EXID_MATCH		0x00000002
#define SAMS70Q20_CIDR_MATCH		0x21120c00
#define SAMS70Q20_EXID_MATCH		0x00000002
#define SAMS70Q19_CIDR_MATCH		0x211d0a00
#define SAMS70Q19_EXID_MATCH		0x00000002

#define SAMV71Q21_CIDR_MATCH		0x21220e00
#define SAMV71Q21_EXID_MATCH		0x00000002
#define SAMV71Q20_CIDR_MATCH		0x21220c00
#define SAMV71Q20_EXID_MATCH		0x00000002
#define SAMV71Q19_CIDR_MATCH		0x212d0a00
#define SAMV71Q19_EXID_MATCH		0x00000002

#define SAMV70Q20_CIDR_MATCH		0x21320c00
#define SAMV70Q20_EXID_MATCH		0x00000002
#define SAMV70Q19_CIDR_MATCH		0x213d0a00
#define SAMV70Q19_EXID_MATCH		0x00000002

#endif /* __AT91_SOC_H */
