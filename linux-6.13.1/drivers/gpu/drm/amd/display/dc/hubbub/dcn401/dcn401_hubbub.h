/*
 * Copyright 2023 Advanced Micro Devices, Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE COPYRIGHT HOLDER(S) OR AUTHOR(S) BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * Authors: AMD
 *
 */

#ifndef __DC_HUBBUB_DCN401_H__
#define __DC_HUBBUB_DCN401_H__

#include "dcn32/dcn32_hubbub.h"

#define DCN4_01_CRB_SIZE_KB 1344
#define DCN4_01_DEFAULT_DET_SIZE 320
#define DCN4_01_CRB_SEGMENT_SIZE_KB 64

#define HUBBUB_MASK_SH_LIST_DCN4_01(mask_sh)\
	HUBBUB_SF(DCHUBBUB_GLOBAL_TIMER_CNTL, DCHUBBUB_GLOBAL_TIMER_ENABLE, mask_sh), \
	HUBBUB_SF(DCHUBBUB_SOFT_RESET, DCHUBBUB_GLOBAL_SOFT_RESET, mask_sh), \
	HUBBUB_SF(DCHUBBUB_ARB_WATERMARK_CHANGE_CNTL, DCHUBBUB_ARB_WATERMARK_CHANGE_REQUEST, mask_sh), \
	HUBBUB_SF(DCHUBBUB_ARB_WATERMARK_CHANGE_CNTL, DCHUBBUB_ARB_WATERMARK_CHANGE_DONE_INTERRUPT_DISABLE, mask_sh), \
	HUBBUB_SF(DCHUBBUB_ARB_DRAM_STATE_CNTL, DCHUBBUB_ARB_ALLOW_SELF_REFRESH_FORCE_VALUE, mask_sh), \
	HUBBUB_SF(DCHUBBUB_ARB_DRAM_STATE_CNTL, DCHUBBUB_ARB_ALLOW_SELF_REFRESH_FORCE_ENABLE, mask_sh), \
	HUBBUB_SF(DCHUBBUB_ARB_DRAM_STATE_CNTL, DCHUBBUB_ARB_ALLOW_PSTATE_CHANGE_FORCE_VALUE, mask_sh), \
	HUBBUB_SF(DCHUBBUB_ARB_DRAM_STATE_CNTL, DCHUBBUB_ARB_ALLOW_PSTATE_CHANGE_FORCE_ENABLE, mask_sh), \
	HUBBUB_SF(DCHUBBUB_ARB_SAT_LEVEL, DCHUBBUB_ARB_SAT_LEVEL, mask_sh), \
	HUBBUB_SF(DCHUBBUB_ARB_DF_REQ_OUTSTAND, DCHUBBUB_ARB_MIN_REQ_OUTSTAND, mask_sh), \
	HUBBUB_SF(DCHUBBUB_ARB_DF_REQ_OUTSTAND, DCHUBBUB_ARB_MAX_REQ_OUTSTAND, mask_sh), \
	HUBBUB_SF(DCHUBBUB_ARB_DATA_URGENCY_WATERMARK_A, DCHUBBUB_ARB_DATA_URGENCY_WATERMARK_A, mask_sh), \
	HUBBUB_SF(DCHUBBUB_ARB_DATA_URGENCY_WATERMARK_B, DCHUBBUB_ARB_DATA_URGENCY_WATERMARK_B, mask_sh), \
	HUBBUB_SF(DCHUBBUB_ARB_ALLOW_SR_ENTER_WATERMARK_A, DCHUBBUB_ARB_ALLOW_SR_ENTER_WATERMARK_A, mask_sh), \
	HUBBUB_SF(DCHUBBUB_ARB_ALLOW_SR_EXIT_WATERMARK_A, DCHUBBUB_ARB_ALLOW_SR_EXIT_WATERMARK_A, mask_sh), \
	HUBBUB_SF(DCHUBBUB_ARB_ALLOW_SR_ENTER_WATERMARK_B, DCHUBBUB_ARB_ALLOW_SR_ENTER_WATERMARK_B, mask_sh), \
	HUBBUB_SF(DCHUBBUB_ARB_ALLOW_SR_EXIT_WATERMARK_B, DCHUBBUB_ARB_ALLOW_SR_EXIT_WATERMARK_B, mask_sh), \
	HUBBUB_SF(DCHUBBUB_ARB_ALLOW_SR_ENTER_WATERMARK1_A, DCHUBBUB_ARB_ALLOW_SR_ENTER_WATERMARK1_A, mask_sh), \
	HUBBUB_SF(DCHUBBUB_ARB_ALLOW_SR_EXIT_WATERMARK1_A, DCHUBBUB_ARB_ALLOW_SR_EXIT_WATERMARK1_A, mask_sh), \
	HUBBUB_SF(DCHUBBUB_ARB_ALLOW_SR_ENTER_WATERMARK1_B, DCHUBBUB_ARB_ALLOW_SR_ENTER_WATERMARK1_B, mask_sh), \
	HUBBUB_SF(DCHUBBUB_ARB_ALLOW_SR_EXIT_WATERMARK1_B, DCHUBBUB_ARB_ALLOW_SR_EXIT_WATERMARK1_B, mask_sh), \
	HUBBUB_SF(DCHUBBUB_ARB_ALLOW_SR_ENTER_WATERMARK2_A, DCHUBBUB_ARB_ALLOW_SR_ENTER_WATERMARK2_A, mask_sh), \
	HUBBUB_SF(DCHUBBUB_ARB_ALLOW_SR_EXIT_WATERMARK2_A, DCHUBBUB_ARB_ALLOW_SR_EXIT_WATERMARK2_A, mask_sh), \
	HUBBUB_SF(DCHUBBUB_ARB_ALLOW_SR_ENTER_WATERMARK2_B, DCHUBBUB_ARB_ALLOW_SR_ENTER_WATERMARK2_B, mask_sh), \
	HUBBUB_SF(DCHUBBUB_ARB_ALLOW_SR_EXIT_WATERMARK2_B, DCHUBBUB_ARB_ALLOW_SR_EXIT_WATERMARK2_B, mask_sh), \
	HUBBUB_SF(DCHUBBUB_ARB_ALLOW_SR_ENTER_WATERMARK3_A, DCHUBBUB_ARB_ALLOW_SR_ENTER_WATERMARK3_A, mask_sh), \
	HUBBUB_SF(DCHUBBUB_ARB_ALLOW_SR_EXIT_WATERMARK3_A, DCHUBBUB_ARB_ALLOW_SR_EXIT_WATERMARK3_A, mask_sh), \
	HUBBUB_SF(DCHUBBUB_ARB_ALLOW_SR_ENTER_WATERMARK3_B, DCHUBBUB_ARB_ALLOW_SR_ENTER_WATERMARK3_B, mask_sh), \
	HUBBUB_SF(DCHUBBUB_ARB_ALLOW_SR_EXIT_WATERMARK3_B, DCHUBBUB_ARB_ALLOW_SR_EXIT_WATERMARK3_B, mask_sh), \
	HUBBUB_SF(DCHUBBUB_GLOBAL_TIMER_CNTL, DCHUBBUB_GLOBAL_TIMER_REFDIV, mask_sh), \
	HUBBUB_SF(DCN_VM_FB_LOCATION_BASE, FB_BASE, mask_sh), \
	HUBBUB_SF(DCN_VM_FB_LOCATION_TOP, FB_TOP, mask_sh), \
	HUBBUB_SF(DCN_VM_FB_OFFSET, FB_OFFSET, mask_sh), \
	HUBBUB_SF(DCN_VM_AGP_BOT, AGP_BOT, mask_sh), \
	HUBBUB_SF(DCN_VM_AGP_TOP, AGP_TOP, mask_sh), \
	HUBBUB_SF(DCN_VM_AGP_BASE, AGP_BASE, mask_sh), \
	HUBBUB_SF(DCHUBBUB_ARB_FRAC_URG_BW_FLIP_A, DCHUBBUB_ARB_FRAC_URG_BW_FLIP_A, mask_sh), \
	HUBBUB_SF(DCHUBBUB_ARB_FRAC_URG_BW_FLIP_B, DCHUBBUB_ARB_FRAC_URG_BW_FLIP_B, mask_sh), \
	HUBBUB_SF(DCHUBBUB_ARB_FRAC_URG_BW_NOM_A, DCHUBBUB_ARB_FRAC_URG_BW_NOM_A, mask_sh), \
	HUBBUB_SF(DCHUBBUB_ARB_FRAC_URG_BW_NOM_B, DCHUBBUB_ARB_FRAC_URG_BW_NOM_B, mask_sh), \
	HUBBUB_SF(DCHUBBUB_ARB_REFCYC_PER_TRIP_TO_MEMORY_A, DCHUBBUB_ARB_REFCYC_PER_TRIP_TO_MEMORY_A, mask_sh), \
	HUBBUB_SF(DCHUBBUB_ARB_REFCYC_PER_TRIP_TO_MEMORY_B, DCHUBBUB_ARB_REFCYC_PER_TRIP_TO_MEMORY_B, mask_sh), \
	HUBBUB_SF(DCHUBBUB_ARB_REFCYC_PER_META_TRIP_A, DCHUBBUB_ARB_REFCYC_PER_META_TRIP_A, mask_sh), \
	HUBBUB_SF(DCHUBBUB_ARB_REFCYC_PER_META_TRIP_B, DCHUBBUB_ARB_REFCYC_PER_META_TRIP_B, mask_sh), \
	HUBBUB_SF(DCHUBBUB_DEBUG_CTRL_0, DET_DEPTH, mask_sh),\
	HUBBUB_SF(DCHUBBUB_DET0_CTRL, DET0_SIZE, mask_sh),\
	HUBBUB_SF(DCHUBBUB_DET0_CTRL, DET0_SIZE_CURRENT, mask_sh),\
	HUBBUB_SF(DCHUBBUB_DET1_CTRL, DET1_SIZE, mask_sh),\
	HUBBUB_SF(DCHUBBUB_DET1_CTRL, DET1_SIZE_CURRENT, mask_sh),\
	HUBBUB_SF(DCHUBBUB_DET2_CTRL, DET2_SIZE, mask_sh),\
	HUBBUB_SF(DCHUBBUB_DET2_CTRL, DET2_SIZE_CURRENT, mask_sh),\
	HUBBUB_SF(DCHUBBUB_DET3_CTRL, DET3_SIZE, mask_sh),\
	HUBBUB_SF(DCHUBBUB_DET3_CTRL, DET3_SIZE_CURRENT, mask_sh),\
	HUBBUB_SF(DCHUBBUB_COMPBUF_CTRL, COMPBUF_SIZE, mask_sh),\
	HUBBUB_SF(DCHUBBUB_COMPBUF_CTRL, COMPBUF_SIZE_CURRENT, mask_sh),\
	HUBBUB_SF(DCHUBBUB_COMPBUF_CTRL, CONFIG_ERROR, mask_sh),\
	HUBBUB_SF(COMPBUF_RESERVED_SPACE, COMPBUF_RESERVED_SPACE_64B, mask_sh),\
	HUBBUB_SF(DCHUBBUB_ARB_USR_RETRAINING_CNTL, DCHUBBUB_ARB_ALLOW_USR_RETRAINING_FORCE_VALUE, mask_sh),\
	HUBBUB_SF(DCHUBBUB_ARB_USR_RETRAINING_CNTL, DCHUBBUB_ARB_ALLOW_USR_RETRAINING_FORCE_ENABLE, mask_sh),\
	HUBBUB_SF(DCHUBBUB_ARB_USR_RETRAINING_CNTL, DCHUBBUB_ARB_DO_NOT_FORCE_ALLOW_USR_RETRAINING_DURING_PSTATE_CHANGE_REQUEST, mask_sh),\
	HUBBUB_SF(DCHUBBUB_ARB_USR_RETRAINING_CNTL, DCHUBBUB_ARB_DO_NOT_FORCE_ALLOW_USR_RETRAINING_DURING_PRE_CSTATE, mask_sh),\
	HUBBUB_SF(DCHUBBUB_ARB_USR_RETRAINING_WATERMARK_A, DCHUBBUB_ARB_USR_RETRAINING_WATERMARK_A, mask_sh), \
	HUBBUB_SF(DCHUBBUB_ARB_USR_RETRAINING_WATERMARK_B, DCHUBBUB_ARB_USR_RETRAINING_WATERMARK_B, mask_sh), \
	HUBBUB_SF(DCHUBBUB_ARB_UCLK_PSTATE_CHANGE_WATERMARK_A, DCHUBBUB_ARB_UCLK_PSTATE_CHANGE_WATERMARK_A, mask_sh),\
	HUBBUB_SF(DCHUBBUB_ARB_UCLK_PSTATE_CHANGE_WATERMARK_B, DCHUBBUB_ARB_UCLK_PSTATE_CHANGE_WATERMARK_B, mask_sh),\
	HUBBUB_SF(DCHUBBUB_ARB_UCLK_PSTATE_CHANGE_WATERMARK1_A, DCHUBBUB_ARB_UCLK_PSTATE_CHANGE_WATERMARK1_A, mask_sh),\
	HUBBUB_SF(DCHUBBUB_ARB_UCLK_PSTATE_CHANGE_WATERMARK1_B, DCHUBBUB_ARB_UCLK_PSTATE_CHANGE_WATERMARK1_B, mask_sh),\
	HUBBUB_SF(DCHUBBUB_ARB_FCLK_PSTATE_CHANGE_WATERMARK_A, DCHUBBUB_ARB_FCLK_PSTATE_CHANGE_WATERMARK_A, mask_sh),\
	HUBBUB_SF(DCHUBBUB_ARB_FCLK_PSTATE_CHANGE_WATERMARK_B, DCHUBBUB_ARB_FCLK_PSTATE_CHANGE_WATERMARK_B, mask_sh),\
	HUBBUB_SF(DCHUBBUB_ARB_FCLK_PSTATE_CHANGE_WATERMARK1_A, DCHUBBUB_ARB_FCLK_PSTATE_CHANGE_WATERMARK1_A, mask_sh),\
	HUBBUB_SF(DCHUBBUB_ARB_FCLK_PSTATE_CHANGE_WATERMARK1_B, DCHUBBUB_ARB_FCLK_PSTATE_CHANGE_WATERMARK1_B, mask_sh),\
	HUBBUB_SF(DCHUBBUB_ARB_FRAC_URG_BW_MALL_A, DCHUBBUB_ARB_FRAC_URG_BW_MALL_A, mask_sh), \
	HUBBUB_SF(DCHUBBUB_ARB_FRAC_URG_BW_MALL_B, DCHUBBUB_ARB_FRAC_URG_BW_MALL_B, mask_sh), \
	HUBBUB_SF(DCN_VM_FAULT_ADDR_MSB, DCN_VM_FAULT_ADDR_MSB, mask_sh), \
	HUBBUB_SF(DCN_VM_FAULT_ADDR_LSB, DCN_VM_FAULT_ADDR_LSB, mask_sh), \
	HUBBUB_SF(DCN_VM_FAULT_CNTL, DCN_VM_ERROR_STATUS_CLEAR, mask_sh), \
	HUBBUB_SF(DCN_VM_FAULT_CNTL, DCN_VM_ERROR_STATUS_MODE, mask_sh), \
	HUBBUB_SF(DCN_VM_FAULT_CNTL, DCN_VM_ERROR_INTERRUPT_ENABLE, mask_sh), \
	HUBBUB_SF(DCN_VM_FAULT_CNTL, DCN_VM_RANGE_FAULT_DISABLE, mask_sh), \
	HUBBUB_SF(DCN_VM_FAULT_CNTL, DCN_VM_PRQ_FAULT_DISABLE, mask_sh), \
	HUBBUB_SF(DCN_VM_FAULT_STATUS, DCN_VM_ERROR_STATUS, mask_sh), \
	HUBBUB_SF(DCN_VM_FAULT_STATUS, DCN_VM_ERROR_VMID, mask_sh), \
	HUBBUB_SF(DCN_VM_FAULT_STATUS, DCN_VM_ERROR_TABLE_LEVEL, mask_sh), \
	HUBBUB_SF(DCN_VM_FAULT_STATUS, DCN_VM_ERROR_PIPE, mask_sh), \
	HUBBUB_SF(DCN_VM_FAULT_STATUS, DCN_VM_ERROR_INTERRUPT_STATUS, mask_sh),\
	HUBBUB_SF(SDPIF_REQUEST_RATE_LIMIT, SDPIF_REQUEST_RATE_LIMIT, mask_sh),\
	HUBBUB_SF(DCHUBBUB_CLOCK_CNTL, DISPCLK_R_DCHUBBUB_GATE_DIS, mask_sh),\
	HUBBUB_SF(DCHUBBUB_CLOCK_CNTL, DCFCLK_R_DCHUBBUB_GATE_DIS, mask_sh),\
	HUBBUB_SF(DCHUBBUB_SDPIF_CFG0, SDPIF_PORT_CONTROL, mask_sh),\
	HUBBUB_SF(DCHUBBUB_SDPIF_CFG1, SDPIF_MAX_NUM_OUTSTANDING, mask_sh),\
	HUBBUB_SF(DCHUBBUB_MEM_PWR_MODE_CTRL, DET_MEM_PWR_LS_MODE, mask_sh),\
	HUBBUB_SF(DCHUBBUB_TIMEOUT_DETECTION_CTRL1, DCHUBBUB_TIMEOUT_ERROR_STATUS, mask_sh),\
	HUBBUB_SF(DCHUBBUB_TIMEOUT_DETECTION_CTRL1, DCHUBBUB_TIMEOUT_REQ_STALL_THRESHOLD, mask_sh),\
	HUBBUB_SF(DCHUBBUB_TIMEOUT_DETECTION_CTRL2, DCHUBBUB_TIMEOUT_PSTATE_STALL_THRESHOLD, mask_sh),\
	HUBBUB_SF(DCHUBBUB_TIMEOUT_DETECTION_CTRL2, DCHUBBUB_TIMEOUT_DETECTION_EN, mask_sh),\
	HUBBUB_SF(DCHUBBUB_TIMEOUT_DETECTION_CTRL2, DCHUBBUB_TIMEOUT_TIMER_RESET, mask_sh),\
	HUBBUB_SF(DCHUBBUB_CTRL_STATUS, ROB_UNDERFLOW_STATUS, mask_sh),\
	HUBBUB_SF(DCHUBBUB_CTRL_STATUS, ROB_OVERFLOW_STATUS, mask_sh),\
	HUBBUB_SF(DCHUBBUB_CTRL_STATUS, ROB_OVERFLOW_CLEAR, mask_sh),\
	HUBBUB_SF(DCHUBBUB_CTRL_STATUS, DCHUBBUB_HW_DEBUG, mask_sh),\
	HUBBUB_SF(DCHUBBUB_CTRL_STATUS, CSTATE_SWATH_CHK_GOOD_MODE, mask_sh)

bool hubbub401_program_urgent_watermarks(
		struct hubbub *hubbub,
		union dcn_watermark_set *watermarks,
		unsigned int refclk_mhz,
		bool safe_to_lower);

bool hubbub401_program_stutter_watermarks(
		struct hubbub *hubbub,
		union dcn_watermark_set *watermarks,
		unsigned int refclk_mhz,
		bool safe_to_lower);

bool hubbub401_program_pstate_watermarks(
		struct hubbub *hubbub,
		union dcn_watermark_set *watermarks,
		unsigned int refclk_mhz,
		bool safe_to_lower);

bool hubbub401_program_usr_watermarks(
		struct hubbub *hubbub,
		union dcn_watermark_set *watermarks,
		unsigned int refclk_mhz,
		bool safe_to_lower);

bool hubbub401_dcc_support_swizzle(
		enum swizzle_mode_addr3_values swizzle,
		unsigned int plane_pitch,
		unsigned int bytes_per_element,
		enum segment_order *segment_order_horz,
		enum segment_order *segment_order_vert);

bool hubbub401_dcc_support_pixel_format(
			enum surface_pixel_format format,
			unsigned int *plane0_bpe,
			unsigned int *plane1_bpe);

void hubbub401_get_blk256_size(
		unsigned int *blk256_width,
		unsigned int *blk256_height,
		unsigned int bytes_per_element);

void hubbub401_det_request_size(
		unsigned int detile_buf_size,
		enum surface_pixel_format format,
		unsigned int p0_height,
		unsigned int p0_width,
		unsigned int p0_bpe,
		unsigned int p1_height,
		unsigned int p1_width,
		unsigned int p1_bpe,
		bool *p0_req128_horz_wc,
		bool *p0_req128_vert_wc,
		bool *p1_req128_horz_wc,
		bool *p1_req128_vert_wc);
bool hubbub401_get_dcc_compression_cap(
		struct hubbub *hubbub,
		const struct dc_dcc_surface_param *input,
		struct dc_surface_dcc_cap *output);

void hubbub401_construct(struct dcn20_hubbub *hubbub2,
	struct dc_context *ctx,
	const struct dcn_hubbub_registers *hubbub_regs,
	const struct dcn_hubbub_shift *hubbub_shift,
	const struct dcn_hubbub_mask *hubbub_mask,
	int det_size_kb,
	int pixel_chunk_size_kb,
	int config_return_buffer_size_kb);

#endif
