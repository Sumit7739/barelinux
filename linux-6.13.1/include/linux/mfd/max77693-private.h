/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * max77693-private.h - Voltage regulator driver for the Maxim 77693
 *
 *  Copyright (C) 2012 Samsung Electrnoics
 *  SangYoung Son <hello.son@samsung.com>
 *
 * This program is not provided / owned by Maxim Integrated Products.
 */

#ifndef __LINUX_MFD_MAX77693_PRIV_H
#define __LINUX_MFD_MAX77693_PRIV_H

#include <linux/i2c.h>

#define MAX77693_REG_INVALID		(0xff)

/* Slave addr = 0xCC: PMIC, Charger, Flash LED */
enum max77693_pmic_reg {
	MAX77693_LED_REG_IFLASH1			= 0x00,
	MAX77693_LED_REG_IFLASH2			= 0x01,
	MAX77693_LED_REG_ITORCH				= 0x02,
	MAX77693_LED_REG_ITORCHTIMER			= 0x03,
	MAX77693_LED_REG_FLASH_TIMER			= 0x04,
	MAX77693_LED_REG_FLASH_EN			= 0x05,
	MAX77693_LED_REG_MAX_FLASH1			= 0x06,
	MAX77693_LED_REG_MAX_FLASH2			= 0x07,
	MAX77693_LED_REG_MAX_FLASH3			= 0x08,
	MAX77693_LED_REG_MAX_FLASH4			= 0x09,
	MAX77693_LED_REG_VOUT_CNTL			= 0x0A,
	MAX77693_LED_REG_VOUT_FLASH1			= 0x0B,
	MAX77693_LED_REG_VOUT_FLASH2			= 0x0C,
	MAX77693_LED_REG_FLASH_INT			= 0x0E,
	MAX77693_LED_REG_FLASH_INT_MASK			= 0x0F,
	MAX77693_LED_REG_FLASH_STATUS			= 0x10,

	MAX77693_PMIC_REG_PMIC_ID1			= 0x20,
	MAX77693_PMIC_REG_PMIC_ID2			= 0x21,
	MAX77693_PMIC_REG_INTSRC			= 0x22,
	MAX77693_PMIC_REG_INTSRC_MASK			= 0x23,
	MAX77693_PMIC_REG_TOPSYS_INT			= 0x24,
	MAX77693_PMIC_REG_TOPSYS_INT_MASK		= 0x26,
	MAX77693_PMIC_REG_TOPSYS_STAT			= 0x28,
	MAX77693_PMIC_REG_MAINCTRL1			= 0x2A,
	MAX77693_PMIC_REG_LSCNFG			= 0x2B,

	MAX77693_CHG_REG_CHG_INT			= 0xB0,
	MAX77693_CHG_REG_CHG_INT_MASK			= 0xB1,
	MAX77693_CHG_REG_CHG_INT_OK			= 0xB2,
	MAX77693_CHG_REG_CHG_DETAILS_00			= 0xB3,
	MAX77693_CHG_REG_CHG_DETAILS_01			= 0xB4,
	MAX77693_CHG_REG_CHG_DETAILS_02			= 0xB5,
	MAX77693_CHG_REG_CHG_DETAILS_03			= 0xB6,
	MAX77693_CHG_REG_CHG_CNFG_00			= 0xB7,
	MAX77693_CHG_REG_CHG_CNFG_01			= 0xB8,
	MAX77693_CHG_REG_CHG_CNFG_02			= 0xB9,
	MAX77693_CHG_REG_CHG_CNFG_03			= 0xBA,
	MAX77693_CHG_REG_CHG_CNFG_04			= 0xBB,
	MAX77693_CHG_REG_CHG_CNFG_05			= 0xBC,
	MAX77693_CHG_REG_CHG_CNFG_06			= 0xBD,
	MAX77693_CHG_REG_CHG_CNFG_07			= 0xBE,
	MAX77693_CHG_REG_CHG_CNFG_08			= 0xBF,
	MAX77693_CHG_REG_CHG_CNFG_09			= 0xC0,
	MAX77693_CHG_REG_CHG_CNFG_10			= 0xC1,
	MAX77693_CHG_REG_CHG_CNFG_11			= 0xC2,
	MAX77693_CHG_REG_CHG_CNFG_12			= 0xC3,
	MAX77693_CHG_REG_CHG_CNFG_13			= 0xC4,
	MAX77693_CHG_REG_CHG_CNFG_14			= 0xC5,
	MAX77693_CHG_REG_SAFEOUT_CTRL			= 0xC6,

	MAX77693_PMIC_REG_END,
};

/* MAX77693 ITORCH register */
#define TORCH_IOUT1_SHIFT	0
#define TORCH_IOUT2_SHIFT	4
#define TORCH_IOUT_MASK(x)	(0xf << (x))
#define TORCH_IOUT_MIN		15625
#define TORCH_IOUT_MAX		250000
#define TORCH_IOUT_STEP		15625

/* MAX77693 IFLASH1 and IFLASH2 registers */
#define FLASH_IOUT_MIN		15625
#define FLASH_IOUT_MAX_1LED	1000000
#define FLASH_IOUT_MAX_2LEDS	625000
#define FLASH_IOUT_STEP		15625

/* MAX77693 TORCH_TIMER register */
#define TORCH_TMR_NO_TIMER	0x40
#define TORCH_TIMEOUT_MIN	262000
#define TORCH_TIMEOUT_MAX	15728000

/* MAX77693 FLASH_TIMER register */
#define FLASH_TMR_LEVEL		0x80
#define FLASH_TIMEOUT_MIN	62500
#define FLASH_TIMEOUT_MAX	1000000
#define FLASH_TIMEOUT_STEP	62500

/* MAX77693 FLASH_EN register */
#define FLASH_EN_OFF		0x0
#define FLASH_EN_FLASH		0x1
#define FLASH_EN_TORCH		0x2
#define FLASH_EN_ON		0x3
#define FLASH_EN_SHIFT(x)	(6 - (x) * 2)
#define TORCH_EN_SHIFT(x)	(2 - (x) * 2)

/* MAX77693 MAX_FLASH1 register */
#define MAX_FLASH1_MAX_FL_EN	0x80
#define MAX_FLASH1_VSYS_MIN	2400
#define MAX_FLASH1_VSYS_MAX	3400
#define MAX_FLASH1_VSYS_STEP	33

/* MAX77693 VOUT_CNTL register */
#define FLASH_BOOST_FIXED	0x04
#define FLASH_BOOST_LEDNUM_2	0x80

/* MAX77693 VOUT_FLASH1 register */
#define FLASH_VOUT_MIN		3300
#define FLASH_VOUT_MAX		5500
#define FLASH_VOUT_STEP		25
#define FLASH_VOUT_RMIN		0x0c

/* MAX77693 FLASH_STATUS register */
#define FLASH_STATUS_FLASH_ON	BIT(3)
#define FLASH_STATUS_TORCH_ON	BIT(2)

/* MAX77693 FLASH_INT register */
#define FLASH_INT_FLED2_OPEN	BIT(0)
#define FLASH_INT_FLED2_SHORT	BIT(1)
#define FLASH_INT_FLED1_OPEN	BIT(2)
#define FLASH_INT_FLED1_SHORT	BIT(3)
#define FLASH_INT_OVER_CURRENT	BIT(4)

/* Fast charge timer in hours */
#define DEFAULT_FAST_CHARGE_TIMER		4
/* microamps */
#define DEFAULT_TOP_OFF_THRESHOLD_CURRENT	150000
/* minutes */
#define DEFAULT_TOP_OFF_TIMER			30
/* microvolts */
#define DEFAULT_CONSTANT_VOLT			4200000
/* microvolts */
#define DEFAULT_MIN_SYSTEM_VOLT			3600000
/* celsius */
#define DEFAULT_THERMAL_REGULATION_TEMP		100
/* microamps */
#define DEFAULT_BATTERY_OVERCURRENT		3500000
/* microvolts */
#define DEFAULT_CHARGER_INPUT_THRESHOLD_VOLT	4300000

/* MAX77693_CHG_REG_CHG_INT_OK register */
#define CHG_INT_OK_BYP_SHIFT		0
#define CHG_INT_OK_BAT_SHIFT		3
#define CHG_INT_OK_CHG_SHIFT		4
#define CHG_INT_OK_CHGIN_SHIFT		6
#define CHG_INT_OK_DETBAT_SHIFT		7
#define CHG_INT_OK_BYP_MASK		BIT(CHG_INT_OK_BYP_SHIFT)
#define CHG_INT_OK_BAT_MASK		BIT(CHG_INT_OK_BAT_SHIFT)
#define CHG_INT_OK_CHG_MASK		BIT(CHG_INT_OK_CHG_SHIFT)
#define CHG_INT_OK_CHGIN_MASK		BIT(CHG_INT_OK_CHGIN_SHIFT)
#define CHG_INT_OK_DETBAT_MASK		BIT(CHG_INT_OK_DETBAT_SHIFT)

/* MAX77693_CHG_REG_CHG_DETAILS_00 register */
#define CHG_DETAILS_00_CHGIN_SHIFT	5
#define CHG_DETAILS_00_CHGIN_MASK	(0x3 << CHG_DETAILS_00_CHGIN_SHIFT)

/* MAX77693_CHG_REG_CHG_DETAILS_01 register */
#define CHG_DETAILS_01_CHG_SHIFT	0
#define CHG_DETAILS_01_BAT_SHIFT	4
#define CHG_DETAILS_01_TREG_SHIFT	7
#define CHG_DETAILS_01_CHG_MASK		(0xf << CHG_DETAILS_01_CHG_SHIFT)
#define CHG_DETAILS_01_BAT_MASK		(0x7 << CHG_DETAILS_01_BAT_SHIFT)
#define CHG_DETAILS_01_TREG_MASK	BIT(7)

/* MAX77693_CHG_REG_CHG_DETAILS_01/CHG field */
enum max77693_charger_charging_state {
	MAX77693_CHARGING_PREQUALIFICATION	= 0x0,
	MAX77693_CHARGING_FAST_CONST_CURRENT,
	MAX77693_CHARGING_FAST_CONST_VOLTAGE,
	MAX77693_CHARGING_TOP_OFF,
	MAX77693_CHARGING_DONE,
	MAX77693_CHARGING_HIGH_TEMP,
	MAX77693_CHARGING_TIMER_EXPIRED,
	MAX77693_CHARGING_THERMISTOR_SUSPEND,
	MAX77693_CHARGING_OFF,
	MAX77693_CHARGING_RESERVED,
	MAX77693_CHARGING_OVER_TEMP,
	MAX77693_CHARGING_WATCHDOG_EXPIRED,
};

/* MAX77693_CHG_REG_CHG_DETAILS_01/BAT field */
enum max77693_charger_battery_state {
	MAX77693_BATTERY_NOBAT			= 0x0,
	/* Dead-battery or low-battery prequalification */
	MAX77693_BATTERY_PREQUALIFICATION,
	MAX77693_BATTERY_TIMER_EXPIRED,
	MAX77693_BATTERY_GOOD,
	MAX77693_BATTERY_LOWVOLTAGE,
	MAX77693_BATTERY_OVERVOLTAGE,
	MAX77693_BATTERY_OVERCURRENT,
	MAX77693_BATTERY_RESERVED,
};

/* MAX77693_CHG_REG_CHG_DETAILS_02 register */
#define CHG_DETAILS_02_BYP_SHIFT	0
#define CHG_DETAILS_02_BYP_MASK		(0xf << CHG_DETAILS_02_BYP_SHIFT)

/* MAX77693 CHG_CNFG_00 register */
#define CHG_CNFG_00_CHG_MASK		0x1
#define CHG_CNFG_00_BUCK_MASK		0x4

/* MAX77693_CHG_REG_CHG_CNFG_01 register */
#define CHG_CNFG_01_FCHGTIME_SHIFT	0
#define CHG_CNFG_01_CHGRSTRT_SHIFT	4
#define CHG_CNFG_01_PQEN_SHIFT		7
#define CHG_CNFG_01_FCHGTIME_MASK	(0x7 << CHG_CNFG_01_FCHGTIME_SHIFT)
#define CHG_CNFG_01_CHGRSTRT_MASK	(0x3 << CHG_CNFG_01_CHGRSTRT_SHIFT)
#define CHG_CNFG_01_PQEN_MAKS		BIT(CHG_CNFG_01_PQEN_SHIFT)

/* MAX77693_CHG_REG_CHG_CNFG_02 register */
#define CHG_CNFG_02_CC_SHIFT		0
#define CHG_CNFG_02_CC_MASK		0x3F

/* MAX77693_CHG_REG_CHG_CNFG_03 register */
#define CHG_CNFG_03_TOITH_SHIFT		0
#define CHG_CNFG_03_TOTIME_SHIFT	3
#define CHG_CNFG_03_TOITH_MASK		(0x7 << CHG_CNFG_03_TOITH_SHIFT)
#define CHG_CNFG_03_TOTIME_MASK		(0x7 << CHG_CNFG_03_TOTIME_SHIFT)

/* MAX77693_CHG_REG_CHG_CNFG_04 register */
#define CHG_CNFG_04_CHGCVPRM_SHIFT	0
#define CHG_CNFG_04_MINVSYS_SHIFT	5
#define CHG_CNFG_04_CHGCVPRM_MASK	(0x1f << CHG_CNFG_04_CHGCVPRM_SHIFT)
#define CHG_CNFG_04_MINVSYS_MASK	(0x7 << CHG_CNFG_04_MINVSYS_SHIFT)

/* MAX77693_CHG_REG_CHG_CNFG_06 register */
#define CHG_CNFG_06_CHGPROT_SHIFT	2
#define CHG_CNFG_06_CHGPROT_MASK	(0x3 << CHG_CNFG_06_CHGPROT_SHIFT)

/* MAX77693_CHG_REG_CHG_CNFG_07 register */
#define CHG_CNFG_07_REGTEMP_SHIFT	5
#define CHG_CNFG_07_REGTEMP_MASK	(0x3 << CHG_CNFG_07_REGTEMP_SHIFT)

/* MAX77693_CHG_REG_CHG_CNFG_12 register */
#define CHG_CNFG_12_B2SOVRC_SHIFT	0
#define CHG_CNFG_12_VCHGINREG_SHIFT	3
#define CHG_CNFG_12_B2SOVRC_MASK	(0x7 << CHG_CNFG_12_B2SOVRC_SHIFT)
#define CHG_CNFG_12_VCHGINREG_MASK	(0x3 << CHG_CNFG_12_VCHGINREG_SHIFT)

/* MAX77693 CHG_CNFG_09 Register */
#define CHG_CNFG_09_CHGIN_ILIM_SHIFT	0
#define CHG_CNFG_09_CHGIN_ILIM_MASK	0x7F

/* MAX77693 CHG_CTRL Register */
#define SAFEOUT_CTRL_SAFEOUT1_MASK	0x3
#define SAFEOUT_CTRL_SAFEOUT2_MASK	0xC
#define SAFEOUT_CTRL_ENSAFEOUT1_MASK	0x40
#define SAFEOUT_CTRL_ENSAFEOUT2_MASK	0x80

/* Slave addr = 0x4A: MUIC */
enum max77693_muic_reg {
	MAX77693_MUIC_REG_ID		= 0x00,
	MAX77693_MUIC_REG_INT1		= 0x01,
	MAX77693_MUIC_REG_INT2		= 0x02,
	MAX77693_MUIC_REG_INT3		= 0x03,
	MAX77693_MUIC_REG_STATUS1	= 0x04,
	MAX77693_MUIC_REG_STATUS2	= 0x05,
	MAX77693_MUIC_REG_STATUS3	= 0x06,
	MAX77693_MUIC_REG_INTMASK1	= 0x07,
	MAX77693_MUIC_REG_INTMASK2	= 0x08,
	MAX77693_MUIC_REG_INTMASK3	= 0x09,
	MAX77693_MUIC_REG_CDETCTRL1	= 0x0A,
	MAX77693_MUIC_REG_CDETCTRL2	= 0x0B,
	MAX77693_MUIC_REG_CTRL1		= 0x0C,
	MAX77693_MUIC_REG_CTRL2		= 0x0D,
	MAX77693_MUIC_REG_CTRL3		= 0x0E,

	MAX77693_MUIC_REG_END,
};

/* MAX77693 INTMASK1~2 Register */
#define INTMASK1_ADC1K_SHIFT		3
#define INTMASK1_ADCERR_SHIFT		2
#define INTMASK1_ADCLOW_SHIFT		1
#define INTMASK1_ADC_SHIFT		0
#define INTMASK1_ADC1K_MASK		(1 << INTMASK1_ADC1K_SHIFT)
#define INTMASK1_ADCERR_MASK		(1 << INTMASK1_ADCERR_SHIFT)
#define INTMASK1_ADCLOW_MASK		(1 << INTMASK1_ADCLOW_SHIFT)
#define INTMASK1_ADC_MASK		(1 << INTMASK1_ADC_SHIFT)

#define INTMASK2_VIDRM_SHIFT		5
#define INTMASK2_VBVOLT_SHIFT		4
#define INTMASK2_DXOVP_SHIFT		3
#define INTMASK2_DCDTMR_SHIFT		2
#define INTMASK2_CHGDETRUN_SHIFT	1
#define INTMASK2_CHGTYP_SHIFT		0
#define INTMASK2_VIDRM_MASK		(1 << INTMASK2_VIDRM_SHIFT)
#define INTMASK2_VBVOLT_MASK		(1 << INTMASK2_VBVOLT_SHIFT)
#define INTMASK2_DXOVP_MASK		(1 << INTMASK2_DXOVP_SHIFT)
#define INTMASK2_DCDTMR_MASK		(1 << INTMASK2_DCDTMR_SHIFT)
#define INTMASK2_CHGDETRUN_MASK		(1 << INTMASK2_CHGDETRUN_SHIFT)
#define INTMASK2_CHGTYP_MASK		(1 << INTMASK2_CHGTYP_SHIFT)

/* MAX77693 MUIC - STATUS1~3 Register */
#define MAX77693_STATUS1_ADC_SHIFT		0
#define MAX77693_STATUS1_ADCLOW_SHIFT		5
#define MAX77693_STATUS1_ADCERR_SHIFT		6
#define MAX77693_STATUS1_ADC1K_SHIFT		7
#define MAX77693_STATUS1_ADC_MASK		(0x1f << MAX77693_STATUS1_ADC_SHIFT)
#define MAX77693_STATUS1_ADCLOW_MASK		BIT(MAX77693_STATUS1_ADCLOW_SHIFT)
#define MAX77693_STATUS1_ADCERR_MASK		BIT(MAX77693_STATUS1_ADCERR_SHIFT)
#define MAX77693_STATUS1_ADC1K_MASK		BIT(MAX77693_STATUS1_ADC1K_SHIFT)

#define MAX77693_STATUS2_CHGTYP_SHIFT		0
#define MAX77693_STATUS2_CHGDETRUN_SHIFT	3
#define MAX77693_STATUS2_DCDTMR_SHIFT		4
#define MAX77693_STATUS2_DXOVP_SHIFT		5
#define MAX77693_STATUS2_VBVOLT_SHIFT		6
#define MAX77693_STATUS2_VIDRM_SHIFT		7
#define MAX77693_STATUS2_CHGTYP_MASK		(0x7 << MAX77693_STATUS2_CHGTYP_SHIFT)
#define MAX77693_STATUS2_CHGDETRUN_MASK		BIT(MAX77693_STATUS2_CHGDETRUN_SHIFT)
#define MAX77693_STATUS2_DCDTMR_MASK		BIT(MAX77693_STATUS2_DCDTMR_SHIFT)
#define MAX77693_STATUS2_DXOVP_MASK		BIT(MAX77693_STATUS2_DXOVP_SHIFT)
#define MAX77693_STATUS2_VBVOLT_MASK		BIT(MAX77693_STATUS2_VBVOLT_SHIFT)
#define MAX77693_STATUS2_VIDRM_MASK		BIT(MAX77693_STATUS2_VIDRM_SHIFT)

#define MAX77693_STATUS3_OVP_SHIFT		2
#define MAX77693_STATUS3_OVP_MASK		BIT(MAX77693_STATUS3_OVP_SHIFT)

/* MAX77693 CDETCTRL1~2 register */
#define CDETCTRL1_CHGDETEN_SHIFT	(0)
#define CDETCTRL1_CHGTYPMAN_SHIFT	(1)
#define CDETCTRL1_DCDEN_SHIFT		(2)
#define CDETCTRL1_DCD2SCT_SHIFT		(3)
#define CDETCTRL1_CDDELAY_SHIFT		(4)
#define CDETCTRL1_DCDCPL_SHIFT		(5)
#define CDETCTRL1_CDPDET_SHIFT		(7)
#define CDETCTRL1_CHGDETEN_MASK		(0x1 << CDETCTRL1_CHGDETEN_SHIFT)
#define CDETCTRL1_CHGTYPMAN_MASK	(0x1 << CDETCTRL1_CHGTYPMAN_SHIFT)
#define CDETCTRL1_DCDEN_MASK		(0x1 << CDETCTRL1_DCDEN_SHIFT)
#define CDETCTRL1_DCD2SCT_MASK		(0x1 << CDETCTRL1_DCD2SCT_SHIFT)
#define CDETCTRL1_CDDELAY_MASK		(0x1 << CDETCTRL1_CDDELAY_SHIFT)
#define CDETCTRL1_DCDCPL_MASK		(0x1 << CDETCTRL1_DCDCPL_SHIFT)
#define CDETCTRL1_CDPDET_MASK		(0x1 << CDETCTRL1_CDPDET_SHIFT)

#define CDETCTRL2_VIDRMEN_SHIFT		(1)
#define CDETCTRL2_DXOVPEN_SHIFT		(3)
#define CDETCTRL2_VIDRMEN_MASK		(0x1 << CDETCTRL2_VIDRMEN_SHIFT)
#define CDETCTRL2_DXOVPEN_MASK		(0x1 << CDETCTRL2_DXOVPEN_SHIFT)

/* MAX77693 MUIC - CONTROL1~3 register */
#define COMN1SW_SHIFT			(0)
#define COMP2SW_SHIFT			(3)
#define COMN1SW_MASK			(0x7 << COMN1SW_SHIFT)
#define COMP2SW_MASK			(0x7 << COMP2SW_SHIFT)
#define COMP_SW_MASK			(COMP2SW_MASK | COMN1SW_MASK)
#define MAX77693_CONTROL1_SW_USB	((1 << COMP2SW_SHIFT) \
						| (1 << COMN1SW_SHIFT))
#define MAX77693_CONTROL1_SW_AUDIO	((2 << COMP2SW_SHIFT) \
						| (2 << COMN1SW_SHIFT))
#define MAX77693_CONTROL1_SW_UART	((3 << COMP2SW_SHIFT) \
						| (3 << COMN1SW_SHIFT))
#define MAX77693_CONTROL1_SW_OPEN	((0 << COMP2SW_SHIFT) \
						| (0 << COMN1SW_SHIFT))

#define MAX77693_CONTROL2_LOWPWR_SHIFT		0
#define MAX77693_CONTROL2_ADCEN_SHIFT		1
#define MAX77693_CONTROL2_CPEN_SHIFT		2
#define MAX77693_CONTROL2_SFOUTASRT_SHIFT	3
#define MAX77693_CONTROL2_SFOUTORD_SHIFT	4
#define MAX77693_CONTROL2_ACCDET_SHIFT		5
#define MAX77693_CONTROL2_USBCPINT_SHIFT	6
#define MAX77693_CONTROL2_RCPS_SHIFT		7
#define MAX77693_CONTROL2_LOWPWR_MASK		BIT(MAX77693_CONTROL2_LOWPWR_SHIFT)
#define MAX77693_CONTROL2_ADCEN_MASK		BIT(MAX77693_CONTROL2_ADCEN_SHIFT)
#define MAX77693_CONTROL2_CPEN_MASK		BIT(MAX77693_CONTROL2_CPEN_SHIFT)
#define MAX77693_CONTROL2_SFOUTASRT_MASK	BIT(MAX77693_CONTROL2_SFOUTASRT_SHIFT)
#define MAX77693_CONTROL2_SFOUTORD_MASK		BIT(MAX77693_CONTROL2_SFOUTORD_SHIFT)
#define MAX77693_CONTROL2_ACCDET_MASK		BIT(MAX77693_CONTROL2_ACCDET_SHIFT)
#define MAX77693_CONTROL2_USBCPINT_MASK		BIT(MAX77693_CONTROL2_USBCPINT_SHIFT)
#define MAX77693_CONTROL2_RCPS_MASK		BIT(MAX77693_CONTROL2_RCPS_SHIFT)

#define MAX77693_CONTROL3_JIGSET_SHIFT		0
#define MAX77693_CONTROL3_BTLDSET_SHIFT		2
#define MAX77693_CONTROL3_ADCDBSET_SHIFT	4
#define MAX77693_CONTROL3_JIGSET_MASK		(0x3 << MAX77693_CONTROL3_JIGSET_SHIFT)
#define MAX77693_CONTROL3_BTLDSET_MASK		(0x3 << MAX77693_CONTROL3_BTLDSET_SHIFT)
#define MAX77693_CONTROL3_ADCDBSET_MASK		(0x3 << MAX77693_CONTROL3_ADCDBSET_SHIFT)

/* Slave addr = 0x90: Haptic */
enum max77693_haptic_reg {
	MAX77693_HAPTIC_REG_STATUS		= 0x00,
	MAX77693_HAPTIC_REG_CONFIG1		= 0x01,
	MAX77693_HAPTIC_REG_CONFIG2		= 0x02,
	MAX77693_HAPTIC_REG_CONFIG_CHNL		= 0x03,
	MAX77693_HAPTIC_REG_CONFG_CYC1		= 0x04,
	MAX77693_HAPTIC_REG_CONFG_CYC2		= 0x05,
	MAX77693_HAPTIC_REG_CONFIG_PER1		= 0x06,
	MAX77693_HAPTIC_REG_CONFIG_PER2		= 0x07,
	MAX77693_HAPTIC_REG_CONFIG_PER3		= 0x08,
	MAX77693_HAPTIC_REG_CONFIG_PER4		= 0x09,
	MAX77693_HAPTIC_REG_CONFIG_DUTY1	= 0x0A,
	MAX77693_HAPTIC_REG_CONFIG_DUTY2	= 0x0B,
	MAX77693_HAPTIC_REG_CONFIG_PWM1		= 0x0C,
	MAX77693_HAPTIC_REG_CONFIG_PWM2		= 0x0D,
	MAX77693_HAPTIC_REG_CONFIG_PWM3		= 0x0E,
	MAX77693_HAPTIC_REG_CONFIG_PWM4		= 0x0F,
	MAX77693_HAPTIC_REG_REV			= 0x10,

	MAX77693_HAPTIC_REG_END,
};

/* max77693-pmic LSCNFG configuration register */
#define MAX77693_PMIC_LOW_SYS_MASK      0x80
#define MAX77693_PMIC_LOW_SYS_SHIFT     7

/* max77693-haptic configuration register */
#define MAX77693_CONFIG2_MODE           7
#define MAX77693_CONFIG2_MEN            6
#define MAX77693_CONFIG2_HTYP           5

#define SRC_IRQ_CHARGER			BIT(0)
#define SRC_IRQ_TOP			BIT(1)
#define SRC_IRQ_FLASH			BIT(2)
#define SRC_IRQ_MUIC			BIT(3)
#define SRC_IRQ_ALL			(SRC_IRQ_CHARGER | SRC_IRQ_TOP \
						| SRC_IRQ_FLASH | SRC_IRQ_MUIC)

#define LED_IRQ_FLED2_OPEN		BIT(0)
#define LED_IRQ_FLED2_SHORT		BIT(1)
#define LED_IRQ_FLED1_OPEN		BIT(2)
#define LED_IRQ_FLED1_SHORT		BIT(3)
#define LED_IRQ_MAX_FLASH		BIT(4)

#define TOPSYS_IRQ_T120C_INT		BIT(0)
#define TOPSYS_IRQ_T140C_INT		BIT(1)
#define TOPSYS_IRQ_LOWSYS_INT		BIT(3)

#define CHG_IRQ_BYP_I			BIT(0)
#define CHG_IRQ_THM_I			BIT(2)
#define CHG_IRQ_BAT_I			BIT(3)
#define CHG_IRQ_CHG_I			BIT(4)
#define CHG_IRQ_CHGIN_I			BIT(6)

#define MUIC_IRQ_INT1_ADC		BIT(0)
#define MUIC_IRQ_INT1_ADC_LOW		BIT(1)
#define MUIC_IRQ_INT1_ADC_ERR		BIT(2)
#define MUIC_IRQ_INT1_ADC1K		BIT(3)

#define MUIC_IRQ_INT2_CHGTYP		BIT(0)
#define MUIC_IRQ_INT2_CHGDETREUN	BIT(1)
#define MUIC_IRQ_INT2_DCDTMR		BIT(2)
#define MUIC_IRQ_INT2_DXOVP		BIT(3)
#define MUIC_IRQ_INT2_VBVOLT		BIT(4)
#define MUIC_IRQ_INT2_VIDRM		BIT(5)

#define MUIC_IRQ_INT3_EOC		BIT(0)
#define MUIC_IRQ_INT3_CGMBC		BIT(1)
#define MUIC_IRQ_INT3_OVP		BIT(2)
#define MUIC_IRQ_INT3_MBCCHG_ERR	BIT(3)
#define MUIC_IRQ_INT3_CHG_ENABLED	BIT(4)
#define MUIC_IRQ_INT3_BAT_DET		BIT(5)

enum max77693_irq {
	/* PMIC - FLASH */
	MAX77693_LED_IRQ_FLED2_OPEN,
	MAX77693_LED_IRQ_FLED2_SHORT,
	MAX77693_LED_IRQ_FLED1_OPEN,
	MAX77693_LED_IRQ_FLED1_SHORT,
	MAX77693_LED_IRQ_MAX_FLASH,

	/* PMIC - TOPSYS */
	MAX77693_TOPSYS_IRQ_T120C_INT,
	MAX77693_TOPSYS_IRQ_T140C_INT,
	MAX77693_TOPSYS_IRQ_LOWSYS_INT,

	/* PMIC - Charger */
	MAX77693_CHG_IRQ_BYP_I,
	MAX77693_CHG_IRQ_THM_I,
	MAX77693_CHG_IRQ_BAT_I,
	MAX77693_CHG_IRQ_CHG_I,
	MAX77693_CHG_IRQ_CHGIN_I,

	MAX77693_IRQ_NR,
};

enum max77693_irq_muic {
	/* MUIC INT1 */
	MAX77693_MUIC_IRQ_INT1_ADC,
	MAX77693_MUIC_IRQ_INT1_ADC_LOW,
	MAX77693_MUIC_IRQ_INT1_ADC_ERR,
	MAX77693_MUIC_IRQ_INT1_ADC1K,

	/* MUIC INT2 */
	MAX77693_MUIC_IRQ_INT2_CHGTYP,
	MAX77693_MUIC_IRQ_INT2_CHGDETREUN,
	MAX77693_MUIC_IRQ_INT2_DCDTMR,
	MAX77693_MUIC_IRQ_INT2_DXOVP,
	MAX77693_MUIC_IRQ_INT2_VBVOLT,
	MAX77693_MUIC_IRQ_INT2_VIDRM,

	/* MUIC INT3 */
	MAX77693_MUIC_IRQ_INT3_EOC,
	MAX77693_MUIC_IRQ_INT3_CGMBC,
	MAX77693_MUIC_IRQ_INT3_OVP,
	MAX77693_MUIC_IRQ_INT3_MBCCHG_ERR,
	MAX77693_MUIC_IRQ_INT3_CHG_ENABLED,
	MAX77693_MUIC_IRQ_INT3_BAT_DET,

	MAX77693_MUIC_IRQ_NR,
};

#endif /*  __LINUX_MFD_MAX77693_PRIV_H */
