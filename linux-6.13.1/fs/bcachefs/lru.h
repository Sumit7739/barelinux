/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _BCACHEFS_LRU_H
#define _BCACHEFS_LRU_H

static inline u64 lru_pos_id(struct bpos pos)
{
	return pos.inode >> LRU_TIME_BITS;
}

static inline u64 lru_pos_time(struct bpos pos)
{
	return pos.inode & ~(~0ULL << LRU_TIME_BITS);
}

static inline struct bpos lru_pos(u16 lru_id, u64 dev_bucket, u64 time)
{
	struct bpos pos = POS(((u64) lru_id << LRU_TIME_BITS)|time, dev_bucket);

	EBUG_ON(time > LRU_TIME_MAX);
	EBUG_ON(lru_pos_id(pos) != lru_id);
	EBUG_ON(lru_pos_time(pos) != time);
	EBUG_ON(pos.offset != dev_bucket);

	return pos;
}

static inline enum bch_lru_type lru_type(struct bkey_s_c l)
{
	u16 lru_id = l.k->p.inode >> 48;

	if (lru_id == BCH_LRU_FRAGMENTATION_START)
		return BCH_LRU_fragmentation;
	return BCH_LRU_read;
}

int bch2_lru_validate(struct bch_fs *, struct bkey_s_c, enum bch_validate_flags);
void bch2_lru_to_text(struct printbuf *, struct bch_fs *, struct bkey_s_c);

void bch2_lru_pos_to_text(struct printbuf *, struct bpos);

#define bch2_bkey_ops_lru ((struct bkey_ops) {	\
	.key_validate	= bch2_lru_validate,	\
	.val_to_text	= bch2_lru_to_text,	\
	.min_val_size	= 8,			\
})

int bch2_lru_del(struct btree_trans *, u16, u64, u64);
int bch2_lru_set(struct btree_trans *, u16, u64, u64);
int bch2_lru_change(struct btree_trans *, u16, u64, u64, u64);

struct bkey_buf;
int bch2_lru_check_set(struct btree_trans *, u16, u64, struct bkey_s_c, struct bkey_buf *);

int bch2_check_lrus(struct bch_fs *);

#endif /* _BCACHEFS_LRU_H */
