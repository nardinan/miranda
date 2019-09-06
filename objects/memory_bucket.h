/*
 * miranda
 * Copyright (C) 2019 Andrea Nardinocchi (andrea@nardinan.it)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef miranda_object_memory_bucket_h
#define miranda_object_memory_bucket_h
#include <stdint.h>
#include "../hash.h"
#define d_memory_bucket_slots 64
typedef t_hash_value (t_memory_bucket_delete)(void *key);
typedef struct s_memory_bucket {
  void *pointers[d_memory_bucket_slots];
  size_t entries, projected_bucket_slots, current_bucket_counter;
} s_memory_bucket;
typedef struct s_memory_buckets {
  struct s_hash_table *hash;
  t_memory_bucket_delete *delete_operator;
} s_memory_buckets;
extern t_hash_value f_memory_bucket_hash(const char *key);
extern int f_memory_bucket_compare(const char *left, const char *right);
extern void f_memory_bucket_init(struct s_memory_buckets **buckets, t_memory_bucket_delete *delete_operator);
extern void f_memory_bucket_destroy_single_bucket(struct s_memory_bucket **bucket, t_memory_bucket_delete *delete_operator);
extern void f_memory_bucket_destroy(struct s_memory_buckets **buckets);
extern void *f_memory_bucket_push(struct s_memory_buckets *buckets, const char *type, void *memory);
extern void *f_memory_bucket_query(struct s_memory_buckets *buckets, const char *type);
#endif