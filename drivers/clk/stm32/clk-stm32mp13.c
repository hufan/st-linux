// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) STMicroelectronics 2020 - All Rights Reserved
 * Author: Gabriel Fernandez <gabriel.fernandez@st.com> for STMicroelectronics.
 */

#include <linux/clk.h>
#include <linux/module.h>
#include <linux/of_address.h>
#include <linux/platform_device.h>

#include <dt-bindings/clock/stm32mp13-clks.h>

#include "clk-stm32-core.h"
#include "stm32mp13_rcc.h"

static void __iomem *rcc_base;

static const char * const adc12_src[] = {
	"pll4_r", "ck_per", "pll3_q"
};

static const char * const dcmipp_src[] = {
	"ck_axi", "pll2_q", "pll4_p", "ck_per",
};

static const char * const eth12_src[] = {
	"pll4_p", "pll3_q"
};

static const char * const fdcan_src[] = {
	"ck_hse", "pll3_q", "pll4_q", "pll4_r"
};

static const char * const fmc_src[] = {
	"ck_axi", "pll3_r", "pll4_p", "ck_per"
};

static const char * const i2c12_src[] = {
	"pclk1", "pll4_r", "ck_hsi", "ck_csi"
};

static const char * const i2c345_src[] = {
	"pclk6", "pll4_r", "ck_hsi", "ck_csi"
};

static const char * const lptim1_src[] = {
	"pclk1", "pll4_p", "pll3_q", "ck_lse", "ck_lsi", "ck_per"
};

static const char * const lptim23_src[] = {
	"pclk3", "pll4_q", "ck_per", "ck_lse", "ck_lsi"
};

static const char * const lptim45_src[] = {
	"pclk3", "pll4_p", "pll3_q", "ck_lse", "ck_lsi", "ck_per"
};

static const char * const mco1_src[] = {
	"ck_hsi", "ck_hse", "ck_csi", "ck_lsi", "ck_lse"
};

static const char * const mco2_src[] = {
	"ck_mpu", "ck_axi", "ck_mlahb", "pll4_p", "ck_hse", "ck_hsi"
};

static const char * const qspi_src[] = {
	"ck_axi", "pll3_r", "pll4_p", "ck_per"
};

static const char * const rng1_src[] = {
	"ck_csi", "pll4_r", "ck_lse", "ck_lsi"
};

static const char * const saes_src[] = {
	"ck_axi", "ck_per", "pll4_r", "ck_lsi"
};

static const char * const sai1_src[] = {
	"pll4_q", "pll3_q", "i2s_ckin", "ck_per", "pll3_r"
};

static const char * const sai2_src[] = {
	"pll4_q", "pll3_q", "i2s_ckin", "ck_per", "spdif_ck_symb", "pll3_r"
};

static const char * const sdmmc12_src[] = {
	"ck_axi", "pll3_r", "pll4_p", "ck_hsi"
};

static const char * const spdif_src[] = {
	"pll4_p", "pll3_q", "ck_hsi"
};

static const char * const spi123_src[] = {
	"pll4_p", "pll3_q", "i2s_ckin", "ck_per", "pll3_r"
};

static const char * const spi4_src[] = {
	"pclk6", "pll4_q", "ck_hsi", "ck_csi", "ck_hse", "i2s_ckin"
};

static const char * const spi5_src[] = {
	"pclk6", "pll4_q", "ck_hsi", "ck_csi", "ck_hse"
};

static const char * const stgen_src[] = {
	"ck_hsi", "ck_hse"
};

static const char * const usart12_src[] = {
	"pclk6", "pll3_q", "ck_hsi", "ck_csi", "pll4_q", "ck_hse"
};

static const char * const usart34578_src[] = {
	"pclk1", "pll4_q", "ck_hsi", "ck_csi", "ck_hse"
};

static const char * const usart6_src[] = {
	"pclk2", "pll4_q", "ck_hsi", "ck_csi", "ck_hse"
};

static const char * const usbo_src[] = {
	"pll4_r", "ck_usbo_48m"
};

static const char * const usbphy_src[] = {
	"ck_hse", "pll4_r", "clk-hse-div2"
};

enum enum_mux_cfg {
	MUX_I2C12,
	MUX_LPTIM45,
	MUX_SPI23,
	MUX_UART35,
	MUX_UART78,
	MUX_ADC1,
	MUX_ADC2,
	MUX_DCMIPP,
	MUX_ETH1,
	MUX_ETH2,
	MUX_FDCAN,
	MUX_FMC,
	MUX_I2C3,
	MUX_I2C4,
	MUX_I2C5,
	MUX_LPTIM1,
	MUX_LPTIM2,
	MUX_LPTIM3,
	MUX_QSPI,
	MUX_RNG1,
	MUX_SAES,
	MUX_SAI1,
	MUX_SAI2,
	MUX_SDMMC1,
	MUX_SDMMC2,
	MUX_SPDIF,
	MUX_SPI1,
	MUX_SPI4,
	MUX_SPI5,
	MUX_STGEN,
	MUX_UART1,
	MUX_UART2,
	MUX_UART4,
	MUX_UART6,
	MUX_USBO,
	MUX_USBPHY,
	MUX_MCO1,
	MUX_MCO2
};

#define MUX_CFG(id, src, _offset, _shift, _witdh)[id] = {\
	.num_parents	= ARRAY_SIZE(src),\
	.parent_names	= src,\
	.reg_off	= (_offset),\
	.shift		= (_shift),\
	.width		= (_witdh),\
}

static const struct stm32_mux_cfg stm32mp13_muxes[] = {
	MUX_CFG(MUX_I2C12,	i2c12_src,	RCC_I2C12CKSELR, 0, 3),
	MUX_CFG(MUX_LPTIM45,	lptim45_src,	RCC_LPTIM45CKSELR, 0, 3),
	MUX_CFG(MUX_SPI23,	spi123_src,	RCC_SPI2S23CKSELR, 0, 3),
	MUX_CFG(MUX_UART35,	usart34578_src,	RCC_UART35CKSELR, 0, 3),
	MUX_CFG(MUX_UART78,	usart34578_src,	RCC_UART78CKSELR, 0, 3),
	MUX_CFG(MUX_ADC1,	adc12_src,	RCC_ADC12CKSELR, 0, 2),
	MUX_CFG(MUX_ADC2,	adc12_src,	RCC_ADC12CKSELR, 2, 2),
	MUX_CFG(MUX_DCMIPP,	dcmipp_src,	RCC_DCMIPPCKSELR, 0, 2),
	MUX_CFG(MUX_ETH1,	eth12_src,	RCC_ETH12CKSELR, 0, 2),
	MUX_CFG(MUX_ETH2,	eth12_src,	RCC_ETH12CKSELR, 8, 2),
	MUX_CFG(MUX_FDCAN,	fdcan_src,	RCC_FDCANCKSELR, 0, 2),
	MUX_CFG(MUX_FMC,	fmc_src,	RCC_FMCCKSELR, 0, 2),
	MUX_CFG(MUX_I2C3,	i2c345_src,	RCC_I2C345CKSELR, 0, 3),
	MUX_CFG(MUX_I2C4,	i2c345_src,	RCC_I2C345CKSELR, 3, 3),
	MUX_CFG(MUX_I2C5,	i2c345_src,	RCC_I2C345CKSELR, 6, 3),
	MUX_CFG(MUX_LPTIM1,	lptim1_src,	RCC_LPTIM1CKSELR, 0, 3),
	MUX_CFG(MUX_LPTIM2,	lptim23_src,	RCC_LPTIM23CKSELR, 0, 3),
	MUX_CFG(MUX_LPTIM3,	lptim23_src,	RCC_LPTIM23CKSELR, 3, 3),
	MUX_CFG(MUX_MCO1,	mco1_src,	RCC_MCO1CFGR, 0, 3),
	MUX_CFG(MUX_MCO2,	mco2_src,	RCC_MCO2CFGR, 0, 3),
	MUX_CFG(MUX_QSPI,	qspi_src,	RCC_QSPICKSELR, 0, 2),
	MUX_CFG(MUX_RNG1,	rng1_src,	RCC_RNG1CKSELR, 0, 2),
	MUX_CFG(MUX_SAES,	saes_src,	RCC_SAESCKSELR, 0, 2),
	MUX_CFG(MUX_SAI1,	sai1_src,	RCC_SAI1CKSELR, 0, 3),
	MUX_CFG(MUX_SAI2,	sai2_src,	RCC_SAI2CKSELR, 0, 3),
	MUX_CFG(MUX_SDMMC1,	sdmmc12_src,	RCC_SDMMC12CKSELR, 0, 3),
	MUX_CFG(MUX_SDMMC2,	sdmmc12_src,	RCC_SDMMC12CKSELR, 3, 3),
	MUX_CFG(MUX_SPDIF,	spdif_src,	RCC_SPDIFCKSELR, 0, 2),
	MUX_CFG(MUX_SPI1,	spi123_src,	RCC_SPI2S1CKSELR, 0, 3),
	MUX_CFG(MUX_SPI4,	spi4_src,	RCC_SPI45CKSELR, 0, 3),
	MUX_CFG(MUX_SPI5,	spi5_src,	RCC_SPI45CKSELR, 3, 3),
	MUX_CFG(MUX_STGEN,	stgen_src,	RCC_STGENCKSELR, 0, 2),
	MUX_CFG(MUX_UART1,	usart12_src,	RCC_UART12CKSELR, 0, 3),
	MUX_CFG(MUX_UART2,	usart12_src,	RCC_UART12CKSELR, 3, 3),
	MUX_CFG(MUX_UART4,	usart34578_src,	RCC_UART4CKSELR, 0, 3),
	MUX_CFG(MUX_UART6,	usart6_src,	RCC_UART6CKSELR, 0, 3),
	MUX_CFG(MUX_USBO,	usbo_src,	RCC_USBCKSELR, 4, 1),
	MUX_CFG(MUX_USBPHY,	usbphy_src,	RCC_USBCKSELR, 0, 2),
};

enum enum_gate_cfg {
	GATE_ZERO, /* reserved for no gate */
	GATE_MCO1,
	GATE_MCO2,
	GATE_DBGCK,
	GATE_TRACECK,
	GATE_DDRC1,
	GATE_DDRC1LP,
	GATE_DDRPHYC,
	GATE_DDRPHYCLP,
	GATE_DDRCAPB,
	GATE_DDRCAPBLP,
	GATE_AXIDCG,
	GATE_DDRPHYCAPB,
	GATE_DDRPHYCAPBLP,
	GATE_TIM2,
	GATE_TIM3,
	GATE_TIM4,
	GATE_TIM5,
	GATE_TIM6,
	GATE_TIM7,
	GATE_LPTIM1,
	GATE_SPI2,
	GATE_SPI3,
	GATE_USART3,
	GATE_UART4,
	GATE_UART5,
	GATE_UART7,
	GATE_UART8,
	GATE_I2C1,
	GATE_I2C2,
	GATE_SPDIF,
	GATE_TIM1,
	GATE_TIM8,
	GATE_SPI1,
	GATE_USART6,
	GATE_SAI1,
	GATE_SAI2,
	GATE_DFSDM,
	GATE_ADFSDM,
	GATE_FDCAN,
	GATE_LPTIM2,
	GATE_LPTIM3,
	GATE_LPTIM4,
	GATE_LPTIM5,
	GATE_VREF,
	GATE_DTS,
	GATE_PMBCTRL,
	GATE_HDP,
	GATE_SYSCFG,
	GATE_DCMIPP,
	GATE_DDRPERFM,
	GATE_IWDG2APB,
	GATE_USBPHY,
	GATE_STGENRO,
	GATE_LTDC,
	GATE_TZC,
	GATE_ETZPC,
	GATE_IWDG1APB,
	GATE_BSEC,
	GATE_STGENC,
	GATE_USART1,
	GATE_USART2,
	GATE_SPI4,
	GATE_SPI5,
	GATE_I2C3,
	GATE_I2C4,
	GATE_I2C5,
	GATE_TIM12,
	GATE_TIM13,
	GATE_TIM14,
	GATE_TIM15,
	GATE_TIM16,
	GATE_TIM17,
	GATE_DMA1,
	GATE_DMA2,
	GATE_DMAMUX1,
	GATE_DMA3,
	GATE_DMAMUX2,
	GATE_ADC1,
	GATE_ADC2,
	GATE_USBO,
	GATE_TSC,
	GATE_GPIOA,
	GATE_GPIOB,
	GATE_GPIOC,
	GATE_GPIOD,
	GATE_GPIOE,
	GATE_GPIOF,
	GATE_GPIOG,
	GATE_GPIOH,
	GATE_GPIOI,
	GATE_PKA,
	GATE_SAES,
	GATE_CRYP1,
	GATE_HASH1,
	GATE_RNG1,
	GATE_BKPSRAM,
	GATE_AXIMC,
	GATE_MCE,
	GATE_ETH1CK,
	GATE_ETH1TX,
	GATE_ETH1RX,
	GATE_ETH1MAC,
	GATE_FMC,
	GATE_QSPI,
	GATE_SDMMC1,
	GATE_SDMMC2,
	GATE_CRC1,
	GATE_USBH,
	GATE_ETH2CK,
	GATE_ETH2TX,
	GATE_ETH2RX,
	GATE_ETH2MAC,
	GATE_ETH1STP,
	GATE_ETH2STP,
	GATE_MDMA
};

#define GATE_CFG(id, _offset, _bit_idx, _offset_clr)[id] = {\
	.reg_off	= (_offset),\
	.bit_idx	= (_bit_idx),\
	.set_clr	= (_offset_clr),\
}

static const struct stm32_gate_cfg stm32mp13_gates[] = {
	GATE_CFG(GATE_MCO1,		RCC_MCO1CFGR,	12,	0),
	GATE_CFG(GATE_MCO2,		RCC_MCO2CFGR,	12,	0),
	GATE_CFG(GATE_DBGCK,		RCC_DBGCFGR,	8,	0),
	GATE_CFG(GATE_TRACECK,		RCC_DBGCFGR,	9,	0),
	GATE_CFG(GATE_DDRC1,		RCC_DDRITFCR,	0,	0),
	GATE_CFG(GATE_DDRC1LP,		RCC_DDRITFCR,	1,	0),
	GATE_CFG(GATE_DDRPHYC,		RCC_DDRITFCR,	4,	0),
	GATE_CFG(GATE_DDRPHYCLP,	RCC_DDRITFCR,	5,	0),
	GATE_CFG(GATE_DDRCAPB,		RCC_DDRITFCR,	6,	0),
	GATE_CFG(GATE_DDRCAPBLP,	RCC_DDRITFCR,	7,	0),
	GATE_CFG(GATE_AXIDCG,		RCC_DDRITFCR,	8,	0),
	GATE_CFG(GATE_DDRPHYCAPB,	RCC_DDRITFCR,	9,	0),
	GATE_CFG(GATE_DDRPHYCAPBLP,	RCC_DDRITFCR,	10,	0),
	GATE_CFG(GATE_TIM2,		RCC_MP_APB1ENSETR,	0,	1),
	GATE_CFG(GATE_TIM3,		RCC_MP_APB1ENSETR,	1,	1),
	GATE_CFG(GATE_TIM4,		RCC_MP_APB1ENSETR,	2,	1),
	GATE_CFG(GATE_TIM5,		RCC_MP_APB1ENSETR,	3,	1),
	GATE_CFG(GATE_TIM6,		RCC_MP_APB1ENSETR,	4,	1),
	GATE_CFG(GATE_TIM7,		RCC_MP_APB1ENSETR,	5,	1),
	GATE_CFG(GATE_LPTIM1,		RCC_MP_APB1ENSETR,	9,	1),
	GATE_CFG(GATE_SPI2,		RCC_MP_APB1ENSETR,	11,	1),
	GATE_CFG(GATE_SPI3,		RCC_MP_APB1ENSETR,	12,	1),
	GATE_CFG(GATE_USART3,		RCC_MP_APB1ENSETR,	15,	1),
	GATE_CFG(GATE_UART4,		RCC_MP_APB1ENSETR,	16,	1),
	GATE_CFG(GATE_UART5,		RCC_MP_APB1ENSETR,	17,	1),
	GATE_CFG(GATE_UART7,		RCC_MP_APB1ENSETR,	18,	1),
	GATE_CFG(GATE_UART8,		RCC_MP_APB1ENSETR,	19,	1),
	GATE_CFG(GATE_I2C1,		RCC_MP_APB1ENSETR,	21,	1),
	GATE_CFG(GATE_I2C2,		RCC_MP_APB1ENSETR,	22,	1),
	GATE_CFG(GATE_SPDIF,		RCC_MP_APB1ENSETR,	26,	1),
	GATE_CFG(GATE_TIM1,		RCC_MP_APB2ENSETR,	0,	1),
	GATE_CFG(GATE_TIM8,		RCC_MP_APB2ENSETR,	1,	1),
	GATE_CFG(GATE_SPI1,		RCC_MP_APB2ENSETR,	8,	1),
	GATE_CFG(GATE_USART6,		RCC_MP_APB2ENSETR,	13,	1),
	GATE_CFG(GATE_SAI1,		RCC_MP_APB2ENSETR,	16,	1),
	GATE_CFG(GATE_SAI2,		RCC_MP_APB2ENSETR,	17,	1),
	GATE_CFG(GATE_DFSDM,		RCC_MP_APB2ENSETR,	20,	1),
	GATE_CFG(GATE_ADFSDM,		RCC_MP_APB2ENSETR,	21,	1),
	GATE_CFG(GATE_FDCAN,		RCC_MP_APB2ENSETR,	24,	1),
	GATE_CFG(GATE_LPTIM2,		RCC_MP_APB3ENSETR,	0,	1),
	GATE_CFG(GATE_LPTIM3,		RCC_MP_APB3ENSETR,	1,	1),
	GATE_CFG(GATE_LPTIM4,		RCC_MP_APB3ENSETR,	2,	1),
	GATE_CFG(GATE_LPTIM5,		RCC_MP_APB3ENSETR,	3,	1),
	GATE_CFG(GATE_VREF,		RCC_MP_APB3ENSETR,	13,	1),
	GATE_CFG(GATE_DTS,		RCC_MP_APB3ENSETR,	16,	1),
	GATE_CFG(GATE_PMBCTRL,		RCC_MP_APB3ENSETR,	17,	1),
	GATE_CFG(GATE_HDP,		RCC_MP_APB3ENSETR,	20,	1),
	GATE_CFG(GATE_SYSCFG,		RCC_MP_NS_APB3ENSETR,	0,	1),
	GATE_CFG(GATE_DCMIPP,		RCC_MP_APB4ENSETR,	1,	1),
	GATE_CFG(GATE_DDRPERFM,		RCC_MP_APB4ENSETR,	8,	1),
	GATE_CFG(GATE_IWDG2APB,		RCC_MP_APB4ENSETR,	15,	1),
	GATE_CFG(GATE_USBPHY,		RCC_MP_APB4ENSETR,	16,	1),
	GATE_CFG(GATE_STGENRO,		RCC_MP_APB4ENSETR,	20,	1),
	GATE_CFG(GATE_LTDC,		RCC_MP_NS_APB4ENSETR,	0,	1),
	GATE_CFG(GATE_TZC,		RCC_MP_APB5ENSETR,	11,	1),
	GATE_CFG(GATE_ETZPC,		RCC_MP_APB5ENSETR,	13,	1),
	GATE_CFG(GATE_IWDG1APB,		RCC_MP_APB5ENSETR,	15,	1),
	GATE_CFG(GATE_BSEC,		RCC_MP_APB5ENSETR,	16,	1),
	GATE_CFG(GATE_STGENC,		RCC_MP_APB5ENSETR,	20,	1),
	GATE_CFG(GATE_USART1,		RCC_MP_APB6ENSETR,	0,	1),
	GATE_CFG(GATE_USART2,		RCC_MP_APB6ENSETR,	1,	1),
	GATE_CFG(GATE_SPI4,		RCC_MP_APB6ENSETR,	2,	1),
	GATE_CFG(GATE_SPI5,		RCC_MP_APB6ENSETR,	3,	1),
	GATE_CFG(GATE_I2C3,		RCC_MP_APB6ENSETR,	4,	1),
	GATE_CFG(GATE_I2C4,		RCC_MP_APB6ENSETR,	5,	1),
	GATE_CFG(GATE_I2C5,		RCC_MP_APB6ENSETR,	6,	1),
	GATE_CFG(GATE_TIM12,		RCC_MP_APB6ENSETR,	7,	1),
	GATE_CFG(GATE_TIM13,		RCC_MP_APB6ENSETR,	8,	1),
	GATE_CFG(GATE_TIM14,		RCC_MP_APB6ENSETR,	9,	1),
	GATE_CFG(GATE_TIM15,		RCC_MP_APB6ENSETR,	10,	1),
	GATE_CFG(GATE_TIM16,		RCC_MP_APB6ENSETR,	11,	1),
	GATE_CFG(GATE_TIM17,		RCC_MP_APB6ENSETR,	12,	1),
	GATE_CFG(GATE_DMA1,		RCC_MP_AHB2ENSETR,	0,	1),
	GATE_CFG(GATE_DMA2,		RCC_MP_AHB2ENSETR,	1,	1),
	GATE_CFG(GATE_DMAMUX1,		RCC_MP_AHB2ENSETR,	2,	1),
	GATE_CFG(GATE_DMA3,		RCC_MP_AHB2ENSETR,	3,	1),
	GATE_CFG(GATE_DMAMUX2,		RCC_MP_AHB2ENSETR,	4,	1),
	GATE_CFG(GATE_ADC1,		RCC_MP_AHB2ENSETR,	5,	1),
	GATE_CFG(GATE_ADC2,		RCC_MP_AHB2ENSETR,	6,	1),
	GATE_CFG(GATE_USBO,		RCC_MP_AHB2ENSETR,	8,	1),
	GATE_CFG(GATE_TSC,		RCC_MP_AHB4ENSETR,	15,	1),
	GATE_CFG(GATE_GPIOA,		RCC_MP_NS_AHB4ENSETR,	0,	1),
	GATE_CFG(GATE_GPIOB,		RCC_MP_NS_AHB4ENSETR,	1,	1),
	GATE_CFG(GATE_GPIOC,		RCC_MP_NS_AHB4ENSETR,	2,	1),
	GATE_CFG(GATE_GPIOD,		RCC_MP_NS_AHB4ENSETR,	3,	1),
	GATE_CFG(GATE_GPIOE,		RCC_MP_NS_AHB4ENSETR,	4,	1),
	GATE_CFG(GATE_GPIOF,		RCC_MP_NS_AHB4ENSETR,	5,	1),
	GATE_CFG(GATE_GPIOG,		RCC_MP_NS_AHB4ENSETR,	6,	1),
	GATE_CFG(GATE_GPIOH,		RCC_MP_NS_AHB4ENSETR,	7,	1),
	GATE_CFG(GATE_GPIOI,		RCC_MP_NS_AHB4ENSETR,	8,	1),
	GATE_CFG(GATE_PKA,		RCC_MP_AHB5ENSETR,	2,	1),
	GATE_CFG(GATE_SAES,		RCC_MP_AHB5ENSETR,	3,	1),
	GATE_CFG(GATE_CRYP1,		RCC_MP_AHB5ENSETR,	4,	1),
	GATE_CFG(GATE_HASH1,		RCC_MP_AHB5ENSETR,	5,	1),
	GATE_CFG(GATE_RNG1,		RCC_MP_AHB5ENSETR,	6,	1),
	GATE_CFG(GATE_BKPSRAM,		RCC_MP_AHB5ENSETR,	8,	1),
	GATE_CFG(GATE_AXIMC,		RCC_MP_AHB5ENSETR,	16,	1),
	GATE_CFG(GATE_MCE,		RCC_MP_AHB6ENSETR,	1,	1),
	GATE_CFG(GATE_ETH1CK,		RCC_MP_AHB6ENSETR,	7,	1),
	GATE_CFG(GATE_ETH1TX,		RCC_MP_AHB6ENSETR,	8,	1),
	GATE_CFG(GATE_ETH1RX,		RCC_MP_AHB6ENSETR,	9,	1),
	GATE_CFG(GATE_ETH1MAC,		RCC_MP_AHB6ENSETR,	10,	1),
	GATE_CFG(GATE_FMC,		RCC_MP_AHB6ENSETR,	12,	1),
	GATE_CFG(GATE_QSPI,		RCC_MP_AHB6ENSETR,	14,	1),
	GATE_CFG(GATE_SDMMC1,		RCC_MP_AHB6ENSETR,	16,	1),
	GATE_CFG(GATE_SDMMC2,		RCC_MP_AHB6ENSETR,	17,	1),
	GATE_CFG(GATE_CRC1,		RCC_MP_AHB6ENSETR,	20,	1),
	GATE_CFG(GATE_USBH,		RCC_MP_AHB6ENSETR,	24,	1),
	GATE_CFG(GATE_ETH2CK,		RCC_MP_AHB6ENSETR,	27,	1),
	GATE_CFG(GATE_ETH2TX,		RCC_MP_AHB6ENSETR,	28,	1),
	GATE_CFG(GATE_ETH2RX,		RCC_MP_AHB6ENSETR,	29,	1),
	GATE_CFG(GATE_ETH2MAC,		RCC_MP_AHB6ENSETR,	30,	1),
	GATE_CFG(GATE_ETH1STP,		RCC_MP_AHB6LPENSETR,	11,	1),
	GATE_CFG(GATE_ETH2STP,		RCC_MP_AHB6LPENSETR,	31,	1),
	GATE_CFG(GATE_MDMA,		RCC_MP_NS_AHB6ENSETR,	0,	1),
};

static const struct clk_div_table ck_trace_div_table[] = {
	{ 0, 1 }, { 1, 2 }, { 2, 4 }, { 3, 8 },
	{ 4, 16 }, { 5, 16 }, { 6, 16 }, { 7, 16 },
	{ 0 },
};

enum enum_div_cfg {
	DIV_MCO1,
	DIV_MCO2,
	DIV_TRACE,
	DIV_ETH1PTP,
	DIV_ETH2PTP,
	LAST_DIV
};

#define DIV_CFG(id, _offset, _shift, _width, _flags, _table)[id] = {\
		.reg_off	= _offset,\
		.shift	= _shift,\
		.width	= _width,\
		.div_flags	= _flags,\
		.table	= _table,\
}

static const struct stm32_div_cfg stm32mp13_dividers[LAST_DIV] = {
	DIV_CFG(DIV_MCO1, RCC_MCO1CFGR, 4, 4, 0, NULL),
	DIV_CFG(DIV_MCO2, RCC_MCO2CFGR, 4, 4, 0, NULL),
	DIV_CFG(DIV_TRACE, RCC_DBGCFGR, 0, 3, 0, ck_trace_div_table),
	DIV_CFG(DIV_ETH1PTP, RCC_ETH12CKSELR, 4, 4, 0, NULL),
	DIV_CFG(DIV_ETH2PTP, RCC_ETH12CKSELR, 12, 4, 0, NULL),
};

struct clk_stm32_securiy {
	u16	offset;
	u8	bit_idx;
};

enum securit_clk {
	SECF_NONE,
	SECF_LPTIM2,
	SECF_LPTIM3,
	SECF_VREF,
	SECF_DCMIPP,
	SECF_USBPHY,
	SECF_RTC,
	SECF_TZC,
	SECF_ETZPC,
	SECF_IWDG1,
	SECF_BSEC,
	SECF_STGENC,
	SECF_STGENRO,
	SECF_USART1,
	SECF_USART2,
	SECF_SPI4,
	SECF_SPI5,
	SECF_I2C3,
	SECF_I2C4,
	SECF_I2C5,
	SECF_TIM12,
	SECF_TIM13,
	SECF_TIM14,
	SECF_TIM15,
	SECF_TIM16,
	SECF_TIM17,
	SECF_DMA3,
	SECF_DMAMUX2,
	SECF_ADC1,
	SECF_ADC2,
	SECF_USBO,
	SECF_TSC,
	SECF_PKA,
	SECF_SAES,
	SECF_CRYP1,
	SECF_HASH1,
	SECF_RNG1,
	SECF_BKPSRAM,
	SECF_MCE,
	SECF_FMC,
	SECF_QSPI,
	SECF_SDMMC1,
	SECF_SDMMC2,
	SECF_ETH1CK,
	SECF_ETH1TX,
	SECF_ETH1RX,
	SECF_ETH1MAC,
	SECF_ETH1STP,
	SECF_ETH2CK,
	SECF_ETH2TX,
	SECF_ETH2RX,
	SECF_ETH2MAC,
	SECF_ETH2STP,
	SECF_MCO1,
	SECF_MCO2
};

#define SECF(_sec_id, _offset, _bit_idx)[_sec_id] = {\
	.offset	= _offset,\
	.bit_idx	= _bit_idx,\
}

static const struct clk_stm32_securiy stm32mp13_security[] = {
	SECF(SECF_LPTIM2, RCC_APB3SECSR, RCC_APB3SECSR_LPTIM2SECF),
	SECF(SECF_LPTIM3, RCC_APB3SECSR, RCC_APB3SECSR_LPTIM3SECF),
	SECF(SECF_VREF, RCC_APB3SECSR, RCC_APB3SECSR_VREFSECF),
	SECF(SECF_DCMIPP, RCC_APB4SECSR, RCC_APB4SECSR_DCMIPPSECF),
	SECF(SECF_USBPHY, RCC_APB4SECSR, RCC_APB4SECSR_USBPHYSECF),
	SECF(SECF_RTC, RCC_APB5SECSR, RCC_APB5SECSR_RTCSECF),
	SECF(SECF_TZC, RCC_APB5SECSR, RCC_APB5SECSR_TZCSECF),
	SECF(SECF_ETZPC, RCC_APB5SECSR, RCC_APB5SECSR_ETZPCSECF),
	SECF(SECF_IWDG1, RCC_APB5SECSR, RCC_APB5SECSR_IWDG1SECF),
	SECF(SECF_BSEC, RCC_APB5SECSR, RCC_APB5SECSR_BSECSECF),
	SECF(SECF_STGENC, RCC_APB5SECSR, RCC_APB5SECSR_STGENCSECF),
	SECF(SECF_STGENRO, RCC_APB5SECSR, RCC_APB5SECSR_STGENROSECF),
	SECF(SECF_USART1, RCC_APB6SECSR, RCC_APB6SECSR_USART1SECF),
	SECF(SECF_USART2, RCC_APB6SECSR, RCC_APB6SECSR_USART2SECF),
	SECF(SECF_SPI4, RCC_APB6SECSR, RCC_APB6SECSR_SPI4SECF),
	SECF(SECF_SPI5, RCC_APB6SECSR, RCC_APB6SECSR_SPI5SECF),
	SECF(SECF_I2C3, RCC_APB6SECSR, RCC_APB6SECSR_I2C3SECF),
	SECF(SECF_I2C4, RCC_APB6SECSR, RCC_APB6SECSR_I2C4SECF),
	SECF(SECF_I2C5, RCC_APB6SECSR, RCC_APB6SECSR_I2C5SECF),
	SECF(SECF_TIM12, RCC_APB6SECSR, RCC_APB6SECSR_TIM12SECF),
	SECF(SECF_TIM13, RCC_APB6SECSR, RCC_APB6SECSR_TIM13SECF),
	SECF(SECF_TIM14, RCC_APB6SECSR, RCC_APB6SECSR_TIM14SECF),
	SECF(SECF_TIM15, RCC_APB6SECSR, RCC_APB6SECSR_TIM15SECF),
	SECF(SECF_TIM16, RCC_APB6SECSR, RCC_APB6SECSR_TIM16SECF),
	SECF(SECF_TIM17, RCC_APB6SECSR, RCC_APB6SECSR_TIM17SECF),
	SECF(SECF_DMA3, RCC_AHB2SECSR, RCC_AHB2SECSR_DMA3SECF),
	SECF(SECF_DMAMUX2, RCC_AHB2SECSR, RCC_AHB2SECSR_DMAMUX2SECF),
	SECF(SECF_ADC1, RCC_AHB2SECSR, RCC_AHB2SECSR_ADC1SECF),
	SECF(SECF_ADC2, RCC_AHB2SECSR, RCC_AHB2SECSR_ADC2SECF),
	SECF(SECF_USBO, RCC_AHB2SECSR, RCC_AHB2SECSR_USBOSECF),
	SECF(SECF_TSC, RCC_AHB4SECSR, RCC_AHB4SECSR_TSCSECF),
	SECF(SECF_PKA, RCC_AHB5SECSR, RCC_AHB5SECSR_PKASECF),
	SECF(SECF_SAES, RCC_AHB5SECSR, RCC_AHB5SECSR_SAESSECF),
	SECF(SECF_CRYP1, RCC_AHB5SECSR, RCC_AHB5SECSR_CRYP1SECF),
	SECF(SECF_HASH1, RCC_AHB5SECSR, RCC_AHB5SECSR_HASH1SECF),
	SECF(SECF_RNG1, RCC_AHB5SECSR, RCC_AHB5SECSR_RNG1SECF),
	SECF(SECF_BKPSRAM, RCC_AHB5SECSR, RCC_AHB5SECSR_BKPSRAMSECF),
	SECF(SECF_MCE, RCC_AHB6SECSR, RCC_AHB6SECSR_MCESECF),
	SECF(SECF_FMC, RCC_AHB6SECSR, RCC_AHB6SECSR_FMCSECF),
	SECF(SECF_QSPI, RCC_AHB6SECSR, RCC_AHB6SECSR_QSPISECF),
	SECF(SECF_SDMMC1, RCC_AHB6SECSR, RCC_AHB6SECSR_SDMMC1SECF),
	SECF(SECF_SDMMC2, RCC_AHB6SECSR, RCC_AHB6SECSR_SDMMC2SECF),
	SECF(SECF_ETH1CK, RCC_AHB6SECSR, RCC_AHB6SECSR_ETH1CKSECF),
	SECF(SECF_ETH1TX, RCC_AHB6SECSR, RCC_AHB6SECSR_ETH1TXSECF),
	SECF(SECF_ETH1RX, RCC_AHB6SECSR, RCC_AHB6SECSR_ETH1RXSECF),
	SECF(SECF_ETH1MAC, RCC_AHB6SECSR, RCC_AHB6SECSR_ETH1MACSECF),
	SECF(SECF_ETH1STP, RCC_AHB6SECSR, RCC_AHB6SECSR_ETH1STPSECF),
	SECF(SECF_ETH2CK, RCC_AHB6SECSR, RCC_AHB6SECSR_ETH2CKSECF),
	SECF(SECF_ETH2TX, RCC_AHB6SECSR, RCC_AHB6SECSR_ETH2TXSECF),
	SECF(SECF_ETH2RX, RCC_AHB6SECSR, RCC_AHB6SECSR_ETH2RXSECF),
	SECF(SECF_ETH2MAC, RCC_AHB6SECSR, RCC_AHB6SECSR_ETH2MACSECF),
	SECF(SECF_ETH2STP, RCC_AHB6SECSR, RCC_AHB6SECSR_ETH2STPSECF),
	SECF(SECF_MCO1, RCC_SECCFGR, RCC_SECCFGR_MCO1SECF),
	SECF(SECF_MCO2, RCC_SECCFGR, RCC_SECCFGR_MCO2SECF),
};

#define PCLK(_id, _name, _parent, _flags, _gate_id, _sec_id)\
	STM32_GATE(_id, _name, _parent, _flags, _gate_id, _sec_id)

#define TIMER(_id, _name, _parent, _flags, _gate_id, _sec_id)\
	STM32_GATE(_id, _name, _parent, ((_flags) | CLK_SET_RATE_PARENT),\
		   _gate_id, _sec_id)

#define KCLK(_id, _name, _flags, _gate_id, _mux_id, _sec_id)\
	STM32_COMPOSITE(_id, _name, (_flags | CLK_OPS_PARENT_ENABLE |\
			CLK_SET_RATE_NO_REPARENT),\
			_sec_id, _gate_id, _mux_id, NO_STM32_DIV)

static int mp1_gate_clk_safe_enable(struct clk_hw *hw)
{
	struct clk_hw *composite_hw = __clk_get_hw(hw->clk);
	struct clk_composite *composite = to_clk_composite(composite_hw);
	struct clk_hw *mux_hw = composite->mux_hw;
	int sel = clk_hw_get_parent_index(hw);

	clk_mux_ops.set_parent(mux_hw, sel);

	mp1_gate_clk_endisable(hw, 1);

	return 0;
}

#define MUX_SAFE_POSITION 0

static void mp1_gate_clk_safe_disable(struct clk_hw *hw)
{
	struct clk_hw *composite_hw = __clk_get_hw(hw->clk);
	struct clk_composite *composite = to_clk_composite(composite_hw);
	struct clk_hw *mux_hw = composite->mux_hw;

	clk_mux_ops.set_parent(mux_hw, MUX_SAFE_POSITION);

	mp1_gate_clk_endisable(hw, 0);
}

const struct clk_ops clk_mp1_safe_gate_ops = {
	.enable		= mp1_gate_clk_safe_enable,
	.disable	= mp1_gate_clk_safe_disable,
	.is_enabled	= clk_gate_is_enabled,
};

static struct clk_hw *_clk_stm32_register_safe_composite(struct device *dev,
							 const struct stm32_rcc_match_data *data,
							 void __iomem *base, spinlock_t *lock,
							 const struct clock_config *cfg)
{
	const struct clk_ops *mux_ops, *rate_ops, *gate_ops;
	struct clk_hw *mux_hw, *rate_hw, *gate_hw;
	const struct stm32_mux_cfg *mux_cfg;
	const char *const *parent_names;
	int num_parents;


	mux_cfg = stm32_get_composite_mux_cfg(data, cfg);
	if (mux_cfg) {
		parent_names = mux_cfg->parent_names;
		num_parents = mux_cfg->num_parents;
	} else {
		parent_names = &cfg->parent_name;
		num_parents = 1;
	}

	mux_hw = stm32_get_composite_mux_hw(dev, data, base, lock, cfg);
	mux_ops = &clk_mux_ops;

	rate_hw = stm32_get_composite_rate_hw(dev, data, base, lock, cfg);
	rate_ops = &clk_divider_ops;

	gate_hw = stm32_get_composite_gate_hw(dev, data, base, lock, cfg);
	gate_ops = &clk_mp1_safe_gate_ops;

	return clk_hw_register_composite(dev, cfg->name, parent_names, num_parents,
					 mux_hw, mux_ops,
					 rate_hw, rate_ops,
					 gate_hw, gate_ops,
					 cfg->flags);
}

#define STM32_SAFE_COMPOSITE(_id, _name, _flags, _sec_id,\
			     _gate_id, _mux_id, _div_id)\
{\
	.id		= _id,\
	.name		= _name,\
	.sec_id		= _sec_id,\
	.flags		= _flags,\
	.clock_cfg	= &(struct stm32_clk_composite_cfg) {\
		.gate_id	= _gate_id,\
		.mux_id		= _mux_id,\
		.div_id		= _div_id,\
	},\
	.func		= _clk_stm32_register_safe_composite,\
}

#define KCLK_SAFE(_id, _name, _flags, _gate_id, _mux_id, _sec_id)\
	STM32_SAFE_COMPOSITE(_id, _name, (_flags | CLK_OPS_PARENT_ENABLE |\
			     CLK_SET_RATE_NO_REPARENT), _sec_id,\
			     _gate_id, _mux_id, NO_STM32_DIV)

#define STM32_MAX_MGATE 2

struct stm32_multi_gate {
	int gate_id;
	struct clk_hw *hws[STM32_MAX_MGATE];
	u32 mstate;
};

struct stm32_multi_gate mp13_multi_gate[] = {
	{ GATE_ADC1},
	{ GATE_ADC2},
	{ GATE_SAI1},
	{ GATE_SAI2},
	{ -1 }
};

static struct stm32_multi_gate *clk_stm32_get_multi_gate_data(void)
{
	return mp13_multi_gate;
}

static int clk_stm32_is_multi_gate(int gate_id)
{
	struct stm32_multi_gate *mgate;
	int i;

	mgate = clk_stm32_get_multi_gate_data();

	for (i = 0; mgate[i].gate_id != -1; i++)
		if (mgate[i].gate_id == gate_id)
			return i;

	return -1;
}

static int clk_stm32_register_mgate(struct clk_hw *hw, int idx_gate)
{
	struct stm32_multi_gate *mgate;
	int i;

	mgate = clk_stm32_get_multi_gate_data();

	for (i = 0; i < STM32_MAX_MGATE; i++)
		if (!mgate[idx_gate].hws[i]) {
			mgate[idx_gate].hws[i] = hw;
			return 0;
		}

	return -1;
}

static void mp1_mgate_clk_set(struct clk_hw *hw)
{
	int i, j;

	for (i = 0; i < ARRAY_SIZE(mp13_multi_gate); i++)
		for (j = 0; j < STM32_MAX_MGATE; j++)
			if (mp13_multi_gate[i].hws[j] == hw) {
				mp13_multi_gate[i].mstate |= (1 << j);
				return;
			}
}

static u32 mp1_mgate_clk_clear(struct clk_hw *hw)
{
	int i, j;

	for (i = 0; i < ARRAY_SIZE(mp13_multi_gate); i++)
		for (j = 0; j < STM32_MAX_MGATE; j++)
			if (mp13_multi_gate[i].hws[j] == hw) {
				mp13_multi_gate[i].mstate &= ~BIT(j);

				return mp13_multi_gate[i].mstate;
			}

	return 0;
}

static int mp1_mgate_clk_enable(struct clk_hw *hw)
{
	mp1_mgate_clk_set(hw);
	mp1_gate_clk_endisable(hw, 1);

	return 0;
}

static void mp1_mgate_clk_disable(struct clk_hw *hw)
{
	u32 ret;

	ret = mp1_mgate_clk_clear(hw);
	if (!ret)
		mp1_gate_clk_endisable(hw, 0);
}

static const struct clk_ops clk_mp1_mgate_ops = {
	.enable		= mp1_mgate_clk_enable,
	.disable	= mp1_mgate_clk_disable,
	.is_enabled	= clk_gate_is_enabled,
};

static struct clk_hw *_clk_stm32_mgate_mp1_register(struct device *dev,
						    const struct stm32_rcc_match_data *data,
						    void __iomem *base, spinlock_t *lock,
						    const struct clock_config *cfg)
{
	struct stm32_clk_gate_cfg *clk_cfg = cfg->clock_cfg;
	const struct stm32_gate_cfg *gate_cfg = &data->gates[clk_cfg->gate_id];
	struct clk_hw *hw;
	int idx_gate;
	int ret;

	hw = clk_stm32_gate_ops_register(dev, cfg->name, cfg->parent_name, cfg->flags, base,
					 gate_cfg, &clk_mp1_mgate_ops, lock);
	if (IS_ERR(hw))
		return hw;

	idx_gate = clk_stm32_is_multi_gate(clk_cfg->gate_id);
	if (idx_gate != -1) {
		ret = clk_stm32_register_mgate(hw, idx_gate);
		if (ret < 0) {
			dev_err(dev, "failed to register mgate %d\n", clk_cfg->gate_id);
			return ERR_PTR(-ENOMEM);
		}
	}

	return hw;
}

#define STM32_MGATE_MP1(_id, _name, _parent, _flags, _gate_id, _sec_id)\
{\
	.id		= _id,\
	.sec_id		= _sec_id,\
	.name		= _name,\
	.parent_name	= _parent,\
	.flags		= _flags,\
	.clock_cfg	= &(struct stm32_clk_gate_cfg) {\
		.gate_id	= _gate_id,\
	},\
	.func		= _clk_stm32_mgate_mp1_register,\
}

#define STM32_MAX_MMUX 2

struct stm32_multi_mux {
	int mux_id;
	struct clk_hw *hws[STM32_MAX_MMUX];
};

static struct stm32_multi_mux mp13_multi_mux[] = {
	{ MUX_SPI23},
	{ MUX_I2C12},
	{ MUX_LPTIM45},
	{ MUX_UART35},
	{ MUX_UART78},
	{ MUX_SAI1},
	{ -1 }
};

static struct stm32_multi_mux *clk_stm32_get_multi_mux_data(void)
{
	return mp13_multi_mux;
}

static int clk_stm32_is_multi_mux(int mux_id)
{
	struct stm32_multi_mux *mmux;
	int i;

	mmux = clk_stm32_get_multi_mux_data();

	for (i = 0; mmux[i].mux_id != -1; i++)
		if (mmux[i].mux_id == mux_id)
			return i;

	return -1;
}

static int clk_stm32_register_mmux(struct clk_hw *hw, int mux_idx)
{
	struct stm32_multi_mux *mmux;
	int i;

	mmux = clk_stm32_get_multi_mux_data();

	for (i = 0; i < STM32_MAX_MMUX; i++)
		if (!mmux[mux_idx].hws[i]) {
			mmux[mux_idx].hws[i] = hw;
			return 0;
		}

	return -1;
}

static void clk_stm32_mmux_set_parent(struct clk_hw *hw, struct clk_hw *hwp)
{
	struct stm32_multi_mux *mmux;
	int i, j, k;

	mmux = clk_stm32_get_multi_mux_data();

	for (i = 0; mmux[i].mux_id != -1; i++)
		for (j = 0; j < STM32_MAX_MMUX; j++)
			if (mmux[i].hws[j] == hw) {
				for (k = 0; k < STM32_MAX_MMUX; k++)
					clk_hw_reparent(mmux[i].hws[k], hwp);
				return;
			}
}

static u8 clk_multi_mux_get_parent(struct clk_hw *hw)
{
	return clk_mux_ops.get_parent(hw);
}

static int clk_multi_mux_set_parent(struct clk_hw *hw, u8 index)
{
	struct clk_hw *hwp = clk_hw_get_parent_by_index(hw, index);

	clk_mux_ops.set_parent(hw, index);

	clk_stm32_mmux_set_parent(hw, hwp);

	return 0;
}

const struct clk_ops clk_multi_mux_ops = {
	.get_parent = clk_multi_mux_get_parent,
	.set_parent = clk_multi_mux_set_parent,
};

static struct clk_hw *_clk_stm32_register_composite_multi(struct device *dev,
							  const struct stm32_rcc_match_data *data,
							  void __iomem *base, spinlock_t *lock,
							  const struct clock_config *cfg)
{
	struct stm32_clk_composite_cfg *composite = cfg->clock_cfg;
	const struct clk_ops *mux_ops, *rate_ops, *gate_ops;
	struct clk_hw *mux_hw, *rate_hw, *gate_hw;
	const struct stm32_mux_cfg *mux_cfg;
	const char *const *parent_names;
	int num_parents;
	int ret;

	mux_cfg = stm32_get_composite_mux_cfg(data, cfg);
	if (mux_cfg) {
		parent_names = mux_cfg->parent_names;
		num_parents = mux_cfg->num_parents;
	} else {
		parent_names = &cfg->parent_name;
		num_parents = 1;
	}

	mux_hw = stm32_get_composite_mux_hw(dev, data, base, lock, cfg);
	mux_ops = &clk_mux_ops;

	if (!IS_ERR(mux_hw)) {
		int idx_mux;

		idx_mux = clk_stm32_is_multi_mux(composite->mux_id);
		if (idx_mux != -1) {
			mux_ops = &clk_multi_mux_ops;

			ret = clk_stm32_register_mmux(mux_hw, idx_mux);
			if (ret < 0) {
				dev_err(dev, "failed to register mmux %d\n", composite->mux_id);
				return ERR_PTR(-ENOMEM);
			}
		}
	}

	rate_hw = stm32_get_composite_rate_hw(dev, data, base, lock, cfg);
	rate_ops = &clk_divider_ops;

	gate_hw = stm32_get_composite_gate_hw(dev, data, base, lock, cfg);
	gate_ops = stm32_get_composite_gate_ops(data, cfg);

	if (!IS_ERR(gate_hw)) {
		int idx_gate;

		idx_gate = clk_stm32_is_multi_gate(composite->gate_id);
		if (idx_gate != -1) {
			gate_ops = &clk_mp1_mgate_ops;

			ret = clk_stm32_register_mgate(gate_hw, idx_gate);
			if (ret < 0) {
				dev_err(dev, "failed to register mgate %d\n", composite->gate_id);
				return ERR_PTR(-ENOMEM);
			}
		}
	}

	return clk_hw_register_composite(dev, cfg->name, parent_names, num_parents,
					 mux_hw, mux_ops,
					 rate_hw, rate_ops,
					 gate_hw, gate_ops,
					 cfg->flags);
}

#define STM32_COMPOSITE_M(_id, _name, _flags, _sec_id, _gate_id, _mux_id, _div_id)\
{\
	.id		= _id,\
	.name		= _name,\
	.sec_id		= _sec_id,\
	.flags		= _flags,\
	.clock_cfg	= &(struct stm32_clk_composite_cfg) {\
		.gate_id	= _gate_id,\
		.mux_id		= _mux_id,\
		.div_id		= _div_id,\
	},\
	.func		= _clk_stm32_register_composite_multi,\
}

#define KMCLK(_id, _name, _flags, _gate_id, _mux_id, _sec_id)\
	STM32_COMPOSITE_M(_id, _name, ((_flags) | CLK_OPS_PARENT_ENABLE |\
			  CLK_SET_RATE_NO_REPARENT),\
			  _sec_id, _gate_id, _mux_id, NO_STM32_DIV)

static const struct clock_config stm32mp13_clock_cfg[] = {
	TIMER(TIM2_K, "tim2_k", "timg1_ck", 0, GATE_TIM2, SECF_NONE),
	TIMER(TIM3_K, "tim3_k", "timg1_ck", 0, GATE_TIM3, SECF_NONE),
	TIMER(TIM4_K, "tim4_k", "timg1_ck", 0, GATE_TIM4, SECF_NONE),
	TIMER(TIM5_K, "tim5_k", "timg1_ck", 0, GATE_TIM5, SECF_NONE),
	TIMER(TIM6_K, "tim6_k", "timg1_ck", 0, GATE_TIM6, SECF_NONE),
	TIMER(TIM7_K, "tim7_k", "timg1_ck", 0, GATE_TIM7, SECF_NONE),
	TIMER(TIM1_K, "tim1_k", "timg2_ck", 0, GATE_TIM1, SECF_NONE),
	TIMER(TIM8_K, "tim8_k", "timg2_ck", 0, GATE_TIM8, SECF_NONE),
	TIMER(TIM12_K, "tim12_k", "timg3_ck", 0, GATE_TIM12, SECF_TIM12),
	TIMER(TIM13_K, "tim13_k", "timg3_ck", 0, GATE_TIM13, SECF_TIM13),
	TIMER(TIM14_K, "tim14_k", "timg3_ck", 0, GATE_TIM14, SECF_TIM14),
	TIMER(TIM15_K, "tim15_k", "timg3_ck", 0, GATE_TIM15, SECF_TIM15),
	TIMER(TIM16_K, "tim16_k", "timg3_ck", 0, GATE_TIM16, SECF_TIM16),
	TIMER(TIM17_K, "tim17_k", "timg3_ck", 0, GATE_TIM17, SECF_TIM17),

	/* Peripheral clocks */
	STM32_MGATE_MP1(SAI1, "sai1", "pclk2", 0, GATE_SAI1, SECF_NONE),
	STM32_MGATE_MP1(SAI2, "sai2", "pclk2", 0, GATE_SAI2, SECF_NONE),
	PCLK(SYSCFG, "syscfg", "pclk3", 0, GATE_SYSCFG, SECF_NONE),
	PCLK(VREF, "vref", "pclk3", 0, GATE_VREF, SECF_VREF),
	PCLK(PMBCTRL, "pmbctrl", "pclk3", 0, GATE_PMBCTRL, SECF_NONE),
	PCLK(HDP, "hdp", "pclk3", 0, GATE_HDP, SECF_NONE),
	PCLK(IWDG2, "iwdg2", "pclk4", 0, GATE_IWDG2APB, SECF_NONE),
	PCLK(STGENRO, "stgenro", "pclk4", 0, GATE_STGENRO, SECF_STGENRO),
	PCLK(TZPC, "tzpc", "pclk5", 0, GATE_TZC, SECF_TZC),
	PCLK(IWDG1, "iwdg1", "pclk5", 0, GATE_IWDG1APB, SECF_IWDG1),
	PCLK(BSEC, "bsec", "pclk5", 0, GATE_BSEC, SECF_BSEC),
	PCLK(DMA1, "dma1", "ck_mlahb", 0, GATE_DMA1, SECF_NONE),
	PCLK(DMA2, "dma2", "ck_mlahb",  0, GATE_DMA2, SECF_NONE),
	PCLK(DMAMUX1, "dmamux1", "ck_mlahb", 0, GATE_DMAMUX1, SECF_NONE),
	PCLK(DMA3, "dma3", "ck_mlahb", 0, GATE_DMA3, SECF_DMA3),
	PCLK(DMAMUX2, "dmamux2", "ck_mlahb", 0, GATE_DMAMUX2, SECF_DMAMUX2),
	STM32_MGATE_MP1(ADC1, "adc1", "ck_mlahb", 0, GATE_ADC1, SECF_ADC1),
	STM32_MGATE_MP1(ADC2, "adc2", "ck_mlahb", 0, GATE_ADC2, SECF_ADC2),
	PCLK(GPIOA, "gpioa", "pclk4", 0, GATE_GPIOA, SECF_NONE),
	PCLK(GPIOB, "gpiob", "pclk4", 0, GATE_GPIOB, SECF_NONE),
	PCLK(GPIOC, "gpioc", "pclk4", 0, GATE_GPIOC, SECF_NONE),
	PCLK(GPIOD, "gpiod", "pclk4", 0, GATE_GPIOD, SECF_NONE),
	PCLK(GPIOE, "gpioe", "pclk4", 0, GATE_GPIOE, SECF_NONE),
	PCLK(GPIOF, "gpiof", "pclk4", 0, GATE_GPIOF, SECF_NONE),
	PCLK(GPIOG, "gpiog", "pclk4", 0, GATE_GPIOG, SECF_NONE),
	PCLK(GPIOH, "gpioh", "pclk4", 0, GATE_GPIOH, SECF_NONE),
	PCLK(GPIOI, "gpioi", "pclk4", 0, GATE_GPIOI, SECF_NONE),
	PCLK(TSC, "tsc", "pclk4", 0, GATE_TSC, SECF_TZC),
	PCLK(PKA, "pka", "ck_axi", 0, GATE_PKA, SECF_PKA),
	PCLK(CRYP1, "cryp1", "ck_axi", 0, GATE_CRYP1, SECF_CRYP1),
	PCLK(HASH1, "hash1", "ck_axi", 0, GATE_HASH1, SECF_HASH1),
	PCLK(BKPSRAM, "bkpsram", "ck_axi", 0, GATE_BKPSRAM, SECF_BKPSRAM),
	PCLK(MDMA, "mdma", "ck_axi", 0, GATE_MDMA, SECF_NONE),
	PCLK(ETH1TX, "eth1tx", "ck_axi", 0, GATE_ETH1TX, SECF_ETH1TX),
	PCLK(ETH1RX, "eth1rx", "ck_axi", 0, GATE_ETH1RX, SECF_ETH1RX),
	PCLK(ETH1MAC, "eth1mac", "ck_axi", 0, GATE_ETH1MAC, SECF_ETH1MAC),
	PCLK(ETH2TX, "eth2tx", "ck_axi", 0, GATE_ETH2TX, SECF_ETH2TX),
	PCLK(ETH2RX, "eth2rx", "ck_axi", 0, GATE_ETH2RX, SECF_ETH2RX),
	PCLK(ETH2MAC, "eth2mac", "ck_axi", 0, GATE_ETH2MAC, SECF_ETH2MAC),
	PCLK(CRC1, "crc1", "ck_axi", 0, GATE_CRC1, SECF_NONE),
	PCLK(USBH, "usbh", "ck_axi", 0, GATE_USBH, SECF_NONE),
	PCLK(DDRPERFM, "ddrperfm", "pclk4", 0, GATE_DDRPERFM, SECF_NONE),
	PCLK(ETH1STP, "eth1stp", "ck_axi", 0, GATE_ETH1STP, SECF_ETH1STP),
	PCLK(ETH2STP, "eth2stp", "ck_axi", 0, GATE_ETH2STP, SECF_ETH2STP),

	/* Kernel clocks */
	KCLK_SAFE(SDMMC1_K, "sdmmc1_k", 0, GATE_SDMMC1, MUX_SDMMC1, SECF_SDMMC1),
	KCLK_SAFE(SDMMC2_K, "sdmmc2_k", 0, GATE_SDMMC2, MUX_SDMMC2, SECF_SDMMC2),
	KCLK_SAFE(FMC_K, "fmc_k", 0, GATE_FMC, MUX_FMC, SECF_FMC),
	KCLK_SAFE(QSPI_K, "qspi_k", 0, GATE_QSPI, MUX_QSPI, SECF_QSPI),

	KMCLK(SPI2_K, "spi2_k", 0, GATE_SPI2, MUX_SPI23, SECF_NONE),
	KMCLK(SPI3_K, "spi3_k", 0, GATE_SPI3, MUX_SPI23, SECF_NONE),
	KMCLK(I2C1_K, "i2c1_k", 0, GATE_I2C1, MUX_I2C12, SECF_NONE),
	KMCLK(I2C2_K, "i2c2_k", 0, GATE_I2C2, MUX_I2C12, SECF_NONE),
	KMCLK(LPTIM4_K, "lptim4_k", 0, GATE_LPTIM4, MUX_LPTIM45, SECF_NONE),
	KMCLK(LPTIM5_K, "lptim5_k", 0, GATE_LPTIM5, MUX_LPTIM45, SECF_NONE),
	KMCLK(USART3_K, "usart3_k", 0, GATE_USART3, MUX_UART35, SECF_NONE),
	KMCLK(UART5_K, "uart5_k", 0, GATE_UART5, MUX_UART35, SECF_NONE),
	KMCLK(UART7_K, "uart7_k", 0, GATE_UART7, MUX_UART78, SECF_NONE),
	KMCLK(UART8_K, "uart8_k", 0, GATE_UART8, MUX_UART78, SECF_NONE),
	KMCLK(SAI1_K, "sai1_k", 0, GATE_SAI1, MUX_SAI1, SECF_NONE),
	KMCLK(SAI2_K, "sai2_k", 0, GATE_SAI2, MUX_SAI2, SECF_NONE),
	KMCLK(ADFSDM_K, "adfsdm_k", 0, GATE_ADFSDM, MUX_SAI1, SECF_NONE),
	KMCLK(ADC1_K, "adc1_k", 0, GATE_ADC1, MUX_ADC1, SECF_ADC1),
	KMCLK(ADC2_K, "adc2_k", 0, GATE_ADC2, MUX_ADC2, SECF_ADC2),

	KCLK(RNG1_K, "rng1_k", 0, GATE_RNG1, MUX_RNG1, SECF_RNG1),
	KCLK(USBPHY_K, "usbphy_k", 0, GATE_USBPHY, MUX_USBPHY, SECF_USBPHY),
	KCLK(STGEN_K, "stgen_k", 0, GATE_STGENC, MUX_STGEN, SECF_STGENC),
	KCLK(SPDIF_K, "spdif_k", 0, GATE_SPDIF, MUX_SPDIF, SECF_NONE),
	KCLK(SPI1_K, "spi1_k", 0, GATE_SPI1, MUX_SPI1, SECF_NONE),
	KCLK(SPI4_K, "spi4_k", 0, GATE_SPI4, MUX_SPI4, SECF_SPI4),
	KCLK(SPI5_K, "spi5_k", 0, GATE_SPI5, MUX_SPI5, SECF_SPI5),
	KCLK(I2C3_K, "i2c3_k", 0, GATE_I2C3, MUX_I2C3, SECF_I2C3),
	KCLK(I2C4_K, "i2c4_k", 0, GATE_I2C4, MUX_I2C4, SECF_I2C4),
	KCLK(I2C5_K, "i2c5_k", 0, GATE_I2C5, MUX_I2C5, SECF_I2C5),
	KCLK(LPTIM1_K, "lptim1_k", 0, GATE_LPTIM1, MUX_LPTIM1, SECF_NONE),
	KCLK(LPTIM2_K, "lptim2_k", 0, GATE_LPTIM2, MUX_LPTIM2, SECF_LPTIM2),
	KCLK(LPTIM3_K, "lptim3_k", 0, GATE_LPTIM3, MUX_LPTIM3, SECF_LPTIM3),
	KCLK(USART1_K, "usart1_k", 0, GATE_USART1, MUX_UART1, SECF_USART1),
	KCLK(USART2_K, "usart2_k", 0, GATE_USART2, MUX_UART2, SECF_USART2),
	KCLK(UART4_K, "uart4_k", 0, GATE_UART4, MUX_UART4, SECF_NONE),
	KCLK(USART6_K, "uart6_k", 0, GATE_USART6, MUX_UART6, SECF_NONE),
	KCLK(FDCAN_K, "fdcan_k", 0, GATE_FDCAN, MUX_FDCAN, SECF_NONE),
	KCLK(DCMIPP_K, "dcmipp_k", 0, GATE_DCMIPP, MUX_DCMIPP, SECF_DCMIPP),
	KCLK(USBO_K, "usbo_k", 0, GATE_USBO, MUX_USBO, SECF_USBO),
	KCLK(ETH1CK_K, "eth1ck_k", 0, GATE_ETH1CK, MUX_ETH1, SECF_ETH1CK),
	KCLK(ETH2CK_K, "eth2ck_k", 0, GATE_ETH2CK, MUX_ETH2, SECF_ETH2CK),
	KCLK(SAES_K, "saes_k", 0, GATE_SAES, MUX_SAES, SECF_SAES),

	STM32_GATE(DFSDM_K, "dfsdm_k", "ck_mlahb", 0, GATE_DFSDM, SECF_NONE),
	STM32_GATE(LTDC_PX, "ltdc_px", "pll4_q", CLK_SET_RATE_PARENT,
		   GATE_LTDC, SECF_NONE),

	STM32_GATE(DTS_K, "dts_k", "ck_lse", 0, GATE_DTS, SECF_NONE),

	STM32_COMPOSITE(ETH1PTP_K, "eth1ptp_k", CLK_OPS_PARENT_ENABLE |
		  CLK_SET_RATE_NO_REPARENT, SECF_ETH1CK,
		  NO_STM32_GATE, MUX_ETH1, DIV_ETH1PTP),

	STM32_COMPOSITE(ETH2PTP_K, "eth2ptp_k", CLK_OPS_PARENT_ENABLE |
		  CLK_SET_RATE_NO_REPARENT, SECF_ETH2CK,
		  NO_STM32_GATE, MUX_ETH2, DIV_ETH2PTP),

	/* MCO clocks */
	STM32_COMPOSITE(CK_MCO1, "ck_mco1", CLK_OPS_PARENT_ENABLE |
			CLK_SET_RATE_NO_REPARENT | CLK_IGNORE_UNUSED, SECF_MCO1,
			GATE_MCO1, MUX_MCO1, DIV_MCO1),

	STM32_COMPOSITE(CK_MCO2, "ck_mco2", CLK_OPS_PARENT_ENABLE |
			CLK_SET_RATE_NO_REPARENT | CLK_IGNORE_UNUSED, SECF_MCO2,
			GATE_MCO2, MUX_MCO2, DIV_MCO2),

	/* Debug clocks */
	STM32_GATE(CK_DBG, "ck_sys_dbg", "ck_axi", CLK_IGNORE_UNUSED,
		   GATE_DBGCK, SECF_NONE),

	STM32_COMPOSITE_NOMUX(CK_TRACE, "ck_trace", "ck_axi", CLK_IGNORE_UNUSED,
			      SECF_NONE, GATE_TRACECK, DIV_TRACE),
};

static int stm32mp13_check_security(void __iomem *base,
				    const struct clock_config *cfg)
{
	int sec_id = cfg->sec_id;
	int secured = 0;

	if (sec_id != SECF_NONE) {
		const struct clk_stm32_securiy *secf;

		secf = &stm32mp13_security[sec_id];
		secured = !!(readl(base + secf->offset) & BIT(secf->bit_idx));
	}

	return secured;
}

static const struct stm32_clock_match_data stm32mp13_data = {
	.tab_clocks	= stm32mp13_clock_cfg,
	.num_clocks	= ARRAY_SIZE(stm32mp13_clock_cfg),
	.gates		= stm32mp13_gates,
	.muxes		= stm32mp13_muxes,
	.dividers	= stm32mp13_dividers,
	.check_security = &stm32mp13_check_security,
	.maxbinding	= STM32MP1_LAST_CLK
};

static const struct of_device_id stm32mp13_match_data[] = {
	{
		.compatible = "st,stm32mp13-rcc",
		.data = &stm32mp13_data,
	},
	{ }
};
MODULE_DEVICE_TABLE(of, stm32mp13_match_data);

static int stm32mp1_rcc_init(struct device *dev)
{
	int ret = -ENOMEM;

	rcc_base = of_iomap(dev_of_node(dev), 0);
	if (!rcc_base) {
		dev_err(dev, "%pOFn: unable to map resource", dev_of_node(dev));
		goto out;
	}

	ret = stm32_rcc_init(dev, stm32mp13_match_data, rcc_base);
out:
	if (ret) {
		if (rcc_base)
			iounmap(rcc_base);
		rcc_base = NULL;

		of_node_put(dev_of_node(dev));
	}

	return ret;
}

static int get_clock_deps(struct device *dev)
{
	static const char * const clock_deps_name[] = {
		"hsi", "hse", "csi", "lsi", "lse",
	};
	size_t deps_size = sizeof(struct clk *) * ARRAY_SIZE(clock_deps_name);
	struct clk **clk_deps;
	int i;

	clk_deps = devm_kzalloc(dev, deps_size, GFP_KERNEL);
	if (!clk_deps)
		return -ENOMEM;

	for (i = 0; i < ARRAY_SIZE(clock_deps_name); i++) {
		struct clk *clk = of_clk_get_by_name(dev_of_node(dev),
						     clock_deps_name[i]);

		if (IS_ERR(clk)) {
			if (PTR_ERR(clk) != -EINVAL && PTR_ERR(clk) != -ENOENT)
				return PTR_ERR(clk);
		} else {
			/* Device gets a reference count on the clock */
			clk_deps[i] = devm_clk_get(dev, __clk_get_name(clk));
			clk_put(clk);
		}
	}

	return 0;
}

static int stm32mp1_rcc_clocks_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	int ret = get_clock_deps(dev);

	if (!ret)
		ret = stm32mp1_rcc_init(dev);

	return ret;
}

static int stm32mp1_rcc_clocks_remove(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct device_node *child, *np = dev_of_node(dev);

	for_each_available_child_of_node(np, child)
		of_clk_del_provider(child);

	return 0;
}

static struct platform_driver stm32mp13_rcc_clocks_driver = {
	.driver	= {
		.name = "stm32mp13_rcc",
		.of_match_table = stm32mp13_match_data,
	},
	.probe = stm32mp1_rcc_clocks_probe,
	.remove = stm32mp1_rcc_clocks_remove,
};

static int __init stm32mp13_clocks_init(void)
{
	return platform_driver_register(&stm32mp13_rcc_clocks_driver);
}
core_initcall(stm32mp13_clocks_init);

