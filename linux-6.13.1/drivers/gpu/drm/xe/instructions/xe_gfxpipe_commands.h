/* SPDX-License-Identifier: MIT */
/*
 * Copyright © 2023 Intel Corporation
 */

#ifndef _XE_GFXPIPE_COMMANDS_H_
#define _XE_GFXPIPE_COMMANDS_H_

#include "instructions/xe_instr_defs.h"

#define GFXPIPE_PIPELINE		REG_GENMASK(28, 27)
#define   PIPELINE_COMMON		REG_FIELD_PREP(GFXPIPE_PIPELINE, 0x0)
#define   PIPELINE_SINGLE_DW		REG_FIELD_PREP(GFXPIPE_PIPELINE, 0x1)
#define   PIPELINE_COMPUTE		REG_FIELD_PREP(GFXPIPE_PIPELINE, 0x2)
#define   PIPELINE_3D			REG_FIELD_PREP(GFXPIPE_PIPELINE, 0x3)

#define GFXPIPE_OPCODE			REG_GENMASK(26, 24)
#define GFXPIPE_SUBOPCODE		REG_GENMASK(23, 16)

#define GFXPIPE_MATCH_MASK		(XE_INSTR_CMD_TYPE | \
					 GFXPIPE_PIPELINE | \
					 GFXPIPE_OPCODE | \
					 GFXPIPE_SUBOPCODE)

#define GFXPIPE_COMMON_CMD(opcode, subopcode) \
	(XE_INSTR_GFXPIPE | PIPELINE_COMMON | \
	 REG_FIELD_PREP(GFXPIPE_OPCODE, opcode) | \
	 REG_FIELD_PREP(GFXPIPE_SUBOPCODE, subopcode))

#define GFXPIPE_SINGLE_DW_CMD(opcode, subopcode) \
	(XE_INSTR_GFXPIPE | PIPELINE_SINGLE_DW | \
	 REG_FIELD_PREP(GFXPIPE_OPCODE, opcode) | \
	 REG_FIELD_PREP(GFXPIPE_SUBOPCODE, subopcode))

#define GFXPIPE_3D_CMD(opcode, subopcode) \
	(XE_INSTR_GFXPIPE | PIPELINE_3D | \
	 REG_FIELD_PREP(GFXPIPE_OPCODE, opcode) | \
	 REG_FIELD_PREP(GFXPIPE_SUBOPCODE, subopcode))

#define GFXPIPE_COMPUTE_CMD(opcode, subopcode) \
	(XE_INSTR_GFXPIPE | PIPELINE_COMPUTE | \
	 REG_FIELD_PREP(GFXPIPE_OPCODE, opcode) | \
	 REG_FIELD_PREP(GFXPIPE_SUBOPCODE, subopcode))

#define STATE_BASE_ADDRESS			GFXPIPE_COMMON_CMD(0x1, 0x1)
#define STATE_SIP				GFXPIPE_COMMON_CMD(0x1, 0x2)
#define GPGPU_CSR_BASE_ADDRESS			GFXPIPE_COMMON_CMD(0x1, 0x4)
#define STATE_COMPUTE_MODE			GFXPIPE_COMMON_CMD(0x1, 0x5)
#define CMD_3DSTATE_BTD				GFXPIPE_COMMON_CMD(0x1, 0x6)
#define STATE_SYSTEM_MEM_FENCE_ADDRESS		GFXPIPE_COMMON_CMD(0x1, 0x9)
#define STATE_CONTEXT_DATA_BASE_ADDRESS		GFXPIPE_COMMON_CMD(0x1, 0xB)

#define CMD_3DSTATE_VF_STATISTICS		GFXPIPE_SINGLE_DW_CMD(0x0, 0xB)

#define PIPELINE_SELECT				GFXPIPE_SINGLE_DW_CMD(0x1, 0x4)

#define CMD_3DSTATE_DRAWING_RECTANGLE_FAST	GFXPIPE_3D_CMD(0x0, 0x0)
#define CMD_3DSTATE_CLEAR_PARAMS		GFXPIPE_3D_CMD(0x0, 0x4)
#define CMD_3DSTATE_DEPTH_BUFFER		GFXPIPE_3D_CMD(0x0, 0x5)
#define CMD_3DSTATE_STENCIL_BUFFER		GFXPIPE_3D_CMD(0x0, 0x6)
#define CMD_3DSTATE_HIER_DEPTH_BUFFER		GFXPIPE_3D_CMD(0x0, 0x7)
#define CMD_3DSTATE_VERTEX_BUFFERS		GFXPIPE_3D_CMD(0x0, 0x8)
#define CMD_3DSTATE_VERTEX_ELEMENTS		GFXPIPE_3D_CMD(0x0, 0x9)
#define CMD_3DSTATE_INDEX_BUFFER		GFXPIPE_3D_CMD(0x0, 0xA)
#define CMD_3DSTATE_VF				GFXPIPE_3D_CMD(0x0, 0xC)
#define CMD_3DSTATE_MULTISAMPLE			GFXPIPE_3D_CMD(0x0, 0xD)
#define CMD_3DSTATE_CC_STATE_POINTERS		GFXPIPE_3D_CMD(0x0, 0xE)
#define CMD_3DSTATE_SCISSOR_STATE_POINTERS	GFXPIPE_3D_CMD(0x0, 0xF)
#define CMD_3DSTATE_VS				GFXPIPE_3D_CMD(0x0, 0x10)
#define CMD_3DSTATE_GS				GFXPIPE_3D_CMD(0x0, 0x11)
#define CMD_3DSTATE_CLIP			GFXPIPE_3D_CMD(0x0, 0x12)
#define CMD_3DSTATE_SF				GFXPIPE_3D_CMD(0x0, 0x13)
#define CMD_3DSTATE_WM				GFXPIPE_3D_CMD(0x0, 0x14)
#define CMD_3DSTATE_CONSTANT_VS			GFXPIPE_3D_CMD(0x0, 0x15)
#define CMD_3DSTATE_CONSTANT_GS			GFXPIPE_3D_CMD(0x0, 0x16)
#define CMD_3DSTATE_CONSTANT_PS			GFXPIPE_3D_CMD(0x0, 0x17)
#define CMD_3DSTATE_SAMPLE_MASK			GFXPIPE_3D_CMD(0x0, 0x18)
#define CMD_3DSTATE_CONSTANT_HS			GFXPIPE_3D_CMD(0x0, 0x19)
#define CMD_3DSTATE_CONSTANT_DS			GFXPIPE_3D_CMD(0x0, 0x1A)
#define CMD_3DSTATE_HS				GFXPIPE_3D_CMD(0x0, 0x1B)
#define CMD_3DSTATE_TE				GFXPIPE_3D_CMD(0x0, 0x1C)
#define CMD_3DSTATE_DS				GFXPIPE_3D_CMD(0x0, 0x1D)
#define CMD_3DSTATE_STREAMOUT			GFXPIPE_3D_CMD(0x0, 0x1E)
#define CMD_3DSTATE_SBE				GFXPIPE_3D_CMD(0x0, 0x1F)
#define CMD_3DSTATE_PS				GFXPIPE_3D_CMD(0x0, 0x20)
#define CMD_3DSTATE_VIEWPORT_STATE_POINTERS_SF_CLIP	GFXPIPE_3D_CMD(0x0, 0x21)
#define CMD_3DSTATE_CPS_POINTERS		GFXPIPE_3D_CMD(0x0, 0x22)
#define CMD_3DSTATE_VIEWPORT_STATE_POINTERS_CC	GFXPIPE_3D_CMD(0x0, 0x23)
#define CMD_3DSTATE_BLEND_STATE_POINTERS	GFXPIPE_3D_CMD(0x0, 0x24)
#define CMD_3DSTATE_BINDING_TABLE_POINTERS_VS	GFXPIPE_3D_CMD(0x0, 0x26)
#define CMD_3DSTATE_BINDING_TABLE_POINTERS_HS	GFXPIPE_3D_CMD(0x0, 0x27)
#define CMD_3DSTATE_BINDING_TABLE_POINTERS_DS	GFXPIPE_3D_CMD(0x0, 0x28)
#define CMD_3DSTATE_BINDING_TABLE_POINTERS_GS	GFXPIPE_3D_CMD(0x0, 0x29)
#define CMD_3DSTATE_BINDING_TABLE_POINTERS_PS	GFXPIPE_3D_CMD(0x0, 0x2A)
#define CMD_3DSTATE_SAMPLER_STATE_POINTERS_VS	GFXPIPE_3D_CMD(0x0, 0x2B)
#define CMD_3DSTATE_SAMPLER_STATE_POINTERS_HS	GFXPIPE_3D_CMD(0x0, 0x2C)
#define CMD_3DSTATE_SAMPLER_STATE_POINTERS_DS	GFXPIPE_3D_CMD(0x0, 0x2D)
#define CMD_3DSTATE_SAMPLER_STATE_POINTERS_GS	GFXPIPE_3D_CMD(0x0, 0x2E)
#define CMD_3DSTATE_SAMPLER_STATE_POINTERS_PS	GFXPIPE_3D_CMD(0x0, 0x2F)
#define CMD_3DSTATE_VF_INSTANCING		GFXPIPE_3D_CMD(0x0, 0x49)
#define CMD_3DSTATE_VF_SGVS			GFXPIPE_3D_CMD(0x0, 0x4A)
#define CMD_3DSTATE_VF_TOPOLOGY			GFXPIPE_3D_CMD(0x0, 0x4B)
#define CMD_3DSTATE_WM_CHROMAKEY		GFXPIPE_3D_CMD(0x0, 0x4C)
#define CMD_3DSTATE_PS_BLEND			GFXPIPE_3D_CMD(0x0, 0x4D)
#define CMD_3DSTATE_WM_DEPTH_STENCIL		GFXPIPE_3D_CMD(0x0, 0x4E)
#define CMD_3DSTATE_PS_EXTRA			GFXPIPE_3D_CMD(0x0, 0x4F)
#define CMD_3DSTATE_RASTER			GFXPIPE_3D_CMD(0x0, 0x50)
#define CMD_3DSTATE_SBE_SWIZ			GFXPIPE_3D_CMD(0x0, 0x51)
#define CMD_3DSTATE_WM_HZ_OP			GFXPIPE_3D_CMD(0x0, 0x52)
#define CMD_3DSTATE_VF_COMPONENT_PACKING	GFXPIPE_3D_CMD(0x0, 0x55)
#define CMD_3DSTATE_VF_SGVS_2			GFXPIPE_3D_CMD(0x0, 0x56)
#define CMD_3DSTATE_VFG				GFXPIPE_3D_CMD(0x0, 0x57)
#define CMD_3DSTATE_URB_ALLOC_VS		GFXPIPE_3D_CMD(0x0, 0x58)
#define CMD_3DSTATE_URB_ALLOC_HS		GFXPIPE_3D_CMD(0x0, 0x59)
#define CMD_3DSTATE_URB_ALLOC_DS		GFXPIPE_3D_CMD(0x0, 0x5A)
#define CMD_3DSTATE_URB_ALLOC_GS		GFXPIPE_3D_CMD(0x0, 0x5B)
#define CMD_3DSTATE_SO_BUFFER_INDEX_0		GFXPIPE_3D_CMD(0x0, 0x60)
#define CMD_3DSTATE_SO_BUFFER_INDEX_1		GFXPIPE_3D_CMD(0x0, 0x61)
#define CMD_3DSTATE_SO_BUFFER_INDEX_2		GFXPIPE_3D_CMD(0x0, 0x62)
#define CMD_3DSTATE_SO_BUFFER_INDEX_3		GFXPIPE_3D_CMD(0x0, 0x63)
#define CMD_3DSTATE_PRIMITIVE_REPLICATION	GFXPIPE_3D_CMD(0x0, 0x6C)
#define CMD_3DSTATE_TBIMR_TILE_PASS_INFO	GFXPIPE_3D_CMD(0x0, 0x6E)
#define CMD_3DSTATE_AMFS			GFXPIPE_3D_CMD(0x0, 0x6F)
#define CMD_3DSTATE_DEPTH_BOUNDS		GFXPIPE_3D_CMD(0x0, 0x71)
#define CMD_3DSTATE_AMFS_TEXTURE_POINTERS	GFXPIPE_3D_CMD(0x0, 0x72)
#define CMD_3DSTATE_CONSTANT_TS_POINTER		GFXPIPE_3D_CMD(0x0, 0x73)
#define CMD_3DSTATE_MESH_CONTROL		GFXPIPE_3D_CMD(0x0, 0x77)
#define CMD_3DSTATE_MESH_DISTRIB		GFXPIPE_3D_CMD(0x0, 0x78)
#define CMD_3DSTATE_TASK_REDISTRIB		GFXPIPE_3D_CMD(0x0, 0x79)
#define CMD_3DSTATE_MESH_SHADER			GFXPIPE_3D_CMD(0x0, 0x7A)
#define CMD_3DSTATE_MESH_SHADER_DATA		GFXPIPE_3D_CMD(0x0, 0x7B)
#define CMD_3DSTATE_TASK_CONTROL		GFXPIPE_3D_CMD(0x0, 0x7C)
#define CMD_3DSTATE_TASK_SHADER			GFXPIPE_3D_CMD(0x0, 0x7D)
#define CMD_3DSTATE_TASK_SHADER_DATA		GFXPIPE_3D_CMD(0x0, 0x7E)
#define CMD_3DSTATE_URB_ALLOC_MESH		GFXPIPE_3D_CMD(0x0, 0x7F)
#define CMD_3DSTATE_URB_ALLOC_TASK		GFXPIPE_3D_CMD(0x0, 0x80)
#define CMD_3DSTATE_CLIP_MESH			GFXPIPE_3D_CMD(0x0, 0x81)
#define CMD_3DSTATE_SBE_MESH			GFXPIPE_3D_CMD(0x0, 0x82)
#define CMD_3DSTATE_CPSIZE_CONTROL_BUFFER	GFXPIPE_3D_CMD(0x0, 0x83)

#define CMD_3DSTATE_DRAWING_RECTANGLE		GFXPIPE_3D_CMD(0x1, 0x0)
#define CMD_3DSTATE_CHROMA_KEY			GFXPIPE_3D_CMD(0x1, 0x4)
#define CMD_3DSTATE_POLY_STIPPLE_OFFSET		GFXPIPE_3D_CMD(0x1, 0x6)
#define CMD_3DSTATE_POLY_STIPPLE_PATTERN	GFXPIPE_3D_CMD(0x1, 0x7)
#define CMD_3DSTATE_LINE_STIPPLE		GFXPIPE_3D_CMD(0x1, 0x8)
#define CMD_3DSTATE_AA_LINE_PARAMETERS		GFXPIPE_3D_CMD(0x1, 0xA)
#define CMD_3DSTATE_MONOFILTER_SIZE		GFXPIPE_3D_CMD(0x1, 0x11)
#define CMD_3DSTATE_PUSH_CONSTANT_ALLOC_VS	GFXPIPE_3D_CMD(0x1, 0x12)
#define CMD_3DSTATE_PUSH_CONSTANT_ALLOC_HS	GFXPIPE_3D_CMD(0x1, 0x13)
#define CMD_3DSTATE_PUSH_CONSTANT_ALLOC_DS	GFXPIPE_3D_CMD(0x1, 0x14)
#define CMD_3DSTATE_PUSH_CONSTANT_ALLOC_GS	GFXPIPE_3D_CMD(0x1, 0x15)
#define CMD_3DSTATE_PUSH_CONSTANT_ALLOC_PS	GFXPIPE_3D_CMD(0x1, 0x16)
#define CMD_3DSTATE_SO_DECL_LIST		GFXPIPE_3D_CMD(0x1, 0x17)
#define   CMD_3DSTATE_SO_DECL_LIST_DW_LEN	REG_GENMASK(8, 0)
#define CMD_3DSTATE_SO_BUFFER			GFXPIPE_3D_CMD(0x1, 0x18)
#define CMD_3DSTATE_BINDING_TABLE_POOL_ALLOC	GFXPIPE_3D_CMD(0x1, 0x19)
#define CMD_3DSTATE_SAMPLE_PATTERN		GFXPIPE_3D_CMD(0x1, 0x1C)
#define CMD_3DSTATE_3D_MODE			GFXPIPE_3D_CMD(0x1, 0x1E)
#define CMD_3DSTATE_SUBSLICE_HASH_TABLE		GFXPIPE_3D_CMD(0x1, 0x1F)
#define CMD_3DSTATE_SLICE_TABLE_STATE_POINTERS	GFXPIPE_3D_CMD(0x1, 0x20)
#define CMD_3DSTATE_PTBR_TILE_PASS_INFO		GFXPIPE_3D_CMD(0x1, 0x22)

#endif
