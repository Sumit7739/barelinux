/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * s390x processor specific defines
 */
#ifndef SELFTEST_KVM_PROCESSOR_H
#define SELFTEST_KVM_PROCESSOR_H

#include <linux/compiler.h>

/* Bits in the region/segment table entry */
#define REGION_ENTRY_ORIGIN	~0xfffUL /* region/segment table origin	   */
#define REGION_ENTRY_PROTECT	0x200	 /* region protection bit	   */
#define REGION_ENTRY_NOEXEC	0x100	 /* region no-execute bit	   */
#define REGION_ENTRY_OFFSET	0xc0	 /* region table offset		   */
#define REGION_ENTRY_INVALID	0x20	 /* invalid region table entry	   */
#define REGION_ENTRY_TYPE	0x0c	 /* region/segment table type mask */
#define REGION_ENTRY_LENGTH	0x03	 /* region third length		   */

/* Bits in the page table entry */
#define PAGE_INVALID	0x400		/* HW invalid bit    */
#define PAGE_PROTECT	0x200		/* HW read-only bit  */
#define PAGE_NOEXEC	0x100		/* HW no-execute bit */

/* Page size definitions */
#define PAGE_SHIFT 12
#define PAGE_SIZE BIT_ULL(PAGE_SHIFT)
#define PAGE_MASK (~(PAGE_SIZE - 1))

/* Is there a portable way to do this? */
static inline void cpu_relax(void)
{
	barrier();
}

/* Get the instruction length */
static inline int insn_length(unsigned char code)
{
	return ((((int)code + 64) >> 7) + 1) << 1;
}

#endif
