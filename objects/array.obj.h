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
#ifndef miranda_object_array_h
#define miranda_object_array_h
#include "memory.obj.h"
#include "mutex.obj.h"
#define d_array_bucket 8
d_exception_declare(bound);
d_declare_class(array) {
  struct s_attributes head;
  size_t elements, size, bucket, pointer;
  struct s_object **content;
} d_declare_class_tail(array);
struct s_array_attributes *p_array_alloc(struct s_object *self);
extern struct s_object *f_array_new(struct s_object *self, size_t size);
extern struct s_object *f_array_new_bucket(struct s_object *self, size_t size, size_t bucket);
extern struct s_object *f_array_new_list(struct s_object *self, size_t size, ...);
extern struct s_object *f_array_new_args(struct s_object *self, size_t size, va_list parameters);
d_declare_method(array, insert)(struct s_object *self, struct s_object *element, size_t position);
d_declare_method(array, remove)(struct s_object *self, size_t position);
d_declare_method(array, push)(struct s_object *self, struct s_object *element);
d_declare_method(array, pop)(struct s_object *self);
d_declare_method(array, get)(struct s_object *self, size_t position);
d_declare_method(array, reset)(struct s_object *self);
d_declare_method(array, next)(struct s_object *self);
#define d_array_foreach(o, v) for(d_call((o),m_array_reset,NULL),(v)=d_call((o),m_array_next,NULL);(v);(v)=d_call((o),m_array_next,NULL))
d_declare_method(array, size)(struct s_object *self, size_t *size);
d_declare_method(array, delete)(struct s_object *self, struct s_array_attributes *attributes);
d_declare_method(array, hash)(struct s_object *self, t_hash_value *value);
d_declare_method(array, compare)(struct s_object *self, struct s_object *other);
#endif

