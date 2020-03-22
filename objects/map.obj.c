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
#include "map.obj.h"
struct s_map_attributes *p_map_alloc(struct s_object *self) {
  struct s_map_attributes *result = d_prepare(self, map);
  f_memory_new(self);     /* inherit */
  f_mutex_new(self);      /* inherit */
  return result;
}
struct s_object *f_map_new(struct s_object *self) {
  struct s_map_attributes *attributes = p_map_alloc(self);
  f_hash_init(&attributes->hash, (t_hash_compare *)&f_object_compare, (t_hash_calculate *)&f_object_hash);
  return self;
}
d_define_method(map, insert)(struct s_object *self, struct s_object *key, struct s_object *value) {
  d_using(map);
  struct s_hash_bucket previous_content;
  struct s_object *retained_key = d_retain(key), *retained_value = d_retain(value);
  if (f_hash_insert(map_attributes->hash, (void *)retained_key, (void *)retained_value, d_true, &previous_content)) {
    /* if the value already exists, hash table keeps the previous key */
    d_delete(retained_key);
    d_delete((struct s_object *)previous_content.value);
  }
  return self;
}
d_define_method(map, remove)(struct s_object *self, struct s_object *key) {
  d_using(map);
  struct s_hash_bucket previous_content;
  if (f_hash_delete(map_attributes->hash, (void *)key, &previous_content))
    d_delete((struct s_object *)previous_content.value);
  return self;
}
d_define_method(map, find)(struct s_object *self, struct s_object *key) {
  d_using(map);
  d_cast_return(f_hash_get(map_attributes->hash, (void *)key));
}
d_define_method(map, reset)(struct s_object *self) {
  d_using(map);
  map_attributes->pointer = 0;
  return self;
}
d_define_method(map, next)(struct s_object *self, struct s_object **key) {
  d_using(map);
  struct s_hash_bucket *current_table = map_attributes->hash->table, *item;
  struct s_object *result = NULL;
  t_hash_value index = map_attributes->pointer;
  for (; index < (map_attributes->hash->mask + 1); ++index) {
    item = &current_table[index];
    map_attributes->pointer = (index + 1); /* the next one */
    if (item->kind == e_hash_kind_fill) {
      *key = item->key;
      result = item->value;
      break;
    }
  }
  return result;
}
d_define_method(map, size)(struct s_object *self, size_t *size) {
  d_using(map);
  *size = (map_attributes->hash->mask + 1);
  return self;
}
d_define_method(map, delete)(struct s_object *self, struct s_map_attributes *attributes) {
  struct s_hash_bucket *current_table = attributes->hash->table, *item;
  t_hash_value elements = attributes->hash->mask;
  for (; elements >= 0; --elements) {
    item = &current_table[elements];
    if (item->kind != e_hash_kind_empty) {
      d_delete(item->key);
      if (item->kind == e_hash_kind_fill)
        d_delete(item->value);
    }
  }
  f_hash_destroy(&(attributes->hash));
  return NULL;
}
d_define_method(map, hash)(struct s_object *self, t_hash_value *value) {
  d_using(map);
  value = 0;
  struct s_hash_bucket *current_table = map_attributes->hash->table, *item;
  t_hash_value elements = map_attributes->hash->mask;
  for (; elements >= 0; --elements) {
    item = &current_table[elements];
    if (item->kind == e_hash_kind_fill)
      value += (f_object_hash((struct s_object *)item->key) + f_object_hash((struct s_object *)item->value));
  }
  return self;
}
d_define_method(map, compare)(struct s_object *self, struct s_object *other) {
  d_using(map);
  struct s_map_attributes *map_attributes_other = d_cast(other, map);
  struct s_hash_bucket *current_table = map_attributes->hash->table, *other_table = map_attributes_other->hash->table, *current_item, *other_item;
  struct s_object *result = NULL;
  t_hash_value elements = map_attributes->hash->mask;
  if (map_attributes_other->hash->mask > map_attributes->hash->mask) {
    result = other;
  } else if (map_attributes_other->hash->mask < map_attributes->hash->mask) {
    result = self;
  } else {
    for (; elements >= 0; --elements) {
      current_item = &(current_table[elements]);
      other_item = &(other_table[elements]);
      if ((result = f_object_compare((struct s_object *)(current_item->key), (struct s_object *)(other_item->key))) ||
          (result = f_object_compare((struct s_object *)(current_item->value), (struct s_object *)(other_item->value))))
        break;
    }
  }
  return result;
}
d_define_class(map) {d_hook_method(map, e_flag_public, insert),
  d_hook_method(map, e_flag_public, remove),
  d_hook_method(map, e_flag_public, find),
  d_hook_method(map, e_flag_public, reset),
  d_hook_method(map, e_flag_public, next),
  d_hook_method(map, e_flag_public, size),
  d_hook_delete(map),
  d_hook_hash(map),
  d_hook_compare(map),
  d_hook_method_tail};
