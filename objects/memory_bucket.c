/*
 * miranda
 * Copyright (C) 2014-2020 Andrea Nardinocchi (andrea@nardinan.it)
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
void f_memory_bucket_init(struct s_memory_buckets **buckets, t_memory_bucket_delete *delete_operator) {
  if ((*buckets = (struct s_memory_buckets *)d_malloc(sizeof(struct s_memory_buckets)))) {
    f_hash_init(&((*buckets)->hash), (t_hash_compare *)&f_memory_bucket_compare, (t_hash_calculate *)&f_memory_bucket_hash);
    (*buckets)->delete_operator = delete_operator;
  } else
    d_die(d_error_malloc);
}
void f_memory_bucket_destroy_single_bucket(struct s_memory_bucket **bucket, t_memory_bucket_delete *delete_operator) {
  size_t index;
  for (index = 0; index < d_memory_bucket_slots; ++index) {
    if ((*bucket)->pointers[index]) {
      if (delete_operator) {
        delete_operator((*bucket)->pointers[index]);
        (*bucket)->pointers[index] = NULL;
      }
    } else
      break;
  }
  d_free((*bucket));
  *bucket = NULL;
}
void f_memory_bucket_destroy(struct s_memory_buckets **buckets) {
  struct s_hash_bucket *current_table = (*buckets)->hash->table, *item;
  struct s_memory_bucket *current_element;
  t_hash_value elements = (*buckets)->hash->mask;
  for (; elements >= 0; --elements) {
    item = &current_table[elements];
    if (item->kind == e_hash_kind_fill) {
      current_element = (struct s_memory_bucket *)(item->value);
      if (current_element->projected_bucket_slots > 0)
        d_log(e_log_level_medium, "Memory queue for type %s is currently %zu, while it shall probably be increased by %zu elements to have better performances",
            (const char *)item->key, d_memory_bucket_slots, current_element->projected_bucket_slots);
      f_memory_bucket_destroy_single_bucket(&current_element, (*buckets)->delete_operator);
    }
  }
  f_hash_destroy(&((*buckets)->hash));
  d_free(*buckets);
  *buckets = NULL;
}
void *f_memory_bucket_push(struct s_memory_buckets *buckets, const char *type, void *memory) {
  void *result = NULL;
  struct s_memory_bucket *bucket = NULL;
  struct s_hash_bucket old_value;
  if (!(bucket = f_hash_get(buckets->hash, (void *)type))) {
    if ((bucket = (struct s_memory_bucket *)d_malloc(sizeof(struct s_memory_bucket)))) {
      memset(bucket, 0, sizeof(struct s_memory_bucket));
      if (f_hash_insert(buckets->hash, (void *)type, bucket, d_true, &old_value)) {
        if (old_value.kind == e_hash_kind_fill)
          f_memory_bucket_destroy_single_bucket((struct s_memory_bucket **)&(old_value.value), buckets->delete_operator);
      }
    } else
      d_die(d_error_malloc);
  }
  if (bucket) {
    if (bucket->entries >= d_memory_bucket_slots) {
      result = bucket->pointers[0];
      memmove(&(bucket->pointers[0]), &(bucket->pointers[1]), ((d_memory_bucket_slots - 1) * sizeof(void *)));
      bucket->pointers[d_memory_bucket_slots - 1] = memory;
      ++(bucket->current_bucket_counter);
    } else {
      bucket->pointers[bucket->entries++] = memory;
    }
  }
  return result;
}
void *f_memory_bucket_query(struct s_memory_buckets *buckets, const char *type) {
  void *result = NULL;
  struct s_memory_bucket *bucket;
  if ((bucket = f_hash_get(buckets->hash, (void *)type)))
    if (bucket->entries > 0) {
      if (bucket->current_bucket_counter > bucket->projected_bucket_slots)
        bucket->projected_bucket_slots = bucket->current_bucket_counter;
      bucket->current_bucket_counter = 0;
      result = bucket->pointers[--(bucket->entries)];
      bucket->pointers[bucket->entries] = NULL;
    }
  return result;
}
