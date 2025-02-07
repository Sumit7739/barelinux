/* SPDX-License-Identifier: GPL-2.0
 *
 * Copyright 2020-2022 HabanaLabs, Ltd.
 * All Rights Reserved.
 *
 */

#ifndef GAUDI2_H
#define GAUDI2_H

#define SRAM_CFG_BAR_ID		0
#define MSIX_BAR_ID		2
#define DRAM_BAR_ID		4

/* Refers to CFG_REGION_SIZE, BAR0_RSRVD_SIZE and SRAM_SIZE */
#define CFG_BAR_SIZE		0x10000000ull		/* 256MB */

#define MSIX_BAR_SIZE		0x4000ull		/* 16KB */

#define CFG_BASE		0x1000007FF8000000ull
#define CFG_SIZE		0x8000000ull		/* 96MB CFG + 32MB DBG*/
#define CFG_REGION_SIZE		0xC000000ull		/* 192MB */

#define STM_FLASH_BASE_ADDR	0x1000007FF4000000ull	/* Not 256MB aligned */
#define STM_FLASH_ALIGNED_OFF	0x4000000ull		/* 256 MB alignment */
#define STM_FLASH_SIZE		0x2000000ull		/* 32MB */

#define SPI_FLASH_BASE_ADDR	0x1000007FF6000000ull
#define SPI_FLASH_SIZE		0x1000000ull		/* 16MB */

#define SCRATCHPAD_SRAM_ADDR	0x1000007FF7FE0000ull
#define SCRATCHPAD_SRAM_SIZE	0x10000ull		/* 64KB */

#define PCIE_FW_SRAM_ADDR	0x1000007FF7FF0000ull
#define PCIE_FW_SRAM_SIZE	0x8000			/* 32KB */

#define BAR0_RSRVD_BASE_ADDR	0x1000FFFFFC000000ull
#define BAR0_RSRVD_SIZE		0x1000000ull		/* 16MB */

#define SRAM_BASE_ADDR		0x1000FFFFFD000000ull
#define SRAM_SIZE		0x3000000ull		/* 48MB */

#define DRAM_PHYS_BASE		0x1001000000000000ull

/* every hint address is masked accordingly */
#define DRAM_VA_HINT_MASK	0xFFFFFFFFFFFFull	/* 48bit mask */

#define HOST_PHYS_BASE_0	0x0000000000000000ull
#define HOST_PHYS_SIZE_0	0x0100000000000000ull	/* 64PB (56 bits) */

#define HOST_PHYS_BASE_1	0xFF00000000000000ull
#define HOST_PHYS_SIZE_1	0x0100000000000000ull	/* 64PB (56 bits) */

#define RESERVED_VA_RANGE_FOR_ARC_ON_HBM_START	0x1001500000000000ull
#define RESERVED_VA_RANGE_FOR_ARC_ON_HBM_END	0x10016FFFFFFFFFFFull

#define RESERVED_VA_FOR_VIRTUAL_MSIX_DOORBELL_START	0xFFF077FFFFFF0000ull
#define RESERVED_VA_FOR_VIRTUAL_MSIX_DOORBELL_END	0xFFF077FFFFFFFFFFull

#define RESERVED_VA_RANGE_FOR_ARC_ON_HOST_START	0xFFF0780000000000ull
#define RESERVED_VA_RANGE_FOR_ARC_ON_HOST_END	0xFFF07FFFFFFFFFFFull

#define RESERVED_VA_RANGE_FOR_ARC_ON_HOST_HPAGE_START	0xFFF0F80000000000ull
#define RESERVED_VA_RANGE_FOR_ARC_ON_HOST_HPAGE_END	0xFFF0FFFFFFFFFFFFull

#define RESERVED_MSIX_UNEXPECTED_USER_ERROR_INTERRUPT	127

#define GAUDI2_MSIX_ENTRIES	128

#define QMAN_PQ_ENTRY_SIZE	16			/* Bytes */

#define MAX_ASID		2

#define NUM_ARC_CPUS			69

/* Every ARC cpu in the system contains a single DCCM block
 * except MME and Scheduler ARCs which contain 2 DCCM blocks
 */
#define ARC_DCCM_BLOCK_SIZE		0x8000

#define NUM_OF_DCORES			4
#define NUM_OF_SFT			4
#define NUM_OF_PSOC_ARC			2
#define NUM_OF_SCHEDULER_ARC		6

#define NUM_OF_PQ_PER_QMAN		4
#define NUM_OF_CQ_PER_QMAN		5
#define NUM_OF_CP_PER_QMAN		5
#define NUM_OF_EDMA_PER_DCORE		2
#define NUM_OF_HIF_PER_DCORE		4
#define NUM_OF_PDMA			2
#define NUM_OF_TPC_PER_DCORE		6
#define NUM_DCORE0_TPC			7
#define NUM_DCORE1_TPC			NUM_OF_TPC_PER_DCORE
#define NUM_DCORE2_TPC			NUM_OF_TPC_PER_DCORE
#define NUM_DCORE3_TPC			NUM_OF_TPC_PER_DCORE
#define NUM_OF_DEC_PER_DCORE		2
#define NUM_OF_ROT			2
#define NUM_OF_HMMU_PER_DCORE		4
#define NUM_OF_MME_PER_DCORE		1
#define NUM_OF_MME_SBTE_PER_DCORE	5
#define NUM_OF_MME_WB_PER_DCORE		2
#define NUM_OF_RTR_PER_DCORE		8
#define NUM_OF_VDEC_PER_DCORE		2
#define NUM_OF_IF_RTR_PER_SFT		3
#define NUM_OF_PCIE_VDEC		2
#define NUM_OF_ARC_FARMS_ARC		4
#define NUM_OF_XBAR			4

#define TPC_NUM_OF_KERNEL_TENSORS	16
#define TPC_NUM_OF_QM_TENSORS		16

#define MME_NUM_OF_LFSR_SEEDS		256

#define NIC_NUMBER_OF_MACROS		12

#define NIC_NUMBER_OF_QM_PER_MACRO	2

#define NIC_NUMBER_OF_ENGINES		(NIC_NUMBER_OF_MACROS * 2)

#define NIC_MAX_NUMBER_OF_PORTS		(NIC_NUMBER_OF_ENGINES * 2)

#define DEVICE_CACHE_LINE_SIZE		128

#endif /* GAUDI2_H */
