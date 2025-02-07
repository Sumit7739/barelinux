// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Hardware monitoring driver for ZL6100 and compatibles
 *
 * Copyright (c) 2011 Ericsson AB.
 * Copyright (c) 2012 Guenter Roeck
 */

#include <linux/bitops.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/err.h>
#include <linux/slab.h>
#include <linux/i2c.h>
#include <linux/ktime.h>
#include <linux/delay.h>
#include "pmbus.h"

enum chips { zl2004, zl2005, zl2006, zl2008, zl2105, zl2106, zl6100, zl6105,
	     zl8802, zl9101, zl9117, zls1003, zls4009 };

struct zl6100_data {
	int id;
	struct pmbus_driver_info info;
};

#define to_zl6100_data(x)  container_of(x, struct zl6100_data, info)

#define ZL6100_MFR_CONFIG		0xd0
#define ZL6100_DEVICE_ID		0xe4

#define ZL6100_MFR_XTEMP_ENABLE		BIT(7)

#define ZL8802_MFR_USER_GLOBAL_CONFIG	0xe9
#define ZL8802_MFR_TMON_ENABLE		BIT(12)
#define ZL8802_MFR_USER_CONFIG		0xd1
#define ZL8802_MFR_XTEMP_ENABLE_2	BIT(1)
#define ZL8802_MFR_DDC_CONFIG		0xd3
#define ZL8802_MFR_PHASES_MASK		0x0007

#define MFR_VMON_OV_FAULT_LIMIT		0xf5
#define MFR_VMON_UV_FAULT_LIMIT		0xf6
#define MFR_READ_VMON			0xf7

#define VMON_UV_WARNING			BIT(5)
#define VMON_OV_WARNING			BIT(4)
#define VMON_UV_FAULT			BIT(1)
#define VMON_OV_FAULT			BIT(0)

#define ZL6100_WAIT_TIME		1000	/* uS	*/

static ushort delay = ZL6100_WAIT_TIME;
module_param(delay, ushort, 0644);
MODULE_PARM_DESC(delay, "Delay between chip accesses in uS");

/* Convert linear sensor value to milli-units */
static long zl6100_l2d(s16 l)
{
	s16 exponent;
	s32 mantissa;
	long val;

	exponent = l >> 11;
	mantissa = ((s16)((l & 0x7ff) << 5)) >> 5;

	val = mantissa;

	/* scale result to milli-units */
	val = val * 1000L;

	if (exponent >= 0)
		val <<= exponent;
	else
		val >>= -exponent;

	return val;
}

#define MAX_MANTISSA	(1023 * 1000)
#define MIN_MANTISSA	(511 * 1000)

static u16 zl6100_d2l(long val)
{
	s16 exponent = 0, mantissa;
	bool negative = false;

	/* simple case */
	if (val == 0)
		return 0;

	if (val < 0) {
		negative = true;
		val = -val;
	}

	/* Reduce large mantissa until it fits into 10 bit */
	while (val >= MAX_MANTISSA && exponent < 15) {
		exponent++;
		val >>= 1;
	}
	/* Increase small mantissa to improve precision */
	while (val < MIN_MANTISSA && exponent > -15) {
		exponent--;
		val <<= 1;
	}

	/* Convert mantissa from milli-units to units */
	mantissa = DIV_ROUND_CLOSEST(val, 1000);

	/* Ensure that resulting number is within range */
	if (mantissa > 0x3ff)
		mantissa = 0x3ff;

	/* restore sign */
	if (negative)
		mantissa = -mantissa;

	/* Convert to 5 bit exponent, 11 bit mantissa */
	return (mantissa & 0x7ff) | ((exponent << 11) & 0xf800);
}

static int zl6100_read_word_data(struct i2c_client *client, int page,
				 int phase, int reg)
{
	const struct pmbus_driver_info *info = pmbus_get_driver_info(client);
	struct zl6100_data *data = to_zl6100_data(info);
	int ret, vreg;

	if (page >= info->pages)
		return -ENXIO;

	if (data->id == zl2005) {
		/*
		 * Limit register detection is not reliable on ZL2005.
		 * Make sure registers are not erroneously detected.
		 */
		switch (reg) {
		case PMBUS_VOUT_OV_WARN_LIMIT:
		case PMBUS_VOUT_UV_WARN_LIMIT:
		case PMBUS_IOUT_OC_WARN_LIMIT:
			return -ENXIO;
		}
	}

	switch (reg) {
	case PMBUS_VIRT_READ_VMON:
		vreg = MFR_READ_VMON;
		break;
	case PMBUS_VIRT_VMON_OV_WARN_LIMIT:
	case PMBUS_VIRT_VMON_OV_FAULT_LIMIT:
		vreg = MFR_VMON_OV_FAULT_LIMIT;
		break;
	case PMBUS_VIRT_VMON_UV_WARN_LIMIT:
	case PMBUS_VIRT_VMON_UV_FAULT_LIMIT:
		vreg = MFR_VMON_UV_FAULT_LIMIT;
		break;
	default:
		if (reg >= PMBUS_VIRT_BASE)
			return -ENXIO;
		vreg = reg;
		break;
	}

	ret = pmbus_read_word_data(client, page, phase, vreg);
	if (ret < 0)
		return ret;

	switch (reg) {
	case PMBUS_VIRT_VMON_OV_WARN_LIMIT:
		ret = zl6100_d2l(DIV_ROUND_CLOSEST(zl6100_l2d(ret) * 9, 10));
		break;
	case PMBUS_VIRT_VMON_UV_WARN_LIMIT:
		ret = zl6100_d2l(DIV_ROUND_CLOSEST(zl6100_l2d(ret) * 11, 10));
		break;
	}

	return ret;
}

static int zl6100_read_byte_data(struct i2c_client *client, int page, int reg)
{
	const struct pmbus_driver_info *info = pmbus_get_driver_info(client);
	int ret, status;

	if (page >= info->pages)
		return -ENXIO;

	switch (reg) {
	case PMBUS_VIRT_STATUS_VMON:
		ret = pmbus_read_byte_data(client, 0,
					   PMBUS_STATUS_MFR_SPECIFIC);
		if (ret < 0)
			break;

		status = 0;
		if (ret & VMON_UV_WARNING)
			status |= PB_VOLTAGE_UV_WARNING;
		if (ret & VMON_OV_WARNING)
			status |= PB_VOLTAGE_OV_WARNING;
		if (ret & VMON_UV_FAULT)
			status |= PB_VOLTAGE_UV_FAULT;
		if (ret & VMON_OV_FAULT)
			status |= PB_VOLTAGE_OV_FAULT;
		ret = status;
		break;
	default:
		ret = pmbus_read_byte_data(client, page, reg);
		break;
	}

	return ret;
}

static int zl6100_write_word_data(struct i2c_client *client, int page, int reg,
				  u16 word)
{
	const struct pmbus_driver_info *info = pmbus_get_driver_info(client);
	int vreg;

	if (page >= info->pages)
		return -ENXIO;

	switch (reg) {
	case PMBUS_VIRT_VMON_OV_WARN_LIMIT:
		word = zl6100_d2l(DIV_ROUND_CLOSEST(zl6100_l2d(word) * 10, 9));
		vreg = MFR_VMON_OV_FAULT_LIMIT;
		pmbus_clear_cache(client);
		break;
	case PMBUS_VIRT_VMON_OV_FAULT_LIMIT:
		vreg = MFR_VMON_OV_FAULT_LIMIT;
		pmbus_clear_cache(client);
		break;
	case PMBUS_VIRT_VMON_UV_WARN_LIMIT:
		word = zl6100_d2l(DIV_ROUND_CLOSEST(zl6100_l2d(word) * 10, 11));
		vreg = MFR_VMON_UV_FAULT_LIMIT;
		pmbus_clear_cache(client);
		break;
	case PMBUS_VIRT_VMON_UV_FAULT_LIMIT:
		vreg = MFR_VMON_UV_FAULT_LIMIT;
		pmbus_clear_cache(client);
		break;
	default:
		if (reg >= PMBUS_VIRT_BASE)
			return -ENXIO;
		vreg = reg;
	}

	return pmbus_write_word_data(client, page, vreg, word);
}

static const struct i2c_device_id zl6100_id[] = {
	{"bmr450", zl2005},
	{"bmr451", zl2005},
	{"bmr462", zl2008},
	{"bmr463", zl2008},
	{"bmr464", zl2008},
	{"bmr465", zls4009},
	{"bmr466", zls1003},
	{"bmr467", zls4009},
	{"bmr469", zl8802},
	{"zl2004", zl2004},
	{"zl2005", zl2005},
	{"zl2006", zl2006},
	{"zl2008", zl2008},
	{"zl2105", zl2105},
	{"zl2106", zl2106},
	{"zl6100", zl6100},
	{"zl6105", zl6105},
	{"zl8802", zl8802},
	{"zl9101", zl9101},
	{"zl9117", zl9117},
	{"zls1003", zls1003},
	{"zls4009", zls4009},
	{ }
};
MODULE_DEVICE_TABLE(i2c, zl6100_id);

static int zl6100_probe(struct i2c_client *client)
{
	int ret, i;
	struct zl6100_data *data;
	struct pmbus_driver_info *info;
	u8 device_id[I2C_SMBUS_BLOCK_MAX + 1];
	const struct i2c_device_id *mid;

	if (!i2c_check_functionality(client->adapter,
				     I2C_FUNC_SMBUS_READ_WORD_DATA
				     | I2C_FUNC_SMBUS_READ_BLOCK_DATA))
		return -ENODEV;

	ret = i2c_smbus_read_block_data(client, ZL6100_DEVICE_ID,
					device_id);
	if (ret < 0) {
		dev_err(&client->dev, "Failed to read device ID\n");
		return ret;
	}
	device_id[ret] = '\0';
	dev_info(&client->dev, "Device ID %s\n", device_id);

	mid = NULL;
	for (mid = zl6100_id; mid->name[0]; mid++) {
		if (!strncasecmp(mid->name, device_id, strlen(mid->name)))
			break;
	}
	if (!mid->name[0]) {
		dev_err(&client->dev, "Unsupported device\n");
		return -ENODEV;
	}
	if (strcmp(client->name, mid->name) != 0)
		dev_notice(&client->dev,
			   "Device mismatch: Configured %s, detected %s\n",
			   client->name, mid->name);

	data = devm_kzalloc(&client->dev, sizeof(struct zl6100_data),
			    GFP_KERNEL);
	if (!data)
		return -ENOMEM;

	data->id = mid->driver_data;

	/*
	 * According to information from the chip vendor, all currently
	 * supported chips are known to require a wait time between I2C
	 * accesses.
	 */
	udelay(delay);

	info = &data->info;

	info->pages = 1;
	info->func[0] = PMBUS_HAVE_VIN | PMBUS_HAVE_STATUS_INPUT
	  | PMBUS_HAVE_VOUT | PMBUS_HAVE_STATUS_VOUT
	  | PMBUS_HAVE_IOUT | PMBUS_HAVE_STATUS_IOUT
	  | PMBUS_HAVE_TEMP | PMBUS_HAVE_STATUS_TEMP;

	/*
	 * ZL2004, ZL8802, ZL9101M, ZL9117M and ZLS4009 support monitoring
	 * an extra voltage (VMON for ZL2004, ZL8802 and ZLS4009,
	 * VDRV for ZL9101M and ZL9117M). Report it as vmon.
	 */
	if (data->id == zl2004 || data->id == zl8802 || data->id == zl9101 ||
	    data->id == zl9117 || data->id == zls4009)
		info->func[0] |= PMBUS_HAVE_VMON | PMBUS_HAVE_STATUS_VMON;

	/*
	 * ZL8802 has two outputs that can be used either independently or in
	 * a current sharing configuration. The driver uses the DDC_CONFIG
	 * register to check if the module is running with independent or
	 * shared outputs. If the module is in shared output mode, only one
	 * output voltage will be reported.
	 */
	if (data->id == zl8802) {
		info->pages = 2;
		info->func[0] |= PMBUS_HAVE_IIN;

		ret = i2c_smbus_read_word_data(client, ZL8802_MFR_DDC_CONFIG);
		if (ret < 0)
			return ret;

		udelay(delay);

		if (ret & ZL8802_MFR_PHASES_MASK)
			info->func[1] |= PMBUS_HAVE_IOUT | PMBUS_HAVE_STATUS_IOUT;
		else
			info->func[1] = PMBUS_HAVE_VOUT | PMBUS_HAVE_STATUS_VOUT
				| PMBUS_HAVE_IOUT | PMBUS_HAVE_STATUS_IOUT;

		for (i = 0; i < 2; i++) {
			ret = i2c_smbus_write_byte_data(client, PMBUS_PAGE, i);
			if (ret < 0)
				return ret;

			udelay(delay);

			ret = i2c_smbus_read_word_data(client, ZL8802_MFR_USER_CONFIG);
			if (ret < 0)
				return ret;

			if (ret & ZL8802_MFR_XTEMP_ENABLE_2)
				info->func[i] |= PMBUS_HAVE_TEMP2;

			udelay(delay);
		}
		ret = i2c_smbus_read_word_data(client, ZL8802_MFR_USER_GLOBAL_CONFIG);
		if (ret < 0)
			return ret;

		if (ret & ZL8802_MFR_TMON_ENABLE)
			info->func[0] |= PMBUS_HAVE_TEMP3;
	} else {
		ret = i2c_smbus_read_word_data(client, ZL6100_MFR_CONFIG);
		if (ret < 0)
			return ret;

		if (ret & ZL6100_MFR_XTEMP_ENABLE)
			info->func[0] |= PMBUS_HAVE_TEMP2;
	}

	udelay(delay);

	info->access_delay = delay;
	info->read_word_data = zl6100_read_word_data;
	info->read_byte_data = zl6100_read_byte_data;
	info->write_word_data = zl6100_write_word_data;

	return pmbus_do_probe(client, info);
}

static struct i2c_driver zl6100_driver = {
	.driver = {
		   .name = "zl6100",
		   },
	.probe = zl6100_probe,
	.id_table = zl6100_id,
};

module_i2c_driver(zl6100_driver);

MODULE_AUTHOR("Guenter Roeck");
MODULE_DESCRIPTION("PMBus driver for ZL6100 and compatibles");
MODULE_LICENSE("GPL");
MODULE_IMPORT_NS("PMBUS");
