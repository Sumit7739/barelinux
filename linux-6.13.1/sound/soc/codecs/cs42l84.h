/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright (C) The Asahi Linux Contributors
 *
 * Based on sound/soc/codecs/cs42l42.h
 *
 * Copyright 2016 Cirrus Logic, Inc.
 */


#ifndef __CS42L84_H__
#define __CS42L84_H__

#include <linux/bits.h>

#define CS42L84_CHIP_ID				0x42a84

#define CS42L84_DEVID				0x0000
#define CS42L84_REVID				0x73fe
#define CS42L84_FRZ_CTL				0x0006
#define CS42L84_FRZ_CTL_ENGAGE			BIT(0)

#define CS42L84_TSRS_PLUG_INT_STATUS		0x0400
#define CS42L84_TSRS_PLUG_INT_MASK		0x0418
#define CS42L84_RS_PLUG_SHIFT			0
#define CS42L84_RS_PLUG				BIT(0)
#define CS42L84_RS_UNPLUG			BIT(1)
#define CS42L84_TS_PLUG_SHIFT			2
#define CS42L84_TS_PLUG				BIT(2)
#define CS42L84_TS_UNPLUG			BIT(3)
#define CS42L84_TSRS_PLUG_VAL_MASK		GENMASK(3, 0)
#define CS42L84_PLL_LOCK_STATUS			0x040e // probably bit 0x10
#define CS42L84_PLL_LOCK_STATUS_LOCKED		BIT(4)
#define CS42L84_PLL_LOCK_STATUS_ERROR		BIT(5)

#define CS42L84_PLUG				3
#define CS42L84_UNPLUG				0
#define CS42L84_TRANS				1

#define CS42L84_CCM_CTL1			0x0600
#define CS42L84_CCM_CTL1_MCLK_SRC		GENMASK(1, 0)
#define CS42L84_CCM_CTL1_MCLK_SRC_RCO		0
#define CS42L84_CCM_CTL1_MCLK_SRC_MCLK		1
#define CS42L84_CCM_CTL1_MCLK_SRC_BCLK		2
#define CS42L84_CCM_CTL1_MCLK_SRC_PLL		3
#define CS42L84_CCM_CTL1_MCLK_FREQ		GENMASK(3, 2)
#define CS42L84_CCM_CTL1_MCLK_F_12MHZ		0b00
#define CS42L84_CCM_CTL1_MCLK_F_24MHZ		0b01
#define CS42L84_CCM_CTL1_MCLK_F_12_288KHZ	0b10
#define CS42L84_CCM_CTL1_MCLK_F_24_576KHZ	0b11
#define CS42L84_CCM_CTL1_RCO \
	(FIELD_PREP(CS42L84_CCM_CTL1_MCLK_SRC, CS42L84_CCM_CTL1_MCLK_SRC_RCO) \
	| FIELD_PREP(CS42L84_CCM_CTL1_MCLK_FREQ, CS42L84_CCM_CTL1_MCLK_F_12MHZ))

#define CS42L84_CCM_SAMP_RATE			0x0601
#define CS42L84_CCM_SAMP_RATE_RATE_48KHZ	4
#define CS42L84_CCM_SAMP_RATE_RATE_96KHZ	5
#define CS42L84_CCM_SAMP_RATE_RATE_192KHZ	6
#define CS42L84_CCM_SAMP_RATE_RATE_44K1HZ	12
#define CS42L84_CCM_SAMP_RATE_RATE_88K2HZ	13
#define CS42L84_CCM_SAMP_RATE_RATE_176K4HZ	14
#define CS42L84_CCM_CTL3			0x0602
#define CS42L84_CCM_CTL3_REFCLK_DIV		GENMASK(2, 1)
#define CS42L84_CCM_CTL4			0x0603
#define CS42L84_CCM_CTL4_REFCLK_EN		BIT(0)

#define CS42L84_CCM_ASP_CLK_CTRL		0x0608

#define CS42L84_PLL_CTL1			0x0800
#define CS42L84_PLL_CTL1_EN			BIT(0)
#define CS42L84_PLL_CTL1_MODE			GENMASK(2, 1)
#define CS42L84_PLL_DIV_FRAC0			0x0804
#define CS42L84_PLL_DIV_FRAC1			0x0805
#define CS42L84_PLL_DIV_FRAC2			0x0806
#define CS42L84_PLL_DIV_INT			0x0807
#define CS42L84_PLL_DIVOUT			0x0808

#define CS42L84_RING_SENSE_CTL			0x1282
#define CS42L84_RING_SENSE_CTL_INV		BIT(7)
#define CS42L84_RING_SENSE_CTL_UNK1		BIT(6)
#define CS42L84_RING_SENSE_CTL_FALLTIME		GENMASK(5, 3)
#define CS42L84_RING_SENSE_CTL_RISETIME		GENMASK(2, 0)
#define CS42L84_TIP_SENSE_CTL			0x1283
#define CS42L84_TIP_SENSE_CTL_INV		BIT(7)
#define CS42L84_TIP_SENSE_CTL_FALLTIME		GENMASK(5, 3)
#define CS42L84_TIP_SENSE_CTL_RISETIME		GENMASK(2, 0)

#define CS42L84_TSRS_PLUG_STATUS		0x1288

#define CS42L84_TIP_SENSE_CTL2			0x1473
#define CS42L84_TIP_SENSE_CTL2_MODE		GENMASK(7, 6)
#define CS42L84_TIP_SENSE_CTL2_MODE_DISABLED	0b00
#define CS42L84_TIP_SENSE_CTL2_MODE_DIG_INPUT	0b01
#define CS42L84_TIP_SENSE_CTL2_MODE_SHORT_DET	0b11
#define CS42L84_TIP_SENSE_CTL2_INV		BIT(5)

#define CS42L84_MISC_DET_CTL			0x1474
#define CS42L84_MISC_DET_CTL_DETECT_MODE	GENMASK(4, 3)
#define CS42L84_MISC_DET_CTL_HSBIAS_CTL		GENMASK(2, 1)
#define CS42L84_MISC_DET_CTL_PDN_MIC_LVL_DET	BIT(0)

#define CS42L84_MIC_DET_CTL1			0x1475
#define CS42L84_MIC_DET_CTL1_HS_DET_LEVEL	GENMASK(5, 0)

#define CS42L84_MIC_DET_CTL4			0x1477
#define CS42L84_MIC_DET_CTL4_LATCH_TO_VP	BIT(1)

#define CS42L84_HS_DET_STATUS2			0x147d

#define CS42L84_MSM_BLOCK_EN1			0x1800
#define CS42L84_MSM_BLOCK_EN2			0x1801
#define CS42L84_MSM_BLOCK_EN2_ASP_SHIFT		6
#define CS42L84_MSM_BLOCK_EN2_BUS_SHIFT		5
#define CS42L84_MSM_BLOCK_EN2_DAC_SHIFT		4
#define CS42L84_MSM_BLOCK_EN2_ADC_SHIFT		3
#define CS42L84_MSM_BLOCK_EN3			0x1802
#define CS42L84_MSM_BLOCK_EN3_TR_SENSE		BIT(3)

#define CS42L84_HS_DET_CTL2			0x1811
#define CS42L84_HS_DET_CTL2_CTL			GENMASK(7, 6)
#define CS42L84_HS_DET_CTL2_SET			GENMASK(5, 4)
#define CS42L84_HS_DET_CTL2_REF			BIT(3)
#define CS42L84_HS_DET_CTL2_AUTO_TIME		GENMASK(1, 0)

#define CS42L84_HS_SWITCH_CTL			0x1812
#define CS42L84_HS_SWITCH_CTL_REF_HS3		BIT(7)
#define CS42L84_HS_SWITCH_CTL_REF_HS4		BIT(6)
#define CS42L84_HS_SWITCH_CTL_HSB_FILT_HS3	BIT(5)
#define CS42L84_HS_SWITCH_CTL_HSB_FILT_HS4	BIT(4)
#define CS42L84_HS_SWITCH_CTL_HSB_HS3		BIT(3)
#define CS42L84_HS_SWITCH_CTL_HSB_HS4		BIT(2)
#define CS42L84_HS_SWITCH_CTL_GNDHS_HS3		BIT(1)
#define CS42L84_HS_SWITCH_CTL_GNDHS_HS4		BIT(0)

#define CS42L84_HS_CLAMP_DISABLE		0x1813

#define CS42L84_ADC_CTL1			0x2000
#define CS42L84_ADC_CTL1_PREAMP_GAIN_SHIFT	6
#define CS42L84_ADC_CTL1_PGA_GAIN_SHIFT		0
#define CS42L84_ADC_CTL4			0x2003
#define CS42L84_ADC_CTL4_WNF_CF_SHIFT		4
#define CS42L84_ADC_CTL4_WNF_EN_SHIFT		3
#define CS42L84_ADC_CTL4_HPF_CF_SHIFT		1
#define CS42L84_ADC_CTL4_HPF_EN_SHIFT		0

#define CS42L84_DAC_CTL1			0x3000
#define CS42L84_DAC_CTL1_UNMUTE			BIT(0)
//#define CS42L84_DAC_CTL1_DACB_INV_SHIFT 1
//#define CS42L84_DAC_CTL1_DACA_INV_SHIFT 0
#define CS42L84_DAC_CTL2			0x3001

#define CS42L84_DAC_CHA_VOL_LSB			0x3004
#define CS42L84_DAC_CHA_VOL_MSB			0x3005
#define CS42L84_DAC_CHB_VOL_LSB			0x3006
#define CS42L84_DAC_CHB_VOL_MSB			0x3007
#define CS42L84_HP_VOL_CTL			0x3020
#define CS42L84_HP_VOL_CTL_ZERO_CROSS		BIT(1)
#define CS42L84_HP_VOL_CTL_SOFT			BIT(0)

#define CS42L84_SRC_ASP_RX_CH1			0b1101
#define CS42L84_SRC_ASP_RX_CH2			0b1110

#define CS42L84_BUS_ASP_TX_SRC                  0x4000
#define CS42L84_BUS_ASP_TX_SRC_CH1_SHIFT        0
#define CS42L84_BUS_DAC_SRC			0x4001
#define CS42L84_BUS_DAC_SRC_DACA_SHIFT		0
#define CS42L84_BUS_DAC_SRC_DACB_SHIFT		4

#define CS42L84_ASP_CTL				0x5000
#define CS42L84_ASP_CTL_BCLK_EN_SHIFT		1
#define CS42L84_ASP_CTL_TDM_MODE		BIT(2)
#define CS42L84_ASP_FSYNC_CTL2			0x5010
#define CS42L84_ASP_FSYNC_CTL2_BCLK_PERIOD_LO	GENMASK(7, 1)
#define CS42L84_ASP_FSYNC_CTL3			0x5011
#define CS42L84_ASP_FSYNC_CTL3_BCLK_PERIOD_HI	GENMASK(4, 0)
#define CS42L84_ASP_DATA_CTL			0x5018

#define CS42L84_ASP_RX_EN			0x5020
#define CS42L84_ASP_RX_EN_CH1_SHIFT		0
#define CS42L84_ASP_RX_EN_CH2_SHIFT		1
#define CS42L84_ASP_TX_EN			0x5024
#define CS42L84_ASP_TX_EN_CH1_SHIFT             0

#define CS42L84_ASP_RX_CH1_CTL1			0x5028
#define CS42L84_ASP_RX_CH1_CTL2			0x5029
#define CS42L84_ASP_RX_CH1_WIDTH		0x502a
#define CS42L84_ASP_RX_CH2_CTL1			0x502c
#define CS42L84_ASP_RX_CH2_CTL2			0x502d
#define CS42L84_ASP_RX_CH2_WIDTH		0x502e

#define CS42L84_ASP_RX_CHx_CTL1_EDGE		BIT(0)
#define CS42L84_ASP_RX_CHx_CTL1_SLOT_START_LSB	GENMASK(7, 1)
#define CS42L84_ASP_RX_CHx_CTL2_SLOT_START_MSB	GENMASK(2, 0)

#define CS42L84_ASP_TX_CH1_CTL1			0x5068
#define CS42L84_ASP_TX_CH1_CTL2			0x5069
#define CS42L84_ASP_TX_CH1_WIDTH		0x506a
#define CS42L84_ASP_TX_CH2_CTL1			0x506c
#define CS42L84_ASP_TX_CH2_CTL2			0x506d
#define CS42L84_ASP_TX_CH2_WIDTH		0x506e

#define CS42L84_DEBOUNCE_TIME_125MS		0b001
#define CS42L84_DEBOUNCE_TIME_500MS		0b011

#define CS42L84_BOOT_TIME_US			3000
#define CS42L84_CLOCK_SWITCH_DELAY_US		150
#define CS42L84_PLL_LOCK_POLL_US		250
#define CS42L84_PLL_LOCK_TIMEOUT_US		1250

#endif /* __CS42L84_H__ */
