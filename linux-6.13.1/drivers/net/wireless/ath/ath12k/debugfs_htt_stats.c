// SPDX-License-Identifier: BSD-3-Clause-Clear
/*
 * Copyright (c) 2018-2021 The Linux Foundation. All rights reserved.
 * Copyright (c) 2021-2024 Qualcomm Innovation Center, Inc. All rights reserved.
 */

#include <linux/vmalloc.h>
#include "core.h"
#include "debug.h"
#include "debugfs_htt_stats.h"
#include "dp_tx.h"
#include "dp_rx.h"

static u32
print_array_to_buf_index(u8 *buf, u32 offset, const char *header, u32 stats_index,
			 const __le32 *array, u32 array_len, const char *footer)
{
	int index = 0;
	u8 i;

	if (header) {
		index += scnprintf(buf + offset,
				   ATH12K_HTT_STATS_BUF_SIZE - offset,
				   "%s = ", header);
	}
	for (i = 0; i < array_len; i++) {
		index += scnprintf(buf + offset + index,
				   (ATH12K_HTT_STATS_BUF_SIZE - offset) - index,
				   " %u:%u,", stats_index++, le32_to_cpu(array[i]));
	}
	/* To overwrite the last trailing comma */
	index--;
	*(buf + offset + index) = '\0';

	if (footer) {
		index += scnprintf(buf + offset + index,
				   (ATH12K_HTT_STATS_BUF_SIZE - offset) - index,
				   "%s", footer);
	}
	return index;
}

static u32
print_array_to_buf(u8 *buf, u32 offset, const char *header,
		   const __le32 *array, u32 array_len, const char *footer)
{
	return print_array_to_buf_index(buf, offset, header, 0, array, array_len,
					footer);
}

static const char *ath12k_htt_be_tx_rx_ru_size_to_str(u8 ru_size)
{
	switch (ru_size) {
	case ATH12K_HTT_TX_RX_PDEV_STATS_BE_RU_SIZE_26:
		return "26";
	case ATH12K_HTT_TX_RX_PDEV_STATS_BE_RU_SIZE_52:
		return "52";
	case ATH12K_HTT_TX_RX_PDEV_STATS_BE_RU_SIZE_52_26:
		return "52+26";
	case ATH12K_HTT_TX_RX_PDEV_STATS_BE_RU_SIZE_106:
		return "106";
	case ATH12K_HTT_TX_RX_PDEV_STATS_BE_RU_SIZE_106_26:
		return "106+26";
	case ATH12K_HTT_TX_RX_PDEV_STATS_BE_RU_SIZE_242:
		return "242";
	case ATH12K_HTT_TX_RX_PDEV_STATS_BE_RU_SIZE_484:
		return "484";
	case ATH12K_HTT_TX_RX_PDEV_STATS_BE_RU_SIZE_484_242:
		return "484+242";
	case ATH12K_HTT_TX_RX_PDEV_STATS_BE_RU_SIZE_996:
		return "996";
	case ATH12K_HTT_TX_RX_PDEV_STATS_BE_RU_SIZE_996_484:
		return "996+484";
	case ATH12K_HTT_TX_RX_PDEV_STATS_BE_RU_SIZE_996_484_242:
		return "996+484+242";
	case ATH12K_HTT_TX_RX_PDEV_STATS_BE_RU_SIZE_996x2:
		return "996x2";
	case ATH12K_HTT_TX_RX_PDEV_STATS_BE_RU_SIZE_996x2_484:
		return "996x2+484";
	case ATH12K_HTT_TX_RX_PDEV_STATS_BE_RU_SIZE_996x3:
		return "996x3";
	case ATH12K_HTT_TX_RX_PDEV_STATS_BE_RU_SIZE_996x3_484:
		return "996x3+484";
	case ATH12K_HTT_TX_RX_PDEV_STATS_BE_RU_SIZE_996x4:
		return "996x4";
	default:
		return "unknown";
	}
}

static void
htt_print_tx_pdev_stats_cmn_tlv(const void *tag_buf, u16 tag_len,
				struct debug_htt_stats_req *stats_req)
{
	const struct ath12k_htt_tx_pdev_stats_cmn_tlv *htt_stats_buf = tag_buf;
	u8 *buf = stats_req->buf;
	u32 len = stats_req->buf_len;
	u32 buf_len = ATH12K_HTT_STATS_BUF_SIZE;
	u32 mac_id_word;

	if (tag_len < sizeof(*htt_stats_buf))
		return;

	mac_id_word = le32_to_cpu(htt_stats_buf->mac_id__word);

	len += scnprintf(buf + len, buf_len - len, "HTT_TX_PDEV_STATS_CMN_TLV:\n");
	len += scnprintf(buf + len, buf_len - len, "mac_id = %u\n",
			 u32_get_bits(mac_id_word, ATH12K_HTT_STATS_MAC_ID));
	len += scnprintf(buf + len, buf_len - len, "comp_delivered = %u\n",
			 le32_to_cpu(htt_stats_buf->comp_delivered));
	len += scnprintf(buf + len, buf_len - len, "self_triggers = %u\n",
			 le32_to_cpu(htt_stats_buf->self_triggers));
	len += scnprintf(buf + len, buf_len - len, "hw_queued = %u\n",
			 le32_to_cpu(htt_stats_buf->hw_queued));
	len += scnprintf(buf + len, buf_len - len, "hw_reaped = %u\n",
			 le32_to_cpu(htt_stats_buf->hw_reaped));
	len += scnprintf(buf + len, buf_len - len, "underrun = %u\n",
			 le32_to_cpu(htt_stats_buf->underrun));
	len += scnprintf(buf + len, buf_len - len, "hw_paused = %u\n",
			 le32_to_cpu(htt_stats_buf->hw_paused));
	len += scnprintf(buf + len, buf_len - len, "hw_flush = %u\n",
			 le32_to_cpu(htt_stats_buf->hw_flush));
	len += scnprintf(buf + len, buf_len - len, "hw_filt = %u\n",
			 le32_to_cpu(htt_stats_buf->hw_filt));
	len += scnprintf(buf + len, buf_len - len, "tx_abort = %u\n",
			 le32_to_cpu(htt_stats_buf->tx_abort));
	len += scnprintf(buf + len, buf_len - len, "ppdu_ok = %u\n",
			 le32_to_cpu(htt_stats_buf->ppdu_ok));
	len += scnprintf(buf + len, buf_len - len, "mpdu_requeued = %u\n",
			 le32_to_cpu(htt_stats_buf->mpdu_requed));
	len += scnprintf(buf + len, buf_len - len, "tx_xretry = %u\n",
			 le32_to_cpu(htt_stats_buf->tx_xretry));
	len += scnprintf(buf + len, buf_len - len, "data_rc = %u\n",
			 le32_to_cpu(htt_stats_buf->data_rc));
	len += scnprintf(buf + len, buf_len - len, "mpdu_dropped_xretry = %u\n",
			 le32_to_cpu(htt_stats_buf->mpdu_dropped_xretry));
	len += scnprintf(buf + len, buf_len - len, "illegal_rate_phy_err = %u\n",
			 le32_to_cpu(htt_stats_buf->illgl_rate_phy_err));
	len += scnprintf(buf + len, buf_len - len, "cont_xretry = %u\n",
			 le32_to_cpu(htt_stats_buf->cont_xretry));
	len += scnprintf(buf + len, buf_len - len, "tx_timeout = %u\n",
			 le32_to_cpu(htt_stats_buf->tx_timeout));
	len += scnprintf(buf + len, buf_len - len, "tx_time_dur_data = %u\n",
			 le32_to_cpu(htt_stats_buf->tx_time_dur_data));
	len += scnprintf(buf + len, buf_len - len, "pdev_resets = %u\n",
			 le32_to_cpu(htt_stats_buf->pdev_resets));
	len += scnprintf(buf + len, buf_len - len, "phy_underrun = %u\n",
			 le32_to_cpu(htt_stats_buf->phy_underrun));
	len += scnprintf(buf + len, buf_len - len, "txop_ovf = %u\n",
			 le32_to_cpu(htt_stats_buf->txop_ovf));
	len += scnprintf(buf + len, buf_len - len, "seq_posted = %u\n",
			 le32_to_cpu(htt_stats_buf->seq_posted));
	len += scnprintf(buf + len, buf_len - len, "seq_failed_queueing = %u\n",
			 le32_to_cpu(htt_stats_buf->seq_failed_queueing));
	len += scnprintf(buf + len, buf_len - len, "seq_completed = %u\n",
			 le32_to_cpu(htt_stats_buf->seq_completed));
	len += scnprintf(buf + len, buf_len - len, "seq_restarted = %u\n",
			 le32_to_cpu(htt_stats_buf->seq_restarted));
	len += scnprintf(buf + len, buf_len - len, "seq_txop_repost_stop = %u\n",
			 le32_to_cpu(htt_stats_buf->seq_txop_repost_stop));
	len += scnprintf(buf + len, buf_len - len, "next_seq_cancel = %u\n",
			 le32_to_cpu(htt_stats_buf->next_seq_cancel));
	len += scnprintf(buf + len, buf_len - len, "dl_mu_mimo_seq_posted = %u\n",
			 le32_to_cpu(htt_stats_buf->mu_seq_posted));
	len += scnprintf(buf + len, buf_len - len, "dl_mu_ofdma_seq_posted = %u\n",
			 le32_to_cpu(htt_stats_buf->mu_ofdma_seq_posted));
	len += scnprintf(buf + len, buf_len - len, "ul_mu_mimo_seq_posted = %u\n",
			 le32_to_cpu(htt_stats_buf->ul_mumimo_seq_posted));
	len += scnprintf(buf + len, buf_len - len, "ul_mu_ofdma_seq_posted = %u\n",
			 le32_to_cpu(htt_stats_buf->ul_ofdma_seq_posted));
	len += scnprintf(buf + len, buf_len - len, "mu_mimo_peer_blacklisted = %u\n",
			 le32_to_cpu(htt_stats_buf->num_mu_peer_blacklisted));
	len += scnprintf(buf + len, buf_len - len, "seq_qdepth_repost_stop = %u\n",
			 le32_to_cpu(htt_stats_buf->seq_qdepth_repost_stop));
	len += scnprintf(buf + len, buf_len - len, "seq_min_msdu_repost_stop = %u\n",
			 le32_to_cpu(htt_stats_buf->seq_min_msdu_repost_stop));
	len += scnprintf(buf + len, buf_len - len, "mu_seq_min_msdu_repost_stop = %u\n",
			 le32_to_cpu(htt_stats_buf->mu_seq_min_msdu_repost_stop));
	len += scnprintf(buf + len, buf_len - len, "seq_switch_hw_paused = %u\n",
			 le32_to_cpu(htt_stats_buf->seq_switch_hw_paused));
	len += scnprintf(buf + len, buf_len - len, "next_seq_posted_dsr = %u\n",
			 le32_to_cpu(htt_stats_buf->next_seq_posted_dsr));
	len += scnprintf(buf + len, buf_len - len, "seq_posted_isr = %u\n",
			 le32_to_cpu(htt_stats_buf->seq_posted_isr));
	len += scnprintf(buf + len, buf_len - len, "seq_ctrl_cached = %u\n",
			 le32_to_cpu(htt_stats_buf->seq_ctrl_cached));
	len += scnprintf(buf + len, buf_len - len, "mpdu_count_tqm = %u\n",
			 le32_to_cpu(htt_stats_buf->mpdu_count_tqm));
	len += scnprintf(buf + len, buf_len - len, "msdu_count_tqm = %u\n",
			 le32_to_cpu(htt_stats_buf->msdu_count_tqm));
	len += scnprintf(buf + len, buf_len - len, "mpdu_removed_tqm = %u\n",
			 le32_to_cpu(htt_stats_buf->mpdu_removed_tqm));
	len += scnprintf(buf + len, buf_len - len, "msdu_removed_tqm = %u\n",
			 le32_to_cpu(htt_stats_buf->msdu_removed_tqm));
	len += scnprintf(buf + len, buf_len - len, "remove_mpdus_max_retries = %u\n",
			 le32_to_cpu(htt_stats_buf->remove_mpdus_max_retries));
	len += scnprintf(buf + len, buf_len - len, "mpdus_sw_flush = %u\n",
			 le32_to_cpu(htt_stats_buf->mpdus_sw_flush));
	len += scnprintf(buf + len, buf_len - len, "mpdus_hw_filter = %u\n",
			 le32_to_cpu(htt_stats_buf->mpdus_hw_filter));
	len += scnprintf(buf + len, buf_len - len, "mpdus_truncated = %u\n",
			 le32_to_cpu(htt_stats_buf->mpdus_truncated));
	len += scnprintf(buf + len, buf_len - len, "mpdus_ack_failed = %u\n",
			 le32_to_cpu(htt_stats_buf->mpdus_ack_failed));
	len += scnprintf(buf + len, buf_len - len, "mpdus_expired = %u\n",
			 le32_to_cpu(htt_stats_buf->mpdus_expired));
	len += scnprintf(buf + len, buf_len - len, "mpdus_seq_hw_retry = %u\n",
			 le32_to_cpu(htt_stats_buf->mpdus_seq_hw_retry));
	len += scnprintf(buf + len, buf_len - len, "ack_tlv_proc = %u\n",
			 le32_to_cpu(htt_stats_buf->ack_tlv_proc));
	len += scnprintf(buf + len, buf_len - len, "coex_abort_mpdu_cnt_valid = %u\n",
			 le32_to_cpu(htt_stats_buf->coex_abort_mpdu_cnt_valid));
	len += scnprintf(buf + len, buf_len - len, "coex_abort_mpdu_cnt = %u\n",
			 le32_to_cpu(htt_stats_buf->coex_abort_mpdu_cnt));
	len += scnprintf(buf + len, buf_len - len, "num_total_ppdus_tried_ota = %u\n",
			 le32_to_cpu(htt_stats_buf->num_total_ppdus_tried_ota));
	len += scnprintf(buf + len, buf_len - len, "num_data_ppdus_tried_ota = %u\n",
			 le32_to_cpu(htt_stats_buf->num_data_ppdus_tried_ota));
	len += scnprintf(buf + len, buf_len - len, "local_ctrl_mgmt_enqued = %u\n",
			 le32_to_cpu(htt_stats_buf->local_ctrl_mgmt_enqued));
	len += scnprintf(buf + len, buf_len - len, "local_ctrl_mgmt_freed = %u\n",
			 le32_to_cpu(htt_stats_buf->local_ctrl_mgmt_freed));
	len += scnprintf(buf + len, buf_len - len, "local_data_enqued = %u\n",
			 le32_to_cpu(htt_stats_buf->local_data_enqued));
	len += scnprintf(buf + len, buf_len - len, "local_data_freed = %u\n",
			 le32_to_cpu(htt_stats_buf->local_data_freed));
	len += scnprintf(buf + len, buf_len - len, "mpdu_tried = %u\n",
			 le32_to_cpu(htt_stats_buf->mpdu_tried));
	len += scnprintf(buf + len, buf_len - len, "isr_wait_seq_posted = %u\n",
			 le32_to_cpu(htt_stats_buf->isr_wait_seq_posted));
	len += scnprintf(buf + len, buf_len - len, "tx_active_dur_us_low = %u\n",
			 le32_to_cpu(htt_stats_buf->tx_active_dur_us_low));
	len += scnprintf(buf + len, buf_len - len, "tx_active_dur_us_high = %u\n",
			 le32_to_cpu(htt_stats_buf->tx_active_dur_us_high));
	len += scnprintf(buf + len, buf_len - len, "fes_offsets_err_cnt = %u\n\n",
			 le32_to_cpu(htt_stats_buf->fes_offsets_err_cnt));

	stats_req->buf_len = len;
}

static void
htt_print_tx_pdev_stats_urrn_tlv(const void *tag_buf,
				 u16 tag_len,
				 struct debug_htt_stats_req *stats_req)
{
	const struct ath12k_htt_tx_pdev_stats_urrn_tlv *htt_stats_buf = tag_buf;
	u8 *buf = stats_req->buf;
	u32 len = stats_req->buf_len;
	u32 buf_len = ATH12K_HTT_STATS_BUF_SIZE;
	u16 num_elems = min_t(u16, (tag_len >> 2),
			      HTT_TX_PDEV_MAX_URRN_STATS);

	len += scnprintf(buf + len, buf_len - len,
			"HTT_TX_PDEV_STATS_URRN_TLV:\n");

	len += print_array_to_buf(buf, len, "urrn_stats", htt_stats_buf->urrn_stats,
				  num_elems, "\n\n");

	stats_req->buf_len = len;
}

static void
htt_print_tx_pdev_stats_flush_tlv(const void *tag_buf,
				  u16 tag_len,
				  struct debug_htt_stats_req *stats_req)
{
	const struct ath12k_htt_tx_pdev_stats_flush_tlv *htt_stats_buf = tag_buf;
	u8 *buf = stats_req->buf;
	u32 len = stats_req->buf_len;
	u32 buf_len = ATH12K_HTT_STATS_BUF_SIZE;
	u16 num_elems = min_t(u16, (tag_len >> 2),
			      ATH12K_HTT_TX_PDEV_MAX_FLUSH_REASON_STATS);

	len += scnprintf(buf + len, buf_len - len,
			 "HTT_TX_PDEV_STATS_FLUSH_TLV:\n");

	len += print_array_to_buf(buf, len, "flush_errs", htt_stats_buf->flush_errs,
				  num_elems, "\n\n");

	stats_req->buf_len = len;
}

static void
htt_print_tx_pdev_stats_sifs_tlv(const void *tag_buf,
				 u16 tag_len,
				 struct debug_htt_stats_req *stats_req)
{
	const struct ath12k_htt_tx_pdev_stats_sifs_tlv *htt_stats_buf = tag_buf;
	u8 *buf = stats_req->buf;
	u32 len = stats_req->buf_len;
	u32 buf_len = ATH12K_HTT_STATS_BUF_SIZE;
	u16 num_elems = min_t(u16, (tag_len >> 2),
			      ATH12K_HTT_TX_PDEV_MAX_SIFS_BURST_STATS);

	len += scnprintf(buf + len, buf_len - len,
			 "HTT_TX_PDEV_STATS_SIFS_TLV:\n");

	len += print_array_to_buf(buf, len, "sifs_status", htt_stats_buf->sifs_status,
				  num_elems, "\n\n");

	stats_req->buf_len = len;
}

static void
htt_print_tx_pdev_mu_ppdu_dist_stats_tlv(const void *tag_buf, u16 tag_len,
					 struct debug_htt_stats_req *stats_req)
{
	const struct ath12k_htt_tx_pdev_mu_ppdu_dist_stats_tlv *htt_stats_buf = tag_buf;
	char *mode;
	u8 j, hw_mode, i, str_buf_len;
	u8 *buf = stats_req->buf;
	u32 len = stats_req->buf_len;
	u32 buf_len = ATH12K_HTT_STATS_BUF_SIZE;
	u32 stats_value;
	u8 max_ppdu = ATH12K_HTT_STATS_MAX_NUM_MU_PPDU_PER_BURST;
	u8 max_sched = ATH12K_HTT_STATS_MAX_NUM_SCHED_STATUS;
	char str_buf[ATH12K_HTT_MAX_STRING_LEN];

	if (tag_len < sizeof(*htt_stats_buf))
		return;

	hw_mode = le32_to_cpu(htt_stats_buf->hw_mode);

	switch (hw_mode) {
	case ATH12K_HTT_STATS_HWMODE_AC:
		len += scnprintf(buf + len, buf_len - len,
				 "HTT_TX_PDEV_AC_MU_PPDU_DISTRIBUTION_STATS:\n");
		mode = "ac";
		break;
	case ATH12K_HTT_STATS_HWMODE_AX:
		len += scnprintf(buf + len, buf_len - len,
				 "HTT_TX_PDEV_AX_MU_PPDU_DISTRIBUTION_STATS:\n");
		mode = "ax";
		break;
	case ATH12K_HTT_STATS_HWMODE_BE:
		len += scnprintf(buf + len, buf_len - len,
				 "HTT_TX_PDEV_BE_MU_PPDU_DISTRIBUTION_STATS:\n");
		mode = "be";
		break;
	default:
		return;
	}

	for (i = 0; i < ATH12K_HTT_STATS_NUM_NR_BINS ; i++) {
		len += scnprintf(buf + len, buf_len - len,
				 "%s_mu_mimo_num_seq_posted_nr%u = %u\n", mode,
				 ((i + 1) * 4), htt_stats_buf->num_seq_posted[i]);
		str_buf_len = 0;
		memset(str_buf, 0x0, sizeof(str_buf));
		for (j = 0; j < ATH12K_HTT_STATS_MAX_NUM_MU_PPDU_PER_BURST ; j++) {
			stats_value = le32_to_cpu(htt_stats_buf->num_ppdu_posted_per_burst
						  [i * max_ppdu + j]);
			str_buf_len += scnprintf(&str_buf[str_buf_len],
						ATH12K_HTT_MAX_STRING_LEN - str_buf_len,
						" %u:%u,", j, stats_value);
		}
		/* To overwrite the last trailing comma */
		str_buf[str_buf_len - 1] = '\0';
		len += scnprintf(buf + len, buf_len - len,
				 "%s_mu_mimo_num_ppdu_posted_per_burst_nr%u = %s\n",
				 mode, ((i + 1) * 4), str_buf);
		str_buf_len = 0;
		memset(str_buf, 0x0, sizeof(str_buf));
		for (j = 0; j < ATH12K_HTT_STATS_MAX_NUM_MU_PPDU_PER_BURST ; j++) {
			stats_value = le32_to_cpu(htt_stats_buf->num_ppdu_cmpl_per_burst
						  [i * max_ppdu + j]);
			str_buf_len += scnprintf(&str_buf[str_buf_len],
						ATH12K_HTT_MAX_STRING_LEN - str_buf_len,
						" %u:%u,", j, stats_value);
		}
		/* To overwrite the last trailing comma */
		str_buf[str_buf_len - 1] = '\0';
		len += scnprintf(buf + len, buf_len - len,
				 "%s_mu_mimo_num_ppdu_completed_per_burst_nr%u = %s\n",
				 mode, ((i + 1) * 4), str_buf);
		str_buf_len = 0;
		memset(str_buf, 0x0, sizeof(str_buf));
		for (j = 0; j < ATH12K_HTT_STATS_MAX_NUM_SCHED_STATUS ; j++) {
			stats_value = le32_to_cpu(htt_stats_buf->num_seq_term_status
						  [i * max_sched + j]);
			str_buf_len += scnprintf(&str_buf[str_buf_len],
						ATH12K_HTT_MAX_STRING_LEN - str_buf_len,
						" %u:%u,", j, stats_value);
		}
		/* To overwrite the last trailing comma */
		str_buf[str_buf_len - 1] = '\0';
		len += scnprintf(buf + len, buf_len - len,
				 "%s_mu_mimo_num_seq_term_status_nr%u = %s\n\n",
				 mode, ((i + 1) * 4), str_buf);
	}

	stats_req->buf_len = len;
}

static void
htt_print_tx_pdev_stats_sifs_hist_tlv(const void *tag_buf,
				      u16 tag_len,
				      struct debug_htt_stats_req *stats_req)
{
	const struct ath12k_htt_tx_pdev_stats_sifs_hist_tlv *htt_stats_buf = tag_buf;
	u8 *buf = stats_req->buf;
	u32 len = stats_req->buf_len;
	u32 buf_len = ATH12K_HTT_STATS_BUF_SIZE;
	u16 num_elems = min_t(u16, (tag_len >> 2),
			      ATH12K_HTT_TX_PDEV_MAX_SIFS_BURST_HIST_STATS);

	len += scnprintf(buf + len, buf_len - len,
			 "HTT_TX_PDEV_STATS_SIFS_HIST_TLV:\n");

	len += print_array_to_buf(buf, len, "sifs_hist_status",
				  htt_stats_buf->sifs_hist_status, num_elems, "\n\n");

	stats_req->buf_len = len;
}

static void
htt_print_pdev_ctrl_path_tx_stats_tlv(const void *tag_buf, u16 tag_len,
				      struct debug_htt_stats_req *stats_req)
{
	const struct ath12k_htt_pdev_ctrl_path_tx_stats_tlv *htt_stats_buf = tag_buf;
	u8 *buf = stats_req->buf;
	u32 len = stats_req->buf_len;
	u32 buf_len = ATH12K_HTT_STATS_BUF_SIZE;

	if (len < sizeof(*htt_stats_buf))
		return;

	len += scnprintf(buf + len, buf_len - len,
			 "HTT_TX_PDEV_STATS_CTRL_PATH_TX_STATS:\n");
	len += print_array_to_buf(buf, len, "fw_tx_mgmt_subtype",
				 htt_stats_buf->fw_tx_mgmt_subtype,
				 ATH12K_HTT_STATS_SUBTYPE_MAX, "\n\n");

	stats_req->buf_len = len;
}

static void
ath12k_htt_print_stats_tx_sched_cmn_tlv(const void *tag_buf,
					u16 tag_len,
					struct debug_htt_stats_req *stats_req)
{
	const struct ath12k_htt_stats_tx_sched_cmn_tlv *htt_stats_buf = tag_buf;
	u8 *buf = stats_req->buf;
	u32 len = stats_req->buf_len;
	u32 buf_len = ATH12K_HTT_STATS_BUF_SIZE;
	u32 mac_id_word;

	if (tag_len < sizeof(*htt_stats_buf))
		return;

	mac_id_word = __le32_to_cpu(htt_stats_buf->mac_id__word);

	len += scnprintf(buf + len, buf_len - len, "HTT_STATS_TX_SCHED_CMN_TLV:\n");
	len += scnprintf(buf + len, buf_len - len, "mac_id = %u\n",
			 u32_get_bits(mac_id_word, ATH12K_HTT_STATS_MAC_ID));
	len += scnprintf(buf + len, buf_len - len, "current_timestamp = %u\n\n",
			 le32_to_cpu(htt_stats_buf->current_timestamp));

	stats_req->buf_len = len;
}

static void
ath12k_htt_print_tx_pdev_stats_sched_per_txq_tlv(const void *tag_buf,
						 u16 tag_len,
						 struct debug_htt_stats_req *stats_req)
{
	const struct ath12k_htt_tx_pdev_stats_sched_per_txq_tlv *htt_stats_buf = tag_buf;
	u8 *buf = stats_req->buf;
	u32 len = stats_req->buf_len;
	u32 buf_len = ATH12K_HTT_STATS_BUF_SIZE;
	u32 mac_id_word;

	if (tag_len < sizeof(*htt_stats_buf))
		return;

	mac_id_word = __le32_to_cpu(htt_stats_buf->mac_id__word);

	len += scnprintf(buf + len, buf_len - len,
			 "HTT_TX_PDEV_STATS_SCHED_PER_TXQ_TLV:\n");
	len += scnprintf(buf + len, buf_len - len, "mac_id = %u\n",
			u32_get_bits(mac_id_word,
				     ATH12K_HTT_TX_PDEV_STATS_SCHED_PER_TXQ_MAC_ID));
	len += scnprintf(buf + len, buf_len - len, "txq_id = %u\n",
			 u32_get_bits(mac_id_word,
				      ATH12K_HTT_TX_PDEV_STATS_SCHED_PER_TXQ_ID));
	len += scnprintf(buf + len, buf_len - len, "sched_policy = %u\n",
			 le32_to_cpu(htt_stats_buf->sched_policy));
	len += scnprintf(buf + len, buf_len - len,
			 "last_sched_cmd_posted_timestamp = %u\n",
			 le32_to_cpu(htt_stats_buf->last_sched_cmd_posted_timestamp));
	len += scnprintf(buf + len, buf_len - len,
			 "last_sched_cmd_compl_timestamp = %u\n",
			 le32_to_cpu(htt_stats_buf->last_sched_cmd_compl_timestamp));
	len += scnprintf(buf + len, buf_len - len, "sched_2_tac_lwm_count = %u\n",
			 le32_to_cpu(htt_stats_buf->sched_2_tac_lwm_count));
	len += scnprintf(buf + len, buf_len - len, "sched_2_tac_ring_full = %u\n",
			 le32_to_cpu(htt_stats_buf->sched_2_tac_ring_full));
	len += scnprintf(buf + len, buf_len - len, "sched_cmd_post_failure = %u\n",
			 le32_to_cpu(htt_stats_buf->sched_cmd_post_failure));
	len += scnprintf(buf + len, buf_len - len, "num_active_tids = %u\n",
			 le32_to_cpu(htt_stats_buf->num_active_tids));
	len += scnprintf(buf + len, buf_len - len, "num_ps_schedules = %u\n",
			 le32_to_cpu(htt_stats_buf->num_ps_schedules));
	len += scnprintf(buf + len, buf_len - len, "sched_cmds_pending = %u\n",
			 le32_to_cpu(htt_stats_buf->sched_cmds_pending));
	len += scnprintf(buf + len, buf_len - len, "num_tid_register = %u\n",
			 le32_to_cpu(htt_stats_buf->num_tid_register));
	len += scnprintf(buf + len, buf_len - len, "num_tid_unregister = %u\n",
			 le32_to_cpu(htt_stats_buf->num_tid_unregister));
	len += scnprintf(buf + len, buf_len - len, "num_qstats_queried = %u\n",
			 le32_to_cpu(htt_stats_buf->num_qstats_queried));
	len += scnprintf(buf + len, buf_len - len, "qstats_update_pending = %u\n",
			 le32_to_cpu(htt_stats_buf->qstats_update_pending));
	len += scnprintf(buf + len, buf_len - len, "last_qstats_query_timestamp = %u\n",
			 le32_to_cpu(htt_stats_buf->last_qstats_query_timestamp));
	len += scnprintf(buf + len, buf_len - len, "num_tqm_cmdq_full = %u\n",
			 le32_to_cpu(htt_stats_buf->num_tqm_cmdq_full));
	len += scnprintf(buf + len, buf_len - len, "num_de_sched_algo_trigger = %u\n",
			 le32_to_cpu(htt_stats_buf->num_de_sched_algo_trigger));
	len += scnprintf(buf + len, buf_len - len, "num_rt_sched_algo_trigger = %u\n",
			 le32_to_cpu(htt_stats_buf->num_rt_sched_algo_trigger));
	len += scnprintf(buf + len, buf_len - len, "num_tqm_sched_algo_trigger = %u\n",
			 le32_to_cpu(htt_stats_buf->num_tqm_sched_algo_trigger));
	len += scnprintf(buf + len, buf_len - len, "notify_sched = %u\n",
			 le32_to_cpu(htt_stats_buf->notify_sched));
	len += scnprintf(buf + len, buf_len - len, "dur_based_sendn_term = %u\n",
			 le32_to_cpu(htt_stats_buf->dur_based_sendn_term));
	len += scnprintf(buf + len, buf_len - len, "su_notify2_sched = %u\n",
			 le32_to_cpu(htt_stats_buf->su_notify2_sched));
	len += scnprintf(buf + len, buf_len - len, "su_optimal_queued_msdus_sched = %u\n",
			 le32_to_cpu(htt_stats_buf->su_optimal_queued_msdus_sched));
	len += scnprintf(buf + len, buf_len - len, "su_delay_timeout_sched = %u\n",
			 le32_to_cpu(htt_stats_buf->su_delay_timeout_sched));
	len += scnprintf(buf + len, buf_len - len, "su_min_txtime_sched_delay = %u\n",
			 le32_to_cpu(htt_stats_buf->su_min_txtime_sched_delay));
	len += scnprintf(buf + len, buf_len - len, "su_no_delay = %u\n",
			 le32_to_cpu(htt_stats_buf->su_no_delay));
	len += scnprintf(buf + len, buf_len - len, "num_supercycles = %u\n",
			 le32_to_cpu(htt_stats_buf->num_supercycles));
	len += scnprintf(buf + len, buf_len - len, "num_subcycles_with_sort = %u\n",
			 le32_to_cpu(htt_stats_buf->num_subcycles_with_sort));
	len += scnprintf(buf + len, buf_len - len, "num_subcycles_no_sort = %u\n\n",
			 le32_to_cpu(htt_stats_buf->num_subcycles_no_sort));

	stats_req->buf_len = len;
}

static void
ath12k_htt_print_sched_txq_cmd_posted_tlv(const void *tag_buf,
					  u16 tag_len,
					  struct debug_htt_stats_req *stats_req)
{
	const struct ath12k_htt_sched_txq_cmd_posted_tlv *htt_stats_buf = tag_buf;
	u8 *buf = stats_req->buf;
	u32 len = stats_req->buf_len;
	u32 buf_len = ATH12K_HTT_STATS_BUF_SIZE;
	u16 num_elements = tag_len >> 2;

	len += scnprintf(buf + len, buf_len - len, "HTT_SCHED_TXQ_CMD_POSTED_TLV:\n");
	len += print_array_to_buf(buf, len, "sched_cmd_posted",
				  htt_stats_buf->sched_cmd_posted, num_elements, "\n\n");

	stats_req->buf_len = len;
}

static void
ath12k_htt_print_sched_txq_cmd_reaped_tlv(const void *tag_buf,
					  u16 tag_len,
					  struct debug_htt_stats_req *stats_req)
{
	const struct ath12k_htt_sched_txq_cmd_reaped_tlv *htt_stats_buf = tag_buf;
	u8 *buf = stats_req->buf;
	u32 len = stats_req->buf_len;
	u32 buf_len = ATH12K_HTT_STATS_BUF_SIZE;
	u16 num_elements = tag_len >> 2;

	len += scnprintf(buf + len, buf_len - len, "HTT_SCHED_TXQ_CMD_REAPED_TLV:\n");
	len += print_array_to_buf(buf, len, "sched_cmd_reaped",
				  htt_stats_buf->sched_cmd_reaped, num_elements, "\n\n");

	stats_req->buf_len = len;
}

static void
ath12k_htt_print_sched_txq_sched_order_su_tlv(const void *tag_buf,
					      u16 tag_len,
					      struct debug_htt_stats_req *stats_req)
{
	const struct ath12k_htt_sched_txq_sched_order_su_tlv *htt_stats_buf = tag_buf;
	u8 *buf = stats_req->buf;
	u32 len = stats_req->buf_len;
	u32 buf_len = ATH12K_HTT_STATS_BUF_SIZE;
	u32 sched_order_su_num_entries = min_t(u32, (tag_len >> 2),
					       ATH12K_HTT_TX_PDEV_NUM_SCHED_ORDER_LOG);

	len += scnprintf(buf + len, buf_len - len,
			 "HTT_SCHED_TXQ_SCHED_ORDER_SU_TLV:\n");
	len += print_array_to_buf(buf, len, "sched_order_su",
				  htt_stats_buf->sched_order_su,
				  sched_order_su_num_entries, "\n\n");

	stats_req->buf_len = len;
}

static void
ath12k_htt_print_sched_txq_sched_ineligibility_tlv(const void *tag_buf,
						   u16 tag_len,
						   struct debug_htt_stats_req *stats_req)
{
	const struct ath12k_htt_sched_txq_sched_ineligibility_tlv *htt_stats_buf =
		     tag_buf;
	u8 *buf = stats_req->buf;
	u32 len = stats_req->buf_len;
	u32 buf_len = ATH12K_HTT_STATS_BUF_SIZE;
	u32 sched_ineligibility_num_entries = tag_len >> 2;

	len += scnprintf(buf + len, buf_len - len,
			 "HTT_SCHED_TXQ_SCHED_INELIGIBILITY:\n");
	len += print_array_to_buf(buf, len, "sched_ineligibility",
				  htt_stats_buf->sched_ineligibility,
				  sched_ineligibility_num_entries, "\n\n");

	stats_req->buf_len = len;
}

static void
ath12k_htt_print_sched_txq_supercycle_trigger_tlv(const void *tag_buf,
						  u16 tag_len,
						  struct debug_htt_stats_req *stats_req)
{
	const struct ath12k_htt_sched_txq_supercycle_triggers_tlv *htt_stats_buf =
		     tag_buf;
	u8 *buf = stats_req->buf;
	u32 len = stats_req->buf_len;
	u32 buf_len = ATH12K_HTT_STATS_BUF_SIZE;
	u16 num_elems = min_t(u16, (tag_len >> 2),
			      ATH12K_HTT_SCHED_SUPERCYCLE_TRIGGER_MAX);

	len += scnprintf(buf + len, buf_len - len,
			 "HTT_SCHED_TXQ_SUPERCYCLE_TRIGGER:\n");
	len += print_array_to_buf(buf, len, "supercycle_triggers",
				  htt_stats_buf->supercycle_triggers, num_elems, "\n\n");

	stats_req->buf_len = len;
}

static void
ath12k_htt_print_hw_stats_pdev_errs_tlv(const void *tag_buf, u16 tag_len,
					struct debug_htt_stats_req *stats_req)
{
	const struct ath12k_htt_hw_stats_pdev_errs_tlv *htt_buf = tag_buf;
	u8 *buf = stats_req->buf;
	u32 len = stats_req->buf_len;
	u32 buf_len = ATH12K_HTT_STATS_BUF_SIZE;
	u32 mac_id_word;

	if (tag_len < sizeof(*htt_buf))
		return;

	mac_id_word = le32_to_cpu(htt_buf->mac_id__word);

	len += scnprintf(buf + len, buf_len - len, "HTT_HW_STATS_PDEV_ERRS_TLV:\n");
	len += scnprintf(buf + len, buf_len - len, "mac_id = %u\n",
			 u32_get_bits(mac_id_word, ATH12K_HTT_STATS_MAC_ID));
	len += scnprintf(buf + len, buf_len - len, "tx_abort = %u\n",
			 le32_to_cpu(htt_buf->tx_abort));
	len += scnprintf(buf + len, buf_len - len, "tx_abort_fail_count = %u\n",
			 le32_to_cpu(htt_buf->tx_abort_fail_count));
	len += scnprintf(buf + len, buf_len - len, "rx_abort = %u\n",
			 le32_to_cpu(htt_buf->rx_abort));
	len += scnprintf(buf + len, buf_len - len, "rx_abort_fail_count = %u\n",
			 le32_to_cpu(htt_buf->rx_abort_fail_count));
	len += scnprintf(buf + len, buf_len - len, "rx_flush_cnt = %u\n",
			 le32_to_cpu(htt_buf->rx_flush_cnt));
	len += scnprintf(buf + len, buf_len - len, "warm_reset = %u\n",
			 le32_to_cpu(htt_buf->warm_reset));
	len += scnprintf(buf + len, buf_len - len, "cold_reset = %u\n",
			 le32_to_cpu(htt_buf->cold_reset));
	len += scnprintf(buf + len, buf_len - len, "mac_cold_reset_restore_cal = %u\n",
			 le32_to_cpu(htt_buf->mac_cold_reset_restore_cal));
	len += scnprintf(buf + len, buf_len - len, "mac_cold_reset = %u\n",
			 le32_to_cpu(htt_buf->mac_cold_reset));
	len += scnprintf(buf + len, buf_len - len, "mac_warm_reset = %u\n",
			 le32_to_cpu(htt_buf->mac_warm_reset));
	len += scnprintf(buf + len, buf_len - len, "mac_only_reset = %u\n",
			 le32_to_cpu(htt_buf->mac_only_reset));
	len += scnprintf(buf + len, buf_len - len, "phy_warm_reset = %u\n",
			 le32_to_cpu(htt_buf->phy_warm_reset));
	len += scnprintf(buf + len, buf_len - len, "phy_warm_reset_ucode_trig = %u\n",
			 le32_to_cpu(htt_buf->phy_warm_reset_ucode_trig));
	len += scnprintf(buf + len, buf_len - len, "mac_warm_reset_restore_cal = %u\n",
			 le32_to_cpu(htt_buf->mac_warm_reset_restore_cal));
	len += scnprintf(buf + len, buf_len - len, "mac_sfm_reset = %u\n",
			 le32_to_cpu(htt_buf->mac_sfm_reset));
	len += scnprintf(buf + len, buf_len - len, "phy_warm_reset_m3_ssr = %u\n",
			 le32_to_cpu(htt_buf->phy_warm_reset_m3_ssr));
	len += scnprintf(buf + len, buf_len - len, "fw_rx_rings_reset = %u\n",
			 le32_to_cpu(htt_buf->fw_rx_rings_reset));
	len += scnprintf(buf + len, buf_len - len, "tx_flush = %u\n",
			 le32_to_cpu(htt_buf->tx_flush));
	len += scnprintf(buf + len, buf_len - len, "tx_glb_reset = %u\n",
			 le32_to_cpu(htt_buf->tx_glb_reset));
	len += scnprintf(buf + len, buf_len - len, "tx_txq_reset = %u\n",
			 le32_to_cpu(htt_buf->tx_txq_reset));
	len += scnprintf(buf + len, buf_len - len, "rx_timeout_reset = %u\n\n",
			 le32_to_cpu(htt_buf->rx_timeout_reset));

	len += scnprintf(buf + len, buf_len - len, "PDEV_PHY_WARM_RESET_REASONS:\n");
	len += scnprintf(buf + len, buf_len - len, "phy_warm_reset_reason_phy_m3 = %u\n",
			 le32_to_cpu(htt_buf->phy_warm_reset_reason_phy_m3));
	len += scnprintf(buf + len, buf_len - len,
			 "phy_warm_reset_reason_tx_hw_stuck = %u\n",
			 le32_to_cpu(htt_buf->phy_warm_reset_reason_tx_hw_stuck));
	len += scnprintf(buf + len, buf_len - len,
			 "phy_warm_reset_reason_num_cca_rx_frame_stuck = %u\n",
			 le32_to_cpu(htt_buf->phy_warm_reset_reason_num_rx_frame_stuck));
	len += scnprintf(buf + len, buf_len - len,
			 "phy_warm_reset_reason_wal_rx_recovery_rst_rx_busy = %u\n",
			 le32_to_cpu(htt_buf->phy_warm_reset_reason_wal_rx_rec_rx_busy));
	len += scnprintf(buf + len, buf_len - len,
			 "phy_warm_reset_reason_wal_rx_recovery_rst_mac_hang = %u\n",
			 le32_to_cpu(htt_buf->phy_warm_reset_reason_wal_rx_rec_mac_hng));
	len += scnprintf(buf + len, buf_len - len,
			 "phy_warm_reset_reason_mac_reset_converted_phy_reset = %u\n",
			 le32_to_cpu(htt_buf->phy_warm_reset_reason_mac_conv_phy_reset));
	len += scnprintf(buf + len, buf_len - len,
			 "phy_warm_reset_reason_tx_lifetime_expiry_cca_stuck = %u\n",
			 le32_to_cpu(htt_buf->phy_warm_reset_reason_tx_exp_cca_stuck));
	len += scnprintf(buf + len, buf_len - len,
			 "phy_warm_reset_reason_tx_consecutive_flush9_war = %u\n",
			 le32_to_cpu(htt_buf->phy_warm_reset_reason_tx_consec_flsh_war));
	len += scnprintf(buf + len, buf_len - len,
			 "phy_warm_reset_reason_tx_hwsch_reset_war = %u\n",
			 le32_to_cpu(htt_buf->phy_warm_reset_reason_tx_hwsch_reset_war));
	len += scnprintf(buf + len, buf_len - len,
			 "phy_warm_reset_reason_hwsch_wdog_or_cca_wdog_war = %u\n\n",
			 le32_to_cpu(htt_buf->phy_warm_reset_reason_hwsch_cca_wdog_war));

	len += scnprintf(buf + len, buf_len - len, "WAL_RX_RECOVERY_STATS:\n");
	len += scnprintf(buf + len, buf_len - len,
			 "wal_rx_recovery_rst_mac_hang_count = %u\n",
			 le32_to_cpu(htt_buf->wal_rx_recovery_rst_mac_hang_cnt));
	len += scnprintf(buf + len, buf_len - len,
			 "wal_rx_recovery_rst_known_sig_count = %u\n",
			 le32_to_cpu(htt_buf->wal_rx_recovery_rst_known_sig_cnt));
	len += scnprintf(buf + len, buf_len - len,
			 "wal_rx_recovery_rst_no_rx_count = %u\n",
			 le32_to_cpu(htt_buf->wal_rx_recovery_rst_no_rx_cnt));
	len += scnprintf(buf + len, buf_len - len,
			 "wal_rx_recovery_rst_no_rx_consecutive_count = %u\n",
			 le32_to_cpu(htt_buf->wal_rx_recovery_rst_no_rx_consec_cnt));
	len += scnprintf(buf + len, buf_len - len,
			 "wal_rx_recovery_rst_rx_busy_count = %u\n",
			 le32_to_cpu(htt_buf->wal_rx_recovery_rst_rx_busy_cnt));
	len += scnprintf(buf + len, buf_len - len,
			 "wal_rx_recovery_rst_phy_mac_hang_count = %u\n\n",
			 le32_to_cpu(htt_buf->wal_rx_recovery_rst_phy_mac_hang_cnt));

	len += scnprintf(buf + len, buf_len - len, "HTT_RX_DEST_DRAIN_STATS:\n");
	len += scnprintf(buf + len, buf_len - len,
			 "rx_dest_drain_rx_descs_leak_prevention_done = %u\n",
			 le32_to_cpu(htt_buf->rx_dest_drain_rx_descs_leak_prevented));
	len += scnprintf(buf + len, buf_len - len,
			 "rx_dest_drain_rx_descs_saved_cnt = %u\n",
			 le32_to_cpu(htt_buf->rx_dest_drain_rx_descs_saved_cnt));
	len += scnprintf(buf + len, buf_len - len,
			 "rx_dest_drain_rxdma2reo_leak_detected = %u\n",
			 le32_to_cpu(htt_buf->rx_dest_drain_rxdma2reo_leak_detected));
	len += scnprintf(buf + len, buf_len - len,
			 "rx_dest_drain_rxdma2fw_leak_detected = %u\n",
			 le32_to_cpu(htt_buf->rx_dest_drain_rxdma2fw_leak_detected));
	len += scnprintf(buf + len, buf_len - len,
			 "rx_dest_drain_rxdma2wbm_leak_detected = %u\n",
			 le32_to_cpu(htt_buf->rx_dest_drain_rxdma2wbm_leak_detected));
	len += scnprintf(buf + len, buf_len - len,
			 "rx_dest_drain_rxdma1_2sw_leak_detected = %u\n",
			 le32_to_cpu(htt_buf->rx_dest_drain_rxdma1_2sw_leak_detected));
	len += scnprintf(buf + len, buf_len - len,
			 "rx_dest_drain_rx_drain_ok_mac_idle = %u\n",
			 le32_to_cpu(htt_buf->rx_dest_drain_rx_drain_ok_mac_idle));
	len += scnprintf(buf + len, buf_len - len,
			 "rx_dest_drain_ok_mac_not_idle = %u\n",
			 le32_to_cpu(htt_buf->rx_dest_drain_ok_mac_not_idle));
	len += scnprintf(buf + len, buf_len - len,
			 "rx_dest_drain_prerequisite_invld = %u\n",
			 le32_to_cpu(htt_buf->rx_dest_drain_prerequisite_invld));
	len += scnprintf(buf + len, buf_len - len,
			 "rx_dest_drain_skip_for_non_lmac_reset = %u\n",
			 le32_to_cpu(htt_buf->rx_dest_drain_skip_non_lmac_reset));
	len += scnprintf(buf + len, buf_len - len,
			 "rx_dest_drain_hw_fifo_not_empty_post_drain_wait = %u\n\n",
			 le32_to_cpu(htt_buf->rx_dest_drain_hw_fifo_notempty_post_wait));

	stats_req->buf_len = len;
}

static void
ath12k_htt_print_hw_stats_intr_misc_tlv(const void *tag_buf, u16 tag_len,
					struct debug_htt_stats_req *stats_req)
{
	const struct ath12k_htt_hw_stats_intr_misc_tlv *htt_stats_buf = tag_buf;
	u8 *buf = stats_req->buf;
	u32 len = stats_req->buf_len;
	u32 buf_len = ATH12K_HTT_STATS_BUF_SIZE;

	if (tag_len < sizeof(*htt_stats_buf))
		return;

	len += scnprintf(buf + len, buf_len - len, "HTT_HW_STATS_INTR_MISC_TLV:\n");
	len += scnprintf(buf + len, buf_len - len, "hw_intr_name = %s\n",
			 htt_stats_buf->hw_intr_name);
	len += scnprintf(buf + len, buf_len - len, "mask = %u\n",
			 le32_to_cpu(htt_stats_buf->mask));
	len += scnprintf(buf + len, buf_len - len, "count = %u\n\n",
			 le32_to_cpu(htt_stats_buf->count));

	stats_req->buf_len = len;
}

static void
ath12k_htt_print_hw_stats_whal_tx_tlv(const void *tag_buf, u16 tag_len,
				      struct debug_htt_stats_req *stats_req)
{
	const struct ath12k_htt_hw_stats_whal_tx_tlv *htt_stats_buf = tag_buf;
	u8 *buf = stats_req->buf;
	u32 len = stats_req->buf_len;
	u32 buf_len = ATH12K_HTT_STATS_BUF_SIZE;
	u32 mac_id_word;

	if (tag_len < sizeof(*htt_stats_buf))
		return;

	mac_id_word = __le32_to_cpu(htt_stats_buf->mac_id__word);

	len += scnprintf(buf + len, buf_len - len, "HTT_HW_STATS_WHAL_TX_TLV:\n");
	len += scnprintf(buf + len, buf_len - len, "mac_id = %u\n",
			 u32_get_bits(mac_id_word, ATH12K_HTT_STATS_MAC_ID));
	len += scnprintf(buf + len, buf_len - len, "last_unpause_ppdu_id = %u\n",
			 le32_to_cpu(htt_stats_buf->last_unpause_ppdu_id));
	len += scnprintf(buf + len, buf_len - len, "hwsch_unpause_wait_tqm_write = %u\n",
			 le32_to_cpu(htt_stats_buf->hwsch_unpause_wait_tqm_write));
	len += scnprintf(buf + len, buf_len - len, "hwsch_dummy_tlv_skipped = %u\n",
			 le32_to_cpu(htt_stats_buf->hwsch_dummy_tlv_skipped));
	len += scnprintf(buf + len, buf_len - len,
			 "hwsch_misaligned_offset_received = %u\n",
			 le32_to_cpu(htt_stats_buf->hwsch_misaligned_offset_received));
	len += scnprintf(buf + len, buf_len - len, "hwsch_reset_count = %u\n",
			 le32_to_cpu(htt_stats_buf->hwsch_reset_count));
	len += scnprintf(buf + len, buf_len - len, "hwsch_dev_reset_war = %u\n",
			 le32_to_cpu(htt_stats_buf->hwsch_dev_reset_war));
	len += scnprintf(buf + len, buf_len - len, "hwsch_delayed_pause = %u\n",
			 le32_to_cpu(htt_stats_buf->hwsch_delayed_pause));
	len += scnprintf(buf + len, buf_len - len, "hwsch_long_delayed_pause = %u\n",
			 le32_to_cpu(htt_stats_buf->hwsch_long_delayed_pause));
	len += scnprintf(buf + len, buf_len - len, "sch_rx_ppdu_no_response = %u\n",
			 le32_to_cpu(htt_stats_buf->sch_rx_ppdu_no_response));
	len += scnprintf(buf + len, buf_len - len, "sch_selfgen_response = %u\n",
			 le32_to_cpu(htt_stats_buf->sch_selfgen_response));
	len += scnprintf(buf + len, buf_len - len, "sch_rx_sifs_resp_trigger= %u\n\n",
			 le32_to_cpu(htt_stats_buf->sch_rx_sifs_resp_trigger));

	stats_req->buf_len = len;
}

static void
ath12k_htt_print_hw_war_tlv(const void *tag_buf, u16 tag_len,
			    struct debug_htt_stats_req *stats_req)
{
	const struct ath12k_htt_hw_war_stats_tlv *htt_stats_buf = tag_buf;
	u8 *buf = stats_req->buf;
	u32 len = stats_req->buf_len;
	u32 buf_len = ATH12K_HTT_STATS_BUF_SIZE;
	u16 fixed_len, array_len;
	u8 i, array_words;
	u32 mac_id;

	if (tag_len < sizeof(*htt_stats_buf))
		return;

	mac_id = __le32_to_cpu(htt_stats_buf->mac_id__word);
	fixed_len = sizeof(*htt_stats_buf);
	array_len = tag_len - fixed_len;
	array_words = array_len >> 2;

	len += scnprintf(buf + len, buf_len - len, "HTT_HW_WAR_STATS_TLV:\n");
	len += scnprintf(buf + len, buf_len - len, "mac_id = %u\n",
			 u32_get_bits(mac_id, ATH12K_HTT_STATS_MAC_ID));

	for (i = 0; i < array_words; i++) {
		len += scnprintf(buf + len, buf_len - len, "hw_war %u = %u\n\n",
				 i, le32_to_cpu(htt_stats_buf->hw_wars[i]));
	}

	stats_req->buf_len = len;
}

static void
ath12k_htt_print_tx_tqm_cmn_stats_tlv(const void *tag_buf, u16 tag_len,
				      struct debug_htt_stats_req *stats_req)
{
	const struct ath12k_htt_tx_tqm_cmn_stats_tlv *htt_stats_buf = tag_buf;
	u8 *buf = stats_req->buf;
	u32 len = stats_req->buf_len;
	u32 buf_len = ATH12K_HTT_STATS_BUF_SIZE;
	u32 mac_id_word;

	if (tag_len < sizeof(*htt_stats_buf))
		return;

	mac_id_word = __le32_to_cpu(htt_stats_buf->mac_id__word);

	len += scnprintf(buf + len, buf_len - len, "HTT_TX_TQM_CMN_STATS_TLV:\n");
	len += scnprintf(buf + len, buf_len - len, "mac_id = %u\n",
			u32_get_bits(mac_id_word, ATH12K_HTT_STATS_MAC_ID));
	len += scnprintf(buf + len, buf_len - len, "max_cmdq_id = %u\n",
			 le32_to_cpu(htt_stats_buf->max_cmdq_id));
	len += scnprintf(buf + len, buf_len - len, "list_mpdu_cnt_hist_intvl = %u\n",
			 le32_to_cpu(htt_stats_buf->list_mpdu_cnt_hist_intvl));
	len += scnprintf(buf + len, buf_len - len, "add_msdu = %u\n",
			 le32_to_cpu(htt_stats_buf->add_msdu));
	len += scnprintf(buf + len, buf_len - len, "q_empty = %u\n",
			 le32_to_cpu(htt_stats_buf->q_empty));
	len += scnprintf(buf + len, buf_len - len, "q_not_empty = %u\n",
			 le32_to_cpu(htt_stats_buf->q_not_empty));
	len += scnprintf(buf + len, buf_len - len, "drop_notification = %u\n",
			 le32_to_cpu(htt_stats_buf->drop_notification));
	len += scnprintf(buf + len, buf_len - len, "desc_threshold = %u\n",
			 le32_to_cpu(htt_stats_buf->desc_threshold));
	len += scnprintf(buf + len, buf_len - len, "hwsch_tqm_invalid_status = %u\n",
			 le32_to_cpu(htt_stats_buf->hwsch_tqm_invalid_status));
	len += scnprintf(buf + len, buf_len - len, "missed_tqm_gen_mpdus = %u\n",
			 le32_to_cpu(htt_stats_buf->missed_tqm_gen_mpdus));
	len += scnprintf(buf + len, buf_len - len,
			 "total_msduq_timestamp_updates = %u\n",
			 le32_to_cpu(htt_stats_buf->msduq_timestamp_updates));
	len += scnprintf(buf + len, buf_len - len,
			 "total_msduq_timestamp_updates_by_get_mpdu_head_info_cmd = %u\n",
			 le32_to_cpu(htt_stats_buf->msduq_updates_mpdu_head_info_cmd));
	len += scnprintf(buf + len, buf_len - len,
			 "total_msduq_timestamp_updates_by_emp_to_nonemp_status = %u\n",
			 le32_to_cpu(htt_stats_buf->msduq_updates_emp_to_nonemp_status));
	len += scnprintf(buf + len, buf_len - len,
			 "total_get_mpdu_head_info_cmds_by_sched_algo_la_query = %u\n",
			 le32_to_cpu(htt_stats_buf->get_mpdu_head_info_cmds_by_query));
	len += scnprintf(buf + len, buf_len - len,
			 "total_get_mpdu_head_info_cmds_by_tac = %u\n",
			 le32_to_cpu(htt_stats_buf->get_mpdu_head_info_cmds_by_tac));
	len += scnprintf(buf + len, buf_len - len,
			 "total_gen_mpdu_cmds_by_sched_algo_la_query = %u\n",
			 le32_to_cpu(htt_stats_buf->gen_mpdu_cmds_by_query));
	len += scnprintf(buf + len, buf_len - len, "active_tqm_tids = %u\n",
			 le32_to_cpu(htt_stats_buf->tqm_active_tids));
	len += scnprintf(buf + len, buf_len - len, "inactive_tqm_tids = %u\n",
			 le32_to_cpu(htt_stats_buf->tqm_inactive_tids));
	len += scnprintf(buf + len, buf_len - len, "tqm_active_msduq_flows = %u\n",
			 le32_to_cpu(htt_stats_buf->tqm_active_msduq_flows));
	len += scnprintf(buf + len, buf_len - len, "hi_prio_q_not_empty = %u\n\n",
			 le32_to_cpu(htt_stats_buf->high_prio_q_not_empty));

	stats_req->buf_len = len;
}

static void
ath12k_htt_print_tx_tqm_error_stats_tlv(const void *tag_buf, u16 tag_len,
					struct debug_htt_stats_req *stats_req)
{
	const struct ath12k_htt_tx_tqm_error_stats_tlv *htt_stats_buf = tag_buf;
	u8 *buf = stats_req->buf;
	u32 len = stats_req->buf_len;
	u32 buf_len = ATH12K_HTT_STATS_BUF_SIZE;

	if (tag_len < sizeof(*htt_stats_buf))
		return;

	len += scnprintf(buf + len, buf_len - len, "HTT_TX_TQM_ERROR_STATS_TLV:\n");
	len += scnprintf(buf + len, buf_len - len, "q_empty_failure = %u\n",
			 le32_to_cpu(htt_stats_buf->q_empty_failure));
	len += scnprintf(buf + len, buf_len - len, "q_not_empty_failure = %u\n",
			 le32_to_cpu(htt_stats_buf->q_not_empty_failure));
	len += scnprintf(buf + len, buf_len - len, "add_msdu_failure = %u\n\n",
			 le32_to_cpu(htt_stats_buf->add_msdu_failure));

	len += scnprintf(buf + len, buf_len - len, "TQM_ERROR_RESET_STATS:\n");
	len += scnprintf(buf + len, buf_len - len, "tqm_cache_ctl_err = %u\n",
			 le32_to_cpu(htt_stats_buf->tqm_cache_ctl_err));
	len += scnprintf(buf + len, buf_len - len, "tqm_soft_reset = %u\n",
			 le32_to_cpu(htt_stats_buf->tqm_soft_reset));
	len += scnprintf(buf + len, buf_len - len,
			 "tqm_reset_total_num_in_use_link_descs = %u\n",
			 le32_to_cpu(htt_stats_buf->tqm_reset_num_in_use_link_descs));
	len += scnprintf(buf + len, buf_len - len,
			 "tqm_reset_worst_case_num_lost_link_descs = %u\n",
			 le32_to_cpu(htt_stats_buf->tqm_reset_num_lost_link_descs));
	len += scnprintf(buf + len, buf_len - len,
			 "tqm_reset_worst_case_num_lost_host_tx_bufs_count = %u\n",
			 le32_to_cpu(htt_stats_buf->tqm_reset_num_lost_host_tx_buf_cnt));
	len += scnprintf(buf + len, buf_len - len,
			 "tqm_reset_num_in_use_link_descs_internal_tqm = %u\n",
			 le32_to_cpu(htt_stats_buf->tqm_reset_num_in_use_internal_tqm));
	len += scnprintf(buf + len, buf_len - len,
			 "tqm_reset_num_in_use_link_descs_wbm_idle_link_ring = %u\n",
			 le32_to_cpu(htt_stats_buf->tqm_reset_num_in_use_idle_link_rng));
	len += scnprintf(buf + len, buf_len - len,
			 "tqm_reset_time_to_tqm_hang_delta_ms = %u\n",
			 le32_to_cpu(htt_stats_buf->tqm_reset_time_to_tqm_hang_delta_ms));
	len += scnprintf(buf + len, buf_len - len, "tqm_reset_recovery_time_ms = %u\n",
			 le32_to_cpu(htt_stats_buf->tqm_reset_recovery_time_ms));
	len += scnprintf(buf + len, buf_len - len, "tqm_reset_num_peers_hdl = %u\n",
			 le32_to_cpu(htt_stats_buf->tqm_reset_num_peers_hdl));
	len += scnprintf(buf + len, buf_len - len,
			 "tqm_reset_cumm_dirty_hw_mpduq_proc_cnt = %u\n",
			 le32_to_cpu(htt_stats_buf->tqm_reset_cumm_dirty_hw_mpduq_cnt));
	len += scnprintf(buf + len, buf_len - len,
			 "tqm_reset_cumm_dirty_hw_msduq_proc = %u\n",
			 le32_to_cpu(htt_stats_buf->tqm_reset_cumm_dirty_hw_msduq_proc));
	len += scnprintf(buf + len, buf_len - len,
			 "tqm_reset_flush_cache_cmd_su_cnt = %u\n",
			 le32_to_cpu(htt_stats_buf->tqm_reset_flush_cache_cmd_su_cnt));
	len += scnprintf(buf + len, buf_len - len,
			 "tqm_reset_flush_cache_cmd_other_cnt = %u\n",
			 le32_to_cpu(htt_stats_buf->tqm_reset_flush_cache_cmd_other_cnt));
	len += scnprintf(buf + len, buf_len - len,
			 "tqm_reset_flush_cache_cmd_trig_type = %u\n",
			 le32_to_cpu(htt_stats_buf->tqm_reset_flush_cache_cmd_trig_type));
	len += scnprintf(buf + len, buf_len - len,
			 "tqm_reset_flush_cache_cmd_trig_cfg = %u\n",
			 le32_to_cpu(htt_stats_buf->tqm_reset_flush_cache_cmd_trig_cfg));
	len += scnprintf(buf + len, buf_len - len,
			 "tqm_reset_flush_cache_cmd_skip_cmd_status_null = %u\n\n",
			 le32_to_cpu(htt_stats_buf->tqm_reset_flush_cmd_skp_status_null));

	stats_req->buf_len = len;
}

static void
ath12k_htt_print_tx_tqm_gen_mpdu_stats_tlv(const void *tag_buf, u16 tag_len,
					   struct debug_htt_stats_req *stats_req)
{
	const struct ath12k_htt_tx_tqm_gen_mpdu_stats_tlv *htt_stats_buf = tag_buf;
	u8 *buf = stats_req->buf;
	u32 len = stats_req->buf_len;
	u32 buf_len = ATH12K_HTT_STATS_BUF_SIZE;
	u16 num_elements = tag_len >> 2;

	len += scnprintf(buf + len, buf_len - len, "HTT_TX_TQM_GEN_MPDU_STATS_TLV:\n");
	len += print_array_to_buf(buf, len, "gen_mpdu_end_reason",
				  htt_stats_buf->gen_mpdu_end_reason, num_elements,
				  "\n\n");

	stats_req->buf_len = len;
}

static void
ath12k_htt_print_tx_tqm_list_mpdu_stats_tlv(const void *tag_buf, u16 tag_len,
					    struct debug_htt_stats_req *stats_req)
{
	const struct ath12k_htt_tx_tqm_list_mpdu_stats_tlv *htt_stats_buf = tag_buf;
	u8 *buf = stats_req->buf;
	u32 len = stats_req->buf_len;
	u32 buf_len = ATH12K_HTT_STATS_BUF_SIZE;
	u16 num_elems = min_t(u16, (tag_len >> 2),
			      ATH12K_HTT_TX_TQM_MAX_LIST_MPDU_END_REASON);

	len += scnprintf(buf + len, buf_len - len, "HTT_TX_TQM_LIST_MPDU_STATS_TLV:\n");
	len += print_array_to_buf(buf, len, "list_mpdu_end_reason",
				  htt_stats_buf->list_mpdu_end_reason, num_elems, "\n\n");

	stats_req->buf_len = len;
}

static void
ath12k_htt_print_tx_tqm_list_mpdu_cnt_tlv(const void *tag_buf, u16 tag_len,
					  struct debug_htt_stats_req *stats_req)
{
	const struct ath12k_htt_tx_tqm_list_mpdu_cnt_tlv *htt_stats_buf = tag_buf;
	u8 *buf = stats_req->buf;
	u32 len = stats_req->buf_len;
	u32 buf_len = ATH12K_HTT_STATS_BUF_SIZE;
	u16 num_elems = min_t(u16, (tag_len >> 2),
			      ATH12K_HTT_TX_TQM_MAX_LIST_MPDU_CNT_HISTOGRAM_BINS);

	len += scnprintf(buf + len, buf_len - len, "HTT_TX_TQM_LIST_MPDU_CNT_TLV_V:\n");
	len += print_array_to_buf(buf, len, "list_mpdu_cnt_hist",
				  htt_stats_buf->list_mpdu_cnt_hist, num_elems, "\n\n");

	stats_req->buf_len = len;
}

static void
ath12k_htt_print_tx_tqm_pdev_stats_tlv(const void *tag_buf, u16 tag_len,
				       struct debug_htt_stats_req *stats_req)
{
	const struct ath12k_htt_tx_tqm_pdev_stats_tlv *htt_stats_buf = tag_buf;
	u8 *buf = stats_req->buf;
	u32 len = stats_req->buf_len;
	u32 buf_len = ATH12K_HTT_STATS_BUF_SIZE;

	if (tag_len < sizeof(*htt_stats_buf))
		return;

	len += scnprintf(buf + len, buf_len - len, "HTT_TX_TQM_PDEV_STATS_TLV_V:\n");
	len += scnprintf(buf + len, buf_len - len, "msdu_count = %u\n",
			 le32_to_cpu(htt_stats_buf->msdu_count));
	len += scnprintf(buf + len, buf_len - len, "mpdu_count = %u\n",
			 le32_to_cpu(htt_stats_buf->mpdu_count));
	len += scnprintf(buf + len, buf_len - len, "remove_msdu = %u\n",
			 le32_to_cpu(htt_stats_buf->remove_msdu));
	len += scnprintf(buf + len, buf_len - len, "remove_mpdu = %u\n",
			 le32_to_cpu(htt_stats_buf->remove_mpdu));
	len += scnprintf(buf + len, buf_len - len, "remove_msdu_ttl = %u\n",
			 le32_to_cpu(htt_stats_buf->remove_msdu_ttl));
	len += scnprintf(buf + len, buf_len - len, "send_bar = %u\n",
			 le32_to_cpu(htt_stats_buf->send_bar));
	len += scnprintf(buf + len, buf_len - len, "bar_sync = %u\n",
			 le32_to_cpu(htt_stats_buf->bar_sync));
	len += scnprintf(buf + len, buf_len - len, "notify_mpdu = %u\n",
			 le32_to_cpu(htt_stats_buf->notify_mpdu));
	len += scnprintf(buf + len, buf_len - len, "sync_cmd = %u\n",
			 le32_to_cpu(htt_stats_buf->sync_cmd));
	len += scnprintf(buf + len, buf_len - len, "write_cmd = %u\n",
			 le32_to_cpu(htt_stats_buf->write_cmd));
	len += scnprintf(buf + len, buf_len - len, "hwsch_trigger = %u\n",
			 le32_to_cpu(htt_stats_buf->hwsch_trigger));
	len += scnprintf(buf + len, buf_len - len, "ack_tlv_proc = %u\n",
			 le32_to_cpu(htt_stats_buf->ack_tlv_proc));
	len += scnprintf(buf + len, buf_len - len, "gen_mpdu_cmd = %u\n",
			 le32_to_cpu(htt_stats_buf->gen_mpdu_cmd));
	len += scnprintf(buf + len, buf_len - len, "gen_list_cmd = %u\n",
			 le32_to_cpu(htt_stats_buf->gen_list_cmd));
	len += scnprintf(buf + len, buf_len - len, "remove_mpdu_cmd = %u\n",
			 le32_to_cpu(htt_stats_buf->remove_mpdu_cmd));
	len += scnprintf(buf + len, buf_len - len, "remove_mpdu_tried_cmd = %u\n",
			 le32_to_cpu(htt_stats_buf->remove_mpdu_tried_cmd));
	len += scnprintf(buf + len, buf_len - len, "mpdu_queue_stats_cmd = %u\n",
			 le32_to_cpu(htt_stats_buf->mpdu_queue_stats_cmd));
	len += scnprintf(buf + len, buf_len - len, "mpdu_head_info_cmd = %u\n",
			 le32_to_cpu(htt_stats_buf->mpdu_head_info_cmd));
	len += scnprintf(buf + len, buf_len - len, "msdu_flow_stats_cmd = %u\n",
			 le32_to_cpu(htt_stats_buf->msdu_flow_stats_cmd));
	len += scnprintf(buf + len, buf_len - len, "remove_msdu_cmd = %u\n",
			 le32_to_cpu(htt_stats_buf->remove_msdu_cmd));
	len += scnprintf(buf + len, buf_len - len, "remove_msdu_ttl_cmd = %u\n",
			 le32_to_cpu(htt_stats_buf->remove_msdu_ttl_cmd));
	len += scnprintf(buf + len, buf_len - len, "flush_cache_cmd = %u\n",
			 le32_to_cpu(htt_stats_buf->flush_cache_cmd));
	len += scnprintf(buf + len, buf_len - len, "update_mpduq_cmd = %u\n",
			 le32_to_cpu(htt_stats_buf->update_mpduq_cmd));
	len += scnprintf(buf + len, buf_len - len, "enqueue = %u\n",
			 le32_to_cpu(htt_stats_buf->enqueue));
	len += scnprintf(buf + len, buf_len - len, "enqueue_notify = %u\n",
			 le32_to_cpu(htt_stats_buf->enqueue_notify));
	len += scnprintf(buf + len, buf_len - len, "notify_mpdu_at_head = %u\n",
			 le32_to_cpu(htt_stats_buf->notify_mpdu_at_head));
	len += scnprintf(buf + len, buf_len - len, "notify_mpdu_state_valid = %u\n",
			 le32_to_cpu(htt_stats_buf->notify_mpdu_state_valid));
	len += scnprintf(buf + len, buf_len - len, "sched_udp_notify1 = %u\n",
			 le32_to_cpu(htt_stats_buf->sched_udp_notify1));
	len += scnprintf(buf + len, buf_len - len, "sched_udp_notify2 = %u\n",
			 le32_to_cpu(htt_stats_buf->sched_udp_notify2));
	len += scnprintf(buf + len, buf_len - len, "sched_nonudp_notify1 = %u\n",
			 le32_to_cpu(htt_stats_buf->sched_nonudp_notify1));
	len += scnprintf(buf + len, buf_len - len, "sched_nonudp_notify2 = %u\n\n",
			 le32_to_cpu(htt_stats_buf->sched_nonudp_notify2));

	stats_req->buf_len = len;
}

static void
ath12k_htt_print_tx_de_cmn_stats_tlv(const void *tag_buf, u16 tag_len,
				     struct debug_htt_stats_req *stats_req)
{
	const struct ath12k_htt_tx_de_cmn_stats_tlv *htt_stats_buf = tag_buf;
	u8 *buf = stats_req->buf;
	u32 len = stats_req->buf_len;
	u32 buf_len = ATH12K_HTT_STATS_BUF_SIZE;
	u32 mac_id_word;

	if (tag_len < sizeof(*htt_stats_buf))
		return;

	mac_id_word = __le32_to_cpu(htt_stats_buf->mac_id__word);

	len += scnprintf(buf + len, buf_len - len, "HTT_TX_DE_CMN_STATS_TLV:\n");
	len += scnprintf(buf + len, buf_len - len, "mac_id = %u\n",
			 u32_get_bits(mac_id_word, ATH12K_HTT_STATS_MAC_ID));
	len += scnprintf(buf + len, buf_len - len, "tcl2fw_entry_count = %u\n",
			 le32_to_cpu(htt_stats_buf->tcl2fw_entry_count));
	len += scnprintf(buf + len, buf_len - len, "not_to_fw = %u\n",
			 le32_to_cpu(htt_stats_buf->not_to_fw));
	len += scnprintf(buf + len, buf_len - len, "invalid_pdev_vdev_peer = %u\n",
			 le32_to_cpu(htt_stats_buf->invalid_pdev_vdev_peer));
	len += scnprintf(buf + len, buf_len - len, "tcl_res_invalid_addrx = %u\n",
			 le32_to_cpu(htt_stats_buf->tcl_res_invalid_addrx));
	len += scnprintf(buf + len, buf_len - len, "wbm2fw_entry_count = %u\n",
			 le32_to_cpu(htt_stats_buf->wbm2fw_entry_count));
	len += scnprintf(buf + len, buf_len - len, "invalid_pdev = %u\n",
			 le32_to_cpu(htt_stats_buf->invalid_pdev));
	len += scnprintf(buf + len, buf_len - len, "tcl_res_addrx_timeout = %u\n",
			 le32_to_cpu(htt_stats_buf->tcl_res_addrx_timeout));
	len += scnprintf(buf + len, buf_len - len, "invalid_vdev = %u\n",
			 le32_to_cpu(htt_stats_buf->invalid_vdev));
	len += scnprintf(buf + len, buf_len - len, "invalid_tcl_exp_frame_desc = %u\n",
			 le32_to_cpu(htt_stats_buf->invalid_tcl_exp_frame_desc));
	len += scnprintf(buf + len, buf_len - len, "vdev_id_mismatch_count = %u\n\n",
			 le32_to_cpu(htt_stats_buf->vdev_id_mismatch_cnt));

	stats_req->buf_len = len;
}

static void
ath12k_htt_print_tx_de_eapol_packets_stats_tlv(const void *tag_buf, u16 tag_len,
					       struct debug_htt_stats_req *stats_req)
{
	const struct ath12k_htt_tx_de_eapol_packets_stats_tlv *htt_stats_buf = tag_buf;
	u8 *buf = stats_req->buf;
	u32 len = stats_req->buf_len;
	u32 buf_len = ATH12K_HTT_STATS_BUF_SIZE;

	if (tag_len < sizeof(*htt_stats_buf))
		return;

	len += scnprintf(buf + len, buf_len - len,
			 "HTT_TX_DE_EAPOL_PACKETS_STATS_TLV:\n");
	len += scnprintf(buf + len, buf_len - len, "m1_packets = %u\n",
			 le32_to_cpu(htt_stats_buf->m1_packets));
	len += scnprintf(buf + len, buf_len - len, "m2_packets = %u\n",
			 le32_to_cpu(htt_stats_buf->m2_packets));
	len += scnprintf(buf + len, buf_len - len, "m3_packets = %u\n",
			 le32_to_cpu(htt_stats_buf->m3_packets));
	len += scnprintf(buf + len, buf_len - len, "m4_packets = %u\n",
			 le32_to_cpu(htt_stats_buf->m4_packets));
	len += scnprintf(buf + len, buf_len - len, "g1_packets = %u\n",
			 le32_to_cpu(htt_stats_buf->g1_packets));
	len += scnprintf(buf + len, buf_len - len, "g2_packets = %u\n",
			 le32_to_cpu(htt_stats_buf->g2_packets));
	len += scnprintf(buf + len, buf_len - len, "rc4_packets = %u\n",
			 le32_to_cpu(htt_stats_buf->rc4_packets));
	len += scnprintf(buf + len, buf_len - len, "eap_packets = %u\n",
			 le32_to_cpu(htt_stats_buf->eap_packets));
	len += scnprintf(buf + len, buf_len - len, "eapol_start_packets = %u\n",
			 le32_to_cpu(htt_stats_buf->eapol_start_packets));
	len += scnprintf(buf + len, buf_len - len, "eapol_logoff_packets = %u\n",
			 le32_to_cpu(htt_stats_buf->eapol_logoff_packets));
	len += scnprintf(buf + len, buf_len - len, "eapol_encap_asf_packets = %u\n\n",
			 le32_to_cpu(htt_stats_buf->eapol_encap_asf_packets));

	stats_req->buf_len = len;
}

static void
ath12k_htt_print_tx_de_classify_stats_tlv(const void *tag_buf, u16 tag_len,
					  struct debug_htt_stats_req *stats_req)
{
	const struct ath12k_htt_tx_de_classify_stats_tlv *htt_stats_buf = tag_buf;
	u8 *buf = stats_req->buf;
	u32 len = stats_req->buf_len;
	u32 buf_len = ATH12K_HTT_STATS_BUF_SIZE;

	if (tag_len < sizeof(*htt_stats_buf))
		return;

	len += scnprintf(buf + len, buf_len - len, "HTT_TX_DE_CLASSIFY_STATS_TLV:\n");
	len += scnprintf(buf + len, buf_len - len, "arp_packets = %u\n",
			 le32_to_cpu(htt_stats_buf->arp_packets));
	len += scnprintf(buf + len, buf_len - len, "igmp_packets = %u\n",
			 le32_to_cpu(htt_stats_buf->igmp_packets));
	len += scnprintf(buf + len, buf_len - len, "dhcp_packets = %u\n",
			 le32_to_cpu(htt_stats_buf->dhcp_packets));
	len += scnprintf(buf + len, buf_len - len, "host_inspected = %u\n",
			 le32_to_cpu(htt_stats_buf->host_inspected));
	len += scnprintf(buf + len, buf_len - len, "htt_included = %u\n",
			 le32_to_cpu(htt_stats_buf->htt_included));
	len += scnprintf(buf + len, buf_len - len, "htt_valid_mcs = %u\n",
			 le32_to_cpu(htt_stats_buf->htt_valid_mcs));
	len += scnprintf(buf + len, buf_len - len, "htt_valid_nss = %u\n",
			 le32_to_cpu(htt_stats_buf->htt_valid_nss));
	len += scnprintf(buf + len, buf_len - len, "htt_valid_preamble_type = %u\n",
			 le32_to_cpu(htt_stats_buf->htt_valid_preamble_type));
	len += scnprintf(buf + len, buf_len - len, "htt_valid_chainmask = %u\n",
			 le32_to_cpu(htt_stats_buf->htt_valid_chainmask));
	len += scnprintf(buf + len, buf_len - len, "htt_valid_guard_interval = %u\n",
			 le32_to_cpu(htt_stats_buf->htt_valid_guard_interval));
	len += scnprintf(buf + len, buf_len - len, "htt_valid_retries = %u\n",
			 le32_to_cpu(htt_stats_buf->htt_valid_retries));
	len += scnprintf(buf + len, buf_len - len, "htt_valid_bw_info = %u\n",
			 le32_to_cpu(htt_stats_buf->htt_valid_bw_info));
	len += scnprintf(buf + len, buf_len - len, "htt_valid_power = %u\n",
			 le32_to_cpu(htt_stats_buf->htt_valid_power));
	len += scnprintf(buf + len, buf_len - len, "htt_valid_key_flags = 0x%x\n",
			 le32_to_cpu(htt_stats_buf->htt_valid_key_flags));
	len += scnprintf(buf + len, buf_len - len, "htt_valid_no_encryption = %u\n",
			 le32_to_cpu(htt_stats_buf->htt_valid_no_encryption));
	len += scnprintf(buf + len, buf_len - len, "fse_entry_count = %u\n",
			 le32_to_cpu(htt_stats_buf->fse_entry_count));
	len += scnprintf(buf + len, buf_len - len, "fse_priority_be = %u\n",
			 le32_to_cpu(htt_stats_buf->fse_priority_be));
	len += scnprintf(buf + len, buf_len - len, "fse_priority_high = %u\n",
			 le32_to_cpu(htt_stats_buf->fse_priority_high));
	len += scnprintf(buf + len, buf_len - len, "fse_priority_low = %u\n",
			 le32_to_cpu(htt_stats_buf->fse_priority_low));
	len += scnprintf(buf + len, buf_len - len, "fse_traffic_ptrn_be = %u\n",
			 le32_to_cpu(htt_stats_buf->fse_traffic_ptrn_be));
	len += scnprintf(buf + len, buf_len - len, "fse_traffic_ptrn_over_sub = %u\n",
			 le32_to_cpu(htt_stats_buf->fse_traffic_ptrn_over_sub));
	len += scnprintf(buf + len, buf_len - len, "fse_traffic_ptrn_bursty = %u\n",
			 le32_to_cpu(htt_stats_buf->fse_traffic_ptrn_bursty));
	len += scnprintf(buf + len, buf_len - len, "fse_traffic_ptrn_interactive = %u\n",
			 le32_to_cpu(htt_stats_buf->fse_traffic_ptrn_interactive));
	len += scnprintf(buf + len, buf_len - len, "fse_traffic_ptrn_periodic = %u\n",
			 le32_to_cpu(htt_stats_buf->fse_traffic_ptrn_periodic));
	len += scnprintf(buf + len, buf_len - len, "fse_hwqueue_alloc = %u\n",
			 le32_to_cpu(htt_stats_buf->fse_hwqueue_alloc));
	len += scnprintf(buf + len, buf_len - len, "fse_hwqueue_created = %u\n",
			 le32_to_cpu(htt_stats_buf->fse_hwqueue_created));
	len += scnprintf(buf + len, buf_len - len, "fse_hwqueue_send_to_host = %u\n",
			 le32_to_cpu(htt_stats_buf->fse_hwqueue_send_to_host));
	len += scnprintf(buf + len, buf_len - len, "mcast_entry = %u\n",
			 le32_to_cpu(htt_stats_buf->mcast_entry));
	len += scnprintf(buf + len, buf_len - len, "bcast_entry = %u\n",
			 le32_to_cpu(htt_stats_buf->bcast_entry));
	len += scnprintf(buf + len, buf_len - len, "htt_update_peer_cache = %u\n",
			 le32_to_cpu(htt_stats_buf->htt_update_peer_cache));
	len += scnprintf(buf + len, buf_len - len, "htt_learning_frame = %u\n",
			 le32_to_cpu(htt_stats_buf->htt_learning_frame));
	len += scnprintf(buf + len, buf_len - len, "fse_invalid_peer = %u\n",
			 le32_to_cpu(htt_stats_buf->fse_invalid_peer));
	len += scnprintf(buf + len, buf_len - len, "mec_notify = %u\n\n",
			 le32_to_cpu(htt_stats_buf->mec_notify));

	stats_req->buf_len = len;
}

static void
ath12k_htt_print_tx_de_classify_failed_stats_tlv(const void *tag_buf, u16 tag_len,
						 struct debug_htt_stats_req *stats_req)
{
	const struct ath12k_htt_tx_de_classify_failed_stats_tlv *htt_stats_buf = tag_buf;
	u8 *buf = stats_req->buf;
	u32 len = stats_req->buf_len;
	u32 buf_len = ATH12K_HTT_STATS_BUF_SIZE;

	if (tag_len < sizeof(*htt_stats_buf))
		return;

	len += scnprintf(buf + len, buf_len - len,
			 "HTT_TX_DE_CLASSIFY_FAILED_STATS_TLV:\n");
	len += scnprintf(buf + len, buf_len - len, "ap_bss_peer_not_found = %u\n",
			 le32_to_cpu(htt_stats_buf->ap_bss_peer_not_found));
	len += scnprintf(buf + len, buf_len - len, "ap_bcast_mcast_no_peer = %u\n",
			 le32_to_cpu(htt_stats_buf->ap_bcast_mcast_no_peer));
	len += scnprintf(buf + len, buf_len - len, "sta_delete_in_progress = %u\n",
			 le32_to_cpu(htt_stats_buf->sta_delete_in_progress));
	len += scnprintf(buf + len, buf_len - len, "ibss_no_bss_peer = %u\n",
			 le32_to_cpu(htt_stats_buf->ibss_no_bss_peer));
	len += scnprintf(buf + len, buf_len - len, "invalid_vdev_type = %u\n",
			 le32_to_cpu(htt_stats_buf->invalid_vdev_type));
	len += scnprintf(buf + len, buf_len - len, "invalid_ast_peer_entry = %u\n",
			 le32_to_cpu(htt_stats_buf->invalid_ast_peer_entry));
	len += scnprintf(buf + len, buf_len - len, "peer_entry_invalid = %u\n",
			 le32_to_cpu(htt_stats_buf->peer_entry_invalid));
	len += scnprintf(buf + len, buf_len - len, "ethertype_not_ip = %u\n",
			 le32_to_cpu(htt_stats_buf->ethertype_not_ip));
	len += scnprintf(buf + len, buf_len - len, "eapol_lookup_failed = %u\n",
			 le32_to_cpu(htt_stats_buf->eapol_lookup_failed));
	len += scnprintf(buf + len, buf_len - len, "qpeer_not_allow_data = %u\n",
			 le32_to_cpu(htt_stats_buf->qpeer_not_allow_data));
	len += scnprintf(buf + len, buf_len - len, "fse_tid_override = %u\n",
			 le32_to_cpu(htt_stats_buf->fse_tid_override));
	len += scnprintf(buf + len, buf_len - len, "ipv6_jumbogram_zero_length = %u\n",
			 le32_to_cpu(htt_stats_buf->ipv6_jumbogram_zero_length));
	len += scnprintf(buf + len, buf_len - len, "qos_to_non_qos_in_prog = %u\n",
			 le32_to_cpu(htt_stats_buf->qos_to_non_qos_in_prog));
	len += scnprintf(buf + len, buf_len - len, "ap_bcast_mcast_eapol = %u\n",
			 le32_to_cpu(htt_stats_buf->ap_bcast_mcast_eapol));
	len += scnprintf(buf + len, buf_len - len, "unicast_on_ap_bss_peer = %u\n",
			 le32_to_cpu(htt_stats_buf->unicast_on_ap_bss_peer));
	len += scnprintf(buf + len, buf_len - len, "ap_vdev_invalid = %u\n",
			 le32_to_cpu(htt_stats_buf->ap_vdev_invalid));
	len += scnprintf(buf + len, buf_len - len, "incomplete_llc = %u\n",
			 le32_to_cpu(htt_stats_buf->incomplete_llc));
	len += scnprintf(buf + len, buf_len - len, "eapol_duplicate_m3 = %u\n",
			 le32_to_cpu(htt_stats_buf->eapol_duplicate_m3));
	len += scnprintf(buf + len, buf_len - len, "eapol_duplicate_m4 = %u\n\n",
			 le32_to_cpu(htt_stats_buf->eapol_duplicate_m4));

	stats_req->buf_len = len;
}

static void
ath12k_htt_print_tx_de_classify_status_stats_tlv(const void *tag_buf, u16 tag_len,
						 struct debug_htt_stats_req *stats_req)
{
	const struct ath12k_htt_tx_de_classify_status_stats_tlv *htt_stats_buf = tag_buf;
	u8 *buf = stats_req->buf;
	u32 len = stats_req->buf_len;
	u32 buf_len = ATH12K_HTT_STATS_BUF_SIZE;

	if (tag_len < sizeof(*htt_stats_buf))
		return;

	len += scnprintf(buf + len, buf_len - len,
			 "HTT_TX_DE_CLASSIFY_STATUS_STATS_TLV:\n");
	len += scnprintf(buf + len, buf_len - len, "eok = %u\n",
			 le32_to_cpu(htt_stats_buf->eok));
	len += scnprintf(buf + len, buf_len - len, "classify_done = %u\n",
			 le32_to_cpu(htt_stats_buf->classify_done));
	len += scnprintf(buf + len, buf_len - len, "lookup_failed = %u\n",
			 le32_to_cpu(htt_stats_buf->lookup_failed));
	len += scnprintf(buf + len, buf_len - len, "send_host_dhcp = %u\n",
			 le32_to_cpu(htt_stats_buf->send_host_dhcp));
	len += scnprintf(buf + len, buf_len - len, "send_host_mcast = %u\n",
			 le32_to_cpu(htt_stats_buf->send_host_mcast));
	len += scnprintf(buf + len, buf_len - len, "send_host_unknown_dest = %u\n",
			 le32_to_cpu(htt_stats_buf->send_host_unknown_dest));
	len += scnprintf(buf + len, buf_len - len, "send_host = %u\n",
			 le32_to_cpu(htt_stats_buf->send_host));
	len += scnprintf(buf + len, buf_len - len, "status_invalid = %u\n\n",
			 le32_to_cpu(htt_stats_buf->status_invalid));

	stats_req->buf_len = len;
}

static void
ath12k_htt_print_tx_de_enqueue_packets_stats_tlv(const void *tag_buf, u16 tag_len,
						 struct debug_htt_stats_req *stats_req)
{
	const struct ath12k_htt_tx_de_enqueue_packets_stats_tlv *htt_stats_buf = tag_buf;
	u8 *buf = stats_req->buf;
	u32 len = stats_req->buf_len;
	u32 buf_len = ATH12K_HTT_STATS_BUF_SIZE;

	if (tag_len < sizeof(*htt_stats_buf))
		return;

	len += scnprintf(buf + len, buf_len - len,
			 "HTT_TX_DE_ENQUEUE_PACKETS_STATS_TLV:\n");
	len += scnprintf(buf + len, buf_len - len, "enqueued_pkts = %u\n",
			 le32_to_cpu(htt_stats_buf->enqueued_pkts));
	len += scnprintf(buf + len, buf_len - len, "to_tqm = %u\n",
			 le32_to_cpu(htt_stats_buf->to_tqm));
	len += scnprintf(buf + len, buf_len - len, "to_tqm_bypass = %u\n\n",
			 le32_to_cpu(htt_stats_buf->to_tqm_bypass));

	stats_req->buf_len = len;
}

static void
ath12k_htt_print_tx_de_enqueue_discard_stats_tlv(const void *tag_buf, u16 tag_len,
						 struct debug_htt_stats_req *stats_req)
{
	const struct ath12k_htt_tx_de_enqueue_discard_stats_tlv *htt_stats_buf = tag_buf;
	u8 *buf = stats_req->buf;
	u32 len = stats_req->buf_len;
	u32 buf_len = ATH12K_HTT_STATS_BUF_SIZE;

	if (tag_len < sizeof(*htt_stats_buf))
		return;

	len += scnprintf(buf + len, buf_len - len,
			 "HTT_TX_DE_ENQUEUE_DISCARD_STATS_TLV:\n");
	len += scnprintf(buf + len, buf_len - len, "discarded_pkts = %u\n",
			 le32_to_cpu(htt_stats_buf->discarded_pkts));
	len += scnprintf(buf + len, buf_len - len, "local_frames = %u\n",
			 le32_to_cpu(htt_stats_buf->local_frames));
	len += scnprintf(buf + len, buf_len - len, "is_ext_msdu = %u\n\n",
			 le32_to_cpu(htt_stats_buf->is_ext_msdu));

	stats_req->buf_len = len;
}

static void
ath12k_htt_print_tx_de_compl_stats_tlv(const void *tag_buf, u16 tag_len,
				       struct debug_htt_stats_req *stats_req)
{
	const struct ath12k_htt_tx_de_compl_stats_tlv *htt_stats_buf = tag_buf;
	u8 *buf = stats_req->buf;
	u32 len = stats_req->buf_len;
	u32 buf_len = ATH12K_HTT_STATS_BUF_SIZE;

	if (tag_len < sizeof(*htt_stats_buf))
		return;

	len += scnprintf(buf + len, buf_len - len, "HTT_TX_DE_COMPL_STATS_TLV:\n");
	len += scnprintf(buf + len, buf_len - len, "tcl_dummy_frame = %u\n",
			 le32_to_cpu(htt_stats_buf->tcl_dummy_frame));
	len += scnprintf(buf + len, buf_len - len, "tqm_dummy_frame = %u\n",
			 le32_to_cpu(htt_stats_buf->tqm_dummy_frame));
	len += scnprintf(buf + len, buf_len - len, "tqm_notify_frame = %u\n",
			 le32_to_cpu(htt_stats_buf->tqm_notify_frame));
	len += scnprintf(buf + len, buf_len - len, "fw2wbm_enq = %u\n",
			 le32_to_cpu(htt_stats_buf->fw2wbm_enq));
	len += scnprintf(buf + len, buf_len - len, "tqm_bypass_frame = %u\n\n",
			 le32_to_cpu(htt_stats_buf->tqm_bypass_frame));

	stats_req->buf_len = len;
}

static void
ath12k_htt_print_tx_selfgen_cmn_stats_tlv(const void *tag_buf, u16 tag_len,
					  struct debug_htt_stats_req *stats_req)
{
	const struct ath12k_htt_tx_selfgen_cmn_stats_tlv *htt_stats_buf = tag_buf;
	u8 *buf = stats_req->buf;
	u32 len = stats_req->buf_len;
	u32 buf_len = ATH12K_HTT_STATS_BUF_SIZE;
	u32 mac_id_word;

	if (tag_len < sizeof(*htt_stats_buf))
		return;

	mac_id_word = __le32_to_cpu(htt_stats_buf->mac_id__word);

	len += scnprintf(buf + len, buf_len - len, "HTT_TX_SELFGEN_CMN_STATS_TLV:\n");
	len += scnprintf(buf + len, buf_len - len, "mac_id = %u\n",
			 u32_get_bits(mac_id_word, ATH12K_HTT_STATS_MAC_ID));
	len += scnprintf(buf + len, buf_len - len, "su_bar = %u\n",
			 le32_to_cpu(htt_stats_buf->su_bar));
	len += scnprintf(buf + len, buf_len - len, "rts = %u\n",
			 le32_to_cpu(htt_stats_buf->rts));
	len += scnprintf(buf + len, buf_len - len, "cts2self = %u\n",
			 le32_to_cpu(htt_stats_buf->cts2self));
	len += scnprintf(buf + len, buf_len - len, "qos_null = %u\n",
			 le32_to_cpu(htt_stats_buf->qos_null));
	len += scnprintf(buf + len, buf_len - len, "delayed_bar_1 = %u\n",
			 le32_to_cpu(htt_stats_buf->delayed_bar_1));
	len += scnprintf(buf + len, buf_len - len, "delayed_bar_2 = %u\n",
			 le32_to_cpu(htt_stats_buf->delayed_bar_2));
	len += scnprintf(buf + len, buf_len - len, "delayed_bar_3 = %u\n",
			 le32_to_cpu(htt_stats_buf->delayed_bar_3));
	len += scnprintf(buf + len, buf_len - len, "delayed_bar_4 = %u\n",
			 le32_to_cpu(htt_stats_buf->delayed_bar_4));
	len += scnprintf(buf + len, buf_len - len, "delayed_bar_5 = %u\n",
			 le32_to_cpu(htt_stats_buf->delayed_bar_5));
	len += scnprintf(buf + len, buf_len - len, "delayed_bar_6 = %u\n",
			 le32_to_cpu(htt_stats_buf->delayed_bar_6));
	len += scnprintf(buf + len, buf_len - len, "delayed_bar_7 = %u\n\n",
			 le32_to_cpu(htt_stats_buf->delayed_bar_7));

	stats_req->buf_len = len;
}

static void
ath12k_htt_print_tx_selfgen_ac_stats_tlv(const void *tag_buf, u16 tag_len,
					 struct debug_htt_stats_req *stats_req)
{
	const struct ath12k_htt_tx_selfgen_ac_stats_tlv *htt_stats_buf = tag_buf;
	u8 *buf = stats_req->buf;
	u32 len = stats_req->buf_len;
	u32 buf_len = ATH12K_HTT_STATS_BUF_SIZE;

	if (tag_len < sizeof(*htt_stats_buf))
		return;

	len += scnprintf(buf + len, buf_len - len, "HTT_TX_SELFGEN_AC_STATS_TLV:\n");
	len += scnprintf(buf + len, buf_len - len, "ac_su_ndpa_tried = %u\n",
			 le32_to_cpu(htt_stats_buf->ac_su_ndpa));
	len += scnprintf(buf + len, buf_len - len, "ac_su_ndp_tried = %u\n",
			 le32_to_cpu(htt_stats_buf->ac_su_ndp));
	len += scnprintf(buf + len, buf_len - len, "ac_mu_mimo_ndpa_tried = %u\n",
			 le32_to_cpu(htt_stats_buf->ac_mu_mimo_ndpa));
	len += scnprintf(buf + len, buf_len - len, "ac_mu_mimo_ndp_tried = %u\n",
			 le32_to_cpu(htt_stats_buf->ac_mu_mimo_ndp));
	len += print_array_to_buf_index(buf, len, "ac_mu_mimo_brpollX_tried = ", 1,
					htt_stats_buf->ac_mu_mimo_brpoll,
					ATH12K_HTT_TX_NUM_AC_MUMIMO_USER_STATS, "\n\n");

	stats_req->buf_len = len;
}

static void
ath12k_htt_print_tx_selfgen_ax_stats_tlv(const void *tag_buf, u16 tag_len,
					 struct debug_htt_stats_req *stats_req)
{
	const struct ath12k_htt_tx_selfgen_ax_stats_tlv *htt_stats_buf = tag_buf;
	u8 *buf = stats_req->buf;
	u32 len = stats_req->buf_len;
	u32 buf_len = ATH12K_HTT_STATS_BUF_SIZE;

	if (tag_len < sizeof(*htt_stats_buf))
		return;

	len += scnprintf(buf + len, buf_len - len, "HTT_TX_SELFGEN_AX_STATS_TLV:\n");
	len += scnprintf(buf + len, buf_len - len, "ax_su_ndpa_tried = %u\n",
			 le32_to_cpu(htt_stats_buf->ax_su_ndpa));
	len += scnprintf(buf + len, buf_len - len, "ax_su_ndp_tried = %u\n",
			 le32_to_cpu(htt_stats_buf->ax_su_ndp));
	len += scnprintf(buf + len, buf_len - len, "ax_mu_mimo_ndpa_tried = %u\n",
			 le32_to_cpu(htt_stats_buf->ax_mu_mimo_ndpa));
	len += scnprintf(buf + len, buf_len - len, "ax_mu_mimo_ndp_tried = %u\n",
			 le32_to_cpu(htt_stats_buf->ax_mu_mimo_ndp));
	len += print_array_to_buf_index(buf, len, "ax_mu_mimo_brpollX_tried = ", 1,
					htt_stats_buf->ax_mu_mimo_brpoll,
					ATH12K_HTT_TX_NUM_AX_MUMIMO_USER_STATS, "\n");
	len += scnprintf(buf + len, buf_len - len, "ax_basic_trigger = %u\n",
			 le32_to_cpu(htt_stats_buf->ax_basic_trigger));
	len += scnprintf(buf + len, buf_len - len, "ax_ulmumimo_total_trigger = %u\n",
			 le32_to_cpu(htt_stats_buf->ax_ulmumimo_trigger));
	len += scnprintf(buf + len, buf_len - len, "ax_bsr_trigger = %u\n",
			 le32_to_cpu(htt_stats_buf->ax_bsr_trigger));
	len += scnprintf(buf + len, buf_len - len, "ax_mu_bar_trigger = %u\n",
			 le32_to_cpu(htt_stats_buf->ax_mu_bar_trigger));
	len += scnprintf(buf + len, buf_len - len, "ax_mu_rts_trigger = %u\n\n",
			 le32_to_cpu(htt_stats_buf->ax_mu_rts_trigger));

	stats_req->buf_len = len;
}

static void
ath12k_htt_print_tx_selfgen_be_stats_tlv(const void *tag_buf, u16 tag_len,
					 struct debug_htt_stats_req *stats_req)
{
	const struct ath12k_htt_tx_selfgen_be_stats_tlv *htt_stats_buf = tag_buf;
	u8 *buf = stats_req->buf;
	u32 len = stats_req->buf_len;
	u32 buf_len = ATH12K_HTT_STATS_BUF_SIZE;

	if (tag_len < sizeof(*htt_stats_buf))
		return;

	len += scnprintf(buf + len, buf_len - len, "HTT_TX_SELFGEN_BE_STATS_TLV:\n");
	len += scnprintf(buf + len, buf_len - len, "be_su_ndpa_queued = %u\n",
			 le32_to_cpu(htt_stats_buf->be_su_ndpa_queued));
	len += scnprintf(buf + len, buf_len - len, "be_su_ndpa_tried = %u\n",
			 le32_to_cpu(htt_stats_buf->be_su_ndpa));
	len += scnprintf(buf + len, buf_len - len, "be_su_ndp_queued = %u\n",
			 le32_to_cpu(htt_stats_buf->be_su_ndp_queued));
	len += scnprintf(buf + len, buf_len - len, "be_su_ndp_tried = %u\n",
			 le32_to_cpu(htt_stats_buf->be_su_ndp));
	len += scnprintf(buf + len, buf_len - len, "be_mu_mimo_ndpa_queued = %u\n",
			 le32_to_cpu(htt_stats_buf->be_mu_mimo_ndpa_queued));
	len += scnprintf(buf + len, buf_len - len, "be_mu_mimo_ndpa_tried = %u\n",
			 le32_to_cpu(htt_stats_buf->be_mu_mimo_ndpa));
	len += scnprintf(buf + len, buf_len - len, "be_mu_mimo_ndp_queued = %u\n",
			 le32_to_cpu(htt_stats_buf->be_mu_mimo_ndp_queued));
	len += scnprintf(buf + len, buf_len - len, "be_mu_mimo_ndp_tried = %u\n",
			 le32_to_cpu(htt_stats_buf->be_mu_mimo_ndp));
	len += print_array_to_buf_index(buf, len, "be_mu_mimo_brpollX_queued = ", 1,
					htt_stats_buf->be_mu_mimo_brpoll_queued,
					ATH12K_HTT_TX_NUM_BE_MUMIMO_USER_STATS - 1,
					"\n");
	len += print_array_to_buf_index(buf, len, "be_mu_mimo_brpollX_tried = ", 1,
					htt_stats_buf->be_mu_mimo_brpoll,
					ATH12K_HTT_TX_NUM_BE_MUMIMO_USER_STATS - 1,
					"\n");
	len += print_array_to_buf(buf, len, "be_ul_mumimo_trigger = ",
				  htt_stats_buf->be_ul_mumimo_trigger,
				  ATH12K_HTT_TX_NUM_BE_MUMIMO_USER_STATS, "\n");
	len += scnprintf(buf + len, buf_len - len, "be_basic_trigger = %u\n",
			 le32_to_cpu(htt_stats_buf->be_basic_trigger));
	len += scnprintf(buf + len, buf_len - len, "be_ulmumimo_total_trigger = %u\n",
			 le32_to_cpu(htt_stats_buf->be_ulmumimo_trigger));
	len += scnprintf(buf + len, buf_len - len, "be_bsr_trigger = %u\n",
			 le32_to_cpu(htt_stats_buf->be_bsr_trigger));
	len += scnprintf(buf + len, buf_len - len, "be_mu_bar_trigger = %u\n",
			 le32_to_cpu(htt_stats_buf->be_mu_bar_trigger));
	len += scnprintf(buf + len, buf_len - len, "be_mu_rts_trigger = %u\n\n",
			 le32_to_cpu(htt_stats_buf->be_mu_rts_trigger));

	stats_req->buf_len = len;
}

static void
ath12k_htt_print_tx_selfgen_ac_err_stats_tlv(const void *tag_buf, u16 tag_len,
					     struct debug_htt_stats_req *stats_req)
{
	const struct ath12k_htt_tx_selfgen_ac_err_stats_tlv *htt_stats_buf = tag_buf;
	u8 *buf = stats_req->buf;
	u32 len = stats_req->buf_len;
	u32 buf_len = ATH12K_HTT_STATS_BUF_SIZE;

	if (tag_len < sizeof(*htt_stats_buf))
		return;

	len += scnprintf(buf + len, buf_len - len, "HTT_TX_SELFGEN_AC_ERR_STATS_TLV:\n");
	len += scnprintf(buf + len, buf_len - len, "ac_su_ndp_err = %u\n",
			 le32_to_cpu(htt_stats_buf->ac_su_ndp_err));
	len += scnprintf(buf + len, buf_len - len, "ac_su_ndpa_err = %u\n",
			 le32_to_cpu(htt_stats_buf->ac_su_ndpa_err));
	len += scnprintf(buf + len, buf_len - len, "ac_mu_mimo_ndpa_err = %u\n",
			 le32_to_cpu(htt_stats_buf->ac_mu_mimo_ndpa_err));
	len += scnprintf(buf + len, buf_len - len, "ac_mu_mimo_ndp_err = %u\n",
			 le32_to_cpu(htt_stats_buf->ac_mu_mimo_ndp_err));
	len += scnprintf(buf + len, buf_len - len, "ac_mu_mimo_brp1_err = %u\n",
			 le32_to_cpu(htt_stats_buf->ac_mu_mimo_brp1_err));
	len += scnprintf(buf + len, buf_len - len, "ac_mu_mimo_brp2_err = %u\n",
			 le32_to_cpu(htt_stats_buf->ac_mu_mimo_brp2_err));
	len += scnprintf(buf + len, buf_len - len, "ac_mu_mimo_brp3_err = %u\n\n",
			 le32_to_cpu(htt_stats_buf->ac_mu_mimo_brp3_err));

	stats_req->buf_len = len;
}

static void
ath12k_htt_print_tx_selfgen_ax_err_stats_tlv(const void *tag_buf, u16 tag_len,
					     struct debug_htt_stats_req *stats_req)
{
	const struct ath12k_htt_tx_selfgen_ax_err_stats_tlv *htt_stats_buf = tag_buf;
	u8 *buf = stats_req->buf;
	u32 len = stats_req->buf_len;
	u32 buf_len = ATH12K_HTT_STATS_BUF_SIZE;

	if (tag_len < sizeof(*htt_stats_buf))
		return;

	len += scnprintf(buf + len, buf_len - len, "HTT_TX_SELFGEN_AX_ERR_STATS_TLV:\n");
	len += scnprintf(buf + len, buf_len - len, "ax_su_ndp_err = %u\n",
			 le32_to_cpu(htt_stats_buf->ax_su_ndp_err));
	len += scnprintf(buf + len, buf_len - len, "ax_su_ndpa_err = %u\n",
			 le32_to_cpu(htt_stats_buf->ax_su_ndpa_err));
	len += scnprintf(buf + len, buf_len - len, "ax_mu_mimo_ndpa_err = %u\n",
			 le32_to_cpu(htt_stats_buf->ax_mu_mimo_ndpa_err));
	len += scnprintf(buf + len, buf_len - len, "ax_mu_mimo_ndp_err = %u\n",
			 le32_to_cpu(htt_stats_buf->ax_mu_mimo_ndp_err));
	len += print_array_to_buf_index(buf, len, "ax_mu_mimo_brpX_err", 1,
					htt_stats_buf->ax_mu_mimo_brp_err,
					ATH12K_HTT_TX_NUM_AX_MUMIMO_USER_STATS - 1,
					"\n");
	len += scnprintf(buf + len, buf_len - len, "ax_basic_trigger_err = %u\n",
			 le32_to_cpu(htt_stats_buf->ax_basic_trigger_err));
	len += scnprintf(buf + len, buf_len - len, "ax_ulmumimo_total_trigger_err = %u\n",
			 le32_to_cpu(htt_stats_buf->ax_ulmumimo_trigger_err));
	len += scnprintf(buf + len, buf_len - len, "ax_bsr_trigger_err = %u\n",
			 le32_to_cpu(htt_stats_buf->ax_bsr_trigger_err));
	len += scnprintf(buf + len, buf_len - len, "ax_mu_bar_trigger_err = %u\n",
			 le32_to_cpu(htt_stats_buf->ax_mu_bar_trigger_err));
	len += scnprintf(buf + len, buf_len - len, "ax_mu_rts_trigger_err = %u\n\n",
			 le32_to_cpu(htt_stats_buf->ax_mu_rts_trigger_err));

	stats_req->buf_len = len;
}

static void
ath12k_htt_print_tx_selfgen_be_err_stats_tlv(const void *tag_buf, u16 tag_len,
					     struct debug_htt_stats_req *stats_req)
{
	const struct ath12k_htt_tx_selfgen_be_err_stats_tlv *htt_stats_buf = tag_buf;
	u8 *buf = stats_req->buf;
	u32 len = stats_req->buf_len;
	u32 buf_len = ATH12K_HTT_STATS_BUF_SIZE;

	if (tag_len < sizeof(*htt_stats_buf))
		return;

	len += scnprintf(buf + len, buf_len - len, "HTT_TX_SELFGEN_BE_ERR_STATS_TLV:\n");
	len += scnprintf(buf + len, buf_len - len, "be_su_ndp_err = %u\n",
			 le32_to_cpu(htt_stats_buf->be_su_ndp_err));
	len += scnprintf(buf + len, buf_len - len, "be_su_ndp_flushed = %u\n",
			 le32_to_cpu(htt_stats_buf->be_su_ndp_flushed));
	len += scnprintf(buf + len, buf_len - len, "be_su_ndpa_err = %u\n",
			 le32_to_cpu(htt_stats_buf->be_su_ndpa_err));
	len += scnprintf(buf + len, buf_len - len, "be_su_ndpa_flushed = %u\n",
			 le32_to_cpu(htt_stats_buf->be_su_ndpa_flushed));
	len += scnprintf(buf + len, buf_len - len, "be_mu_mimo_ndpa_err = %u\n",
			 le32_to_cpu(htt_stats_buf->be_mu_mimo_ndpa_err));
	len += scnprintf(buf + len, buf_len - len, "be_mu_mimo_ndpa_flushed = %u\n",
			 le32_to_cpu(htt_stats_buf->be_mu_mimo_ndpa_flushed));
	len += scnprintf(buf + len, buf_len - len, "be_mu_mimo_ndp_err = %u\n",
			 le32_to_cpu(htt_stats_buf->be_mu_mimo_ndp_err));
	len += scnprintf(buf + len, buf_len - len, "be_mu_mimo_ndp_flushed = %u\n",
			 le32_to_cpu(htt_stats_buf->be_mu_mimo_ndp_flushed));
	len += print_array_to_buf_index(buf, len, "be_mu_mimo_brpX_err", 1,
					htt_stats_buf->be_mu_mimo_brp_err,
					ATH12K_HTT_TX_NUM_BE_MUMIMO_USER_STATS - 1,
					"\n");
	len += print_array_to_buf_index(buf, len, "be_mu_mimo_brpollX_flushed", 1,
					htt_stats_buf->be_mu_mimo_brpoll_flushed,
					ATH12K_HTT_TX_NUM_BE_MUMIMO_USER_STATS - 1,
					"\n");
	len += print_array_to_buf(buf, len, "be_mu_mimo_num_cbf_rcvd_on_brp_err",
				  htt_stats_buf->be_mu_mimo_brp_err_num_cbf_rxd,
				  ATH12K_HTT_TX_NUM_BE_MUMIMO_USER_STATS, "\n");
	len += print_array_to_buf(buf, len, "be_ul_mumimo_trigger_err",
				  htt_stats_buf->be_ul_mumimo_trigger_err,
				  ATH12K_HTT_TX_NUM_BE_MUMIMO_USER_STATS, "\n");
	len += scnprintf(buf + len, buf_len - len, "be_basic_trigger_err = %u\n",
			 le32_to_cpu(htt_stats_buf->be_basic_trigger_err));
	len += scnprintf(buf + len, buf_len - len, "be_ulmumimo_total_trig_err = %u\n",
			 le32_to_cpu(htt_stats_buf->be_ulmumimo_trigger_err));
	len += scnprintf(buf + len, buf_len - len, "be_bsr_trigger_err = %u\n",
			 le32_to_cpu(htt_stats_buf->be_bsr_trigger_err));
	len += scnprintf(buf + len, buf_len - len, "be_mu_bar_trigger_err = %u\n",
			 le32_to_cpu(htt_stats_buf->be_mu_bar_trigger_err));
	len += scnprintf(buf + len, buf_len - len, "be_mu_rts_trigger_err = %u\n\n",
			 le32_to_cpu(htt_stats_buf->be_mu_rts_trigger_err));

	stats_req->buf_len = len;
}

static void
ath12k_htt_print_tx_selfgen_ac_sched_status_stats_tlv(const void *tag_buf, u16 tag_len,
						      struct debug_htt_stats_req *stats)
{
	const struct ath12k_htt_tx_selfgen_ac_sched_status_stats_tlv *htt_stats_buf =
		     tag_buf;
	u8 *buf = stats->buf;
	u32 len = stats->buf_len;
	u32 buf_len = ATH12K_HTT_STATS_BUF_SIZE;

	if (tag_len < sizeof(*htt_stats_buf))
		return;

	len += scnprintf(buf + len, buf_len - len,
			 "HTT_TX_SELFGEN_AC_SCHED_STATUS_STATS_TLV:\n");
	len += print_array_to_buf(buf, len, "ac_su_ndpa_sch_status",
				  htt_stats_buf->ac_su_ndpa_sch_status,
				  ATH12K_HTT_TX_PDEV_STATS_NUM_TX_ERR_STATUS, "\n");
	len += print_array_to_buf(buf, len, "ac_su_ndp_sch_status",
				  htt_stats_buf->ac_su_ndp_sch_status,
				  ATH12K_HTT_TX_PDEV_STATS_NUM_TX_ERR_STATUS, "\n");
	len += print_array_to_buf(buf, len, "ac_mu_mimo_ndpa_sch_status",
				  htt_stats_buf->ac_mu_mimo_ndpa_sch_status,
				  ATH12K_HTT_TX_PDEV_STATS_NUM_TX_ERR_STATUS, "\n");
	len += print_array_to_buf(buf, len, "ac_mu_mimo_ndp_sch_status",
				  htt_stats_buf->ac_mu_mimo_ndp_sch_status,
				  ATH12K_HTT_TX_PDEV_STATS_NUM_TX_ERR_STATUS, "\n");
	len += print_array_to_buf(buf, len, "ac_mu_mimo_brp_sch_status",
				  htt_stats_buf->ac_mu_mimo_brp_sch_status,
				  ATH12K_HTT_TX_PDEV_STATS_NUM_TX_ERR_STATUS, "\n");
	len += print_array_to_buf(buf, len, "ac_su_ndp_sch_flag_err",
				  htt_stats_buf->ac_su_ndp_sch_flag_err,
				  ATH12K_HTT_TX_SELFGEN_SCH_TSFLAG_ERR_STATS, "\n");
	len += print_array_to_buf(buf, len, "ac_mu_mimo_ndp_sch_flag_err",
				  htt_stats_buf->ac_mu_mimo_ndp_sch_flag_err,
				  ATH12K_HTT_TX_SELFGEN_SCH_TSFLAG_ERR_STATS, "\n");
	len += print_array_to_buf(buf, len, "ac_mu_mimo_brp_sch_flag_err",
				  htt_stats_buf->ac_mu_mimo_brp_sch_flag_err,
				  ATH12K_HTT_TX_SELFGEN_SCH_TSFLAG_ERR_STATS, "\n\n");

	stats->buf_len = len;
}

static void
ath12k_htt_print_tx_selfgen_ax_sched_status_stats_tlv(const void *tag_buf, u16 tag_len,
						      struct debug_htt_stats_req *stats)
{
	const struct ath12k_htt_tx_selfgen_ax_sched_status_stats_tlv *htt_stats_buf =
		     tag_buf;
	u8 *buf = stats->buf;
	u32 len = stats->buf_len;
	u32 buf_len = ATH12K_HTT_STATS_BUF_SIZE;

	if (tag_len < sizeof(*htt_stats_buf))
		return;

	len += scnprintf(buf + len, buf_len - len,
			 "HTT_TX_SELFGEN_AX_SCHED_STATUS_STATS_TLV:\n");
	len += print_array_to_buf(buf, len, "ax_su_ndpa_sch_status",
				  htt_stats_buf->ax_su_ndpa_sch_status,
				  ATH12K_HTT_TX_PDEV_STATS_NUM_TX_ERR_STATUS, "\n");
	len += print_array_to_buf(buf, len, "ax_su_ndp_sch_status",
				  htt_stats_buf->ax_su_ndp_sch_status,
				  ATH12K_HTT_TX_PDEV_STATS_NUM_TX_ERR_STATUS, "\n");
	len += print_array_to_buf(buf, len, "ax_mu_mimo_ndpa_sch_status",
				  htt_stats_buf->ax_mu_mimo_ndpa_sch_status,
				  ATH12K_HTT_TX_PDEV_STATS_NUM_TX_ERR_STATUS, "\n");
	len += print_array_to_buf(buf, len, "ax_mu_mimo_ndp_sch_status",
				  htt_stats_buf->ax_mu_mimo_ndp_sch_status,
				  ATH12K_HTT_TX_PDEV_STATS_NUM_TX_ERR_STATUS, "\n");
	len += print_array_to_buf(buf, len, "ax_mu_brp_sch_status",
				  htt_stats_buf->ax_mu_brp_sch_status,
				  ATH12K_HTT_TX_PDEV_STATS_NUM_TX_ERR_STATUS, "\n");
	len += print_array_to_buf(buf, len, "ax_mu_bar_sch_status",
				  htt_stats_buf->ax_mu_bar_sch_status,
				  ATH12K_HTT_TX_PDEV_STATS_NUM_TX_ERR_STATUS, "\n");
	len += print_array_to_buf(buf, len, "ax_basic_trig_sch_status",
				  htt_stats_buf->ax_basic_trig_sch_status,
				  ATH12K_HTT_TX_PDEV_STATS_NUM_TX_ERR_STATUS, "\n");
	len += print_array_to_buf(buf, len, "ax_su_ndp_sch_flag_err",
				  htt_stats_buf->ax_su_ndp_sch_flag_err,
				  ATH12K_HTT_TX_SELFGEN_SCH_TSFLAG_ERR_STATS, "\n");
	len += print_array_to_buf(buf, len, "ax_mu_mimo_ndp_sch_flag_err",
				  htt_stats_buf->ax_mu_mimo_ndp_sch_flag_err,
				  ATH12K_HTT_TX_SELFGEN_SCH_TSFLAG_ERR_STATS, "\n");
	len += print_array_to_buf(buf, len, "ax_mu_brp_sch_flag_err",
				  htt_stats_buf->ax_mu_brp_sch_flag_err,
				  ATH12K_HTT_TX_SELFGEN_SCH_TSFLAG_ERR_STATS, "\n");
	len += print_array_to_buf(buf, len, "ax_mu_bar_sch_flag_err",
				  htt_stats_buf->ax_mu_bar_sch_flag_err,
				  ATH12K_HTT_TX_SELFGEN_SCH_TSFLAG_ERR_STATS, "\n");
	len += print_array_to_buf(buf, len, "ax_basic_trig_sch_flag_err",
				  htt_stats_buf->ax_basic_trig_sch_flag_err,
				  ATH12K_HTT_TX_SELFGEN_SCH_TSFLAG_ERR_STATS, "\n");
	len += print_array_to_buf(buf, len, "ax_ulmumimo_trig_sch_status",
				  htt_stats_buf->ax_ulmumimo_trig_sch_status,
				  ATH12K_HTT_TX_PDEV_STATS_NUM_TX_ERR_STATUS, "\n");
	len += print_array_to_buf(buf, len, "ax_ulmumimo_trig_sch_flag_err",
				  htt_stats_buf->ax_ulmumimo_trig_sch_flag_err,
				  ATH12K_HTT_TX_SELFGEN_SCH_TSFLAG_ERR_STATS, "\n\n");

	stats->buf_len = len;
}

static void
ath12k_htt_print_tx_selfgen_be_sched_status_stats_tlv(const void *tag_buf, u16 tag_len,
						      struct debug_htt_stats_req *stats)
{
	const struct ath12k_htt_tx_selfgen_be_sched_status_stats_tlv *htt_stats_buf =
		     tag_buf;
	u8 *buf = stats->buf;
	u32 len = stats->buf_len;
	u32 buf_len = ATH12K_HTT_STATS_BUF_SIZE;

	if (tag_len < sizeof(*htt_stats_buf))
		return;

	len += scnprintf(buf + len, buf_len - len,
			 "HTT_TX_SELFGEN_BE_SCHED_STATUS_STATS_TLV:\n");
	len += print_array_to_buf(buf, len, "be_su_ndpa_sch_status",
				  htt_stats_buf->be_su_ndpa_sch_status,
				  ATH12K_HTT_TX_PDEV_STATS_NUM_TX_ERR_STATUS, "\n");
	len += print_array_to_buf(buf, len, "be_su_ndp_sch_status",
				  htt_stats_buf->be_su_ndp_sch_status,
				  ATH12K_HTT_TX_PDEV_STATS_NUM_TX_ERR_STATUS, "\n");
	len += print_array_to_buf(buf, len, "be_mu_mimo_ndpa_sch_status",
				  htt_stats_buf->be_mu_mimo_ndpa_sch_status,
				  ATH12K_HTT_TX_PDEV_STATS_NUM_TX_ERR_STATUS, "\n");
	len += print_array_to_buf(buf, len, "be_mu_mimo_ndp_sch_status",
				  htt_stats_buf->be_mu_mimo_ndp_sch_status,
				  ATH12K_HTT_TX_PDEV_STATS_NUM_TX_ERR_STATUS, "\n");
	len += print_array_to_buf(buf, len, "be_mu_brp_sch_status",
				  htt_stats_buf->be_mu_brp_sch_status,
				  ATH12K_HTT_TX_PDEV_STATS_NUM_TX_ERR_STATUS, "\n");
	len += print_array_to_buf(buf, len, "be_mu_bar_sch_status",
				  htt_stats_buf->be_mu_bar_sch_status,
				  ATH12K_HTT_TX_PDEV_STATS_NUM_TX_ERR_STATUS, "\n");
	len += print_array_to_buf(buf, len, "be_basic_trig_sch_status",
				  htt_stats_buf->be_basic_trig_sch_status,
				  ATH12K_HTT_TX_PDEV_STATS_NUM_TX_ERR_STATUS, "\n");
	len += print_array_to_buf(buf, len, "be_su_ndp_sch_flag_err",
				  htt_stats_buf->be_su_ndp_sch_flag_err,
				  ATH12K_HTT_TX_SELFGEN_SCH_TSFLAG_ERR_STATS, "\n");
	len += print_array_to_buf(buf, len, "be_mu_mimo_ndp_sch_flag_err",
				  htt_stats_buf->be_mu_mimo_ndp_sch_flag_err,
				  ATH12K_HTT_TX_SELFGEN_SCH_TSFLAG_ERR_STATS, "\n");
	len += print_array_to_buf(buf, len, "be_mu_brp_sch_flag_err",
				  htt_stats_buf->be_mu_brp_sch_flag_err,
				  ATH12K_HTT_TX_SELFGEN_SCH_TSFLAG_ERR_STATS, "\n");
	len += print_array_to_buf(buf, len, "be_mu_bar_sch_flag_err",
				  htt_stats_buf->be_mu_bar_sch_flag_err,
				  ATH12K_HTT_TX_SELFGEN_SCH_TSFLAG_ERR_STATS, "\n");
	len += print_array_to_buf(buf, len, "be_basic_trig_sch_flag_err",
				  htt_stats_buf->be_basic_trig_sch_flag_err,
				  ATH12K_HTT_TX_SELFGEN_SCH_TSFLAG_ERR_STATS, "\n");
	len += print_array_to_buf(buf, len, "be_basic_trig_sch_flag_err",
				  htt_stats_buf->be_basic_trig_sch_flag_err,
				  ATH12K_HTT_TX_PDEV_STATS_NUM_TX_ERR_STATUS, "\n");
	len += print_array_to_buf(buf, len, "be_ulmumimo_trig_sch_flag_err",
				  htt_stats_buf->be_ulmumimo_trig_sch_flag_err,
				  ATH12K_HTT_TX_SELFGEN_SCH_TSFLAG_ERR_STATS, "\n\n");

	stats->buf_len = len;
}

static void
ath12k_htt_print_stats_string_tlv(const void *tag_buf, u16 tag_len,
				  struct debug_htt_stats_req *stats_req)
{
	const struct ath12k_htt_stats_string_tlv *htt_stats_buf = tag_buf;
	u8 *buf = stats_req->buf;
	u32 len = stats_req->buf_len;
	u32 buf_len = ATH12K_HTT_STATS_BUF_SIZE;
	u8 i;
	u16 index = 0;
	u32 datum;
	char data[ATH12K_HTT_MAX_STRING_LEN] = {0};

	tag_len = tag_len >> 2;

	len += scnprintf(buf + len, buf_len - len, "HTT_STATS_STRING_TLV:\n");
	for (i = 0; i < tag_len; i++) {
		datum = __le32_to_cpu(htt_stats_buf->data[i]);
		index += scnprintf(&data[index], ATH12K_HTT_MAX_STRING_LEN - index,
				   "%.*s", 4, (char *)&datum);
		if (index >= ATH12K_HTT_MAX_STRING_LEN)
			break;
	}
	len += scnprintf(buf + len, buf_len - len, "data = %s\n\n", data);

	stats_req->buf_len = len;
}

static void
ath12k_htt_print_sring_stats_tlv(const void *tag_buf, u16 tag_len,
				 struct debug_htt_stats_req *stats_req)
{
	const struct ath12k_htt_sring_stats_tlv *htt_stats_buf = tag_buf;
	u8 *buf = stats_req->buf;
	u32 len = stats_req->buf_len;
	u32 buf_len = ATH12K_HTT_STATS_BUF_SIZE;
	u32 mac_id_word;
	u32 avail_words;
	u32 head_tail_ptr;
	u32 sring_stat;
	u32 tail_ptr;

	if (tag_len < sizeof(*htt_stats_buf))
		return;

	mac_id_word = __le32_to_cpu(htt_stats_buf->mac_id__ring_id__arena__ep);
	avail_words = __le32_to_cpu(htt_stats_buf->num_avail_words__num_valid_words);
	head_tail_ptr = __le32_to_cpu(htt_stats_buf->head_ptr__tail_ptr);
	sring_stat = __le32_to_cpu(htt_stats_buf->consumer_empty__producer_full);
	tail_ptr = __le32_to_cpu(htt_stats_buf->prefetch_count__internal_tail_ptr);

	len += scnprintf(buf + len, buf_len - len, "HTT_SRING_STATS_TLV:\n");
	len += scnprintf(buf + len, buf_len - len, "mac_id = %u\n",
			 u32_get_bits(mac_id_word, ATH12K_HTT_SRING_STATS_MAC_ID));
	len += scnprintf(buf + len, buf_len - len, "ring_id = %u\n",
			 u32_get_bits(mac_id_word, ATH12K_HTT_SRING_STATS_RING_ID));
	len += scnprintf(buf + len, buf_len - len, "arena = %u\n",
			 u32_get_bits(mac_id_word, ATH12K_HTT_SRING_STATS_ARENA));
	len += scnprintf(buf + len, buf_len - len, "ep = %u\n",
			 u32_get_bits(mac_id_word, ATH12K_HTT_SRING_STATS_EP));
	len += scnprintf(buf + len, buf_len - len, "base_addr_lsb = 0x%x\n",
			 le32_to_cpu(htt_stats_buf->base_addr_lsb));
	len += scnprintf(buf + len, buf_len - len, "base_addr_msb = 0x%x\n",
			 le32_to_cpu(htt_stats_buf->base_addr_msb));
	len += scnprintf(buf + len, buf_len - len, "ring_size = %u\n",
			 le32_to_cpu(htt_stats_buf->ring_size));
	len += scnprintf(buf + len, buf_len - len, "elem_size = %u\n",
			 le32_to_cpu(htt_stats_buf->elem_size));
	len += scnprintf(buf + len, buf_len - len, "num_avail_words = %u\n",
			 u32_get_bits(avail_words,
				      ATH12K_HTT_SRING_STATS_NUM_AVAIL_WORDS));
	len += scnprintf(buf + len, buf_len - len, "num_valid_words = %u\n",
			 u32_get_bits(avail_words,
				      ATH12K_HTT_SRING_STATS_NUM_VALID_WORDS));
	len += scnprintf(buf + len, buf_len - len, "head_ptr = %u\n",
			 u32_get_bits(head_tail_ptr, ATH12K_HTT_SRING_STATS_HEAD_PTR));
	len += scnprintf(buf + len, buf_len - len, "tail_ptr = %u\n",
			 u32_get_bits(head_tail_ptr, ATH12K_HTT_SRING_STATS_TAIL_PTR));
	len += scnprintf(buf + len, buf_len - len, "consumer_empty = %u\n",
			 u32_get_bits(sring_stat,
				      ATH12K_HTT_SRING_STATS_CONSUMER_EMPTY));
	len += scnprintf(buf + len, buf_len - len, "producer_full = %u\n",
			 u32_get_bits(head_tail_ptr,
				      ATH12K_HTT_SRING_STATS_PRODUCER_FULL));
	len += scnprintf(buf + len, buf_len - len, "prefetch_count = %u\n",
			 u32_get_bits(tail_ptr, ATH12K_HTT_SRING_STATS_PREFETCH_COUNT));
	len += scnprintf(buf + len, buf_len - len, "internal_tail_ptr = %u\n\n",
			 u32_get_bits(tail_ptr,
				      ATH12K_HTT_SRING_STATS_INTERNAL_TAIL_PTR));

	stats_req->buf_len = len;
}

static void
ath12k_htt_print_sfm_cmn_tlv(const void *tag_buf, u16 tag_len,
			     struct debug_htt_stats_req *stats_req)
{
	const struct ath12k_htt_sfm_cmn_tlv *htt_stats_buf = tag_buf;
	u8 *buf = stats_req->buf;
	u32 len = stats_req->buf_len;
	u32 buf_len = ATH12K_HTT_STATS_BUF_SIZE;
	u32 mac_id_word;

	if (tag_len < sizeof(*htt_stats_buf))
		return;

	mac_id_word = __le32_to_cpu(htt_stats_buf->mac_id__word);

	len += scnprintf(buf + len, buf_len - len, "HTT_SFM_CMN_TLV:\n");
	len += scnprintf(buf + len, buf_len - len, "mac_id = %u\n",
			 u32_get_bits(mac_id_word, ATH12K_HTT_STATS_MAC_ID));
	len += scnprintf(buf + len, buf_len - len, "buf_total = %u\n",
			 le32_to_cpu(htt_stats_buf->buf_total));
	len += scnprintf(buf + len, buf_len - len, "mem_empty = %u\n",
			 le32_to_cpu(htt_stats_buf->mem_empty));
	len += scnprintf(buf + len, buf_len - len, "deallocate_bufs = %u\n",
			 le32_to_cpu(htt_stats_buf->deallocate_bufs));
	len += scnprintf(buf + len, buf_len - len, "num_records = %u\n\n",
			 le32_to_cpu(htt_stats_buf->num_records));

	stats_req->buf_len = len;
}

static void
ath12k_htt_print_sfm_client_tlv(const void *tag_buf, u16 tag_len,
				struct debug_htt_stats_req *stats_req)
{
	const struct ath12k_htt_sfm_client_tlv *htt_stats_buf = tag_buf;
	u8 *buf = stats_req->buf;
	u32 len = stats_req->buf_len;
	u32 buf_len = ATH12K_HTT_STATS_BUF_SIZE;

	if (tag_len < sizeof(*htt_stats_buf))
		return;

	len += scnprintf(buf + len, buf_len - len, "HTT_SFM_CLIENT_TLV:\n");
	len += scnprintf(buf + len, buf_len - len, "client_id = %u\n",
			 le32_to_cpu(htt_stats_buf->client_id));
	len += scnprintf(buf + len, buf_len - len, "buf_min = %u\n",
			 le32_to_cpu(htt_stats_buf->buf_min));
	len += scnprintf(buf + len, buf_len - len, "buf_max = %u\n",
			 le32_to_cpu(htt_stats_buf->buf_max));
	len += scnprintf(buf + len, buf_len - len, "buf_busy = %u\n",
			 le32_to_cpu(htt_stats_buf->buf_busy));
	len += scnprintf(buf + len, buf_len - len, "buf_alloc = %u\n",
			 le32_to_cpu(htt_stats_buf->buf_alloc));
	len += scnprintf(buf + len, buf_len - len, "buf_avail = %u\n",
			 le32_to_cpu(htt_stats_buf->buf_avail));
	len += scnprintf(buf + len, buf_len - len, "num_users = %u\n\n",
			 le32_to_cpu(htt_stats_buf->num_users));

	stats_req->buf_len = len;
}

static void
ath12k_htt_print_sfm_client_user_tlv(const void *tag_buf, u16 tag_len,
				     struct debug_htt_stats_req *stats_req)
{
	const struct ath12k_htt_sfm_client_user_tlv *htt_stats_buf = tag_buf;
	u8 *buf = stats_req->buf;
	u32 len = stats_req->buf_len;
	u32 buf_len = ATH12K_HTT_STATS_BUF_SIZE;
	u16 num_elems = tag_len >> 2;

	len += scnprintf(buf + len, buf_len - len, "HTT_SFM_CLIENT_USER_TLV:\n");
	len += print_array_to_buf(buf, len, "dwords_used_by_user_n",
				  htt_stats_buf->dwords_used_by_user_n,
				  num_elems, "\n\n");

	stats_req->buf_len = len;
}

static void
ath12k_htt_print_tx_pdev_mu_mimo_sch_stats_tlv(const void *tag_buf, u16 tag_len,
					       struct debug_htt_stats_req *stats_req)
{
	const struct ath12k_htt_tx_pdev_mu_mimo_sch_stats_tlv *htt_stats_buf = tag_buf;
	u8 *buf = stats_req->buf;
	u32 len = stats_req->buf_len;
	u32 buf_len = ATH12K_HTT_STATS_BUF_SIZE;
	u8 i;

	if (tag_len < sizeof(*htt_stats_buf))
		return;

	len += scnprintf(buf + len, buf_len - len,
			 "HTT_TX_PDEV_MU_MIMO_SCH_STATS_TLV:\n");
	len += scnprintf(buf + len, buf_len - len, "mu_mimo_sch_posted = %u\n",
			 le32_to_cpu(htt_stats_buf->mu_mimo_sch_posted));
	len += scnprintf(buf + len, buf_len - len, "mu_mimo_sch_failed = %u\n",
			 le32_to_cpu(htt_stats_buf->mu_mimo_sch_failed));
	len += scnprintf(buf + len, buf_len - len, "mu_mimo_ppdu_posted = %u\n",
			 le32_to_cpu(htt_stats_buf->mu_mimo_ppdu_posted));
	len += scnprintf(buf + len, buf_len - len,
			 "\nac_mu_mimo_sch_posted_per_group_index %u (SU) = %u\n", 0,
			 le32_to_cpu(htt_stats_buf->ac_mu_mimo_per_grp_sz[0]));
	for (i = 1; i < ATH12K_HTT_TX_NUM_AC_MUMIMO_USER_STATS; i++) {
		len += scnprintf(buf + len, buf_len - len,
				 "ac_mu_mimo_sch_posted_per_group_index %u ", i);
		len += scnprintf(buf + len, buf_len - len,
				 "(TOTAL STREAMS = %u) = %u\n", i + 1,
				 le32_to_cpu(htt_stats_buf->ac_mu_mimo_per_grp_sz[i]));
	}

	for (i = 0; i < ATH12K_HTT_TX_NUM_AC_MUMIMO_USER_STATS; i++) {
		len += scnprintf(buf + len, buf_len - len,
				 "ac_mu_mimo_sch_posted_per_group_index %u ",
				 i + ATH12K_HTT_TX_NUM_AC_MUMIMO_USER_STATS);
		len += scnprintf(buf + len, buf_len - len,
				 "(TOTAL STREAMS = %u) = %u\n",
				 i + ATH12K_HTT_TX_NUM_AC_MUMIMO_USER_STATS + 1,
				 le32_to_cpu(htt_stats_buf->ac_mu_mimo_grp_sz_ext[i]));
	}

	len += scnprintf(buf + len, buf_len - len,
			 "\nax_mu_mimo_sch_posted_per_group_index %u (SU) = %u\n", 0,
			 le32_to_cpu(htt_stats_buf->ax_mu_mimo_per_grp_sz[0]));
	for (i = 1; i < ATH12K_HTT_TX_NUM_AX_MUMIMO_USER_STATS; i++) {
		len += scnprintf(buf + len, buf_len - len,
				 "ax_mu_mimo_sch_posted_per_group_index %u ", i);
		len += scnprintf(buf + len, buf_len - len,
				 "(TOTAL STREAMS = %u) = %u\n", i + 1,
				 le32_to_cpu(htt_stats_buf->ax_mu_mimo_per_grp_sz[i]));
	}

	len += scnprintf(buf + len, buf_len - len,
			"\nbe_mu_mimo_sch_posted_per_group_index %u (SU) = %u\n", 0,
			le32_to_cpu(htt_stats_buf->be_mu_mimo_per_grp_sz[0]));
	for (i = 1; i < ATH12K_HTT_TX_NUM_BE_MUMIMO_USER_STATS; i++) {
		len += scnprintf(buf + len, buf_len - len,
				 "be_mu_mimo_sch_posted_per_group_index %u ", i);
		len += scnprintf(buf + len, buf_len - len,
				 "(TOTAL STREAMS = %u) = %u\n", i + 1,
				 le32_to_cpu(htt_stats_buf->be_mu_mimo_per_grp_sz[i]));
	}

	len += scnprintf(buf + len, buf_len - len, "\n11ac MU_MIMO SCH STATS:\n");
	for (i = 0; i < ATH12K_HTT_TX_NUM_AC_MUMIMO_USER_STATS; i++) {
		len += scnprintf(buf + len, buf_len - len, "ac_mu_mimo_sch_nusers_");
		len += scnprintf(buf + len, buf_len - len, "%u = %u\n", i,
				 le32_to_cpu(htt_stats_buf->ac_mu_mimo_sch_nusers[i]));
	}

	len += scnprintf(buf + len, buf_len - len, "\n11ax MU_MIMO SCH STATS:\n");
	for (i = 0; i < ATH12K_HTT_TX_NUM_AX_MUMIMO_USER_STATS; i++) {
		len += scnprintf(buf + len, buf_len - len, "ax_mu_mimo_sch_nusers_");
		len += scnprintf(buf + len, buf_len - len, "%u = %u\n", i,
				 le32_to_cpu(htt_stats_buf->ax_mu_mimo_sch_nusers[i]));
	}

	len += scnprintf(buf + len, buf_len - len, "\n11be MU_MIMO SCH STATS:\n");
	for (i = 0; i < ATH12K_HTT_TX_NUM_BE_MUMIMO_USER_STATS; i++) {
		len += scnprintf(buf + len, buf_len - len, "be_mu_mimo_sch_nusers_");
		len += scnprintf(buf + len, buf_len - len, "%u = %u\n", i,
				 le32_to_cpu(htt_stats_buf->be_mu_mimo_sch_nusers[i]));
	}

	len += scnprintf(buf + len, buf_len - len, "\n11ax OFDMA SCH STATS:\n");
	for (i = 0; i < ATH12K_HTT_TX_NUM_OFDMA_USER_STATS; i++) {
		len += scnprintf(buf + len, buf_len - len,
				 "ax_ofdma_sch_nusers_%u = %u\n", i,
				 le32_to_cpu(htt_stats_buf->ax_ofdma_sch_nusers[i]));
		len += scnprintf(buf + len, buf_len - len,
				 "ax_ul_ofdma_basic_sch_nusers_%u = %u\n", i,
				 le32_to_cpu(htt_stats_buf->ax_ul_ofdma_nusers[i]));
		len += scnprintf(buf + len, buf_len - len,
				 "ax_ul_ofdma_bsr_sch_nusers_%u = %u\n", i,
				 le32_to_cpu(htt_stats_buf->ax_ul_ofdma_bsr_nusers[i]));
		len += scnprintf(buf + len, buf_len - len,
				 "ax_ul_ofdma_bar_sch_nusers_%u = %u\n", i,
				 le32_to_cpu(htt_stats_buf->ax_ul_ofdma_bar_nusers[i]));
		len += scnprintf(buf + len, buf_len - len,
				 "ax_ul_ofdma_brp_sch_nusers_%u = %u\n\n", i,
				 le32_to_cpu(htt_stats_buf->ax_ul_ofdma_brp_nusers[i]));
	}

	len += scnprintf(buf + len, buf_len - len, "11ax UL MUMIMO SCH STATS:\n");
	for (i = 0; i < ATH12K_HTT_TX_NUM_UL_MUMIMO_USER_STATS; i++) {
		len += scnprintf(buf + len, buf_len - len,
				 "ax_ul_mumimo_basic_sch_nusers_%u = %u\n", i,
				 le32_to_cpu(htt_stats_buf->ax_ul_mumimo_nusers[i]));
		len += scnprintf(buf + len, buf_len - len,
				 "ax_ul_mumimo_brp_sch_nusers_%u = %u\n\n", i,
				 le32_to_cpu(htt_stats_buf->ax_ul_mumimo_brp_nusers[i]));
	}

	stats_req->buf_len = len;
}

static void
ath12k_htt_print_tx_pdev_mumimo_grp_stats_tlv(const void *tag_buf, u16 tag_len,
					      struct debug_htt_stats_req *stats_req)
{
	const struct ath12k_htt_tx_pdev_mumimo_grp_stats_tlv *htt_stats_buf = tag_buf;
	u8 *buf = stats_req->buf;
	u32 len = stats_req->buf_len;
	u32 buf_len = ATH12K_HTT_STATS_BUF_SIZE;
	int j;

	if (tag_len < sizeof(*htt_stats_buf))
		return;

	len += scnprintf(buf + len, buf_len - len, "HTT_TX_PDEV_MUMIMO_GRP_STATS:\n");
	len += print_array_to_buf(buf, len,
				  "dl_mumimo_grp_tputs_observed (per bin = 300 mbps)",
				  htt_stats_buf->dl_mumimo_grp_tputs,
				  ATH12K_HTT_STATS_MUMIMO_TPUT_NUM_BINS, "\n");
	len += print_array_to_buf(buf, len, "dl_mumimo_grp eligible",
				  htt_stats_buf->dl_mumimo_grp_eligible,
				  ATH12K_HTT_STATS_NUM_MAX_MUMIMO_SZ, "\n");
	len += print_array_to_buf(buf, len, "dl_mumimo_grp_ineligible",
				  htt_stats_buf->dl_mumimo_grp_ineligible,
				  ATH12K_HTT_STATS_NUM_MAX_MUMIMO_SZ, "\n");
	len += scnprintf(buf + len, buf_len - len, "dl_mumimo_grp_invalid:\n");
	for (j = 0; j < ATH12K_HTT_STATS_NUM_MAX_MUMIMO_SZ; j++) {
		len += scnprintf(buf + len, buf_len - len, "grp_id = %u", j);
		len += print_array_to_buf(buf, len, "",
					  htt_stats_buf->dl_mumimo_grp_invalid,
					  ATH12K_HTT_STATS_MAX_INVALID_REASON_CODE,
					  "\n");
	}

	len += print_array_to_buf(buf, len, "ul_mumimo_grp_best_grp_size",
				  htt_stats_buf->ul_mumimo_grp_best_grp_size,
				  ATH12K_HTT_STATS_NUM_MAX_MUMIMO_SZ, "\n");
	len += print_array_to_buf_index(buf, len, "ul_mumimo_grp_best_num_usrs = ", 1,
					htt_stats_buf->ul_mumimo_grp_best_usrs,
					ATH12K_HTT_TX_NUM_AX_MUMIMO_USER_STATS, "\n");
	len += print_array_to_buf(buf, len,
				  "ul_mumimo_grp_tputs_observed (per bin = 300 mbps)",
				  htt_stats_buf->ul_mumimo_grp_tputs,
				  ATH12K_HTT_STATS_MUMIMO_TPUT_NUM_BINS, "\n\n");

	stats_req->buf_len = len;
}

static void
ath12k_htt_print_tx_pdev_mu_mimo_mpdu_stats_tlv(const void *tag_buf, u16 tag_len,
						struct debug_htt_stats_req *stats_req)
{
	const struct ath12k_htt_tx_pdev_mpdu_stats_tlv *htt_stats_buf = tag_buf;
	u8 *buf = stats_req->buf;
	u32 len = stats_req->buf_len;
	u32 buf_len = ATH12K_HTT_STATS_BUF_SIZE;
	u32 user_index;
	u32 tx_sched_mode;

	if (tag_len < sizeof(*htt_stats_buf))
		return;

	user_index = __le32_to_cpu(htt_stats_buf->user_index);
	tx_sched_mode = __le32_to_cpu(htt_stats_buf->tx_sched_mode);

	if (tx_sched_mode == ATH12K_HTT_STATS_TX_SCHED_MODE_MU_MIMO_AC) {
		if (!user_index)
			len += scnprintf(buf + len, buf_len - len,
					 "HTT_TX_PDEV_MU_MIMO_AC_MPDU_STATS:\n");

		if (user_index < ATH12K_HTT_TX_NUM_AC_MUMIMO_USER_STATS) {
			len += scnprintf(buf + len, buf_len - len,
					 "ac_mu_mimo_mpdus_queued_usr_%u = %u\n",
					 user_index,
					 le32_to_cpu(htt_stats_buf->mpdus_queued_usr));
			len += scnprintf(buf + len, buf_len - len,
					 "ac_mu_mimo_mpdus_tried_usr_%u = %u\n",
					 user_index,
					 le32_to_cpu(htt_stats_buf->mpdus_tried_usr));
			len += scnprintf(buf + len, buf_len - len,
					 "ac_mu_mimo_mpdus_failed_usr_%u = %u\n",
					 user_index,
					 le32_to_cpu(htt_stats_buf->mpdus_failed_usr));
			len += scnprintf(buf + len, buf_len - len,
					 "ac_mu_mimo_mpdus_requeued_usr_%u = %u\n",
					 user_index,
					 le32_to_cpu(htt_stats_buf->mpdus_requeued_usr));
			len += scnprintf(buf + len, buf_len - len,
					 "ac_mu_mimo_err_no_ba_usr_%u = %u\n",
					 user_index,
					 le32_to_cpu(htt_stats_buf->err_no_ba_usr));
			len += scnprintf(buf + len, buf_len - len,
					 "ac_mu_mimo_mpdu_underrun_usr_%u = %u\n",
					 user_index,
					 le32_to_cpu(htt_stats_buf->mpdu_underrun_usr));
			len += scnprintf(buf + len, buf_len - len,
					"ac_mu_mimo_ampdu_underrun_usr_%u = %u\n\n",
					 user_index,
					 le32_to_cpu(htt_stats_buf->ampdu_underrun_usr));
		}
	}

	if (tx_sched_mode == ATH12K_HTT_STATS_TX_SCHED_MODE_MU_MIMO_AX) {
		if (!user_index)
			len += scnprintf(buf + len, buf_len - len,
					 "HTT_TX_PDEV_MU_MIMO_AX_MPDU_STATS:\n");

		if (user_index < ATH12K_HTT_TX_NUM_AX_MUMIMO_USER_STATS) {
			len += scnprintf(buf + len, buf_len - len,
					 "ax_mu_mimo_mpdus_queued_usr_%u = %u\n",
					 user_index,
					 le32_to_cpu(htt_stats_buf->mpdus_queued_usr));
			len += scnprintf(buf + len, buf_len - len,
					 "ax_mu_mimo_mpdus_tried_usr_%u = %u\n",
					 user_index,
					 le32_to_cpu(htt_stats_buf->mpdus_tried_usr));
			len += scnprintf(buf + len, buf_len - len,
					 "ax_mu_mimo_mpdus_failed_usr_%u = %u\n",
					 user_index,
					 le32_to_cpu(htt_stats_buf->mpdus_failed_usr));
			len += scnprintf(buf + len, buf_len - len,
					 "ax_mu_mimo_mpdus_requeued_usr_%u = %u\n",
					 user_index,
					 le32_to_cpu(htt_stats_buf->mpdus_requeued_usr));
			len += scnprintf(buf + len, buf_len - len,
					 "ax_mu_mimo_err_no_ba_usr_%u = %u\n",
					 user_index,
					 le32_to_cpu(htt_stats_buf->err_no_ba_usr));
			len += scnprintf(buf + len, buf_len - len,
					 "ax_mu_mimo_mpdu_underrun_usr_%u = %u\n",
					 user_index,
					 le32_to_cpu(htt_stats_buf->mpdu_underrun_usr));
			len += scnprintf(buf + len, buf_len - len,
					 "ax_mu_mimo_ampdu_underrun_usr_%u = %u\n\n",
					 user_index,
					 le32_to_cpu(htt_stats_buf->ampdu_underrun_usr));
		}
	}

	if (tx_sched_mode == ATH12K_HTT_STATS_TX_SCHED_MODE_MU_OFDMA_AX) {
		if (!user_index)
			len += scnprintf(buf + len, buf_len - len,
					 "HTT_TX_PDEV_AX_MU_OFDMA_MPDU_STATS:\n");

		if (user_index < ATH12K_HTT_TX_NUM_OFDMA_USER_STATS) {
			len += scnprintf(buf + len, buf_len - len,
					 "ax_mu_ofdma_mpdus_queued_usr_%u = %u\n",
					 user_index,
					 le32_to_cpu(htt_stats_buf->mpdus_queued_usr));
			len += scnprintf(buf + len, buf_len - len,
					 "ax_mu_ofdma_mpdus_tried_usr_%u = %u\n",
					 user_index,
					 le32_to_cpu(htt_stats_buf->mpdus_tried_usr));
			len += scnprintf(buf + len, buf_len - len,
					 "ax_mu_ofdma_mpdus_failed_usr_%u = %u\n",
					 user_index,
					 le32_to_cpu(htt_stats_buf->mpdus_failed_usr));
			len += scnprintf(buf + len, buf_len - len,
					 "ax_mu_ofdma_mpdus_requeued_usr_%u = %u\n",
					 user_index,
					 le32_to_cpu(htt_stats_buf->mpdus_requeued_usr));
			len += scnprintf(buf + len, buf_len - len,
					 "ax_mu_ofdma_err_no_ba_usr_%u = %u\n",
					 user_index,
					 le32_to_cpu(htt_stats_buf->err_no_ba_usr));
			len += scnprintf(buf + len, buf_len - len,
					 "ax_mu_ofdma_mpdu_underrun_usr_%u = %u\n",
					 user_index,
					 le32_to_cpu(htt_stats_buf->mpdu_underrun_usr));
			len += scnprintf(buf + len, buf_len - len,
					 "ax_mu_ofdma_ampdu_underrun_usr_%u = %u\n\n",
					 user_index,
					 le32_to_cpu(htt_stats_buf->ampdu_underrun_usr));
		}
	}

	stats_req->buf_len = len;
}

static void
ath12k_htt_print_pdev_cca_stats_hist_tlv(const void *tag_buf, u16 tag_len,
					 struct debug_htt_stats_req *stats_req)
{
	const struct ath12k_htt_pdev_cca_stats_hist_v1_tlv *htt_stats_buf = tag_buf;
	u8 *buf = stats_req->buf;
	u32 len = stats_req->buf_len;
	u32 buf_len = ATH12K_HTT_STATS_BUF_SIZE;

	if (tag_len < sizeof(*htt_stats_buf))
		return;

	len += scnprintf(buf + len, buf_len - len, "HTT_PDEV_CCA_STATS_HIST_TLV :\n");
	len += scnprintf(buf + len, buf_len - len, "chan_num = %u\n",
			 le32_to_cpu(htt_stats_buf->chan_num));
	len += scnprintf(buf + len, buf_len - len, "num_records = %u\n",
			 le32_to_cpu(htt_stats_buf->num_records));
	len += scnprintf(buf + len, buf_len - len, "valid_cca_counters_bitmap = 0x%x\n",
			 le32_to_cpu(htt_stats_buf->valid_cca_counters_bitmap));
	len += scnprintf(buf + len, buf_len - len, "collection_interval = %u\n\n",
			 le32_to_cpu(htt_stats_buf->collection_interval));

	stats_req->buf_len = len;
}

static void
ath12k_htt_print_pdev_stats_cca_counters_tlv(const void *tag_buf, u16 tag_len,
					     struct debug_htt_stats_req *stats_req)
{
	const struct ath12k_htt_pdev_stats_cca_counters_tlv *htt_stats_buf = tag_buf;
	u8 *buf = stats_req->buf;
	u32 len = stats_req->buf_len;
	u32 buf_len = ATH12K_HTT_STATS_BUF_SIZE;

	if (tag_len < sizeof(*htt_stats_buf))
		return;

	len += scnprintf(buf + len, buf_len - len,
			 "HTT_PDEV_STATS_CCA_COUNTERS_TLV:(in usec)\n");
	len += scnprintf(buf + len, buf_len - len, "tx_frame_usec = %u\n",
			 le32_to_cpu(htt_stats_buf->tx_frame_usec));
	len += scnprintf(buf + len, buf_len - len, "rx_frame_usec = %u\n",
			 le32_to_cpu(htt_stats_buf->rx_frame_usec));
	len += scnprintf(buf + len, buf_len - len, "rx_clear_usec = %u\n",
			 le32_to_cpu(htt_stats_buf->rx_clear_usec));
	len += scnprintf(buf + len, buf_len - len, "my_rx_frame_usec = %u\n",
			 le32_to_cpu(htt_stats_buf->my_rx_frame_usec));
	len += scnprintf(buf + len, buf_len - len, "usec_cnt = %u\n",
			 le32_to_cpu(htt_stats_buf->usec_cnt));
	len += scnprintf(buf + len, buf_len - len, "med_rx_idle_usec = %u\n",
			 le32_to_cpu(htt_stats_buf->med_rx_idle_usec));
	len += scnprintf(buf + len, buf_len - len, "med_tx_idle_global_usec = %u\n",
			 le32_to_cpu(htt_stats_buf->med_tx_idle_global_usec));
	len += scnprintf(buf + len, buf_len - len, "cca_obss_usec = %u\n\n",
			 le32_to_cpu(htt_stats_buf->cca_obss_usec));

	stats_req->buf_len = len;
}

static void
ath12k_htt_print_pdev_obss_pd_stats_tlv(const void *tag_buf, u16 tag_len,
					struct debug_htt_stats_req *stats_req)
{
	const struct ath12k_htt_pdev_obss_pd_stats_tlv *htt_stats_buf = tag_buf;
	u8 *buf = stats_req->buf;
	u32 len = stats_req->buf_len;
	u32 buf_len = ATH12K_HTT_STATS_BUF_SIZE;
	u8 i;
	static const char *access_cat_names[ATH12K_HTT_NUM_AC_WMM] = {"best effort",
								      "background",
								      "video", "voice"};

	if (tag_len < sizeof(*htt_stats_buf))
		return;

	len += scnprintf(buf + len, buf_len - len, "HTT_PDEV_OBSS_PD_STATS_TLV:\n");
	len += scnprintf(buf + len, buf_len - len, "num_spatial_reuse_tx = %u\n",
			 le32_to_cpu(htt_stats_buf->num_sr_tx_transmissions));
	len += scnprintf(buf + len, buf_len - len,
			 "num_spatial_reuse_opportunities = %u\n",
			 le32_to_cpu(htt_stats_buf->num_spatial_reuse_opportunities));
	len += scnprintf(buf + len, buf_len - len, "num_non_srg_opportunities = %u\n",
			 le32_to_cpu(htt_stats_buf->num_non_srg_opportunities));
	len += scnprintf(buf + len, buf_len - len, "num_non_srg_ppdu_tried = %u\n",
			 le32_to_cpu(htt_stats_buf->num_non_srg_ppdu_tried));
	len += scnprintf(buf + len, buf_len - len, "num_non_srg_ppdu_success = %u\n",
			 le32_to_cpu(htt_stats_buf->num_non_srg_ppdu_success));
	len += scnprintf(buf + len, buf_len - len, "num_srg_opportunities = %u\n",
			 le32_to_cpu(htt_stats_buf->num_srg_opportunities));
	len += scnprintf(buf + len, buf_len - len, "num_srg_ppdu_tried = %u\n",
			 le32_to_cpu(htt_stats_buf->num_srg_ppdu_tried));
	len += scnprintf(buf + len, buf_len - len, "num_srg_ppdu_success = %u\n",
			 le32_to_cpu(htt_stats_buf->num_srg_ppdu_success));
	len += scnprintf(buf + len, buf_len - len, "num_psr_opportunities = %u\n",
			 le32_to_cpu(htt_stats_buf->num_psr_opportunities));
	len += scnprintf(buf + len, buf_len - len, "num_psr_ppdu_tried = %u\n",
			 le32_to_cpu(htt_stats_buf->num_psr_ppdu_tried));
	len += scnprintf(buf + len, buf_len - len, "num_psr_ppdu_success = %u\n",
			 le32_to_cpu(htt_stats_buf->num_psr_ppdu_success));
	len += scnprintf(buf + len, buf_len - len, "min_duration_check_flush_cnt = %u\n",
			 le32_to_cpu(htt_stats_buf->num_obss_min_dur_check_flush_cnt));
	len += scnprintf(buf + len, buf_len - len, "sr_ppdu_abort_flush_cnt = %u\n\n",
			 le32_to_cpu(htt_stats_buf->num_sr_ppdu_abort_flush_cnt));

	len += scnprintf(buf + len, buf_len - len, "HTT_PDEV_OBSS_PD_PER_AC_STATS:\n");
	for (i = 0; i < ATH12K_HTT_NUM_AC_WMM; i++) {
		len += scnprintf(buf + len, buf_len - len, "Access Category %u (%s)\n",
				 i, access_cat_names[i]);
		len += scnprintf(buf + len, buf_len - len,
				 "num_non_srg_ppdu_tried = %u\n",
				 le32_to_cpu(htt_stats_buf->num_non_srg_tried_per_ac[i]));
		len += scnprintf(buf + len, buf_len - len,
				 "num_non_srg_ppdu_success = %u\n",
				 le32_to_cpu(htt_stats_buf->num_non_srg_success_ac[i]));
		len += scnprintf(buf + len, buf_len - len, "num_srg_ppdu_tried = %u\n",
				 le32_to_cpu(htt_stats_buf->num_srg_tried_per_ac[i]));
		len += scnprintf(buf + len, buf_len - len,
				 "num_srg_ppdu_success = %u\n\n",
				 le32_to_cpu(htt_stats_buf->num_srg_success_per_ac[i]));
	}

	stats_req->buf_len = len;
}

static void
ath12k_htt_print_dmac_reset_stats_tlv(const void *tag_buf, u16 tag_len,
				      struct debug_htt_stats_req *stats_req)
{
	const struct ath12k_htt_dmac_reset_stats_tlv *htt_stats_buf = tag_buf;
	u8 *buf = stats_req->buf;
	u32 len = stats_req->buf_len;
	u32 buf_len = ATH12K_HTT_STATS_BUF_SIZE;
	u64 time;

	if (tag_len < sizeof(*htt_stats_buf))
		return;

	len += scnprintf(buf + len, buf_len - len, "HTT_DMAC_RESET_STATS_TLV:\n");
	len += scnprintf(buf + len, buf_len - len, "reset_count = %u\n",
			 le32_to_cpu(htt_stats_buf->reset_count));
	time = ath12k_le32hilo_to_u64(htt_stats_buf->reset_time_hi_ms,
				      htt_stats_buf->reset_time_lo_ms);
	len += scnprintf(buf + len, buf_len - len, "reset_time_ms = %llu\n", time);

	time = ath12k_le32hilo_to_u64(htt_stats_buf->disengage_time_hi_ms,
				      htt_stats_buf->disengage_time_lo_ms);
	len += scnprintf(buf + len, buf_len - len, "disengage_time_ms = %llu\n", time);

	time = ath12k_le32hilo_to_u64(htt_stats_buf->engage_time_hi_ms,
				      htt_stats_buf->engage_time_lo_ms);
	len += scnprintf(buf + len, buf_len - len, "engage_time_ms = %llu\n", time);

	len += scnprintf(buf + len, buf_len - len, "disengage_count = %u\n",
			 le32_to_cpu(htt_stats_buf->disengage_count));
	len += scnprintf(buf + len, buf_len - len, "engage_count = %u\n",
			 le32_to_cpu(htt_stats_buf->engage_count));
	len += scnprintf(buf + len, buf_len - len, "drain_dest_ring_mask = 0x%x\n\n",
			 le32_to_cpu(htt_stats_buf->drain_dest_ring_mask));

	stats_req->buf_len = len;
}

static void
ath12k_htt_print_pdev_sched_algo_ofdma_stats_tlv(const void *tag_buf, u16 tag_len,
						 struct debug_htt_stats_req *stats_req)
{
	const struct ath12k_htt_pdev_sched_algo_ofdma_stats_tlv *htt_stats_buf = tag_buf;
	u8 *buf = stats_req->buf;
	u32 len = stats_req->buf_len;
	u32 buf_len = ATH12K_HTT_STATS_BUF_SIZE;
	u32 mac_id_word;

	if (tag_len < sizeof(*htt_stats_buf))
		return;

	mac_id_word = le32_to_cpu(htt_stats_buf->mac_id__word);

	len += scnprintf(buf + len, buf_len - len, "HTT_PDEV_SCHED_ALGO_TLV:\n");
	len += scnprintf(buf + len, buf_len - len, "mac_id = %u\n",
			 u32_get_bits(mac_id_word, ATH12K_HTT_STATS_MAC_ID));
	len += print_array_to_buf(buf, len, "rate_based_dlofdma_enabled_count",
				  htt_stats_buf->rate_based_dlofdma_enabled_cnt,
				  ATH12K_HTT_NUM_AC_WMM, "\n");
	len += print_array_to_buf(buf, len, "rate_based_dlofdma_disabled_count",
				  htt_stats_buf->rate_based_dlofdma_disabled_cnt,
				  ATH12K_HTT_NUM_AC_WMM, "\n");
	len += print_array_to_buf(buf, len, "rate_based_dlofdma_probing_count",
				  htt_stats_buf->rate_based_dlofdma_disabled_cnt,
				  ATH12K_HTT_NUM_AC_WMM, "\n");
	len += print_array_to_buf(buf, len, "rate_based_dlofdma_monitoring_count",
				  htt_stats_buf->rate_based_dlofdma_monitor_cnt,
				  ATH12K_HTT_NUM_AC_WMM, "\n");
	len += print_array_to_buf(buf, len, "chan_acc_lat_based_dlofdma_enabled_count",
				  htt_stats_buf->chan_acc_lat_based_dlofdma_enabled_cnt,
				  ATH12K_HTT_NUM_AC_WMM, "\n");
	len += print_array_to_buf(buf, len, "chan_acc_lat_based_dlofdma_disabled_count",
				  htt_stats_buf->chan_acc_lat_based_dlofdma_disabled_cnt,
				  ATH12K_HTT_NUM_AC_WMM, "\n");
	len += print_array_to_buf(buf, len, "chan_acc_lat_based_dlofdma_monitoring_count",
				  htt_stats_buf->chan_acc_lat_based_dlofdma_monitor_cnt,
				  ATH12K_HTT_NUM_AC_WMM, "\n");
	len += print_array_to_buf(buf, len, "downgrade_to_dl_su_ru_alloc_fail",
				  htt_stats_buf->downgrade_to_dl_su_ru_alloc_fail,
				  ATH12K_HTT_NUM_AC_WMM, "\n");
	len += print_array_to_buf(buf, len, "candidate_list_single_user_disable_ofdma",
				  htt_stats_buf->candidate_list_single_user_disable_ofdma,
				  ATH12K_HTT_NUM_AC_WMM, "\n");
	len += print_array_to_buf(buf, len, "dl_cand_list_dropped_high_ul_qos_weight",
				  htt_stats_buf->dl_cand_list_dropped_high_ul_qos_weight,
				  ATH12K_HTT_NUM_AC_WMM, "\n");
	len += print_array_to_buf(buf, len, "ax_dlofdma_disabled_due_to_pipelining",
				  htt_stats_buf->ax_dlofdma_disabled_due_to_pipelining,
				  ATH12K_HTT_NUM_AC_WMM, "\n");
	len += print_array_to_buf(buf, len, "dlofdma_disabled_su_only_eligible",
				  htt_stats_buf->dlofdma_disabled_su_only_eligible,
				  ATH12K_HTT_NUM_AC_WMM, "\n");
	len += print_array_to_buf(buf, len, "dlofdma_disabled_consec_no_mpdus_tried",
				  htt_stats_buf->dlofdma_disabled_consec_no_mpdus_tried,
				  ATH12K_HTT_NUM_AC_WMM, "\n");
	len += print_array_to_buf(buf, len, "dlofdma_disabled_consec_no_mpdus_success",
				  htt_stats_buf->dlofdma_disabled_consec_no_mpdus_success,
				  ATH12K_HTT_NUM_AC_WMM, "\n\n");

	stats_req->buf_len = len;
}

static void
ath12k_htt_print_tx_pdev_rate_stats_be_ofdma_tlv(const void *tag_buf, u16 tag_len,
						 struct debug_htt_stats_req *stats_req)
{
	const struct ath12k_htt_tx_pdev_rate_stats_be_ofdma_tlv *htt_stats_buf = tag_buf;
	u8 *buf = stats_req->buf;
	u32 len = stats_req->buf_len;
	u32 buf_len = ATH12K_HTT_STATS_BUF_SIZE;
	u32 mac_id_word;
	u8 i;

	if (tag_len < sizeof(*htt_stats_buf))
		return;

	mac_id_word = le32_to_cpu(htt_stats_buf->mac_id__word);

	len += scnprintf(buf + len, buf_len - len,
			 "HTT_TX_PDEV_RATE_STATS_BE_OFDMA_TLV:\n");
	len += scnprintf(buf + len, buf_len - len, "mac_id = %u\n",
			 u32_get_bits(mac_id_word, ATH12K_HTT_STATS_MAC_ID));
	len += scnprintf(buf + len, buf_len - len, "be_ofdma_tx_ldpc = %u\n",
			 le32_to_cpu(htt_stats_buf->be_ofdma_tx_ldpc));
	len += print_array_to_buf(buf, len, "be_ofdma_tx_mcs",
				  htt_stats_buf->be_ofdma_tx_mcs,
				  ATH12K_HTT_TX_PDEV_NUM_BE_MCS_CNTRS, "\n");
	len += print_array_to_buf(buf, len, "be_ofdma_eht_sig_mcs",
				  htt_stats_buf->be_ofdma_eht_sig_mcs,
				  ATH12K_HTT_TX_PDEV_NUM_EHT_SIG_MCS_CNTRS, "\n");
	len += scnprintf(buf + len, buf_len - len, "be_ofdma_tx_ru_size = ");
	for (i = 0; i < ATH12K_HTT_TX_RX_PDEV_NUM_BE_RU_SIZE_CNTRS; i++)
		len += scnprintf(buf + len, buf_len - len, " %s:%u ",
				 ath12k_htt_be_tx_rx_ru_size_to_str(i),
				 le32_to_cpu(htt_stats_buf->be_ofdma_tx_ru_size[i]));
	len += scnprintf(buf + len, buf_len - len, "\n");
	len += print_array_to_buf_index(buf, len, "be_ofdma_tx_nss = ", 1,
					htt_stats_buf->be_ofdma_tx_nss,
					ATH12K_HTT_TX_PDEV_STATS_NUM_SPATIAL_STREAMS,
					"\n");
	len += print_array_to_buf(buf, len, "be_ofdma_tx_bw",
				  htt_stats_buf->be_ofdma_tx_bw,
				  ATH12K_HTT_TX_PDEV_NUM_BE_BW_CNTRS, "\n");
	for (i = 0; i < ATH12K_HTT_TX_PDEV_NUM_GI_CNTRS; i++) {
		len += scnprintf(buf + len, buf_len - len,
				 "be_ofdma_tx_gi[%u]", i);
		len += print_array_to_buf(buf, len, "", htt_stats_buf->gi[i],
					  ATH12K_HTT_TX_PDEV_NUM_BE_MCS_CNTRS, "\n");
	}
	len += scnprintf(buf + len, buf_len - len, "\n");

	stats_req->buf_len = len;
}

static int ath12k_dbg_htt_ext_stats_parse(struct ath12k_base *ab,
					  u16 tag, u16 len, const void *tag_buf,
					  void *user_data)
{
	struct debug_htt_stats_req *stats_req = user_data;

	switch (tag) {
	case HTT_STATS_TX_PDEV_CMN_TAG:
		htt_print_tx_pdev_stats_cmn_tlv(tag_buf, len, stats_req);
		break;
	case HTT_STATS_TX_PDEV_UNDERRUN_TAG:
		htt_print_tx_pdev_stats_urrn_tlv(tag_buf, len, stats_req);
		break;
	case HTT_STATS_TX_PDEV_SIFS_TAG:
		htt_print_tx_pdev_stats_sifs_tlv(tag_buf, len, stats_req);
		break;
	case HTT_STATS_TX_PDEV_FLUSH_TAG:
		htt_print_tx_pdev_stats_flush_tlv(tag_buf, len, stats_req);
		break;
	case HTT_STATS_TX_PDEV_SIFS_HIST_TAG:
		htt_print_tx_pdev_stats_sifs_hist_tlv(tag_buf, len, stats_req);
		break;
	case HTT_STATS_PDEV_CTRL_PATH_TX_STATS_TAG:
		htt_print_pdev_ctrl_path_tx_stats_tlv(tag_buf, len, stats_req);
		break;
	case HTT_STATS_MU_PPDU_DIST_TAG:
		htt_print_tx_pdev_mu_ppdu_dist_stats_tlv(tag_buf, len, stats_req);
		break;
	case HTT_STATS_TX_SCHED_CMN_TAG:
		ath12k_htt_print_stats_tx_sched_cmn_tlv(tag_buf, len, stats_req);
		break;
	case HTT_STATS_TX_PDEV_SCHEDULER_TXQ_STATS_TAG:
		ath12k_htt_print_tx_pdev_stats_sched_per_txq_tlv(tag_buf, len, stats_req);
		break;
	case HTT_STATS_SCHED_TXQ_CMD_POSTED_TAG:
		ath12k_htt_print_sched_txq_cmd_posted_tlv(tag_buf, len, stats_req);
		break;
	case HTT_STATS_SCHED_TXQ_CMD_REAPED_TAG:
		ath12k_htt_print_sched_txq_cmd_reaped_tlv(tag_buf, len, stats_req);
		break;
	case HTT_STATS_SCHED_TXQ_SCHED_ORDER_SU_TAG:
		ath12k_htt_print_sched_txq_sched_order_su_tlv(tag_buf, len, stats_req);
		break;
	case HTT_STATS_SCHED_TXQ_SCHED_INELIGIBILITY_TAG:
		ath12k_htt_print_sched_txq_sched_ineligibility_tlv(tag_buf, len,
								   stats_req);
		break;
	case HTT_STATS_SCHED_TXQ_SUPERCYCLE_TRIGGER_TAG:
		ath12k_htt_print_sched_txq_supercycle_trigger_tlv(tag_buf, len,
								  stats_req);
		break;
	case HTT_STATS_HW_PDEV_ERRS_TAG:
		ath12k_htt_print_hw_stats_pdev_errs_tlv(tag_buf, len, stats_req);
		break;
	case HTT_STATS_HW_INTR_MISC_TAG:
		ath12k_htt_print_hw_stats_intr_misc_tlv(tag_buf, len, stats_req);
		break;
	case HTT_STATS_WHAL_TX_TAG:
		ath12k_htt_print_hw_stats_whal_tx_tlv(tag_buf, len, stats_req);
		break;
	case HTT_STATS_HW_WAR_TAG:
		ath12k_htt_print_hw_war_tlv(tag_buf, len, stats_req);
		break;
	case HTT_STATS_TX_TQM_CMN_TAG:
		ath12k_htt_print_tx_tqm_cmn_stats_tlv(tag_buf, len, stats_req);
		break;
	case HTT_STATS_TX_TQM_ERROR_STATS_TAG:
		ath12k_htt_print_tx_tqm_error_stats_tlv(tag_buf, len, stats_req);
		break;
	case HTT_STATS_TX_TQM_GEN_MPDU_TAG:
		ath12k_htt_print_tx_tqm_gen_mpdu_stats_tlv(tag_buf, len, stats_req);
		break;
	case HTT_STATS_TX_TQM_LIST_MPDU_TAG:
		ath12k_htt_print_tx_tqm_list_mpdu_stats_tlv(tag_buf, len, stats_req);
		break;
	case HTT_STATS_TX_TQM_LIST_MPDU_CNT_TAG:
		ath12k_htt_print_tx_tqm_list_mpdu_cnt_tlv(tag_buf, len, stats_req);
		break;
	case HTT_STATS_TX_TQM_PDEV_TAG:
		ath12k_htt_print_tx_tqm_pdev_stats_tlv(tag_buf, len, stats_req);
		break;
	case HTT_STATS_TX_DE_CMN_TAG:
		ath12k_htt_print_tx_de_cmn_stats_tlv(tag_buf, len, stats_req);
		break;
	case HTT_STATS_TX_DE_EAPOL_PACKETS_TAG:
		ath12k_htt_print_tx_de_eapol_packets_stats_tlv(tag_buf, len, stats_req);
		break;
	case HTT_STATS_TX_DE_CLASSIFY_STATS_TAG:
		ath12k_htt_print_tx_de_classify_stats_tlv(tag_buf, len, stats_req);
		break;
	case HTT_STATS_TX_DE_CLASSIFY_FAILED_TAG:
		ath12k_htt_print_tx_de_classify_failed_stats_tlv(tag_buf, len, stats_req);
		break;
	case HTT_STATS_TX_DE_CLASSIFY_STATUS_TAG:
		ath12k_htt_print_tx_de_classify_status_stats_tlv(tag_buf, len, stats_req);
		break;
	case HTT_STATS_TX_DE_ENQUEUE_PACKETS_TAG:
		ath12k_htt_print_tx_de_enqueue_packets_stats_tlv(tag_buf, len, stats_req);
		break;
	case HTT_STATS_TX_DE_ENQUEUE_DISCARD_TAG:
		ath12k_htt_print_tx_de_enqueue_discard_stats_tlv(tag_buf, len, stats_req);
		break;
	case HTT_STATS_TX_DE_COMPL_STATS_TAG:
		ath12k_htt_print_tx_de_compl_stats_tlv(tag_buf, len, stats_req);
		break;
	case HTT_STATS_TX_SELFGEN_CMN_STATS_TAG:
		ath12k_htt_print_tx_selfgen_cmn_stats_tlv(tag_buf, len, stats_req);
		break;
	case HTT_STATS_TX_SELFGEN_AC_STATS_TAG:
		ath12k_htt_print_tx_selfgen_ac_stats_tlv(tag_buf, len, stats_req);
		break;
	case HTT_STATS_TX_SELFGEN_AX_STATS_TAG:
		ath12k_htt_print_tx_selfgen_ax_stats_tlv(tag_buf, len, stats_req);
		break;
	case HTT_STATS_TX_SELFGEN_BE_STATS_TAG:
		ath12k_htt_print_tx_selfgen_be_stats_tlv(tag_buf, len, stats_req);
		break;
	case HTT_STATS_TX_SELFGEN_AC_ERR_STATS_TAG:
		ath12k_htt_print_tx_selfgen_ac_err_stats_tlv(tag_buf, len, stats_req);
		break;
	case HTT_STATS_TX_SELFGEN_AX_ERR_STATS_TAG:
		ath12k_htt_print_tx_selfgen_ax_err_stats_tlv(tag_buf, len, stats_req);
		break;
	case HTT_STATS_TX_SELFGEN_BE_ERR_STATS_TAG:
		ath12k_htt_print_tx_selfgen_be_err_stats_tlv(tag_buf, len, stats_req);
		break;
	case HTT_STATS_TX_SELFGEN_AC_SCHED_STATUS_STATS_TAG:
		ath12k_htt_print_tx_selfgen_ac_sched_status_stats_tlv(tag_buf, len,
								      stats_req);
		break;
	case HTT_STATS_TX_SELFGEN_AX_SCHED_STATUS_STATS_TAG:
		ath12k_htt_print_tx_selfgen_ax_sched_status_stats_tlv(tag_buf, len,
								      stats_req);
		break;
	case HTT_STATS_TX_SELFGEN_BE_SCHED_STATUS_STATS_TAG:
		ath12k_htt_print_tx_selfgen_be_sched_status_stats_tlv(tag_buf, len,
								      stats_req);
		break;
	case HTT_STATS_STRING_TAG:
		ath12k_htt_print_stats_string_tlv(tag_buf, len, stats_req);
		break;
	case HTT_STATS_SRING_STATS_TAG:
		ath12k_htt_print_sring_stats_tlv(tag_buf, len, stats_req);
		break;
	case HTT_STATS_SFM_CMN_TAG:
		ath12k_htt_print_sfm_cmn_tlv(tag_buf, len, stats_req);
		break;
	case HTT_STATS_SFM_CLIENT_TAG:
		ath12k_htt_print_sfm_client_tlv(tag_buf, len, stats_req);
		break;
	case HTT_STATS_SFM_CLIENT_USER_TAG:
		ath12k_htt_print_sfm_client_user_tlv(tag_buf, len, stats_req);
		break;
	case HTT_STATS_TX_PDEV_MU_MIMO_STATS_TAG:
		ath12k_htt_print_tx_pdev_mu_mimo_sch_stats_tlv(tag_buf, len, stats_req);
		break;
	case HTT_STATS_TX_PDEV_MUMIMO_GRP_STATS_TAG:
		ath12k_htt_print_tx_pdev_mumimo_grp_stats_tlv(tag_buf, len, stats_req);
		break;
	case HTT_STATS_TX_PDEV_MPDU_STATS_TAG:
		ath12k_htt_print_tx_pdev_mu_mimo_mpdu_stats_tlv(tag_buf, len, stats_req);
		break;
	case HTT_STATS_PDEV_CCA_1SEC_HIST_TAG:
	case HTT_STATS_PDEV_CCA_100MSEC_HIST_TAG:
	case HTT_STATS_PDEV_CCA_STAT_CUMULATIVE_TAG:
		ath12k_htt_print_pdev_cca_stats_hist_tlv(tag_buf, len, stats_req);
		break;
	case HTT_STATS_PDEV_CCA_COUNTERS_TAG:
		ath12k_htt_print_pdev_stats_cca_counters_tlv(tag_buf, len, stats_req);
		break;
	case HTT_STATS_PDEV_OBSS_PD_TAG:
		ath12k_htt_print_pdev_obss_pd_stats_tlv(tag_buf, len, stats_req);
		break;
	case HTT_STATS_DMAC_RESET_STATS_TAG:
		ath12k_htt_print_dmac_reset_stats_tlv(tag_buf, len, stats_req);
		break;
	case HTT_STATS_PDEV_SCHED_ALGO_OFDMA_STATS_TAG:
		ath12k_htt_print_pdev_sched_algo_ofdma_stats_tlv(tag_buf, len, stats_req);
		break;
	case HTT_STATS_TX_PDEV_RATE_STATS_BE_OFDMA_TAG:
		ath12k_htt_print_tx_pdev_rate_stats_be_ofdma_tlv(tag_buf, len, stats_req);
		break;
	default:
		break;
	}

	return 0;
}

void ath12k_debugfs_htt_ext_stats_handler(struct ath12k_base *ab,
					  struct sk_buff *skb)
{
	struct ath12k_htt_extd_stats_msg *msg;
	struct debug_htt_stats_req *stats_req;
	struct ath12k *ar;
	u32 len, pdev_id, stats_info;
	u64 cookie;
	int ret;
	bool send_completion = false;

	msg = (struct ath12k_htt_extd_stats_msg *)skb->data;
	cookie = le64_to_cpu(msg->cookie);

	if (u64_get_bits(cookie, ATH12K_HTT_STATS_COOKIE_MSB) !=
			 ATH12K_HTT_STATS_MAGIC_VALUE) {
		ath12k_warn(ab, "received invalid htt ext stats event\n");
		return;
	}

	pdev_id = u64_get_bits(cookie, ATH12K_HTT_STATS_COOKIE_LSB);
	rcu_read_lock();
	ar = ath12k_mac_get_ar_by_pdev_id(ab, pdev_id);
	if (!ar) {
		ath12k_warn(ab, "failed to get ar for pdev_id %d\n", pdev_id);
		goto exit;
	}

	stats_req = ar->debug.htt_stats.stats_req;
	if (!stats_req)
		goto exit;

	spin_lock_bh(&ar->data_lock);

	stats_info = le32_to_cpu(msg->info1);
	stats_req->done = u32_get_bits(stats_info, ATH12K_HTT_T2H_EXT_STATS_INFO1_DONE);
	if (stats_req->done)
		send_completion = true;

	spin_unlock_bh(&ar->data_lock);

	len = u32_get_bits(stats_info, ATH12K_HTT_T2H_EXT_STATS_INFO1_LENGTH);
	if (len > skb->len) {
		ath12k_warn(ab, "invalid length %d for HTT stats", len);
		goto exit;
	}

	ret = ath12k_dp_htt_tlv_iter(ab, msg->data, len,
				     ath12k_dbg_htt_ext_stats_parse,
				     stats_req);
	if (ret)
		ath12k_warn(ab, "Failed to parse tlv %d\n", ret);

	if (send_completion)
		complete(&stats_req->htt_stats_rcvd);
exit:
	rcu_read_unlock();
}

static ssize_t ath12k_read_htt_stats_type(struct file *file,
					  char __user *user_buf,
					  size_t count, loff_t *ppos)
{
	struct ath12k *ar = file->private_data;
	enum ath12k_dbg_htt_ext_stats_type type;
	char buf[32];
	size_t len;

	wiphy_lock(ath12k_ar_to_hw(ar)->wiphy);
	type = ar->debug.htt_stats.type;
	wiphy_unlock(ath12k_ar_to_hw(ar)->wiphy);

	len = scnprintf(buf, sizeof(buf), "%u\n", type);

	return simple_read_from_buffer(user_buf, count, ppos, buf, len);
}

static ssize_t ath12k_write_htt_stats_type(struct file *file,
					   const char __user *user_buf,
					   size_t count, loff_t *ppos)
{
	struct ath12k *ar = file->private_data;
	enum ath12k_dbg_htt_ext_stats_type type;
	unsigned int cfg_param[4] = {0};
	const int size = 32;
	int num_args;

	char *buf __free(kfree) = kzalloc(size, GFP_KERNEL);
	if (!buf)
		return -ENOMEM;

	if (copy_from_user(buf, user_buf, count))
		return -EFAULT;

	num_args = sscanf(buf, "%u %u %u %u %u\n", &type, &cfg_param[0],
			  &cfg_param[1], &cfg_param[2], &cfg_param[3]);
	if (!num_args || num_args > 5)
		return -EINVAL;

	if (type == ATH12K_DBG_HTT_EXT_STATS_RESET ||
	    type >= ATH12K_DBG_HTT_NUM_EXT_STATS)
		return -EINVAL;

	wiphy_lock(ath12k_ar_to_hw(ar)->wiphy);

	ar->debug.htt_stats.type = type;
	ar->debug.htt_stats.cfg_param[0] = cfg_param[0];
	ar->debug.htt_stats.cfg_param[1] = cfg_param[1];
	ar->debug.htt_stats.cfg_param[2] = cfg_param[2];
	ar->debug.htt_stats.cfg_param[3] = cfg_param[3];

	wiphy_unlock(ath12k_ar_to_hw(ar)->wiphy);

	return count;
}

static const struct file_operations fops_htt_stats_type = {
	.read = ath12k_read_htt_stats_type,
	.write = ath12k_write_htt_stats_type,
	.open = simple_open,
	.owner = THIS_MODULE,
	.llseek = default_llseek,
};

static int ath12k_debugfs_htt_stats_req(struct ath12k *ar)
{
	struct debug_htt_stats_req *stats_req = ar->debug.htt_stats.stats_req;
	enum ath12k_dbg_htt_ext_stats_type type = stats_req->type;
	u64 cookie;
	int ret, pdev_id;
	struct htt_ext_stats_cfg_params cfg_params = { 0 };

	lockdep_assert_wiphy(ath12k_ar_to_hw(ar)->wiphy);

	init_completion(&stats_req->htt_stats_rcvd);

	pdev_id = ath12k_mac_get_target_pdev_id(ar);
	stats_req->done = false;
	stats_req->pdev_id = pdev_id;

	cookie = u64_encode_bits(ATH12K_HTT_STATS_MAGIC_VALUE,
				 ATH12K_HTT_STATS_COOKIE_MSB);
	cookie |= u64_encode_bits(pdev_id, ATH12K_HTT_STATS_COOKIE_LSB);

	if (stats_req->override_cfg_param) {
		cfg_params.cfg0 = stats_req->cfg_param[0];
		cfg_params.cfg1 = stats_req->cfg_param[1];
		cfg_params.cfg2 = stats_req->cfg_param[2];
		cfg_params.cfg3 = stats_req->cfg_param[3];
	}

	ret = ath12k_dp_tx_htt_h2t_ext_stats_req(ar, type, &cfg_params, cookie);
	if (ret) {
		ath12k_warn(ar->ab, "failed to send htt stats request: %d\n", ret);
		return ret;
	}
	if (!wait_for_completion_timeout(&stats_req->htt_stats_rcvd, 3 * HZ)) {
		spin_lock_bh(&ar->data_lock);
		if (!stats_req->done) {
			stats_req->done = true;
			spin_unlock_bh(&ar->data_lock);
			ath12k_warn(ar->ab, "stats request timed out\n");
			return -ETIMEDOUT;
		}
		spin_unlock_bh(&ar->data_lock);
	}

	return 0;
}

static int ath12k_open_htt_stats(struct inode *inode,
				 struct file *file)
{
	struct ath12k *ar = inode->i_private;
	struct debug_htt_stats_req *stats_req;
	enum ath12k_dbg_htt_ext_stats_type type = ar->debug.htt_stats.type;
	struct ath12k_hw *ah = ath12k_ar_to_ah(ar);
	int ret;

	if (type == ATH12K_DBG_HTT_EXT_STATS_RESET)
		return -EPERM;

	wiphy_lock(ath12k_ar_to_hw(ar)->wiphy);

	if (ah->state != ATH12K_HW_STATE_ON) {
		ret = -ENETDOWN;
		goto err_unlock;
	}

	if (ar->debug.htt_stats.stats_req) {
		ret = -EAGAIN;
		goto err_unlock;
	}

	stats_req = kzalloc(sizeof(*stats_req) + ATH12K_HTT_STATS_BUF_SIZE, GFP_KERNEL);
	if (!stats_req) {
		ret = -ENOMEM;
		goto err_unlock;
	}

	ar->debug.htt_stats.stats_req = stats_req;
	stats_req->type = type;
	stats_req->cfg_param[0] = ar->debug.htt_stats.cfg_param[0];
	stats_req->cfg_param[1] = ar->debug.htt_stats.cfg_param[1];
	stats_req->cfg_param[2] = ar->debug.htt_stats.cfg_param[2];
	stats_req->cfg_param[3] = ar->debug.htt_stats.cfg_param[3];
	stats_req->override_cfg_param = !!stats_req->cfg_param[0] ||
					!!stats_req->cfg_param[1] ||
					!!stats_req->cfg_param[2] ||
					!!stats_req->cfg_param[3];

	ret = ath12k_debugfs_htt_stats_req(ar);
	if (ret < 0)
		goto out;

	file->private_data = stats_req;

	wiphy_unlock(ath12k_ar_to_hw(ar)->wiphy);

	return 0;
out:
	kfree(stats_req);
	ar->debug.htt_stats.stats_req = NULL;
err_unlock:
	wiphy_unlock(ath12k_ar_to_hw(ar)->wiphy);

	return ret;
}

static int ath12k_release_htt_stats(struct inode *inode,
				    struct file *file)
{
	struct ath12k *ar = inode->i_private;

	wiphy_lock(ath12k_ar_to_hw(ar)->wiphy);
	kfree(file->private_data);
	ar->debug.htt_stats.stats_req = NULL;
	wiphy_unlock(ath12k_ar_to_hw(ar)->wiphy);

	return 0;
}

static ssize_t ath12k_read_htt_stats(struct file *file,
				     char __user *user_buf,
				     size_t count, loff_t *ppos)
{
	struct debug_htt_stats_req *stats_req = file->private_data;
	char *buf;
	u32 length;

	buf = stats_req->buf;
	length = min_t(u32, stats_req->buf_len, ATH12K_HTT_STATS_BUF_SIZE);
	return simple_read_from_buffer(user_buf, count, ppos, buf, length);
}

static const struct file_operations fops_dump_htt_stats = {
	.open = ath12k_open_htt_stats,
	.release = ath12k_release_htt_stats,
	.read = ath12k_read_htt_stats,
	.owner = THIS_MODULE,
	.llseek = default_llseek,
};

static ssize_t ath12k_write_htt_stats_reset(struct file *file,
					    const char __user *user_buf,
					    size_t count, loff_t *ppos)
{
	struct ath12k *ar = file->private_data;
	enum ath12k_dbg_htt_ext_stats_type type;
	struct htt_ext_stats_cfg_params cfg_params = { 0 };
	u8 param_pos;
	int ret;

	ret = kstrtou32_from_user(user_buf, count, 0, &type);
	if (ret)
		return ret;

	if (type >= ATH12K_DBG_HTT_NUM_EXT_STATS ||
	    type == ATH12K_DBG_HTT_EXT_STATS_RESET)
		return -E2BIG;

	wiphy_lock(ath12k_ar_to_hw(ar)->wiphy);
	cfg_params.cfg0 = HTT_STAT_DEFAULT_RESET_START_OFFSET;
	param_pos = (type >> 5) + 1;

	switch (param_pos) {
	case ATH12K_HTT_STATS_RESET_PARAM_CFG_32_BYTES:
		cfg_params.cfg1 = 1 << (cfg_params.cfg0 + type);
		break;
	case ATH12K_HTT_STATS_RESET_PARAM_CFG_64_BYTES:
		cfg_params.cfg2 = ATH12K_HTT_STATS_RESET_BITMAP32_BIT(cfg_params.cfg0 +
								      type);
		break;
	case ATH12K_HTT_STATS_RESET_PARAM_CFG_128_BYTES:
		cfg_params.cfg3 = ATH12K_HTT_STATS_RESET_BITMAP64_BIT(cfg_params.cfg0 +
								      type);
		break;
	default:
		break;
	}

	ret = ath12k_dp_tx_htt_h2t_ext_stats_req(ar,
						 ATH12K_DBG_HTT_EXT_STATS_RESET,
						 &cfg_params,
						 0ULL);
	if (ret) {
		ath12k_warn(ar->ab, "failed to send htt stats request: %d\n", ret);
		wiphy_unlock(ath12k_ar_to_hw(ar)->wiphy);
		return ret;
	}

	ar->debug.htt_stats.reset = type;
	wiphy_unlock(ath12k_ar_to_hw(ar)->wiphy);

	return count;
}

static const struct file_operations fops_htt_stats_reset = {
	.write = ath12k_write_htt_stats_reset,
	.open = simple_open,
	.owner = THIS_MODULE,
	.llseek = default_llseek,
};

void ath12k_debugfs_htt_stats_register(struct ath12k *ar)
{
	debugfs_create_file("htt_stats_type", 0600, ar->debug.debugfs_pdev,
			    ar, &fops_htt_stats_type);
	debugfs_create_file("htt_stats", 0400, ar->debug.debugfs_pdev,
			    ar, &fops_dump_htt_stats);
	debugfs_create_file("htt_stats_reset", 0200, ar->debug.debugfs_pdev,
			    ar, &fops_htt_stats_reset);
}
