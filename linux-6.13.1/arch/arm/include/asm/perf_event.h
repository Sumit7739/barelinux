/* SPDX-License-Identifier: GPL-2.0-only */
/*
 *  linux/arch/arm/include/asm/perf_event.h
 *
 *  Copyright (C) 2009 picoChip Designs Ltd, Jamie Iles
 */

#ifndef __ARM_PERF_EVENT_H__
#define __ARM_PERF_EVENT_H__

#define perf_arch_fetch_caller_regs(regs, __ip) { \
	(regs)->ARM_pc = (__ip); \
	frame_pointer((regs)) = (unsigned long) __builtin_frame_address(0); \
	(regs)->ARM_sp = current_stack_pointer; \
	(regs)->ARM_cpsr = SVC_MODE; \
}

#endif /* __ARM_PERF_EVENT_H__ */
