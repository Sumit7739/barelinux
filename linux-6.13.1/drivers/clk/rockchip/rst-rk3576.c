// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Copyright (c) 2021 Rockchip Electronics Co., Ltd.
 * Copyright (c) 2024 Collabora Ltd.
 * Author: Detlev Casanova <detlev.casanova@collabora.com>
 * Based on Sebastien Reichel's implementation for RK3588
 */

#include <linux/module.h>
#include <linux/of.h>
#include <dt-bindings/reset/rockchip,rk3576-cru.h>
#include "clk.h"

/* 0x27200000 + 0x0A00 */
#define RK3576_CRU_RESET_OFFSET(id, reg, bit) [id] = (0 + reg * 16 + bit)
/* 0x27208000 + 0x0A00 */
#define RK3576_PHPCRU_RESET_OFFSET(id, reg, bit) [id] = (0x8000*4 + reg * 16 + bit)
/* 0x27210000 + 0x0A00 */
#define RK3576_SECURENSCRU_RESET_OFFSET(id, reg, bit) [id] = (0x10000*4 + reg * 16 + bit)
/* 0x27220000 + 0x0A00 */
#define RK3576_PMU1CRU_RESET_OFFSET(id, reg, bit) [id] = (0x20000*4 + reg * 16 + bit)

/* mapping table for reset ID to register offset */
static const int rk3576_register_offset[] = {
	/* SOFTRST_CON01 */
	RK3576_CRU_RESET_OFFSET(SRST_A_TOP_BIU, 1, 3),
	RK3576_CRU_RESET_OFFSET(SRST_P_TOP_BIU, 1, 5),
	RK3576_CRU_RESET_OFFSET(SRST_A_TOP_MID_BIU, 1, 6),
	RK3576_CRU_RESET_OFFSET(SRST_A_SECURE_HIGH_BIU, 1, 7),
	RK3576_CRU_RESET_OFFSET(SRST_H_TOP_BIU, 1, 14),

	/* SOFTRST_CON02 */
	RK3576_CRU_RESET_OFFSET(SRST_H_VO0VOP_CHANNEL_BIU, 2, 0),
	RK3576_CRU_RESET_OFFSET(SRST_A_VO0VOP_CHANNEL_BIU, 2, 1),

	/* SOFTRST_CON06 */
	RK3576_CRU_RESET_OFFSET(SRST_BISRINTF, 6, 2),

	/* SOFTRST_CON07 */
	RK3576_CRU_RESET_OFFSET(SRST_H_AUDIO_BIU, 7, 2),
	RK3576_CRU_RESET_OFFSET(SRST_H_ASRC_2CH_0, 7, 3),
	RK3576_CRU_RESET_OFFSET(SRST_H_ASRC_2CH_1, 7, 4),
	RK3576_CRU_RESET_OFFSET(SRST_H_ASRC_4CH_0, 7, 5),
	RK3576_CRU_RESET_OFFSET(SRST_H_ASRC_4CH_1, 7, 6),
	RK3576_CRU_RESET_OFFSET(SRST_ASRC_2CH_0, 7, 7),
	RK3576_CRU_RESET_OFFSET(SRST_ASRC_2CH_1, 7, 8),
	RK3576_CRU_RESET_OFFSET(SRST_ASRC_4CH_0, 7, 9),
	RK3576_CRU_RESET_OFFSET(SRST_ASRC_4CH_1, 7, 10),
	RK3576_CRU_RESET_OFFSET(SRST_M_SAI0_8CH, 7, 12),
	RK3576_CRU_RESET_OFFSET(SRST_H_SAI0_8CH, 7, 13),
	RK3576_CRU_RESET_OFFSET(SRST_H_SPDIF_RX0, 7, 14),
	RK3576_CRU_RESET_OFFSET(SRST_M_SPDIF_RX0, 7, 15),

	/* SOFTRST_CON08 */
	RK3576_CRU_RESET_OFFSET(SRST_H_SPDIF_RX1, 8, 0),
	RK3576_CRU_RESET_OFFSET(SRST_M_SPDIF_RX1, 8, 1),
	RK3576_CRU_RESET_OFFSET(SRST_M_SAI1_8CH, 8, 5),
	RK3576_CRU_RESET_OFFSET(SRST_H_SAI1_8CH, 8, 6),
	RK3576_CRU_RESET_OFFSET(SRST_M_SAI2_2CH, 8, 8),
	RK3576_CRU_RESET_OFFSET(SRST_H_SAI2_2CH, 8, 10),
	RK3576_CRU_RESET_OFFSET(SRST_M_SAI3_2CH, 8, 12),
	RK3576_CRU_RESET_OFFSET(SRST_H_SAI3_2CH, 8, 14),

	/* SOFTRST_CON09 */
	RK3576_CRU_RESET_OFFSET(SRST_M_SAI4_2CH, 9, 0),
	RK3576_CRU_RESET_OFFSET(SRST_H_SAI4_2CH, 9, 2),
	RK3576_CRU_RESET_OFFSET(SRST_H_ACDCDIG_DSM, 9, 3),
	RK3576_CRU_RESET_OFFSET(SRST_M_ACDCDIG_DSM, 9, 4),
	RK3576_CRU_RESET_OFFSET(SRST_PDM1, 9, 5),
	RK3576_CRU_RESET_OFFSET(SRST_H_PDM1, 9, 7),
	RK3576_CRU_RESET_OFFSET(SRST_M_PDM1, 9, 8),
	RK3576_CRU_RESET_OFFSET(SRST_H_SPDIF_TX0, 9, 9),
	RK3576_CRU_RESET_OFFSET(SRST_M_SPDIF_TX0, 9, 10),
	RK3576_CRU_RESET_OFFSET(SRST_H_SPDIF_TX1, 9, 11),
	RK3576_CRU_RESET_OFFSET(SRST_M_SPDIF_TX1, 9, 12),

	/* SOFTRST_CON11 */
	RK3576_CRU_RESET_OFFSET(SRST_A_BUS_BIU, 11, 3),
	RK3576_CRU_RESET_OFFSET(SRST_P_BUS_BIU, 11, 4),
	RK3576_CRU_RESET_OFFSET(SRST_P_CRU, 11, 5),
	RK3576_CRU_RESET_OFFSET(SRST_H_CAN0, 11, 6),
	RK3576_CRU_RESET_OFFSET(SRST_CAN0, 11, 7),
	RK3576_CRU_RESET_OFFSET(SRST_H_CAN1, 11, 8),
	RK3576_CRU_RESET_OFFSET(SRST_CAN1, 11, 9),
	RK3576_CRU_RESET_OFFSET(SRST_P_INTMUX2BUS, 11, 12),
	RK3576_CRU_RESET_OFFSET(SRST_P_VCCIO_IOC, 11, 13),
	RK3576_CRU_RESET_OFFSET(SRST_H_BUS_BIU, 11, 14),
	RK3576_CRU_RESET_OFFSET(SRST_KEY_SHIFT, 11, 15),

	/* SOFTRST_CON12 */
	RK3576_CRU_RESET_OFFSET(SRST_P_I2C1, 12, 0),
	RK3576_CRU_RESET_OFFSET(SRST_P_I2C2, 12, 1),
	RK3576_CRU_RESET_OFFSET(SRST_P_I2C3, 12, 2),
	RK3576_CRU_RESET_OFFSET(SRST_P_I2C4, 12, 3),
	RK3576_CRU_RESET_OFFSET(SRST_P_I2C5, 12, 4),
	RK3576_CRU_RESET_OFFSET(SRST_P_I2C6, 12, 5),
	RK3576_CRU_RESET_OFFSET(SRST_P_I2C7, 12, 6),
	RK3576_CRU_RESET_OFFSET(SRST_P_I2C8, 12, 7),
	RK3576_CRU_RESET_OFFSET(SRST_P_I2C9, 12, 8),
	RK3576_CRU_RESET_OFFSET(SRST_P_WDT_BUSMCU, 12, 9),
	RK3576_CRU_RESET_OFFSET(SRST_T_WDT_BUSMCU, 12, 10),
	RK3576_CRU_RESET_OFFSET(SRST_A_GIC, 12, 11),
	RK3576_CRU_RESET_OFFSET(SRST_I2C1, 12, 12),
	RK3576_CRU_RESET_OFFSET(SRST_I2C2, 12, 13),
	RK3576_CRU_RESET_OFFSET(SRST_I2C3, 12, 14),
	RK3576_CRU_RESET_OFFSET(SRST_I2C4, 12, 15),

	/* SOFTRST_CON13 */
	RK3576_CRU_RESET_OFFSET(SRST_I2C5, 13, 0),
	RK3576_CRU_RESET_OFFSET(SRST_I2C6, 13, 1),
	RK3576_CRU_RESET_OFFSET(SRST_I2C7, 13, 2),
	RK3576_CRU_RESET_OFFSET(SRST_I2C8, 13, 3),
	RK3576_CRU_RESET_OFFSET(SRST_I2C9, 13, 4),
	RK3576_CRU_RESET_OFFSET(SRST_P_SARADC, 13, 6),
	RK3576_CRU_RESET_OFFSET(SRST_SARADC, 13, 7),
	RK3576_CRU_RESET_OFFSET(SRST_P_TSADC, 13, 8),
	RK3576_CRU_RESET_OFFSET(SRST_TSADC, 13, 9),
	RK3576_CRU_RESET_OFFSET(SRST_P_UART0, 13, 10),
	RK3576_CRU_RESET_OFFSET(SRST_P_UART2, 13, 11),
	RK3576_CRU_RESET_OFFSET(SRST_P_UART3, 13, 12),
	RK3576_CRU_RESET_OFFSET(SRST_P_UART4, 13, 13),
	RK3576_CRU_RESET_OFFSET(SRST_P_UART5, 13, 14),
	RK3576_CRU_RESET_OFFSET(SRST_P_UART6, 13, 15),

	/* SOFTRST_CON14 */
	RK3576_CRU_RESET_OFFSET(SRST_P_UART7, 14, 0),
	RK3576_CRU_RESET_OFFSET(SRST_P_UART8, 14, 1),
	RK3576_CRU_RESET_OFFSET(SRST_P_UART9, 14, 2),
	RK3576_CRU_RESET_OFFSET(SRST_P_UART10, 14, 3),
	RK3576_CRU_RESET_OFFSET(SRST_P_UART11, 14, 4),
	RK3576_CRU_RESET_OFFSET(SRST_S_UART0, 14, 5),
	RK3576_CRU_RESET_OFFSET(SRST_S_UART2, 14, 6),
	RK3576_CRU_RESET_OFFSET(SRST_S_UART3, 14, 9),
	RK3576_CRU_RESET_OFFSET(SRST_S_UART4, 14, 12),
	RK3576_CRU_RESET_OFFSET(SRST_S_UART5, 14, 15),

	/* SOFTRST_CON15 */
	RK3576_CRU_RESET_OFFSET(SRST_S_UART6, 15, 2),
	RK3576_CRU_RESET_OFFSET(SRST_S_UART7, 15, 5),
	RK3576_CRU_RESET_OFFSET(SRST_S_UART8, 15, 8),
	RK3576_CRU_RESET_OFFSET(SRST_S_UART9, 15, 9),
	RK3576_CRU_RESET_OFFSET(SRST_S_UART10, 15, 10),
	RK3576_CRU_RESET_OFFSET(SRST_S_UART11, 15, 11),
	RK3576_CRU_RESET_OFFSET(SRST_P_SPI0, 15, 13),
	RK3576_CRU_RESET_OFFSET(SRST_P_SPI1, 15, 14),
	RK3576_CRU_RESET_OFFSET(SRST_P_SPI2, 15, 15),

	/* SOFTRST_CON16 */
	RK3576_CRU_RESET_OFFSET(SRST_P_SPI3, 16, 0),
	RK3576_CRU_RESET_OFFSET(SRST_P_SPI4, 16, 1),
	RK3576_CRU_RESET_OFFSET(SRST_SPI0, 16, 2),
	RK3576_CRU_RESET_OFFSET(SRST_SPI1, 16, 3),
	RK3576_CRU_RESET_OFFSET(SRST_SPI2, 16, 4),
	RK3576_CRU_RESET_OFFSET(SRST_SPI3, 16, 5),
	RK3576_CRU_RESET_OFFSET(SRST_SPI4, 16, 6),
	RK3576_CRU_RESET_OFFSET(SRST_P_WDT0, 16, 7),
	RK3576_CRU_RESET_OFFSET(SRST_T_WDT0, 16, 8),
	RK3576_CRU_RESET_OFFSET(SRST_P_SYS_GRF, 16, 9),
	RK3576_CRU_RESET_OFFSET(SRST_P_PWM1, 16, 10),
	RK3576_CRU_RESET_OFFSET(SRST_PWM1, 16, 11),

	/* SOFTRST_CON17 */
	RK3576_CRU_RESET_OFFSET(SRST_P_BUSTIMER0, 17, 3),
	RK3576_CRU_RESET_OFFSET(SRST_P_BUSTIMER1, 17, 4),
	RK3576_CRU_RESET_OFFSET(SRST_TIMER0, 17, 6),
	RK3576_CRU_RESET_OFFSET(SRST_TIMER1, 17, 7),
	RK3576_CRU_RESET_OFFSET(SRST_TIMER2, 17, 8),
	RK3576_CRU_RESET_OFFSET(SRST_TIMER3, 17, 9),
	RK3576_CRU_RESET_OFFSET(SRST_TIMER4, 17, 10),
	RK3576_CRU_RESET_OFFSET(SRST_TIMER5, 17, 11),
	RK3576_CRU_RESET_OFFSET(SRST_P_BUSIOC, 17, 12),
	RK3576_CRU_RESET_OFFSET(SRST_P_MAILBOX0, 17, 13),
	RK3576_CRU_RESET_OFFSET(SRST_P_GPIO1, 17, 15),

	/* SOFTRST_CON18 */
	RK3576_CRU_RESET_OFFSET(SRST_GPIO1, 18, 0),
	RK3576_CRU_RESET_OFFSET(SRST_P_GPIO2, 18, 1),
	RK3576_CRU_RESET_OFFSET(SRST_GPIO2, 18, 2),
	RK3576_CRU_RESET_OFFSET(SRST_P_GPIO3, 18, 3),
	RK3576_CRU_RESET_OFFSET(SRST_GPIO3, 18, 4),
	RK3576_CRU_RESET_OFFSET(SRST_P_GPIO4, 18, 5),
	RK3576_CRU_RESET_OFFSET(SRST_GPIO4, 18, 6),
	RK3576_CRU_RESET_OFFSET(SRST_A_DECOM, 18, 7),
	RK3576_CRU_RESET_OFFSET(SRST_P_DECOM, 18, 8),
	RK3576_CRU_RESET_OFFSET(SRST_D_DECOM, 18, 9),
	RK3576_CRU_RESET_OFFSET(SRST_TIMER6, 18, 11),
	RK3576_CRU_RESET_OFFSET(SRST_TIMER7, 18, 12),
	RK3576_CRU_RESET_OFFSET(SRST_TIMER8, 18, 13),
	RK3576_CRU_RESET_OFFSET(SRST_TIMER9, 18, 14),
	RK3576_CRU_RESET_OFFSET(SRST_TIMER10, 18, 15),

	/* SOFTRST_CON19 */
	RK3576_CRU_RESET_OFFSET(SRST_TIMER11, 19, 0),
	RK3576_CRU_RESET_OFFSET(SRST_A_DMAC0, 19, 1),
	RK3576_CRU_RESET_OFFSET(SRST_A_DMAC1, 19, 2),
	RK3576_CRU_RESET_OFFSET(SRST_A_DMAC2, 19, 3),
	RK3576_CRU_RESET_OFFSET(SRST_A_SPINLOCK, 19, 4),
	RK3576_CRU_RESET_OFFSET(SRST_REF_PVTPLL_BUS, 19, 5),
	RK3576_CRU_RESET_OFFSET(SRST_H_I3C0, 19, 7),
	RK3576_CRU_RESET_OFFSET(SRST_H_I3C1, 19, 9),
	RK3576_CRU_RESET_OFFSET(SRST_H_BUS_CM0_BIU, 19, 11),
	RK3576_CRU_RESET_OFFSET(SRST_F_BUS_CM0_CORE, 19, 12),
	RK3576_CRU_RESET_OFFSET(SRST_T_BUS_CM0_JTAG, 19, 13),

	/* SOFTRST_CON20 */
	RK3576_CRU_RESET_OFFSET(SRST_P_INTMUX2PMU, 20, 0),
	RK3576_CRU_RESET_OFFSET(SRST_P_INTMUX2DDR, 20, 1),
	RK3576_CRU_RESET_OFFSET(SRST_P_PVTPLL_BUS, 20, 3),
	RK3576_CRU_RESET_OFFSET(SRST_P_PWM2, 20, 4),
	RK3576_CRU_RESET_OFFSET(SRST_PWM2, 20, 5),
	RK3576_CRU_RESET_OFFSET(SRST_FREQ_PWM1, 20, 8),
	RK3576_CRU_RESET_OFFSET(SRST_COUNTER_PWM1, 20, 9),
	RK3576_CRU_RESET_OFFSET(SRST_I3C0, 20, 12),
	RK3576_CRU_RESET_OFFSET(SRST_I3C1, 20, 13),

	/* SOFTRST_CON21 */
	RK3576_CRU_RESET_OFFSET(SRST_P_DDR_MON_CH0, 21, 1),
	RK3576_CRU_RESET_OFFSET(SRST_P_DDR_BIU, 21, 2),
	RK3576_CRU_RESET_OFFSET(SRST_P_DDR_UPCTL_CH0, 21, 3),
	RK3576_CRU_RESET_OFFSET(SRST_TM_DDR_MON_CH0, 21, 4),
	RK3576_CRU_RESET_OFFSET(SRST_A_DDR_BIU, 21, 5),
	RK3576_CRU_RESET_OFFSET(SRST_DFI_CH0, 21, 6),
	RK3576_CRU_RESET_OFFSET(SRST_DDR_MON_CH0, 21, 10),
	RK3576_CRU_RESET_OFFSET(SRST_P_DDR_HWLP_CH0, 21, 13),
	RK3576_CRU_RESET_OFFSET(SRST_P_DDR_MON_CH1, 21, 14),
	RK3576_CRU_RESET_OFFSET(SRST_P_DDR_HWLP_CH1, 21, 15),

	/* SOFTRST_CON22 */
	RK3576_CRU_RESET_OFFSET(SRST_P_DDR_UPCTL_CH1, 22, 0),
	RK3576_CRU_RESET_OFFSET(SRST_TM_DDR_MON_CH1, 22, 1),
	RK3576_CRU_RESET_OFFSET(SRST_DFI_CH1, 22, 2),
	RK3576_CRU_RESET_OFFSET(SRST_A_DDR01_MSCH0, 22, 3),
	RK3576_CRU_RESET_OFFSET(SRST_A_DDR01_MSCH1, 22, 4),
	RK3576_CRU_RESET_OFFSET(SRST_DDR_MON_CH1, 22, 6),
	RK3576_CRU_RESET_OFFSET(SRST_DDR_SCRAMBLE_CH0, 22, 9),
	RK3576_CRU_RESET_OFFSET(SRST_DDR_SCRAMBLE_CH1, 22, 10),
	RK3576_CRU_RESET_OFFSET(SRST_P_AHB2APB, 22, 12),
	RK3576_CRU_RESET_OFFSET(SRST_H_AHB2APB, 22, 13),
	RK3576_CRU_RESET_OFFSET(SRST_H_DDR_BIU, 22, 14),
	RK3576_CRU_RESET_OFFSET(SRST_F_DDR_CM0_CORE, 22, 15),

	/* SOFTRST_CON23 */
	RK3576_CRU_RESET_OFFSET(SRST_P_DDR01_MSCH0, 23, 1),
	RK3576_CRU_RESET_OFFSET(SRST_P_DDR01_MSCH1, 23, 2),
	RK3576_CRU_RESET_OFFSET(SRST_DDR_TIMER0, 23, 4),
	RK3576_CRU_RESET_OFFSET(SRST_DDR_TIMER1, 23, 5),
	RK3576_CRU_RESET_OFFSET(SRST_T_WDT_DDR, 23, 6),
	RK3576_CRU_RESET_OFFSET(SRST_P_WDT, 23, 7),
	RK3576_CRU_RESET_OFFSET(SRST_P_TIMER, 23, 8),
	RK3576_CRU_RESET_OFFSET(SRST_T_DDR_CM0_JTAG, 23, 9),
	RK3576_CRU_RESET_OFFSET(SRST_P_DDR_GRF, 23, 11),

	/* SOFTRST_CON25 */
	RK3576_CRU_RESET_OFFSET(SRST_DDR_UPCTL_CH0, 25, 1),
	RK3576_CRU_RESET_OFFSET(SRST_A_DDR_UPCTL_0_CH0, 25, 2),
	RK3576_CRU_RESET_OFFSET(SRST_A_DDR_UPCTL_1_CH0, 25, 3),
	RK3576_CRU_RESET_OFFSET(SRST_A_DDR_UPCTL_2_CH0, 25, 4),
	RK3576_CRU_RESET_OFFSET(SRST_A_DDR_UPCTL_3_CH0, 25, 5),
	RK3576_CRU_RESET_OFFSET(SRST_A_DDR_UPCTL_4_CH0, 25, 6),

	/* SOFTRST_CON26 */
	RK3576_CRU_RESET_OFFSET(SRST_DDR_UPCTL_CH1, 26, 1),
	RK3576_CRU_RESET_OFFSET(SRST_A_DDR_UPCTL_0_CH1, 26, 2),
	RK3576_CRU_RESET_OFFSET(SRST_A_DDR_UPCTL_1_CH1, 26, 3),
	RK3576_CRU_RESET_OFFSET(SRST_A_DDR_UPCTL_2_CH1, 26, 4),
	RK3576_CRU_RESET_OFFSET(SRST_A_DDR_UPCTL_3_CH1, 26, 5),
	RK3576_CRU_RESET_OFFSET(SRST_A_DDR_UPCTL_4_CH1, 26, 6),

	/* SOFTRST_CON27 */
	RK3576_CRU_RESET_OFFSET(SRST_REF_PVTPLL_DDR, 27, 0),
	RK3576_CRU_RESET_OFFSET(SRST_P_PVTPLL_DDR, 27, 1),

	/* SOFTRST_CON28 */
	RK3576_CRU_RESET_OFFSET(SRST_A_RKNN0, 28, 9),
	RK3576_CRU_RESET_OFFSET(SRST_A_RKNN0_BIU, 28, 11),
	RK3576_CRU_RESET_OFFSET(SRST_L_RKNN0_BIU, 28, 12),

	/* SOFTRST_CON29 */
	RK3576_CRU_RESET_OFFSET(SRST_A_RKNN1, 29, 0),
	RK3576_CRU_RESET_OFFSET(SRST_A_RKNN1_BIU, 29, 2),
	RK3576_CRU_RESET_OFFSET(SRST_L_RKNN1_BIU, 29, 3),

	/* SOFTRST_CON31 */
	RK3576_CRU_RESET_OFFSET(SRST_NPU_DAP, 31, 0),
	RK3576_CRU_RESET_OFFSET(SRST_L_NPUSUBSYS_BIU, 31, 1),
	RK3576_CRU_RESET_OFFSET(SRST_P_NPUTOP_BIU, 31, 9),
	RK3576_CRU_RESET_OFFSET(SRST_P_NPU_TIMER, 31, 10),
	RK3576_CRU_RESET_OFFSET(SRST_NPUTIMER0, 31, 12),
	RK3576_CRU_RESET_OFFSET(SRST_NPUTIMER1, 31, 13),
	RK3576_CRU_RESET_OFFSET(SRST_P_NPU_WDT, 31, 14),
	RK3576_CRU_RESET_OFFSET(SRST_T_NPU_WDT, 31, 15),

	/* SOFTRST_CON32 */
	RK3576_CRU_RESET_OFFSET(SRST_A_RKNN_CBUF, 32, 0),
	RK3576_CRU_RESET_OFFSET(SRST_A_RVCORE0, 32, 1),
	RK3576_CRU_RESET_OFFSET(SRST_P_NPU_GRF, 32, 2),
	RK3576_CRU_RESET_OFFSET(SRST_P_PVTPLL_NPU, 32, 3),
	RK3576_CRU_RESET_OFFSET(SRST_NPU_PVTPLL, 32, 4),
	RK3576_CRU_RESET_OFFSET(SRST_H_NPU_CM0_BIU, 32, 6),
	RK3576_CRU_RESET_OFFSET(SRST_F_NPU_CM0_CORE, 32, 7),
	RK3576_CRU_RESET_OFFSET(SRST_T_NPU_CM0_JTAG, 32, 8),
	RK3576_CRU_RESET_OFFSET(SRST_A_RKNNTOP_BIU, 32, 11),
	RK3576_CRU_RESET_OFFSET(SRST_H_RKNN_CBUF, 32, 12),
	RK3576_CRU_RESET_OFFSET(SRST_H_RKNNTOP_BIU, 32, 13),

	/* SOFTRST_CON33 */
	RK3576_CRU_RESET_OFFSET(SRST_H_NVM_BIU, 33, 2),
	RK3576_CRU_RESET_OFFSET(SRST_A_NVM_BIU, 33, 3),
	RK3576_CRU_RESET_OFFSET(SRST_S_FSPI, 33, 6),
	RK3576_CRU_RESET_OFFSET(SRST_H_FSPI, 33, 7),
	RK3576_CRU_RESET_OFFSET(SRST_C_EMMC, 33, 8),
	RK3576_CRU_RESET_OFFSET(SRST_H_EMMC, 33, 9),
	RK3576_CRU_RESET_OFFSET(SRST_A_EMMC, 33, 10),
	RK3576_CRU_RESET_OFFSET(SRST_B_EMMC, 33, 11),
	RK3576_CRU_RESET_OFFSET(SRST_T_EMMC, 33, 12),

	/* SOFTRST_CON34 */
	RK3576_CRU_RESET_OFFSET(SRST_P_GRF, 34, 1),
	RK3576_CRU_RESET_OFFSET(SRST_P_PHP_BIU, 34, 5),
	RK3576_CRU_RESET_OFFSET(SRST_A_PHP_BIU, 34, 9),
	RK3576_CRU_RESET_OFFSET(SRST_P_PCIE0, 34, 13),
	RK3576_CRU_RESET_OFFSET(SRST_PCIE0_POWER_UP, 34, 15),

	/* SOFTRST_CON35 */
	RK3576_CRU_RESET_OFFSET(SRST_A_USB3OTG1, 35, 3),
	RK3576_CRU_RESET_OFFSET(SRST_A_MMU0, 35, 11),
	RK3576_CRU_RESET_OFFSET(SRST_A_SLV_MMU0, 35, 13),
	RK3576_CRU_RESET_OFFSET(SRST_A_MMU1, 35, 14),

	/* SOFTRST_CON36 */
	RK3576_CRU_RESET_OFFSET(SRST_A_SLV_MMU1, 36, 0),
	RK3576_CRU_RESET_OFFSET(SRST_P_PCIE1, 36, 7),
	RK3576_CRU_RESET_OFFSET(SRST_PCIE1_POWER_UP, 36, 9),

	/* SOFTRST_CON37 */
	RK3576_CRU_RESET_OFFSET(SRST_RXOOB0, 37, 0),
	RK3576_CRU_RESET_OFFSET(SRST_RXOOB1, 37, 1),
	RK3576_CRU_RESET_OFFSET(SRST_PMALIVE0, 37, 2),
	RK3576_CRU_RESET_OFFSET(SRST_PMALIVE1, 37, 3),
	RK3576_CRU_RESET_OFFSET(SRST_A_SATA0, 37, 4),
	RK3576_CRU_RESET_OFFSET(SRST_A_SATA1, 37, 5),
	RK3576_CRU_RESET_OFFSET(SRST_ASIC1, 37, 6),
	RK3576_CRU_RESET_OFFSET(SRST_ASIC0, 37, 7),

	/* SOFTRST_CON40 */
	RK3576_CRU_RESET_OFFSET(SRST_P_CSIDPHY1, 40, 2),
	RK3576_CRU_RESET_OFFSET(SRST_SCAN_CSIDPHY1, 40, 3),

	/* SOFTRST_CON42 */
	RK3576_CRU_RESET_OFFSET(SRST_P_SDGMAC_GRF, 42, 3),
	RK3576_CRU_RESET_OFFSET(SRST_P_SDGMAC_BIU, 42, 4),
	RK3576_CRU_RESET_OFFSET(SRST_A_SDGMAC_BIU, 42, 5),
	RK3576_CRU_RESET_OFFSET(SRST_H_SDGMAC_BIU, 42, 6),
	RK3576_CRU_RESET_OFFSET(SRST_A_GMAC0, 42, 7),
	RK3576_CRU_RESET_OFFSET(SRST_A_GMAC1, 42, 8),
	RK3576_CRU_RESET_OFFSET(SRST_P_GMAC0, 42, 9),
	RK3576_CRU_RESET_OFFSET(SRST_P_GMAC1, 42, 10),
	RK3576_CRU_RESET_OFFSET(SRST_H_SDIO, 42, 12),

	/* SOFTRST_CON43 */
	RK3576_CRU_RESET_OFFSET(SRST_H_SDMMC0, 43, 2),
	RK3576_CRU_RESET_OFFSET(SRST_S_FSPI1, 43, 3),
	RK3576_CRU_RESET_OFFSET(SRST_H_FSPI1, 43, 4),
	RK3576_CRU_RESET_OFFSET(SRST_A_DSMC_BIU, 43, 6),
	RK3576_CRU_RESET_OFFSET(SRST_A_DSMC, 43, 7),
	RK3576_CRU_RESET_OFFSET(SRST_P_DSMC, 43, 8),
	RK3576_CRU_RESET_OFFSET(SRST_H_HSGPIO, 43, 10),
	RK3576_CRU_RESET_OFFSET(SRST_HSGPIO, 43, 11),
	RK3576_CRU_RESET_OFFSET(SRST_A_HSGPIO, 43, 13),

	/* SOFTRST_CON45 */
	RK3576_CRU_RESET_OFFSET(SRST_H_RKVDEC, 45, 3),
	RK3576_CRU_RESET_OFFSET(SRST_H_RKVDEC_BIU, 45, 5),
	RK3576_CRU_RESET_OFFSET(SRST_A_RKVDEC_BIU, 45, 6),
	RK3576_CRU_RESET_OFFSET(SRST_RKVDEC_HEVC_CA, 45, 8),
	RK3576_CRU_RESET_OFFSET(SRST_RKVDEC_CORE, 45, 9),

	/* SOFTRST_CON47 */
	RK3576_CRU_RESET_OFFSET(SRST_A_USB_BIU, 47, 3),
	RK3576_CRU_RESET_OFFSET(SRST_P_USBUFS_BIU, 47, 4),
	RK3576_CRU_RESET_OFFSET(SRST_A_USB3OTG0, 47, 5),
	RK3576_CRU_RESET_OFFSET(SRST_A_UFS_BIU, 47, 10),
	RK3576_CRU_RESET_OFFSET(SRST_A_MMU2, 47, 12),
	RK3576_CRU_RESET_OFFSET(SRST_A_SLV_MMU2, 47, 13),
	RK3576_CRU_RESET_OFFSET(SRST_A_UFS_SYS, 47, 15),

	/* SOFTRST_CON48 */
	RK3576_CRU_RESET_OFFSET(SRST_A_UFS, 48, 0),
	RK3576_CRU_RESET_OFFSET(SRST_P_USBUFS_GRF, 48, 1),
	RK3576_CRU_RESET_OFFSET(SRST_P_UFS_GRF, 48, 2),

	/* SOFTRST_CON49 */
	RK3576_CRU_RESET_OFFSET(SRST_H_VPU_BIU, 49, 6),
	RK3576_CRU_RESET_OFFSET(SRST_A_JPEG_BIU, 49, 7),
	RK3576_CRU_RESET_OFFSET(SRST_A_RGA_BIU, 49, 10),
	RK3576_CRU_RESET_OFFSET(SRST_A_VDPP_BIU, 49, 11),
	RK3576_CRU_RESET_OFFSET(SRST_A_EBC_BIU, 49, 12),
	RK3576_CRU_RESET_OFFSET(SRST_H_RGA2E_0, 49, 13),
	RK3576_CRU_RESET_OFFSET(SRST_A_RGA2E_0, 49, 14),
	RK3576_CRU_RESET_OFFSET(SRST_CORE_RGA2E_0, 49, 15),

	/* SOFTRST_CON50 */
	RK3576_CRU_RESET_OFFSET(SRST_A_JPEG, 50, 0),
	RK3576_CRU_RESET_OFFSET(SRST_H_JPEG, 50, 1),
	RK3576_CRU_RESET_OFFSET(SRST_H_VDPP, 50, 2),
	RK3576_CRU_RESET_OFFSET(SRST_A_VDPP, 50, 3),
	RK3576_CRU_RESET_OFFSET(SRST_CORE_VDPP, 50, 4),
	RK3576_CRU_RESET_OFFSET(SRST_H_RGA2E_1, 50, 5),
	RK3576_CRU_RESET_OFFSET(SRST_A_RGA2E_1, 50, 6),
	RK3576_CRU_RESET_OFFSET(SRST_CORE_RGA2E_1, 50, 7),
	RK3576_CRU_RESET_OFFSET(SRST_H_EBC, 50, 10),
	RK3576_CRU_RESET_OFFSET(SRST_A_EBC, 50, 11),
	RK3576_CRU_RESET_OFFSET(SRST_D_EBC, 50, 12),

	/* SOFTRST_CON51 */
	RK3576_CRU_RESET_OFFSET(SRST_H_VEPU0_BIU, 51, 2),
	RK3576_CRU_RESET_OFFSET(SRST_A_VEPU0_BIU, 51, 3),
	RK3576_CRU_RESET_OFFSET(SRST_H_VEPU0, 51, 4),
	RK3576_CRU_RESET_OFFSET(SRST_A_VEPU0, 51, 5),
	RK3576_CRU_RESET_OFFSET(SRST_VEPU0_CORE, 51, 6),

	/* SOFTRST_CON53 */
	RK3576_CRU_RESET_OFFSET(SRST_A_VI_BIU, 53, 3),
	RK3576_CRU_RESET_OFFSET(SRST_H_VI_BIU, 53, 4),
	RK3576_CRU_RESET_OFFSET(SRST_P_VI_BIU, 53, 5),
	RK3576_CRU_RESET_OFFSET(SRST_D_VICAP, 53, 6),
	RK3576_CRU_RESET_OFFSET(SRST_A_VICAP, 53, 7),
	RK3576_CRU_RESET_OFFSET(SRST_H_VICAP, 53, 8),
	RK3576_CRU_RESET_OFFSET(SRST_ISP0, 53, 10),
	RK3576_CRU_RESET_OFFSET(SRST_ISP0_VICAP, 53, 11),

	/* SOFTRST_CON54 */
	RK3576_CRU_RESET_OFFSET(SRST_CORE_VPSS, 54, 1),
	RK3576_CRU_RESET_OFFSET(SRST_P_CSI_HOST_0, 54, 4),
	RK3576_CRU_RESET_OFFSET(SRST_P_CSI_HOST_1, 54, 5),
	RK3576_CRU_RESET_OFFSET(SRST_P_CSI_HOST_2, 54, 6),
	RK3576_CRU_RESET_OFFSET(SRST_P_CSI_HOST_3, 54, 7),
	RK3576_CRU_RESET_OFFSET(SRST_P_CSI_HOST_4, 54, 8),

	/* SOFTRST_CON59 */
	RK3576_CRU_RESET_OFFSET(SRST_CIFIN, 59, 0),
	RK3576_CRU_RESET_OFFSET(SRST_VICAP_I0CLK, 59, 1),
	RK3576_CRU_RESET_OFFSET(SRST_VICAP_I1CLK, 59, 2),
	RK3576_CRU_RESET_OFFSET(SRST_VICAP_I2CLK, 59, 3),
	RK3576_CRU_RESET_OFFSET(SRST_VICAP_I3CLK, 59, 4),
	RK3576_CRU_RESET_OFFSET(SRST_VICAP_I4CLK, 59, 5),

	/* SOFTRST_CON61 */
	RK3576_CRU_RESET_OFFSET(SRST_A_VOP_BIU, 61, 4),
	RK3576_CRU_RESET_OFFSET(SRST_A_VOP2_BIU, 61, 5),
	RK3576_CRU_RESET_OFFSET(SRST_H_VOP_BIU, 61, 6),
	RK3576_CRU_RESET_OFFSET(SRST_P_VOP_BIU, 61, 7),
	RK3576_CRU_RESET_OFFSET(SRST_H_VOP, 61, 8),
	RK3576_CRU_RESET_OFFSET(SRST_A_VOP, 61, 9),
	RK3576_CRU_RESET_OFFSET(SRST_D_VP0, 61, 13),

	/* SOFTRST_CON62 */
	RK3576_CRU_RESET_OFFSET(SRST_D_VP1, 62, 0),
	RK3576_CRU_RESET_OFFSET(SRST_D_VP2, 62, 1),
	RK3576_CRU_RESET_OFFSET(SRST_P_VOP2_BIU, 62, 2),
	RK3576_CRU_RESET_OFFSET(SRST_P_VOPGRF, 62, 3),

	/* SOFTRST_CON63 */
	RK3576_CRU_RESET_OFFSET(SRST_H_VO0_BIU, 63, 5),
	RK3576_CRU_RESET_OFFSET(SRST_P_VO0_BIU, 63, 7),
	RK3576_CRU_RESET_OFFSET(SRST_A_HDCP0_BIU, 63, 9),
	RK3576_CRU_RESET_OFFSET(SRST_P_VO0_GRF, 63, 10),
	RK3576_CRU_RESET_OFFSET(SRST_A_HDCP0, 63, 12),
	RK3576_CRU_RESET_OFFSET(SRST_H_HDCP0, 63, 13),
	RK3576_CRU_RESET_OFFSET(SRST_HDCP0, 63, 14),

	/* SOFTRST_CON64 */
	RK3576_CRU_RESET_OFFSET(SRST_P_DSIHOST0, 64, 5),
	RK3576_CRU_RESET_OFFSET(SRST_DSIHOST0, 64, 6),
	RK3576_CRU_RESET_OFFSET(SRST_P_HDMITX0, 64, 7),
	RK3576_CRU_RESET_OFFSET(SRST_HDMITX0_REF, 64, 9),
	RK3576_CRU_RESET_OFFSET(SRST_P_EDP0, 64, 13),
	RK3576_CRU_RESET_OFFSET(SRST_EDP0_24M, 64, 14),

	/* SOFTRST_CON65 */
	RK3576_CRU_RESET_OFFSET(SRST_M_SAI5_8CH, 65, 4),
	RK3576_CRU_RESET_OFFSET(SRST_H_SAI5_8CH, 65, 5),
	RK3576_CRU_RESET_OFFSET(SRST_M_SAI6_8CH, 65, 8),
	RK3576_CRU_RESET_OFFSET(SRST_H_SAI6_8CH, 65, 9),
	RK3576_CRU_RESET_OFFSET(SRST_H_SPDIF_TX2, 65, 10),
	RK3576_CRU_RESET_OFFSET(SRST_M_SPDIF_TX2, 65, 13),
	RK3576_CRU_RESET_OFFSET(SRST_H_SPDIF_RX2, 65, 14),
	RK3576_CRU_RESET_OFFSET(SRST_M_SPDIF_RX2, 65, 15),

	/* SOFTRST_CON66 */
	RK3576_CRU_RESET_OFFSET(SRST_H_SAI8_8CH, 66, 0),
	RK3576_CRU_RESET_OFFSET(SRST_M_SAI8_8CH, 66, 2),

	/* SOFTRST_CON67 */
	RK3576_CRU_RESET_OFFSET(SRST_H_VO1_BIU, 67, 5),
	RK3576_CRU_RESET_OFFSET(SRST_P_VO1_BIU, 67, 6),
	RK3576_CRU_RESET_OFFSET(SRST_M_SAI7_8CH, 67, 9),
	RK3576_CRU_RESET_OFFSET(SRST_H_SAI7_8CH, 67, 10),
	RK3576_CRU_RESET_OFFSET(SRST_H_SPDIF_TX3, 67, 11),
	RK3576_CRU_RESET_OFFSET(SRST_H_SPDIF_TX4, 67, 12),
	RK3576_CRU_RESET_OFFSET(SRST_H_SPDIF_TX5, 67, 13),
	RK3576_CRU_RESET_OFFSET(SRST_M_SPDIF_TX3, 67, 14),

	/* SOFTRST_CON68 */
	RK3576_CRU_RESET_OFFSET(SRST_DP0, 68, 0),
	RK3576_CRU_RESET_OFFSET(SRST_P_VO1_GRF, 68, 2),
	RK3576_CRU_RESET_OFFSET(SRST_A_HDCP1_BIU, 68, 3),
	RK3576_CRU_RESET_OFFSET(SRST_A_HDCP1, 68, 4),
	RK3576_CRU_RESET_OFFSET(SRST_H_HDCP1, 68, 5),
	RK3576_CRU_RESET_OFFSET(SRST_HDCP1, 68, 6),
	RK3576_CRU_RESET_OFFSET(SRST_H_SAI9_8CH, 68, 9),
	RK3576_CRU_RESET_OFFSET(SRST_M_SAI9_8CH, 68, 11),
	RK3576_CRU_RESET_OFFSET(SRST_M_SPDIF_TX4, 68, 12),
	RK3576_CRU_RESET_OFFSET(SRST_M_SPDIF_TX5, 68, 13),

	/* SOFTRST_CON69 */
	RK3576_CRU_RESET_OFFSET(SRST_GPU, 69, 3),
	RK3576_CRU_RESET_OFFSET(SRST_A_S_GPU_BIU, 69, 6),
	RK3576_CRU_RESET_OFFSET(SRST_A_M0_GPU_BIU, 69, 7),
	RK3576_CRU_RESET_OFFSET(SRST_P_GPU_BIU, 69, 9),
	RK3576_CRU_RESET_OFFSET(SRST_P_GPU_GRF, 69, 13),
	RK3576_CRU_RESET_OFFSET(SRST_GPU_PVTPLL, 69, 14),
	RK3576_CRU_RESET_OFFSET(SRST_P_PVTPLL_GPU, 69, 15),

	/* SOFTRST_CON72 */
	RK3576_CRU_RESET_OFFSET(SRST_A_CENTER_BIU, 72, 4),
	RK3576_CRU_RESET_OFFSET(SRST_A_DMA2DDR, 72, 5),
	RK3576_CRU_RESET_OFFSET(SRST_A_DDR_SHAREMEM, 72, 6),
	RK3576_CRU_RESET_OFFSET(SRST_A_DDR_SHAREMEM_BIU, 72, 7),
	RK3576_CRU_RESET_OFFSET(SRST_H_CENTER_BIU, 72, 8),
	RK3576_CRU_RESET_OFFSET(SRST_P_CENTER_GRF, 72, 9),
	RK3576_CRU_RESET_OFFSET(SRST_P_DMA2DDR, 72, 10),
	RK3576_CRU_RESET_OFFSET(SRST_P_SHAREMEM, 72, 11),
	RK3576_CRU_RESET_OFFSET(SRST_P_CENTER_BIU, 72, 12),

	/* SOFTRST_CON75 */
	RK3576_CRU_RESET_OFFSET(SRST_LINKSYM_HDMITXPHY0, 75, 1),

	/* SOFTRST_CON78 */
	RK3576_CRU_RESET_OFFSET(SRST_DP0_PIXELCLK, 78, 1),
	RK3576_CRU_RESET_OFFSET(SRST_PHY_DP0_TX, 78, 2),
	RK3576_CRU_RESET_OFFSET(SRST_DP1_PIXELCLK, 78, 3),
	RK3576_CRU_RESET_OFFSET(SRST_DP2_PIXELCLK, 78, 4),

	/* SOFTRST_CON79 */
	RK3576_CRU_RESET_OFFSET(SRST_H_VEPU1_BIU, 79, 1),
	RK3576_CRU_RESET_OFFSET(SRST_A_VEPU1_BIU, 79, 2),
	RK3576_CRU_RESET_OFFSET(SRST_H_VEPU1, 79, 3),
	RK3576_CRU_RESET_OFFSET(SRST_A_VEPU1, 79, 4),
	RK3576_CRU_RESET_OFFSET(SRST_VEPU1_CORE, 79, 5),

	/* PPLL_SOFTRST_CON00 */
	RK3576_PHPCRU_RESET_OFFSET(SRST_P_PHPPHY_CRU, 0, 1),
	RK3576_PHPCRU_RESET_OFFSET(SRST_P_APB2ASB_SLV_CHIP_TOP, 0, 3),
	RK3576_PHPCRU_RESET_OFFSET(SRST_P_PCIE2_COMBOPHY0, 0, 5),
	RK3576_PHPCRU_RESET_OFFSET(SRST_P_PCIE2_COMBOPHY0_GRF, 0, 6),
	RK3576_PHPCRU_RESET_OFFSET(SRST_P_PCIE2_COMBOPHY1, 0, 7),
	RK3576_PHPCRU_RESET_OFFSET(SRST_P_PCIE2_COMBOPHY1_GRF, 0, 8),

	/* PPLL_SOFTRST_CON01 */
	RK3576_PHPCRU_RESET_OFFSET(SRST_PCIE0_PIPE_PHY, 1, 5),
	RK3576_PHPCRU_RESET_OFFSET(SRST_PCIE1_PIPE_PHY, 1, 8),

	/* SECURENS_SOFTRST_CON00 */
	RK3576_SECURENSCRU_RESET_OFFSET(SRST_H_CRYPTO_NS, 0, 3),
	RK3576_SECURENSCRU_RESET_OFFSET(SRST_H_TRNG_NS, 0, 4),
	RK3576_SECURENSCRU_RESET_OFFSET(SRST_P_OTPC_NS, 0, 8),
	RK3576_SECURENSCRU_RESET_OFFSET(SRST_OTPC_NS, 0, 9),

	/* PMU1_SOFTRST_CON00 */
	RK3576_PMU1CRU_RESET_OFFSET(SRST_P_HDPTX_GRF, 0, 0),
	RK3576_PMU1CRU_RESET_OFFSET(SRST_P_HDPTX_APB, 0, 1),
	RK3576_PMU1CRU_RESET_OFFSET(SRST_P_MIPI_DCPHY, 0, 2),
	RK3576_PMU1CRU_RESET_OFFSET(SRST_P_DCPHY_GRF, 0, 3),
	RK3576_PMU1CRU_RESET_OFFSET(SRST_P_BOT0_APB2ASB, 0, 4),
	RK3576_PMU1CRU_RESET_OFFSET(SRST_P_BOT1_APB2ASB, 0, 5),
	RK3576_PMU1CRU_RESET_OFFSET(SRST_USB2DEBUG, 0, 6),
	RK3576_PMU1CRU_RESET_OFFSET(SRST_P_CSIPHY_GRF, 0, 7),
	RK3576_PMU1CRU_RESET_OFFSET(SRST_P_CSIPHY, 0, 8),
	RK3576_PMU1CRU_RESET_OFFSET(SRST_P_USBPHY_GRF_0, 0, 9),
	RK3576_PMU1CRU_RESET_OFFSET(SRST_P_USBPHY_GRF_1, 0, 10),
	RK3576_PMU1CRU_RESET_OFFSET(SRST_P_USBDP_GRF, 0, 11),
	RK3576_PMU1CRU_RESET_OFFSET(SRST_P_USBDPPHY, 0, 12),
	RK3576_PMU1CRU_RESET_OFFSET(SRST_USBDP_COMBO_PHY_INIT, 0, 15),

	/* PMU1_SOFTRST_CON01 */
	RK3576_PMU1CRU_RESET_OFFSET(SRST_USBDP_COMBO_PHY_CMN, 1, 0),
	RK3576_PMU1CRU_RESET_OFFSET(SRST_USBDP_COMBO_PHY_LANE, 1, 1),
	RK3576_PMU1CRU_RESET_OFFSET(SRST_USBDP_COMBO_PHY_PCS, 1, 2),
	RK3576_PMU1CRU_RESET_OFFSET(SRST_M_MIPI_DCPHY, 1, 3),
	RK3576_PMU1CRU_RESET_OFFSET(SRST_S_MIPI_DCPHY, 1, 4),
	RK3576_PMU1CRU_RESET_OFFSET(SRST_SCAN_CSIPHY, 1, 5),
	RK3576_PMU1CRU_RESET_OFFSET(SRST_P_VCCIO6_IOC, 1, 6),
	RK3576_PMU1CRU_RESET_OFFSET(SRST_OTGPHY_0, 1, 7),
	RK3576_PMU1CRU_RESET_OFFSET(SRST_OTGPHY_1, 1, 8),
	RK3576_PMU1CRU_RESET_OFFSET(SRST_HDPTX_INIT, 1, 9),
	RK3576_PMU1CRU_RESET_OFFSET(SRST_HDPTX_CMN, 1, 10),
	RK3576_PMU1CRU_RESET_OFFSET(SRST_HDPTX_LANE, 1, 11),
	RK3576_PMU1CRU_RESET_OFFSET(SRST_HDMITXHDP, 1, 13),

	/* PMU1_SOFTRST_CON02 */
	RK3576_PMU1CRU_RESET_OFFSET(SRST_MPHY_INIT, 2, 0),
	RK3576_PMU1CRU_RESET_OFFSET(SRST_P_MPHY_GRF, 2, 1),
	RK3576_PMU1CRU_RESET_OFFSET(SRST_P_VCCIO7_IOC, 2, 3),

	/* PMU1_SOFTRST_CON03 */
	RK3576_PMU1CRU_RESET_OFFSET(SRST_H_PMU1_BIU, 3, 9),
	RK3576_PMU1CRU_RESET_OFFSET(SRST_P_PMU1_NIU, 3, 10),
	RK3576_PMU1CRU_RESET_OFFSET(SRST_H_PMU_CM0_BIU, 3, 11),
	RK3576_PMU1CRU_RESET_OFFSET(SRST_PMU_CM0_CORE, 3, 12),
	RK3576_PMU1CRU_RESET_OFFSET(SRST_PMU_CM0_JTAG, 3, 13),

	/* PMU1_SOFTRST_CON04 */
	RK3576_PMU1CRU_RESET_OFFSET(SRST_P_CRU_PMU1, 4, 1),
	RK3576_PMU1CRU_RESET_OFFSET(SRST_P_PMU1_GRF, 4, 3),
	RK3576_PMU1CRU_RESET_OFFSET(SRST_P_PMU1_IOC, 4, 4),
	RK3576_PMU1CRU_RESET_OFFSET(SRST_P_PMU1WDT, 4, 5),
	RK3576_PMU1CRU_RESET_OFFSET(SRST_T_PMU1WDT, 4, 6),
	RK3576_PMU1CRU_RESET_OFFSET(SRST_P_PMUTIMER, 4, 7),
	RK3576_PMU1CRU_RESET_OFFSET(SRST_PMUTIMER0, 4, 9),
	RK3576_PMU1CRU_RESET_OFFSET(SRST_PMUTIMER1, 4, 10),
	RK3576_PMU1CRU_RESET_OFFSET(SRST_P_PMU1PWM, 4, 11),
	RK3576_PMU1CRU_RESET_OFFSET(SRST_PMU1PWM, 4, 12),

	/* PMU1_SOFTRST_CON05 */
	RK3576_PMU1CRU_RESET_OFFSET(SRST_P_I2C0, 5, 1),
	RK3576_PMU1CRU_RESET_OFFSET(SRST_I2C0, 5, 2),
	RK3576_PMU1CRU_RESET_OFFSET(SRST_S_UART1, 5, 5),
	RK3576_PMU1CRU_RESET_OFFSET(SRST_P_UART1, 5, 6),
	RK3576_PMU1CRU_RESET_OFFSET(SRST_PDM0, 5, 13),
	RK3576_PMU1CRU_RESET_OFFSET(SRST_H_PDM0, 5, 15),

	/* PMU1_SOFTRST_CON06 */
	RK3576_PMU1CRU_RESET_OFFSET(SRST_M_PDM0, 6, 0),
	RK3576_PMU1CRU_RESET_OFFSET(SRST_H_VAD, 6, 1),

	/* PMU1_SOFTRST_CON07 */
	RK3576_PMU1CRU_RESET_OFFSET(SRST_P_PMU0GRF, 7, 4),
	RK3576_PMU1CRU_RESET_OFFSET(SRST_P_PMU0IOC, 7, 5),
	RK3576_PMU1CRU_RESET_OFFSET(SRST_P_GPIO0, 7, 6),
	RK3576_PMU1CRU_RESET_OFFSET(SRST_DB_GPIO0, 7, 7),
};

void rk3576_rst_init(struct device_node *np, void __iomem *reg_base)
{
	rockchip_register_softrst_lut(np,
				      rk3576_register_offset,
				      ARRAY_SIZE(rk3576_register_offset),
				      reg_base + RK3576_SOFTRST_CON(0),
				      ROCKCHIP_SOFTRST_HIWORD_MASK);
}
