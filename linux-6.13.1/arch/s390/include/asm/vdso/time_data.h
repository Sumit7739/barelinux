/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __S390_ASM_VDSO_TIME_DATA_H
#define __S390_ASM_VDSO_TIME_DATA_H

#include <linux/types.h>

struct arch_vdso_time_data {
	__s64 tod_steering_delta;
	__u64 tod_steering_end;
};

#endif /* __S390_ASM_VDSO_TIME_DATA_H */
