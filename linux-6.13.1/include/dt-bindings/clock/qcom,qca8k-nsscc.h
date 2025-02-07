/* SPDX-License-Identifier: (GPL-2.0-only OR BSD-2-Clause) */
/*
 * Copyright (c) 2023 Qualcomm Innovation Center, Inc. All rights reserved.
 */

#ifndef _DT_BINDINGS_CLK_QCOM_QCA8K_NSS_CC_H
#define _DT_BINDINGS_CLK_QCOM_QCA8K_NSS_CC_H

#define NSS_CC_SWITCH_CORE_CLK_SRC				0
#define NSS_CC_SWITCH_CORE_CLK					1
#define NSS_CC_APB_BRIDGE_CLK					2
#define NSS_CC_MAC0_TX_CLK_SRC					3
#define NSS_CC_MAC0_TX_DIV_CLK_SRC				4
#define NSS_CC_MAC0_TX_CLK					5
#define NSS_CC_MAC0_TX_SRDS1_CLK				6
#define NSS_CC_MAC0_RX_CLK_SRC					7
#define NSS_CC_MAC0_RX_DIV_CLK_SRC				8
#define NSS_CC_MAC0_RX_CLK					9
#define NSS_CC_MAC0_RX_SRDS1_CLK				10
#define NSS_CC_MAC1_TX_CLK_SRC					11
#define NSS_CC_MAC1_TX_DIV_CLK_SRC				12
#define NSS_CC_MAC1_SRDS1_CH0_XGMII_RX_DIV_CLK_SRC		13
#define NSS_CC_MAC1_SRDS1_CH0_RX_CLK				14
#define NSS_CC_MAC1_TX_CLK					15
#define NSS_CC_MAC1_GEPHY0_TX_CLK				16
#define NSS_CC_MAC1_SRDS1_CH0_XGMII_RX_CLK			17
#define NSS_CC_MAC1_RX_CLK_SRC					18
#define NSS_CC_MAC1_RX_DIV_CLK_SRC				19
#define NSS_CC_MAC1_SRDS1_CH0_XGMII_TX_DIV_CLK_SRC		20
#define NSS_CC_MAC1_SRDS1_CH0_TX_CLK				21
#define NSS_CC_MAC1_RX_CLK					22
#define NSS_CC_MAC1_GEPHY0_RX_CLK				23
#define NSS_CC_MAC1_SRDS1_CH0_XGMII_TX_CLK			24
#define NSS_CC_MAC2_TX_CLK_SRC					25
#define NSS_CC_MAC2_TX_DIV_CLK_SRC				26
#define NSS_CC_MAC2_SRDS1_CH1_XGMII_RX_DIV_CLK_SRC		27
#define NSS_CC_MAC2_SRDS1_CH1_RX_CLK				28
#define NSS_CC_MAC2_TX_CLK					29
#define NSS_CC_MAC2_GEPHY1_TX_CLK				30
#define NSS_CC_MAC2_SRDS1_CH1_XGMII_RX_CLK			31
#define NSS_CC_MAC2_RX_CLK_SRC					32
#define NSS_CC_MAC2_RX_DIV_CLK_SRC				33
#define NSS_CC_MAC2_SRDS1_CH1_XGMII_TX_DIV_CLK_SRC		34
#define NSS_CC_MAC2_SRDS1_CH1_TX_CLK				35
#define NSS_CC_MAC2_RX_CLK					36
#define NSS_CC_MAC2_GEPHY1_RX_CLK				37
#define NSS_CC_MAC2_SRDS1_CH1_XGMII_TX_CLK			38
#define NSS_CC_MAC3_TX_CLK_SRC					39
#define NSS_CC_MAC3_TX_DIV_CLK_SRC				40
#define NSS_CC_MAC3_SRDS1_CH2_XGMII_RX_DIV_CLK_SRC		41
#define NSS_CC_MAC3_SRDS1_CH2_RX_CLK				42
#define NSS_CC_MAC3_TX_CLK					43
#define NSS_CC_MAC3_GEPHY2_TX_CLK				44
#define NSS_CC_MAC3_SRDS1_CH2_XGMII_RX_CLK			45
#define NSS_CC_MAC3_RX_CLK_SRC					46
#define NSS_CC_MAC3_RX_DIV_CLK_SRC				47
#define NSS_CC_MAC3_SRDS1_CH2_XGMII_TX_DIV_CLK_SRC		48
#define NSS_CC_MAC3_SRDS1_CH2_TX_CLK				49
#define NSS_CC_MAC3_RX_CLK					50
#define NSS_CC_MAC3_GEPHY2_RX_CLK				51
#define NSS_CC_MAC3_SRDS1_CH2_XGMII_TX_CLK			52
#define NSS_CC_MAC4_TX_CLK_SRC					53
#define NSS_CC_MAC4_TX_DIV_CLK_SRC				54
#define NSS_CC_MAC4_SRDS1_CH3_XGMII_RX_DIV_CLK_SRC		55
#define NSS_CC_MAC4_SRDS1_CH3_RX_CLK				56
#define NSS_CC_MAC4_TX_CLK					57
#define NSS_CC_MAC4_GEPHY3_TX_CLK				58
#define NSS_CC_MAC4_SRDS1_CH3_XGMII_RX_CLK			59
#define NSS_CC_MAC4_RX_CLK_SRC					60
#define NSS_CC_MAC4_RX_DIV_CLK_SRC				61
#define NSS_CC_MAC4_SRDS1_CH3_XGMII_TX_DIV_CLK_SRC		62
#define NSS_CC_MAC4_SRDS1_CH3_TX_CLK				63
#define NSS_CC_MAC4_RX_CLK					64
#define NSS_CC_MAC4_GEPHY3_RX_CLK				65
#define NSS_CC_MAC4_SRDS1_CH3_XGMII_TX_CLK			66
#define NSS_CC_MAC5_TX_CLK_SRC					67
#define NSS_CC_MAC5_TX_DIV_CLK_SRC				68
#define NSS_CC_MAC5_TX_SRDS0_CLK				69
#define NSS_CC_MAC5_TX_CLK					70
#define NSS_CC_MAC5_RX_CLK_SRC					71
#define NSS_CC_MAC5_RX_DIV_CLK_SRC				72
#define NSS_CC_MAC5_RX_SRDS0_CLK				73
#define NSS_CC_MAC5_RX_CLK					74
#define NSS_CC_MAC5_TX_SRDS0_CLK_SRC				75
#define NSS_CC_MAC5_RX_SRDS0_CLK_SRC				76
#define NSS_CC_AHB_CLK_SRC					77
#define NSS_CC_AHB_CLK						78
#define NSS_CC_SEC_CTRL_AHB_CLK					79
#define NSS_CC_TLMM_CLK						80
#define NSS_CC_TLMM_AHB_CLK					81
#define NSS_CC_CNOC_AHB_CLK					82
#define NSS_CC_MDIO_AHB_CLK					83
#define NSS_CC_MDIO_MASTER_AHB_CLK				84
#define NSS_CC_SYS_CLK_SRC					85
#define NSS_CC_SRDS0_SYS_CLK					86
#define NSS_CC_SRDS1_SYS_CLK					87
#define NSS_CC_GEPHY0_SYS_CLK					88
#define NSS_CC_GEPHY1_SYS_CLK					89
#define NSS_CC_GEPHY2_SYS_CLK					90
#define NSS_CC_GEPHY3_SYS_CLK					91
#endif
