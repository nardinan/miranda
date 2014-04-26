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
#include "string.obj.h"
struct s_object *f_string_new(struct s_object *self, char *format, ...) {
	va_list parameters;
	va_start(parameters, format);
	f_string_new_args(self, format, parameters);
	va_end(parameters);
	return self;
}

struct s_object *f_string_new_args(struct s_object *self, char *format, va_list parameters) {
	struct s_string_attributes *attributes = d_prepare(self, string);
	size_t length;
	char buffer_character[d_string_buffer];
	va_list parameters_backup;
	va_copy(parameters_backup, parameters);
	f_string_format_args(buffer_character, &length, d_string_buffer, NULL, NULL, format, parameters);
	if (length > 0) {
		attributes->size = length;
		if ((attributes->content = (char *) d_malloc(attributes->size+1)))
			f_string_format_args(attributes->content, &length, attributes->size+1, NULL, NULL, format, parameters_backup);
		else
			d_die(d_error_malloc);
	}
	va_end(parameters_backup);
	return self;
}

struct s_object *f_string_new_size(struct s_object *self, char *content, size_t size) {
	struct s_string_attributes *attributes = d_prepare(self, string);
	if (size > 0) {
		attributes->size = size;
		if ((attributes->content = (char *) d_malloc(attributes->size+1)))
			strncpy(attributes->content, content, size);
		else
			d_die(d_error_malloc);
	}
	return self;
}

struct s_object *f_string_new_constant(struct s_object *self, char *format) {
	struct s_string_attributes *attributes = d_prepare(self, string);
	attributes->flags.constant = d_true;
	attributes->content = format;
	attributes->size = f_string_strlen(format);
	return self;
}

d_define_method(string, trim)(struct s_object *self) {
	d_using(string);
	f_string_trim(string_attributes->content);
	string_attributes->size = f_string_strlen(string_attributes->content);
	return self;
}

d_define_method(string, append)(struct s_object *self, struct s_object *other) {
	d_using(string);
	struct s_string_attributes *other_attributes = d_cast(other, string);
	size_t length;
	if ((!string_attributes->flags.constant) && (length = f_string_strlen(string_attributes->content))) {
		f_string_append(&(string_attributes->content), other_attributes->content, &length);
		string_attributes->size = f_string_strlen(string_attributes->content);
	}
	return self;
}

d_define_method(string, length)(struct s_object *self, size_t *length) {
	d_using(string);
	*length = string_attributes->size;
	return self;
}

d_define_method(string, to_int)(struct s_object *self, int *value) {
	d_using(string);
	*value = atoi(string_attributes->content);
	return self;
}

d_define_method(string, to_float)(struct s_object *self, float *value) {
	d_using(string);
	*value = atof(string_attributes->content);
	return self;
}

d_define_method(string, character)(struct s_object *self, size_t position, char *character) {
	d_using(string);
	*character = '\0';
	if (string_attributes->size < position)
		*character = string_attributes->content[position];
	return self;
}

d_define_method(string, substring)(struct s_object *self, size_t begin, size_t size) {
	d_using(string);
	struct s_object *result = NULL;
	if (string_attributes->size >= begin) {
		if ((begin+size) > string_attributes->size)
			size = string_attributes->size-begin;
		result = f_string_new_size(d_new(string), string_attributes->content+begin, size);
	}
	return result;
}

d_define_method(string, split)(struct s_object *self, char character) {
	/* TODO: implement with array class */
	return self;
}

d_define_method(string, delete)(struct s_object *self, struct s_string_attributes *attributes) {
	if (!attributes->flags.constant)
		if (attributes->content)
			d_free(attributes->content);
	return NULL;
}

d_define_method(string, hash)(struct s_object *self, t_hash_value *value) {
	d_using(string);
	int index;
	*value = 7;
	for (index = 0; index < string_attributes->size; index++)
		*value = (*value*31)+(int)string_attributes->content[index];
	return self;
}

d_define_method(string, compare)(struct s_object *self, struct s_object *other) {
	d_using(string);
	struct s_string_attributes *other_attributes = d_cast(other, string);
	struct s_object *result = NULL;
	int compare = f_string_strcmp(string_attributes->content, other_attributes->content);
	if (compare > 0)
		result = self;
	else if (compare < 0)
		result = other;
	return result;
}

d_define_class(string) {
	d_hook_method(string, e_flag_public, trim),
	d_hook_method(string, e_flag_public, append),
	d_hook_method(string, e_flag_public, length),
	d_hook_method(string, e_flag_public, character),
	d_hook_method(string, e_flag_public, substring),
	d_hook_method(string, e_flag_public, split),
	d_hook_delete(string),
	d_hook_hash(string),
	d_hook_compare(string),
	d_hook_method_tail
};

