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
#include "memory_bucket.h"
t_hash_value f_memory_bucket_hash(const char *key) {
  return (intptr_t)key;
}
int f_memory_bucket_compare(const char *left, const char *right) {
  return (intptr_t)(left-right);
}
void f_memory_bucket_init(struct s_memory_buckets **buckets) {
  if ((*buckets = (struct s_memory_buckets *)d_malloc(sizeof(struct s_memory_buckets))))
    f_hash_init(&((*buckets)->hash), (t_hash_compare *)&f_memory_bucket_compare, (t_hash_calculate *)&f_memory_bucket_hash);
  else
    d_die(d_error_malloc);
}
void f_memory_bucket_destroy_single_bucket(struct s_memory_bucket **bucket) {
  size_t index;
  for (index = 0; index < d_memory_bucket_slots; ++index) {
    if ((*bucket)->pointers[index])
      d_free((*bucket)->pointers[index]);
    else
      break;
  }
  d_free((*bucket));
  *bucket = NULL;
}
void f_memory_bucket_destroy(struct s_memory_buckets **buckets) {
  struct s_hash_bucket *current_table = (*buckets)->hash->table, *item;
  t_hash_value elements = (*buckets)->hash->mask;
  size_t index;
  for (; elements >= 0; --elements) {
    item = &current_table[elements];
    if (item->kind == e_hash_kind_fill)
      f_memory_bucket_destroy_single_bucket((struct s_memory_bucket **)&(item->value));
  }
  f_hash_destroy(&((*buckets)->hash));
  d_free(*buckets);
}
void *f_memory_bucket_push(struct s_memory_buckets *buckets, const char *type, void *memory) {
  void *result = NULL;
  struct s_memory_bucket *bucket;
  struct s_hash_bucket old_value;
  size_t index;
  if (!(bucket = f_hash_get(buckets->hash, (void *)type))) {
    if ((bucket = (struct s_memory_bucket *)d_malloc(sizeof(struct s_memory_bucket)))) {
      memset(bucket, 0, sizeof(struct s_memory_bucket));
      bucket->entries = 0;
      if (f_hash_insert(buckets->hash, (void *)type, bucket, d_true, &old_value)) {
        if (old_value.kind == e_hash_kind_fill)
          f_memory_bucket_destroy_single_bucket((struct s_memory_bucket **)&(old_value.value));
      } else {
        d_free(bucket);
        bucket = NULL;
      }
    } else
      d_die(d_error_malloc);
  }
  if (bucket) {
    if (bucket->entries >= d_memory_bucket_slots) {
      result = bucket->pointers[0];
      memmove(bucket->pointers, (bucket->pointers + sizeof(void *)), ((d_memory_bucket_slots - 1) * sizeof(void *)));
      bucket->pointers[d_memory_bucket_slots - 1] = memory;
    } else
      bucket->pointers[bucket->entries++] = memory;
  }
  return result;
}
void *f_memory_bucket_query(struct s_memory_buckets *buckets, const char *type) {
  void *result = NULL;
  struct s_memory_bucket *bucket;
  if ((bucket = f_hash_get(buckets->hash, (void *)type)))
    if (bucket->entries > 0)
      result =  bucket->pointers[bucket->entries--];
  return result;
}