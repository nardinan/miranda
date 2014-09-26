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
#include "array.obj.h"
d_exception_define(bound, 1, "out of bound exception");
struct s_array_attributes *p_array_alloc(struct s_object *self) {
	struct s_array_attributes *result = d_prepare(self, array);
	f_memory_new(self); /* inherit */
	return result;
}

struct s_object *f_array_new(struct s_object *self, size_t size) {
	return f_array_new_bucket(self, size, d_array_bucket);
}

struct s_object *f_array_new_bucket(struct s_object *self, size_t size, size_t bucket) {
	struct s_array_attributes *attributes = p_array_alloc(self);
	if ((attributes->content = (struct s_object **) d_malloc(size*sizeof(struct s_object *)))) {
		attributes->size = size;
		attributes->bucket = bucket;
	} else
		d_die(d_error_malloc);
	return self;
}

struct s_object *f_array_new_list(struct s_object *self, size_t size, ...) {
	struct s_array_attributes *attributes = p_array_alloc(self);
	va_list parameters;
	va_start(parameters, size);
	f_array_new_args(self, size, parameters);
	va_end(parameters);
	return self;
}

struct s_object *f_array_new_args(struct s_object *self, size_t size, va_list parameters) {
	struct s_array_attributes *attributes = d_cast(self, array);
	struct s_object *object;
	size_t index;
	if ((attributes->content = (struct s_object **) d_malloc(size*sizeof(struct s_object *)))) {
		attributes->size = size;
		attributes->bucket = d_array_bucket;
		for (index = 0; index < size; ++index)
			if ((object = va_arg(parameters, struct s_object *))) {
				attributes->content[index] = d_retain(object);
				attributes->elements++;
			}
	} else
		d_die(d_error_malloc);
		return self;
}

d_define_method(array, insert)(struct s_object *self, struct s_object *element, size_t position) {
	d_using(array);
	if (position <= array_attributes->size) {
		if (position == array_attributes->size) {
			if ((array_attributes->content = (struct s_object **) d_realloc(array_attributes->content,
							((array_attributes->size+array_attributes->bucket)*sizeof(struct s_object *))))) {
				memset(&(array_attributes->content[array_attributes->size]), 0, (array_attributes->size*sizeof(struct s_object *)));
				array_attributes->size += array_attributes->bucket;
			} else
				d_die(d_error_malloc);
		} else if (array_attributes->content[position]) {
			if (array_attributes->content[array_attributes->size-1]) {
				if ((array_attributes->content = (struct s_object **) d_realloc(array_attributes->content,
								((array_attributes->size+array_attributes->bucket)*sizeof(struct s_object *))))) {
					memset(&(array_attributes->content[array_attributes->size]), 0, (array_attributes->size*sizeof(struct s_object *)));
					memmove(&(array_attributes->content[position+1]), &(array_attributes->content[position]),
							((array_attributes->size-position)*sizeof(struct s_object *)));
					array_attributes->size += array_attributes->bucket;
				} else
					d_die(d_error_malloc);
			} else
				memmove(&(array_attributes->content[position+1]), &(array_attributes->content[position]),
						((array_attributes->size-position-1)*sizeof(struct s_object *)));
		}
		if (element) {
			array_attributes->content[position] = d_retain(element);
			array_attributes->elements++;
		} else
			array_attributes->content[position] = NULL;
	} else
		d_throw(v_exception_bound, "index is bigger than array size");
	return self;
}

d_define_method(array, remove)(struct s_object *self, size_t position) {
	d_using(array);
	struct s_object *result = NULL;
	if (position < array_attributes->size) {
		if ((result = array_attributes->content[position])) {
			d_delete(result);
			array_attributes->content[position] = NULL;
			array_attributes->elements--;
			result = self;
		}
	} else
		d_throw(v_exception_bound, "index is bigger than array size");
	return result;
}

d_define_method(array, get)(struct s_object *self, size_t position) {
	d_using(array);
	struct s_object *result = NULL;
	if (position < array_attributes->size)
		result = array_attributes->content[position];
	else
		d_throw(v_exception_bound, "index is bigget than array size");
	return result;
}

d_define_method(array, reset)(struct s_object *self) {
	d_using(array);
	array_attributes->pointer = 0;
	return self;
}

d_define_method(array, next)(struct s_object *self) {
	d_using(array);
	struct s_object *result = NULL;
	if (array_attributes->pointer < array_attributes->size)
		result = array_attributes->content[array_attributes->pointer++];
	return result;
}

d_define_method(array, delete)(struct s_object *self, struct s_array_attributes *attributes) {
	d_using(array);
	int index;
	if (array_attributes->content) {
		for (index = 0; index < array_attributes->size; ++index)
			if (array_attributes->content[index]) {
				d_delete(array_attributes->content[index]);
				array_attributes->content[index] = NULL;
			}
		d_free(array_attributes->content);
	}
	return NULL;
}

d_define_method(array, hash)(struct s_object *self, t_hash_value *value) {
	d_using(array);
	int index;
	t_hash_value local_value;
	*value = 7;
	for (index = 0; index < array_attributes->size; ++index) {
		local_value = 0;
		if (array_attributes->content[index])
			d_call(array_attributes->content[index], m_object_hash, &local_value);
		*value = (*value*31)+local_value;
	}
	return self;
}

d_define_method(array, compare)(struct s_object *self, struct s_object *other) {
	d_using(array);
	struct s_array_attributes *other_attributes = d_cast(other, array);
	struct s_object *result = NULL;
	int index, compare = 0;
	if (((compare = (array_attributes->size-other_attributes->size)) == 0) && ((compare = (array_attributes->elements-other_attributes->elements)) == 0))
		for (index = 0; (index < array_attributes->size) && (compare == 0); ++index)
			compare = (array_attributes->content[index]-other_attributes->content[index]);
	if (compare < 0)
		result = self;
	else if (compare > 0)
		result = other;
	return result;
}

d_define_class(array) {
	d_hook_method(array, e_flag_public, insert),
		d_hook_method(array, e_flag_public, remove),
		d_hook_method(array, e_flag_public, get),
		d_hook_method(array, e_flag_public, reset),
		d_hook_method(array, e_flag_public, next),
		d_hook_delete(array),
		d_hook_hash(array),
		d_hook_compare(array),
		d_hook_method_tail
};

