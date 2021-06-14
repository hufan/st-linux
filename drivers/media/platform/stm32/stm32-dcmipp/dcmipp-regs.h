/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Driver for STM32 Digital Camera Memory Interface Pixel Processor
 *
 * Copyright (C) STMicroelectronics SA 2021
 * Authors: Hugues Fruchet <hugues.fruchet@foss.st.com>
 *          Alain Volmat <alain.volmat@foss.st.com>
 *          for STMicroelectronics.
 */

/*
 * DCMIPP_IPGR1: DCMIPP IPPLUG Global register 1
 */
#define DCMIPP_IPGR1 (0x0)
/*
 * DCMIPP_IPGR1_MEMORYPAGE:
 * - 0x0: 64B
 * - 0x1: 128B
 */
#define DCMIPP_IPGR1_MEMORYPAGE BIT(0)
#define DCMIPP_IPGR1_MEMORYPAGE_MASK GENMASK(2, 0)
#define DCMIPP_IPGR1_MEMORYPAGE_64B 0x0
#define DCMIPP_IPGR1_MEMORYPAGE_128B 0x1
#define DCMIPP_IPGR1_QOS_MODE BIT(24)

/*
 * DCMIPP_IPGR2: DCMIPP IPPLUG Global register 2
 */
#define DCMIPP_IPGR2 (0x4)
/*
 * DCMIPP_IPGR2_PSTART:
 * - 0x0: no lock requested, IP-Plug runs on demand by background HW.
 * - 0x1: lock requested: IP-Plug will freeze asap (see IDLE bit when lock is
 *        active).
 */
#define DCMIPP_IPGR2_PSTART BIT(0)

/*
 * DCMIPP_IPGR3: DCMIPP IPPLUG Global register 3
 */
#define DCMIPP_IPGR3 (0x8)
/*
 * DCMIPP_IPGR3_IDLE:
 * - 0x0: IP-Plug is running (on demand by background HW)
 * - 0x1: IP-Plug is currently locked and may be reconfigured.
 */
#define DCMIPP_IPGR3_IDLE BIT(0)

/*
 * DCMIPP_IPGR8: DCMIPP IPPLUG Identification register
 */
#define DCMIPP_IPGR8 (0x1C)
#define DCMIPP_IPGR8_DID BIT(0)
#define DCMIPP_IPGR8_DID_MASK GENMASK(5, 0)
#define DCMIPP_IPGR8_REVID BIT(8)
#define DCMIPP_IPGR8_REVID_MASK GENMASK(12, 8)
#define DCMIPP_IPGR8_ARCHIID BIT(16)
#define DCMIPP_IPGR8_ARCHIID_MASK GENMASK(20, 16)
#define DCMIPP_IPGR8_IPPID BIT(24)
#define DCMIPP_IPGR8_IPPID_MASK GENMASK(31, 24)

/*
 * DCMIPP_IPC1R1: DCMIPP IPPLUG Client 1 register 1
 */
#define DCMIPP_IPC1R1 (0x20)
/*
 * DCMIPP_IPC1R1_TRAFFIC:
 * - 0x0: 8B
 * - 0x1: 16B
 */
#define DCMIPP_IPC1R1_TRAFFIC BIT(0)
#define DCMIPP_IPC1R1_TRAFFIC_MASK GENMASK(2, 0)
#define DCMIPP_IPC1R1_TRAFFIC_8B 0x0
#define DCMIPP_IPC1R1_TRAFFIC_16B 0x1
/*
 * DCMIPP_IPC1R1_OTR:
 * - 0x0: Disabled.  No outstanding transaction limitation (except via fifo
 *        size)
 * - 0x1: max 2 outstanding transactions ongoing.
 */
#define DCMIPP_IPC1R1_OTR BIT(8)
#define DCMIPP_IPC1R1_OTR_MASK GENMASK(9, 8)
#define DCMIPP_IPC1R1_OTR_DISABLED_NO_OUTSTANDING_TRANSACTIO 0x0
#define DCMIPP_IPC1R1_OTR_MAX_2_OUTSTANDING_TRANSACTIONS_ONG 0x1

/*
 * DCMIPP_IPC1R2: DCMIPP IPPLUG Client 1 register 2
 */
#define DCMIPP_IPC1R2 (0x24)
#define DCMIPP_IPC1R2_SVCMAPPING BIT(8)
#define DCMIPP_IPC1R2_SVCMAPPING_MASK GENMASK(11, 8)
/*
 * DCMIPP_IPC1R2_WLRU:
 * - 0x0: ratio part =1
 * - 0x1: ratio part =2
 */
#define DCMIPP_IPC1R2_WLRU BIT(16)
#define DCMIPP_IPC1R2_WLRU_MASK GENMASK(19, 16)
#define DCMIPP_IPC1R2_WLRU_RATIO_PART_1 0x0
#define DCMIPP_IPC1R2_WLRU_RATIO_PART_2 0x1

/*
 * DCMIPP_IPC1R3: DCMIPP IPPLUG Client 1 register 3
 */
#define DCMIPP_IPC1R3 (0x28)
#define DCMIPP_IPC1R3_DPREGSTART BIT(0)
#define DCMIPP_IPC1R3_DPREGSTART_MASK GENMASK(5, 0)
#define DCMIPP_IPC1R3_DPREGEND BIT(16)
#define DCMIPP_IPC1R3_DPREGEND_MASK GENMASK(21, 16)

/*
 * DCMIPP_PRHWCFGR: Parallel Interface HW Configuration register
 */
#define DCMIPP_PRHWCFGR (0x100)

/*
 * DCMIPP_PRCR: Parallel Interface control register
 */
#define DCMIPP_PRCR (0x104)
/*
 * DCMIPP_PRCR_ESS:
 * - 0x0: Hardware synchronization data capture (frame/line start/stop) is
 *        synchronized with the HSYNC/VSYNC signals.
 * - 0x1: Embedded synchronization data capture is synchronized with
 *        synchronization codes embedded in the data flow.
 */
#define DCMIPP_PRCR_ESS BIT(4)
/*
 * DCMIPP_PRCR_PCKPOL:
 * - 0x0: Falling edge active.
 * - 0x1: Rising edge active.
 */
#define DCMIPP_PRCR_PCKPOL BIT(5)
/*
 * DCMIPP_PRCR_HSPOL:
 * - 0x0: HSYNC active low
 * - 0x1: HSYNC active high
 */
#define DCMIPP_PRCR_HSPOL BIT(6)
/*
 * DCMIPP_PRCR_VSPOL:
 * - 0x0: VSYNC active low
 * - 0x1: VSYNC active high
 */
#define DCMIPP_PRCR_VSPOL BIT(7)
/*
 * DCMIPP_PRCR_EDM:
 * - 0x0: Interface captures 8-bit data on every pixel clock
 * - 0x1: Interface captures 10-bit data on every pixel clock
 * - 0x2: Interface captures 12-bit data on every pixel clock
 * - 0x3: Interface captures 14-bit data on every pixel clock
 * - 0x4: Interface captures 16-bit data on every pixel clock
 */
#define DCMIPP_PRCR_EDM BIT(10)
#define DCMIPP_PRCR_EDM_MASK GENMASK(12, 10)
/*
 * DCMIPP_PRCR_ENABLE:
 * - 0x0: Parallel Interface disabled to lower power consumption.
 * - 0x1: Parallel Interface enabled
 */
#define DCMIPP_PRCR_ENABLE BIT(14)
/*
 * DCMIPP_PRCR_FORMAT:
 * - 0x1E: YUV422
 * - 0x22: RGB565
 * - 0x24: RGB888 (=YUV444)
 * - 0x2A: RAW8
 * - 0x2B: RAW10
 * - 0x2C: RAW12
 * - 0x2D: RAW14
 * - 0x4A: monochrome 8-bit
 * - 0x4B: monochrome 10-bit
 * - 0x4C: monochrome 12-bit
 * - 0x4D: monochrome 14-bit
 * - 0x5A: Byte stream (JPEG, compressed video)
 */
#define DCMIPP_PRCR_FORMAT BIT(16)
#define DCMIPP_PRCR_FORMAT_MASK GENMASK(23, 16)
#define DCMIPP_PRCR_FORMAT_YUV422 0x1E
#define DCMIPP_PRCR_FORMAT_RGB565 0x22
#define DCMIPP_PRCR_FORMAT_RGB888_YUV444 0x24
#define DCMIPP_PRCR_FORMAT_RAW8 0x2A
#define DCMIPP_PRCR_FORMAT_RAW10 0x2B
#define DCMIPP_PRCR_FORMAT_RAW12 0x2C
#define DCMIPP_PRCR_FORMAT_RAW14 0x2D
#define DCMIPP_PRCR_FORMAT_G8 0x4A
#define DCMIPP_PRCR_FORMAT_G10 0x4B
#define DCMIPP_PRCR_FORMAT_G12 0x4C
#define DCMIPP_PRCR_FORMAT_G14 0x4D
#define DCMIPP_PRCR_FORMAT_BYTE_STREAM 0x5A
/*
 * DCMIPP_PRCR_SWAPCYCLES:
 * - 0x0: default
 * - 0x1: swap active: the data of cycle 1 is used before the data of cycle 0.
 */
#define DCMIPP_PRCR_SWAPCYCLES BIT(25)
/*
 * DCMIPP_PRCR_SWAPBITS:
 * - 0x0: As received.
 * - 0x1: Swapped Msb vs Lsb
 */
#define DCMIPP_PRCR_SWAPBITS BIT(26)

/*
 * DCMIPP_PRESCR: Parallel Interface Embedded Sync code register
 */
#define DCMIPP_PRESCR (0x108)
#define DCMIPP_PRESCR_FSC BIT(0)
#define DCMIPP_PRESCR_FSC_MASK GENMASK(7, 0)
#define DCMIPP_PRESCR_LSC BIT(8)
#define DCMIPP_PRESCR_LSC_MASK GENMASK(15, 8)
#define DCMIPP_PRESCR_LEC BIT(16)
#define DCMIPP_PRESCR_LEC_MASK GENMASK(23, 16)
#define DCMIPP_PRESCR_FEC BIT(24)
#define DCMIPP_PRESCR_FEC_MASK GENMASK(31, 24)

/*
 * DCMIPP_PRESUR: Parallel Interface Embedded Sync Unmsk register
 */
#define DCMIPP_PRESUR (0x10C)
/*
 * DCMIPP_PRESUR_FSU:
 * - 0x0: The corresponding bit in the FSC byte in DCMIPP_ESCR is masked while
 *        comparing the frame start delimiter with the received data
 * - 0x1: The corresponding bit in the FSC byte in DCMIPP_ESCR is compared while
 *        comparing the frame start delimiter with the received data
 */
#define DCMIPP_PRESUR_FSU BIT(0)
#define DCMIPP_PRESUR_FSU_MASK GENMASK(7, 0)
/*
 * DCMIPP_PRESUR_LSU:
 * - 0x0: The corresponding bit in the LSC byte in DCMIPP_ESCR is masked while
 *        comparing the line start delimiter with the received data
 * - 0x1: The corresponding bit in the LSC byte in DCMIPP_ESCR is compared while
 *        comparing the line start delimiter with the received data
 */
#define DCMIPP_PRESUR_LSU BIT(8)
#define DCMIPP_PRESUR_LSU_MASK GENMASK(15, 8)
/*
 * DCMIPP_PRESUR_LEU:
 * - 0x0: The corresponding bit in the LEC byte in DCMIPP_ESCR is masked while
 *        comparing the line end delimiter with the received data
 * - 0x1: The corresponding bit in the LEC byte in DCMIPP_ESCR is compared while
 *        comparing the line end delimiter with the received data
 */
#define DCMIPP_PRESUR_LEU BIT(16)
#define DCMIPP_PRESUR_LEU_MASK GENMASK(23, 16)
/*
 * DCMIPP_PRESUR_FEU:
 * - 0x0: The corresponding bit in the FEC byte in DCMIPP_ESCR is masked while
 *        comparing the frame end delimiter with the received data.
 * - 0x1: The corresponding bit in the FEC byte in DCMIPP_ESCR is compared while
 *        comparing the frame end delimiter with the received data
 */
#define DCMIPP_PRESUR_FEU BIT(24)
#define DCMIPP_PRESUR_FEU_MASK GENMASK(31, 24)

/*
 * DCMIPP_PRIER: Parallel Interface Interrupt enable register
 */
#define DCMIPP_PRIER (0x1F4)
/*
 * DCMIPP_PRIER_ERRIE:
 * - 0x0: No interrupt generation
 * - 0x1: An interrupt is generated if the embedded synchronization codes are
 *        not received in the correct order.
 */
#define DCMIPP_PRIER_ERRIE BIT(6)

/*
 * DCMIPP_PRSR: Parallel Interface status register
 */
#define DCMIPP_PRSR (0x1F8)
/*
 * DCMIPP_PRSR_ERRF:
 * - 0x0: No synchronization error detected
 * - 0x1: Embedded synchronization characters are not received in the correct
 *        order.
 */
#define DCMIPP_PRSR_ERRF BIT(6)
/*
 * DCMIPP_PRSR_HSYNC:
 * - 0x0: active line
 * - 0x1: synchronization between lines
 */
#define DCMIPP_PRSR_HSYNC BIT(16)
/*
 * DCMIPP_PRSR_VSYNC:
 * - 0x0: active frame
 * - 0x1: synchronization between frames
 */
#define DCMIPP_PRSR_VSYNC BIT(17)

/*
 * DCMIPP_PRFCR: Parallel Interface Interrupt clear register
 */
#define DCMIPP_PRFCR (0x1FC)
#define DCMIPP_PRFCR_CERRF BIT(6)

/*
 * DCMIPP_CMHWCFGR: Common IP HW Configuration register
 */
#define DCMIPP_CMHWCFGR (0x200)
#define DCMIPP_CMHWCFGR_CPIPES BIT(2)
#define DCMIPP_CMHWCFGR_CPIPES_MASK GENMASK(3, 2)
/*
 * DCMIPP_CMHWCFGR_CPAR:
 * - 0x0: Parallel Interface input is not implemented.
 * - 0x1: Parallel Interface input is implemented.
 */
#define DCMIPP_CMHWCFGR_CPAR BIT(4)
/*
 * DCMIPP_CMHWCFGR_CCSI:
 * - 0x0: no Input from CSI-2 Host.
 * - 0x1: Input from CSI-2 Host is implemented (independently of presence of
 *        CSI-2 Host).
 */
#define DCMIPP_CMHWCFGR_CCSI BIT(5)

/*
 * DCMIPP_CMCR: Common Configuration register
 */
#define DCMIPP_CMCR (0x204)
#define DCMIPP_CMCR_CFC BIT(4)

/*
 * DCMIPP_CMFRCR: Common frame counter register
 */
#define DCMIPP_CMFRCR (0x208)
#define DCMIPP_CMFRCR_FRMCNT BIT(0)
#define DCMIPP_CMFRCR_FRMCNT_MASK GENMASK(31, 0)

/*
 * DCMIPP_CMIER: Common Interrupt enable register
 */
#define DCMIPP_CMIER (0x3F0)
/*
 * DCMIPP_CMIER_ATXERRIE:
 * - 0x0: No interrupt generation
 * - 0x1: An interrupt is generated.
 */
#define DCMIPP_CMIER_ATXERRIE BIT(5)
/*
 * DCMIPP_CMIER_PRERRIE:
 * - 0x0: No interrupt generation
 * - 0x1: An interrupt is generated.
 */
#define DCMIPP_CMIER_PRERRIE BIT(6)
/*
 * DCMIPP_CMIER_P0LINEIE:
 * - 0x0: No interrupt generation
 * - 0x1: An interrupt is generated..
 */
#define DCMIPP_CMIER_P0LINEIE BIT(8)
/*
 * DCMIPP_CMIER_P0FRAMEIE:
 * - 0x0: No interrupt generation
 * - 0x1: An interrupt is generated.
 */
#define DCMIPP_CMIER_P0FRAMEIE BIT(9)
/*
 * DCMIPP_CMIER_P0VSYNCIE:
 * - 0x0: No interrupt generation
 * - 0x1: An interrupt is generated.
 */
#define DCMIPP_CMIER_P0VSYNCIE BIT(10)
/*
 * DCMIPP_CMIER_P0LIMITIE:
 * - 0x0: No interrupt generation
 * - 0x1: An interrupt is generated.
 */
#define DCMIPP_CMIER_P0LIMITIE BIT(14)
/*
 * DCMIPP_CMIER_P0OVRIE:
 * - 0x0: No interrupt generation
 * - 0x1: An interrupt is generated.
 */
#define DCMIPP_CMIER_P0OVRIE BIT(15)

/*
 * DCMIPP_CMSR1: Common status register 1
 */
#define DCMIPP_CMSR1 (0x3F4)
/*
 * DCMIPP_CMSR1_PRHSYNC:
 * - 0x0: active line
 * - 0x1: synchronization between lines
 */
#define DCMIPP_CMSR1_PRHSYNC BIT(0)
/*
 * DCMIPP_CMSR1_PRVSYNC:
 * - 0x0: active frame
 * - 0x1: synchronization between frames
 */
#define DCMIPP_CMSR1_PRVSYNC BIT(1)
/*
 * DCMIPP_CMSR1_P0CPTACT:
 * - 0x0: No capture currently active.
 * - 0x1: Capture currently active.
 */
#define DCMIPP_CMSR1_P0CPTACT BIT(15)

/*
 * DCMIPP_CMSR2: Common status register 2
 */
#define DCMIPP_CMSR2 (0x3F8)
/*
 * DCMIPP_CMSR2_ATXERRF:
 * - 0x0: No AXI transfer error detected
 * - 0x1: AXI transfer error occurred on an AXI client.  This bit is signaling an
 *        error on a client without any specific hardware action.  It is up to
 *        the software to handle the situation (normally used when debugging
 *        software application code)
 */
#define DCMIPP_CMSR2_ATXERRF BIT(5)
/*
 * DCMIPP_CMSR2_PRERRF:
 * - 0x0: No synchronization error detected
 * - 0x1: Embedded synchronization characters are not received in the correct
 *        order.
 */
#define DCMIPP_CMSR2_PRERRF BIT(6)
#define DCMIPP_CMSR2_P0LINEF BIT(8)
/*
 * DCMIPP_CMSR2_P0FRAMEF:
 * - 0x0: No capture or ongoing capture
 * - 0x1: All data of a frame have been captured.
 */
#define DCMIPP_CMSR2_P0FRAMEF BIT(9)
#define DCMIPP_CMSR2_P0VSYNCF BIT(10)
#define DCMIPP_CMSR2_P0LIMITF BIT(14)
/*
 * DCMIPP_CMSR2_P0OVRF:
 * - 0x0: No data buffer overrun occurred
 * - 0x1: A data buffer overrun occurred and this frame data are corrupted.
 */
#define DCMIPP_CMSR2_P0OVRF BIT(15)

/*
 * DCMIPP_CMFCR: Common Interrupt clear register
 */
#define DCMIPP_CMFCR (0x3FC)
#define DCMIPP_CMFCR_CATXERRF BIT(5)
#define DCMIPP_CMFCR_CPRERRF BIT(6)
#define DCMIPP_CMFCR_CP0LINEF BIT(8)
#define DCMIPP_CMFCR_CP0FRAMEF BIT(9)
#define DCMIPP_CMFCR_CP0VSYNCF BIT(10)
#define DCMIPP_CMFCR_CP0LIMITF BIT(14)
#define DCMIPP_CMFCR_CP0OVRF BIT(15)

/*
 * DCMIPP_P0HWCFGR: Pipe0 HW Configuration register
 */
#define DCMIPP_P0HWCFGR (0x400)
/*
 * DCMIPP_P0HWCFGR_CPLA:
 * - 0x0: capability to dump to a coplanar buffer only
 * - 0x1: capability to dump to a Semi-Planar Buffer (ie: YUV420-2)
 * - 0x2: capability to dump to Full-Planar Buffer (ie: YUV420-3, ..)
 */
#define DCMIPP_P0HWCFGR_CPLA BIT(0)
#define DCMIPP_P0HWCFGR_CPLA_MASK GENMASK(1, 0)
#define DCMIPP_P0HWCFGR_CPLA_CAPABILITY_TO_DUMP_TO_A_COPLANA 0x0
#define DCMIPP_P0HWCFGR_CPLA_CAPABILITY_TO_DUMP_TO_A_SEMIPLA 0x1
#define DCMIPP_P0HWCFGR_CPLA_CAPABILITY_TO_DUMP_TO_FULLPLANA 0x2
/*
 * DCMIPP_P0HWCFGR_CDS:
 * - 0x0: No Downsize Filter, basic decimation instead.
 * - 0x1: Downsize Filter based on a BoxFilter
 */
#define DCMIPP_P0HWCFGR_CDS BIT(2)
/*
 * DCMIPP_P0HWCFGR_DBM:
 * - 0x0: No Double buffer mode supported.
 * - 0x1: Double buffer mode supported
 */
#define DCMIPP_P0HWCFGR_DBM BIT(3)
/*
 * DCMIPP_P0HWCFGR_CRB:
 * - 0x0: No demosaicing.
 * - 0x1: Demosaicing with 2018-03 Algo (Matrix3x3 with Edge/LineHV/Peak)
 */
#define DCMIPP_P0HWCFGR_CRB BIT(4)
#define DCMIPP_P0HWCFGR_CRB_MASK GENMASK(5, 4)
#define DCMIPP_P0HWCFGR_CRB_NO_DEMOSAICING 0x0
#define DCMIPP_P0HWCFGR_CRB_DEMOSAICING_WITH_201803_ALGO_MAT 0x1
/*
 * DCMIPP_P0HWCFGR_CGM:
 * - 0x0: No Gamma Conversion.
 * - 0x1: Gamma Conversion with 3 interpolated segments.
 */
#define DCMIPP_P0HWCFGR_CGM BIT(6)
#define DCMIPP_P0HWCFGR_CGM_MASK GENMASK(7, 6)
#define DCMIPP_P0HWCFGR_CGM_NO_GAMMA_CONVERSION 0x0
#define DCMIPP_P0HWCFGR_CGM_GAMMA_CONVERSION_WITH_3_INTERPOL 0x1
#define DCMIPP_P0HWCFGR_CROI BIT(8)
#define DCMIPP_P0HWCFGR_CROI_MASK GENMASK(11, 8)
/*
 * DCMIPP_P0HWCFGR_CVP:
 * - 0x0: No virtualization for the pipe
 * - 0x1: Virtualization mechanism implemented for the pipe
 */
#define DCMIPP_P0HWCFGR_CVP BIT(13)

/*
 * DCMIPP_P0FSCR: Pipe0 Flow Selection configuration register
 */
#define DCMIPP_P0FSCR (0x404)
/*
 * DCMIPP_P0FSCR_PIPEN:
 * - 0x0: pipe disabled
 * - 0x1: pipe enabled and can start capturing with CPTMODE, CPTREQ, CPTACK.
 */
#define DCMIPP_P0FSCR_PIPEN BIT(31)

/*
 * DCMIPP_P0FCTCR: Pipe0 Flow Control configuration register
 */
#define DCMIPP_P0FCTCR (0x500)
/*
 * DCMIPP_P0FCTCR_FRATE:
 * - 0x0: All frames are captured
 * - 0x1: One frame every 2 frames captured (50% bandwidth reduction)
 * - 0x2: One frame every 4 frames captured (75% bandwidth reduction)
 * - 0x3: One frame every 8 frames captured (87% bandwidth reduction)
 */
#define DCMIPP_P0FCTCR_FRATE BIT(0)
#define DCMIPP_P0FCTCR_FRATE_MASK GENMASK(1, 0)
#define DCMIPP_P0FCTCR_FRATE_ALL_FRAMES_ARE_CAPTURED 0x0
#define DCMIPP_P0FCTCR_FRATE_ONE_FRAME_EVERY_2_FRAMES_CAPTUR 0x1
#define DCMIPP_P0FCTCR_FRATE_ONE_FRAME_EVERY_4_FRAMES_CAPTUR 0x2
#define DCMIPP_P0FCTCR_FRATE_ONE_FRAME_EVERY_8_FRAMES_CAPTUR 0x3
/*
 * DCMIPP_P0FCTCR_CPTMODE:
 * - 0x0: Continuous grab mode - The received data are transferred into the
 *        destination memory through the AXI Master.
 * - 0x1: Snapshot mode (single frame) - Once activated, the interface waits for
 *        the start of frame and then transfers a single frame through the AXI
 *        Master.  At the end of the frame, the CPTACT bit is automatically
 *        reset.
 */
#define DCMIPP_P0FCTCR_CPTMODE BIT(2)
/*
 * DCMIPP_P0FCTCR_CPTREQ:
 * - 0x0: Capture not requested for next frame.
 * - 0x1: Capture requested for next frame.
 */
#define DCMIPP_P0FCTCR_CPTREQ BIT(3)

/*
 * DCMIPP_P0SCSTR: Pipe0 Stat/Crop Start register
 */
#define DCMIPP_P0SCSTR (0x504)
#define DCMIPP_P0SCSTR_HSTART BIT(0)
#define DCMIPP_P0SCSTR_HSTART_MASK GENMASK(11, 0)
#define DCMIPP_P0SCSTR_VSTART BIT(16)
#define DCMIPP_P0SCSTR_VSTART_MASK GENMASK(27, 16)

/*
 * DCMIPP_P0SCSZR: Pipe0 Stat/Crop Size register
 */
#define DCMIPP_P0SCSZR (0x508)
#define DCMIPP_P0SCSZR_HSIZE BIT(0)
#define DCMIPP_P0SCSZR_HSIZE_MASK GENMASK(11, 0)
#define DCMIPP_P0SCSZR_VSIZE BIT(16)
#define DCMIPP_P0SCSZR_VSIZE_MASK GENMASK(27, 16)
/*
 * DCMIPP_P0SCSZR_POSNEG:
 * - 0x0: Positive area.  The rectangle defined by VSIZE,HSIZE, and VSTART,
 *        HSTART will be the active area.
 * - 0x1: Negative area.  The active area is the area excluding the rectangle
 *        defined by VSIZE,HSIZE, and VSTART, HSTART..
 */
#define DCMIPP_P0SCSZR_POSNEG BIT(30)
/*
 * DCMIPP_P0SCSZR_ENABLE:
 * - 0x0: bypass.  All the data are computed, if the statistic data are sent
 *        within the frame, they will be sent to the processing pipe as pixels
 *        data.
 * - 0x1: enable : Depending on the bit POSNEG value, the rectangle defined by
 *        the VSIZE, HSIZE, VSTART, HSTART may be used to extract or to remove
 *        certain amount of data (statiscal extraction or removal, or basic 2D
 *        crop features)
 */
#define DCMIPP_P0SCSZR_ENABLE BIT(31)

/*
 * DCMIPP_P0DCCNTR: Pipe0 Dump counter register
 */
#define DCMIPP_P0DCCNTR (0x5B0)
#define DCMIPP_P0DCCNTR_CNT BIT(0)
#define DCMIPP_P0DCCNTR_CNT_MASK GENMASK(25, 0)

/*
 * DCMIPP_P0DCLMTR: Pipe0 Dump limit register
 */
#define DCMIPP_P0DCLMTR (0x5B4)
#define DCMIPP_P0DCLMTR_LIMIT BIT(0)
#define DCMIPP_P0DCLMTR_LIMIT_MASK GENMASK(23, 0)
/*
 * DCMIPP_P0DCLMTR_ENABLE:
 * - 0x0: disabled, no check on the amount of 32b-words transmitted.
 * - 0x1: enabled, check done versus limit.
 */
#define DCMIPP_P0DCLMTR_ENABLE BIT(31)

/*
 * DCMIPP_P0PPCR: Pipe0 Pixel Packer configuration register
 */
#define DCMIPP_P0PPCR (0x5C0)
/*
 * DCMIPP_P0PPCR_PAD:
 * - 0x0: Aligns on LSB (and pads null bits on MSB), for backward compatibility
 *        with former DCMI.
 * - 0x1: Aligns on MSB (and pads null bits on LSB), for better ease of Software
 *        or GPU.
 */
#define DCMIPP_P0PPCR_PAD BIT(5)
/*
 * DCMIPP_P0PPCR_BSM:
 * - 0x0: Interface captures all received data
 * - 0x1: Interface captures 1 data out of 2
 * - 0x2: Interface captures one byte out of four
 * - 0x3: Interface captures two bytes out of four
 */
#define DCMIPP_P0PPCR_BSM BIT(7)
#define DCMIPP_P0PPCR_BSM_MASK GENMASK(8, 7)
#define DCMIPP_P0PPCR_BSM_ALL 0x0
#define DCMIPP_P0PPCR_BSM_1_2 0x1
#define DCMIPP_P0PPCR_BSM_1_4 0x2
#define DCMIPP_P0PPCR_BSM_2_4 0x3
/*
 * DCMIPP_P0PPCR_OEBS:
 * - 0x0: Interface captures first data (byte or double byte) from the
 *        frame/line start,
 * - 0x1: Interface captures second data (byte or double byte) from the
 *        frame/line start,
 */
#define DCMIPP_P0PPCR_OEBS BIT(9)
/*
 * DCMIPP_P0PPCR_LSM:
 * - 0x0: Interface captures all received lines
 * - 0x1: Interface captures one line out of two
 */
#define DCMIPP_P0PPCR_LSM BIT(10)
/*
 * DCMIPP_P0PPCR_OELS:
 * - 0x0: Interface captures first line after the frame start, second one being
 *        dropped
 * - 0x1: Interface captures second line from the frame start, first one being
 *        dropped
 */
#define DCMIPP_P0PPCR_OELS BIT(11)
/*
 * DCMIPP_P0PPCR_LINEMULT:
 * - 0x0: Event after every 1 line.
 * - 0x1: Event after every 2 lines.
 * - 0x2: Event after every 4lines.
 * - 0x3: Event after every 8 lines.
 * - 0x4: Event after every 16 line.
 * - 0x5: Event after every 32 lines.
 * - 0x6: Event after every 64 lines.
 * - 0x7: Event after every 128 lines.
 */
#define DCMIPP_P0PPCR_LINEMULT BIT(13)
#define DCMIPP_P0PPCR_LINEMULT_MASK GENMASK(15, 13)
#define DCMIPP_P0PPCR_LINEMULT_EVENT_AFTER_EVERY_1_LINE 0x0
#define DCMIPP_P0PPCR_LINEMULT_EVENT_AFTER_EVERY_2_LINES 0x1
#define DCMIPP_P0PPCR_LINEMULT_EVENT_AFTER_EVERY_4LINES 0x2
#define DCMIPP_P0PPCR_LINEMULT_EVENT_AFTER_EVERY_8_LINES 0x3
#define DCMIPP_P0PPCR_LINEMULT_EVENT_AFTER_EVERY_16_LINE 0x4
#define DCMIPP_P0PPCR_LINEMULT_EVENT_AFTER_EVERY_32_LINES 0x5
#define DCMIPP_P0PPCR_LINEMULT_EVENT_AFTER_EVERY_64_LINES 0x6
#define DCMIPP_P0PPCR_LINEMULT_EVENT_AFTER_EVERY_128_LINES 0x7
/*
 * DCMIPP_P0PPCR_DBM:
 * - 0x0: No double buffer mode activated.  Pipe 0 is always dump to memory
 *        address set by DCMIPP_P0PPM0AR1
 * - 0x1: Double buffer mode activated.  Dump address location switches from
 *        DCMIPP_P0PPM0AR1 to DCMIPP_P0PPM0AR2 alternatively on each frame.
 */
#define DCMIPP_P0PPCR_DBM BIT(16)

/*
 * DCMIPP_P0PPM0AR1: Pipe0 Pixel Packer memory0 address register 1
 */
#define DCMIPP_P0PPM0AR1 (0x5C4)
#define DCMIPP_P0PPM0AR1_M0A BIT(0)
#define DCMIPP_P0PPM0AR1_M0A_MASK GENMASK(31, 0)

/*
 * DCMIPP_P0PPM0AR2: Pipe0 Pixel Packer memory0 address register 2
 */
#define DCMIPP_P0PPM0AR2 (0x5C8)
#define DCMIPP_P0PPM0AR2_M0A BIT(0)
#define DCMIPP_P0PPM0AR2_M0A_MASK GENMASK(31, 0)

/*
 * DCMIPP_P0IER: Pipe0 Interrupt enable register
 */
#define DCMIPP_P0IER (0x5F4)
/*
 * DCMIPP_P0IER_LINEIE:
 * - 0x0: No interrupt generation when the line is received
 * - 0x1: An Interrupt is generated after the full capture of a group of lines
 *        (or last line reached)
 */
#define DCMIPP_P0IER_LINEIE BIT(0)
/*
 * DCMIPP_P0IER_FRAMEIE:
 * - 0x0: No interrupt generation
 * - 0x1: An interrupt is generated after the full capture of a cropped frame.
 */
#define DCMIPP_P0IER_FRAMEIE BIT(1)
/*
 * DCMIPP_P0IER_VSYNCIE:
 * - 0x0: No interrupt generation
 * - 0x1: An interrupt is generated on each VSYNC (captured or not).
 */
#define DCMIPP_P0IER_VSYNCIE BIT(2)
/*
 * DCMIPP_P0IER_LIMITIE:
 * - 0x0: No interrupt generation when the limit is reached
 * - 0x1: An Interrupt is generated when the limit is reached
 */
#define DCMIPP_P0IER_LIMITIE BIT(6)
/*
 * DCMIPP_P0IER_OVRIE:
 * - 0x0: No interrupt generation
 * - 0x1: An interrupt is generated if the AXI Master was not able to transfer
 *        the last data before new data (32-bit) are received.
 */
#define DCMIPP_P0IER_OVRIE BIT(7)

/*
 * DCMIPP_P0SR: Pipe0 status register
 */
#define DCMIPP_P0SR (0x5F8)
#define DCMIPP_P0SR_LINEF BIT(0)
/*
 * DCMIPP_P0SR_FRAMEF:
 * - 0x0: No capture or ongoing capture
 * - 0x1: All data of a frame have been captured.
 */
#define DCMIPP_P0SR_FRAMEF BIT(1)
#define DCMIPP_P0SR_VSYNCF BIT(2)
#define DCMIPP_P0SR_LIMITF BIT(6)
/*
 * DCMIPP_P0SR_OVRF:
 * - 0x0: No data buffer overrun occurred
 * - 0x1: A data buffer overrun occurred and this frame data are corrupted.
 */
#define DCMIPP_P0SR_OVRF BIT(7)
#define DCMIPP_P0SR_LSTLINE BIT(16)
/*
 * DCMIPP_P0SR_CPTACT:
 * - 0x0: Capture currently inactive.
 * - 0x1: Capture currently active.
 */
#define DCMIPP_P0SR_CPTACT BIT(23)

/*
 * DCMIPP_P0FCR: Pipe0 Interrupt clear register
 */
#define DCMIPP_P0FCR (0x5FC)
#define DCMIPP_P0FCR_CLINEF BIT(0)
#define DCMIPP_P0FCR_CFRAMEF BIT(1)
#define DCMIPP_P0FCR_CVSYNCF BIT(2)
#define DCMIPP_P0FCR_CLIMITF BIT(6)
#define DCMIPP_P0FCR_COVRF BIT(7)

/*
 * DCMIPP_P0CFCTCR: Pipe0 Current Flow Control configuration register
 */
#define DCMIPP_P0CFCTCR (0x700)
/*
 * DCMIPP_P0CFCTCR_FRATE:
 * - 0x0: All frames are captured
 * - 0x1: One frame every 2 frames captured (50% bandwidth reduction)
 * - 0x2: One frame every 4 frames captured (75% bandwidth reduction)
 * - 0x3: One frame every 8 frames captured (87% bandwidth reduction)
 */
#define DCMIPP_P0CFCTCR_FRATE BIT(0)
#define DCMIPP_P0CFCTCR_FRATE_MASK GENMASK(1, 0)
#define DCMIPP_P0CFCTCR_FRATE_ALL_FRAMES_ARE_CAPTURED 0x0
#define DCMIPP_P0CFCTCR_FRATE_ONE_FRAME_EVERY_2_FRAMES_CAPTU 0x1
#define DCMIPP_P0CFCTCR_FRATE_ONE_FRAME_EVERY_4_FRAMES_CAPTU 0x2
#define DCMIPP_P0CFCTCR_FRATE_ONE_FRAME_EVERY_8_FRAMES_CAPTU 0x3
/*
 * DCMIPP_P0CFCTCR_CPTMODE:
 * - 0x0: Continuous grab mode - The received data are transferred into the
 *        destination memory through the AXI Master.
 * - 0x1: Snapshot mode (single frame) - Once activated, the interface waits for
 *        the start of frame and then transfers a single frame through the AXI
 *        Master.  At the end of the frame, the CPTACT bit is automatically
 *        reset.
 */
#define DCMIPP_P0CFCTCR_CPTMODE BIT(2)
/*
 * DCMIPP_P0CFCTCR_CPTREQ:
 * - 0x0: Capture not requested for next frame.
 * - 0x1: Capture requested for next frame.
 */
#define DCMIPP_P0CFCTCR_CPTREQ BIT(3)

/*
 * DCMIPP_P0CSCSTR: Pipe0 Current Stat/Crop Start register
 */
#define DCMIPP_P0CSCSTR (0x704)
#define DCMIPP_P0CSCSTR_HSTART BIT(0)
#define DCMIPP_P0CSCSTR_HSTART_MASK GENMASK(11, 0)
#define DCMIPP_P0CSCSTR_VSTART BIT(16)
#define DCMIPP_P0CSCSTR_VSTART_MASK GENMASK(27, 16)

/*
 * DCMIPP_P0CSCSZR: Pipe0 Current Stat/Crop Size register
 */
#define DCMIPP_P0CSCSZR (0x708)
#define DCMIPP_P0CSCSZR_HSIZE BIT(0)
#define DCMIPP_P0CSCSZR_HSIZE_MASK GENMASK(11, 0)
#define DCMIPP_P0CSCSZR_VSIZE BIT(16)
#define DCMIPP_P0CSCSZR_VSIZE_MASK GENMASK(27, 16)
/*
 * DCMIPP_P0CSCSZR_POSNEG:
 * - 0x0: Positive area.  The rectangle defined by VSIZE,HSIZE, and VSTART,
 *        HSTART will be the active area.
 * - 0x1: Negative area.  The active area is the area excluding the rectangle
 *        defined by VSIZE,HSIZE, and VSTART, HSTART..
 */
#define DCMIPP_P0CSCSZR_POSNEG BIT(30)
/*
 * DCMIPP_P0CSCSZR_ENABLE:
 * - 0x0: bypass.  All the data are computed, if the statistic data are sent
 *        within the frame, they will be sent to the processing pipe as pixels
 *        data.
 * - 0x1: enable : Depending on the bit POSNEG value, the rectangle defined by
 *        the VSIZE, HSIZE, VSTART, HSTART may be used to extract or to remove
 *        certain amount of data (statiscal extraction or removal, or basic 2D
 *        crop features)
 */
#define DCMIPP_P0CSCSZR_ENABLE BIT(31)

/*
 * DCMIPP_P0CPPCR: Pipe0 Current Pixel Packer configuration register
 */
#define DCMIPP_P0CPPCR (0x7C0)
/*
 * DCMIPP_P0CPPCR_PAD:
 * - 0x0: Aligns on LSB (and pads null bits on MSB), for backward compatibility
 *        with former DCMI.
 * - 0x1: Aligns on MSB (and pads null bits on LSB), for better ease of Software
 *        or GPU.
 */
#define DCMIPP_P0CPPCR_PAD BIT(5)
/*
 * DCMIPP_P0CPPCR_BSM:
 * - 0x0: Interface captures all received data
 * - 0x1: Interface captures 1 data out of 2
 * - 0x2: Interface captures one byte out of four
 * - 0x3: Interface captures two bytes out of four
 */
#define DCMIPP_P0CPPCR_BSM BIT(7)
#define DCMIPP_P0CPPCR_BSM_MASK GENMASK(8, 7)
#define DCMIPP_P0CPPCR_BSM_INTERFACE_CAPTURES_ALL_RECEIVED_D 0x0
#define DCMIPP_P0CPPCR_BSM_INTERFACE_CAPTURES_1_DATA_OUT_OF_ 0x1
#define DCMIPP_P0CPPCR_BSM_INTERFACE_CAPTURES_ONE_BYTE_OUT_O 0x2
#define DCMIPP_P0CPPCR_BSM_INTERFACE_CAPTURES_TWO_BYTES_OUT_ 0x3
/*
 * DCMIPP_P0CPPCR_OEBS:
 * - 0x0: Interface captures first data (byte or double byte) from the
 *        frame/line start,
 * - 0x1: Interface captures second data (byte or double byte) from the
 *        frame/line start,
 */
#define DCMIPP_P0CPPCR_OEBS BIT(9)
/*
 * DCMIPP_P0CPPCR_LSM:
 * - 0x0: Interface captures all received lines
 * - 0x1: Interface captures one line out of two
 */
#define DCMIPP_P0CPPCR_LSM BIT(10)
/*
 * DCMIPP_P0CPPCR_OELS:
 * - 0x0: Interface captures first line after the frame start, second one being
 *        dropped
 * - 0x1: Interface captures second line from the frame start, first one being
 *        dropped
 */
#define DCMIPP_P0CPPCR_OELS BIT(11)
/*
 * DCMIPP_P0CPPCR_LINEMULT:
 * - 0x0: Event after every 1 line.
 * - 0x1: Event after every 2 lines.
 * - 0x2: Event after every 4lines.
 * - 0x3: Event after every 8 lines.
 * - 0x4: Event after every 16 line.
 * - 0x5: Event after every 32 lines.
 * - 0x6: Event after every 64 lines.
 * - 0x7: Event after every 128 lines.
 */
#define DCMIPP_P0CPPCR_LINEMULT BIT(13)
#define DCMIPP_P0CPPCR_LINEMULT_MASK GENMASK(15, 13)
#define DCMIPP_P0CPPCR_LINEMULT_EVENT_AFTER_EVERY_1_LINE 0x0
#define DCMIPP_P0CPPCR_LINEMULT_EVENT_AFTER_EVERY_2_LINES 0x1
#define DCMIPP_P0CPPCR_LINEMULT_EVENT_AFTER_EVERY_4LINES 0x2
#define DCMIPP_P0CPPCR_LINEMULT_EVENT_AFTER_EVERY_8_LINES 0x3
#define DCMIPP_P0CPPCR_LINEMULT_EVENT_AFTER_EVERY_16_LINE 0x4
#define DCMIPP_P0CPPCR_LINEMULT_EVENT_AFTER_EVERY_32_LINES 0x5
#define DCMIPP_P0CPPCR_LINEMULT_EVENT_AFTER_EVERY_64_LINES 0x6
#define DCMIPP_P0CPPCR_LINEMULT_EVENT_AFTER_EVERY_128_LINES 0x7
/*
 * DCMIPP_P0CPPCR_DBM:
 * - 0x0: No double buffer mode activated.  Pipe 0 is always dump to memory
 *        address set by DCMIPP_P0PPM0AR1
 * - 0x1: Double buffer mode activated.  Dump address location switches from
 *        DCMIPP_P0PPM0AR1 to DCMIPP_P0PPM0AR2 alternatively on each frame.
 */
#define DCMIPP_P0CPPCR_DBM BIT(16)

/*
 * DCMIPP_P0CPPM0AR1: Pipe0 Current Pixel Packer memory0 address register 1
 */
#define DCMIPP_P0CPPM0AR1 (0x7C4)
#define DCMIPP_P0CPPM0AR1_M0A BIT(0)
#define DCMIPP_P0CPPM0AR1_M0A_MASK GENMASK(31, 0)

/*
 * DCMIPP_P0CPPM0AR2: Pipe0 Current Pixel Packer Current memory0 address register
 * 2
 */
#define DCMIPP_P0CPPM0AR2 (0x7C8)
#define DCMIPP_P0CPPM0AR2_M0A BIT(0)
#define DCMIPP_P0CPPM0AR2_M0A_MASK GENMASK(31, 0)

/*
 * DCMIPP_HWCFGR2: IP HW Configuration register 2
 */
#define DCMIPP_HWCFGR2 (0xFEC)
#define DCMIPP_HWCFGR2_VPFT BIT(0)
#define DCMIPP_HWCFGR2_VPFT_MASK GENMASK(2, 0)
#define DCMIPP_HWCFGR2_DBMFT BIT(4)
/*
 * DCMIPP_HWCFGR2_PROCCLK:
 * - 0x0: Independent processing clock not connected to AXI
 * - 0x1: Processing clock is directly the AXI
 */
#define DCMIPP_HWCFGR2_PROCCLK BIT(8)

/*
 * DCMIPP_HWCFGR1: IP HW Configuration register 1
 */
#define DCMIPP_HWCFGR1 (0xFF0)
#define DCMIPP_HWCFGR1_CSIFT BIT(0)
#define DCMIPP_HWCFGR1_PIPENB BIT(4)
#define DCMIPP_HWCFGR1_PIPENB_MASK GENMASK(5, 4)
#define DCMIPP_HWCFGR1_IPPLUGCFG BIT(8)
#define DCMIPP_HWCFGR1_DSP1FT BIT(12)
#define DCMIPP_HWCFGR1_DSP2FT BIT(13)
#define DCMIPP_HWCFGR1_RB2RGB BIT(16)
/*
 * DCMIPP_HWCFGR1_PLANARFT:
 * - 0x0: Coplanar
 * - 0x1: Semi planar
 * - 0x2: Full Planar ( One buffer per component)
 */
#define DCMIPP_HWCFGR1_PLANARFT BIT(20)
#define DCMIPP_HWCFGR1_PLANARFT_MASK GENMASK(21, 20)
#define DCMIPP_HWCFGR1_PLANARFT_COPLANAR 0x0
#define DCMIPP_HWCFGR1_PLANARFT_SEMI_PLANAR 0x1
#define DCMIPP_HWCFGR1_PLANARFT_FULL_PLANAR__ONE_BUFFER_PER_ 0x2
#define DCMIPP_HWCFGR1_ROI1NB BIT(24)
#define DCMIPP_HWCFGR1_ROI1NB_MASK GENMASK(27, 24)
#define DCMIPP_HWCFGR1_ROI2NB BIT(28)
#define DCMIPP_HWCFGR1_ROI2NB_MASK GENMASK(31, 28)

/*
 * DCMIPP_VERR: IP Version register
 */
#define DCMIPP_VERR (0xFF4)
#define DCMIPP_VERR_MINREV BIT(0)
#define DCMIPP_VERR_MINREV_MASK GENMASK(3, 0)
#define DCMIPP_VERR_MAJREV BIT(4)
#define DCMIPP_VERR_MAJREV_MASK GENMASK(7, 4)

/*
 * DCMIPP_IPIDR: IP Identification register
 */
#define DCMIPP_IPIDR (0xFF8)
#define DCMIPP_IPIDR_IDR BIT(0)
#define DCMIPP_IPIDR_IDR_MASK GENMASK(31, 0)

/*
 * DCMIPP_SIDR: Size Identification register
 */
#define DCMIPP_SIDR (0xFFC)
#define DCMIPP_SIDR_SID BIT(0)
#define DCMIPP_SIDR_SID_MASK GENMASK(31, 0)

/*
 * Generic
 */
#define DCMIPP_NUM_PIPES 1

#define DCMIPP_PXPPM0AR1(id) (DCMIPP_P0PPM0AR1)
#define DCMIPP_PXPPM0AR2(id) (DCMIPP_P0PPM0AR2)
#define DCMIPP_PXFSCR(id) (DCMIPP_P0FSCR)
#define DCMIPP_PXFCTCR(id) (DCMIPP_P0FCTCR)
#define DCMIPP_PXIER(id) (DCMIPP_P0IER)
#define DCMIPP_PXFCR(id) (DCMIPP_P0FCR)
#define DCMIPP_PXSR(id) (DCMIPP_P0SR)
#define DCMIPP_PXDCCNTR(id) (DCMIPP_P0DCCNTR)
#define DCMIPP_PXHWCFGR(id) (DCMIPP_P0HWCFGR)

#define DCMIPP_CMSR2_PXOVRF(id) (DCMIPP_CMSR2_P0OVRF)
#define DCMIPP_CMSR2_PXVSYNCF(id) (DCMIPP_CMSR2_P0VSYNCF)
#define DCMIPP_CMSR2_PXFRAMEF(id) (DCMIPP_CMSR2_P0FRAMEF)

#define DCMIPP_CMIER_PXALL(id) (DCMIPP_CMIER_P0VSYNCIE |\
				DCMIPP_CMIER_P0FRAMEIE |\
				DCMIPP_CMIER_P0LIMITIE |\
				DCMIPP_CMIER_P0OVRIE)

#define DCMIPP_FORMAT_YUV422 DCMIPP_PRCR_FORMAT_YUV422
#define DCMIPP_FORMAT_RGB565 DCMIPP_PRCR_FORMAT_RGB565
#define DCMIPP_FORMAT_RGB888_YUV444 DCMIPP_PRCR_FORMAT_RGB888_YUV444
#define DCMIPP_FORMAT_RAW8 DCMIPP_PRCR_FORMAT_RAW8
#define DCMIPP_FORMAT_RAW10 DCMIPP_PRCR_FORMAT_RAW10
#define DCMIPP_FORMAT_RAW12 DCMIPP_PRCR_FORMAT_RAW12
#define DCMIPP_FORMAT_RAW14 DCMIPP_PRCR_FORMAT_RAW14
#define DCMIPP_FORMAT_G8 DCMIPP_PRCR_FORMAT_G8
#define DCMIPP_FORMAT_G10 DCMIPP_PRCR_FORMAT_G10
#define DCMIPP_FORMAT_G12 DCMIPP_PRCR_FORMAT_G12
#define DCMIPP_FORMAT_G14 DCMIPP_PRCR_FORMAT_G14
#define DCMIPP_FORMAT_BYTE_STREAM DCMIPP_PRCR_FORMAT_BYTE_STREAM

struct dcmipp_reg {
	u32 offset;
	const char *name;
};

static const struct dcmipp_reg regs_to_name[] = {
	{DCMIPP_PRHWCFGR, "PRHWCFGR"},
	{DCMIPP_PRCR, "PRCR"},
	{DCMIPP_PRESCR, "PRESCR"},
	{DCMIPP_PRESUR, "PRESUR"},
	{DCMIPP_PRIER, "PRIER"},
	{DCMIPP_PRSR, "PRSR"},
	{DCMIPP_PRFCR, "PRFCR"},

	{DCMIPP_CMHWCFGR, "CMHWCFGR"},
	{DCMIPP_CMCR, "CMCR"},
	{DCMIPP_CMFRCR, "CMFRCR"},
	{DCMIPP_CMIER, "CMIER"},
	{DCMIPP_CMSR1, "CMSR1"},
	{DCMIPP_CMSR2, "CMSR2"},
	{DCMIPP_CMFCR, "CMFCR"},

	{DCMIPP_P0HWCFGR, "P0HWCFGR"},
	{DCMIPP_P0FSCR, "P0FSCR"},
	{DCMIPP_P0FCTCR, "P0FCTCR"},
	{DCMIPP_P0SCSTR, "P0SCSTR"},
	{DCMIPP_P0SCSZR, "P0SCSZR"},
	{DCMIPP_P0DCCNTR, "P0DCCNTR"},
	{DCMIPP_P0DCLMTR, "P0DCLMTR"},
	{DCMIPP_P0PPCR, "P0PPCR"},
	{DCMIPP_P0PPM0AR1, "P0PPM0AR1"},
	{DCMIPP_P0IER, "P0IER"},
	{DCMIPP_P0SR, "P0SR"},
	{DCMIPP_P0FCR, "P0FCR"},
	{DCMIPP_P0CFCTCR, "P0CFCTCR"},
	{DCMIPP_P0CSCSTR, "P0CSCSTR"},
	{DCMIPP_P0CSCSZR, "P0CSCSZR"},
	{DCMIPP_P0CPPCR, "P0CPPCR"},
	{DCMIPP_P0CPPM0AR1, "P0CPPM0AR1"},

	{DCMIPP_VERR, "VERR"},
	{DCMIPP_IPIDR, "IPIDR"},
	{DCMIPP_SIDR, "SIDR"},
};
