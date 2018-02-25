/*
 * miranda
 * Copyright (C) 2018 Andrea Nardinocchi (andrea@nardinan.it)
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
#ifndef miranda_object_map_h
#define miranda_object_map_h
#include "../hash.h"
#include "memory.obj.h"
#include "mutex.obj.h"
d_declare_class(map) {
  struct s_attributes head;
  struct s_hash_table *hash;
  t_hash_value pointer;
} d_declare_class_tail(map);
struct s_map_attributes *p_map_alloc(struct s_object *self);
extern struct s_object *f_map_new(struct s_object *self);
d_declare_method(map, insert)(struct s_object *self, struct s_object *key, struct s_object *value);
d_declare_method(map, remove)(struct s_object *self, struct s_object *key);
d_declare_method(map, find)(struct s_object *self, struct s_object *key);
d_declare_method(map, reset)(struct s_object *self);
d_declare_method(map, next)(struct s_object *self, struct s_object **key);
#define d_map_foreach(o,v,k) for(d_call((o),m_map_reset,NULL),(v)=d_call((o),m_map_next,&(k));(v);(v)=d_call((o),m_map_next,&(k)))
d_declare_method(map, size)(struct s_object *self, size_t *size);
d_declare_method(map, delete)(struct s_object *self, struct s_map_attributes *attributes);
d_declare_method(map, hash)(struct s_object *self, t_hash_value *value);
d_declare_method(map, compare)(struct s_object *self, struct s_object *other);
#endif
