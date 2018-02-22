/*
 * miranda
 * Copyright (C) 2014 Andrea Nardinocchi (andrea@nardinan.it)
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
#ifndef miranda_hash_h
#define miranda_hash_h
#include "string.local.h"
#include "memory.h"
#define d_hash_segments 8 /* must be a power of two */
#define d_hash_shift 5
#define d_hash_numerator 2
#define d_hash_denominator 3
#define d_hash_multiplicator 2
typedef long long t_hash_value;
typedef t_hash_value (t_hash_calculate)(void *key);
typedef int (t_hash_compare)(void *left, void *right);
typedef enum e_hash_kind {
  e_hash_kind_deleted,
  e_hash_kind_empty,
  e_hash_kind_fill
} e_hash_kind;
typedef struct s_hash_bucket {
  t_hash_value hash;
  void *key, *value;
  enum e_hash_kind kind;
} s_hash_bucket;
typedef struct s_hash_table {
  t_hash_value mask, fill;
  t_hash_compare *compare;
  t_hash_calculate *calculate;
  struct s_hash_bucket *table;
} s_hash_table;
extern void p_hash_allocate(struct s_hash_table *table, t_hash_value dimension);
extern void p_hash_resize(struct s_hash_table *table, t_hash_value dimension);
extern struct s_hash_bucket *p_hash_lookup(struct s_hash_table *table, void *key, t_hash_value hash);
extern void f_hash_init(struct s_hash_table **table, t_hash_compare *compare, t_hash_calculate *calculate);
extern void f_hash_destroy(struct s_hash_table **table);
extern int f_hash_insert(struct s_hash_table *table, void *key, void *value, t_boolean update, struct s_hash_bucket *current);
extern void *f_hash_get(struct s_hash_table *table, void *key);
extern int f_hash_delete(struct s_hash_table *table, void *key, struct s_hash_bucket *current);
#endif
