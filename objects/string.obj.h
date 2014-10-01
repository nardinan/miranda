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
#ifndef miranda_object_string_h
#define miranda_object_string_h
#include "array.obj.h"
#define d_string_buffer 8
d_declare_class(string) {
	struct s_attributes head;
	char *content;
	size_t size, length;
	struct {
		int constant:1;
	} flags;
} d_declare_class_tail(string);
struct s_string_attributes *p_string_alloc(struct s_object *self);
extern struct s_object *f_string_new(struct s_object *self, char *format, ...);
extern struct s_object *f_string_new_args(struct s_object *self, char *format, va_list parameters);
extern struct s_object *f_string_new_size(struct s_object *self, char *content, size_t size);
extern struct s_object *f_string_new_constant(struct s_object *self, char *content);
d_declare_method(string, trim)(struct s_object *self);
d_declare_method(string, append)(struct s_object *self, struct s_object *other);
d_declare_method(string, substring)(struct s_object *self, size_t begin, size_t end);
d_declare_method(string, split)(struct s_object *self, char character);
#define d_string_cstring(str) ((char *)d_call(str,m_string_cstring,NULL))
d_declare_method(string, cstring)(struct s_object *self);
d_declare_method(string, length)(struct s_object *self, size_t *length);
d_declare_method(string, size)(struct s_object *self, size_t *size);
d_declare_method(string, delete)(struct s_object *self, struct s_string_attributes *attributes);
d_declare_method(string, hash)(struct s_object *self, t_hash_value *value);
d_declare_method(string, compare)(struct s_object *self, struct s_object *other);
#endif

