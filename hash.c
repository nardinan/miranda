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
#include "hash.h"
void p_hash_allocate(struct s_hash_table *table, t_hash_value dimension) {
  t_hash_value index;
  if ((table->table = (s_hash_bucket *)d_malloc(dimension * sizeof(struct s_hash_bucket)))) {
    for (index = 0; index < dimension; ++index) {
      table->table[index].kind = e_hash_kind_empty;
      table->table[index].value = NULL;
    }
    table->fill = 0;
    table->mask = (dimension - 1);
  } else
    d_die(d_error_malloc);
}
void p_hash_resize(struct s_hash_table *table, t_hash_value dimension) {
  struct s_hash_bucket *current = table->table, *item;
  t_hash_value elements = table->mask;
  p_hash_allocate(table, dimension);
  for (; elements >= 0; --elements) {
    item = &current[elements];
    if (item->kind == e_hash_kind_fill)
      f_hash_insert(table, item->key, item->value, d_true, NULL);
  }
  d_free(current);
}
struct s_hash_bucket *p_hash_lookup(struct s_hash_table *table, void *key, t_hash_value hash) {
  t_hash_value index = hash & table->mask, shift = hash;
  struct s_hash_bucket *item, *backup = NULL;
  while (d_true) {
    item = &table->table[index & table->mask];
    if (item->key == key)
      break;
    else if (item->kind == e_hash_kind_deleted)
      backup = item;
    else if (item->kind == e_hash_kind_empty) {
      if (backup)
        item = backup;
      break;
    } else if ((item->hash == hash) && (table->compare(item->key, key) == 0))
      break;
    index = ((index << 2) + index + shift) + 1;
    shift >>= d_hash_shift;
  }
  return item;
}
void f_hash_init(struct s_hash_table **table, t_hash_compare *compare, t_hash_calculate *calculate) {
  if ((*table = (struct s_hash_table *)d_malloc(sizeof(struct s_hash_table)))) {
    (*table)->compare = compare;
    (*table)->calculate = calculate;
    p_hash_allocate((*table), d_hash_segments);
  } else
    d_die(d_error_malloc);
}
void f_hash_destroy(struct s_hash_table **table) {
  d_free((*table)->table);
  d_free((*table));
  *table = NULL;
}
int f_hash_insert(struct s_hash_table *table, void *key, void *value, t_boolean update, struct s_hash_bucket *current) {
  t_hash_value hash = table->calculate(key);
  struct s_hash_bucket *item = p_hash_lookup(table, key, hash);
  int result = d_false;
  if (!item->value) {
    if (item->kind == e_hash_kind_empty)
      table->fill++;
    item->hash = hash;
    item->key = key;
    item->value = value;
  } else {
    result = d_true;
    if (current)
      memcpy(current, item, sizeof(struct s_hash_bucket));
    if (update)
      item->value = value;
  }
  item->kind = e_hash_kind_fill;
  if (((table->fill) * d_hash_denominator) > ((table->mask + 1) * d_hash_numerator))
    p_hash_resize(table, ((table->mask + 1) * d_hash_multiplicator));
  return result;
}
void *f_hash_get(struct s_hash_table *table, void *key) {
  t_hash_value hash = table->calculate(key);
  return p_hash_lookup(table, key, hash)->value;
}
int f_hash_delete(struct s_hash_table *table, void *key, struct s_hash_bucket *current) {
  t_hash_value hash = table->calculate(key);
  s_hash_bucket *item = p_hash_lookup(table, key, hash);
  int result = d_false;
  if (item->kind == e_hash_kind_fill) {
    result = d_true;
    if (current)
      memcpy(current, item, sizeof(struct s_hash_bucket));
    item->value = NULL;
    item->kind = e_hash_kind_deleted;
    table->fill--;
  }
  return result;
}

