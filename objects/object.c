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
#include "object.h"
const char m_object_delete[] = "delete";
const char m_object_hash[] = "hash";
const char m_object_compare[] = "compare";
d_exception_define(undefined_method, 1, "undefined method exception");
d_exception_define(private_method, 2, "private method exception");
d_exception_define(wrong_casting, 3, "wrong casting exception");
const struct s_method *p_object_recall(const char *file, int line, struct s_object *object, const char *symbol) {
	struct s_virtual_table *singleton;
	const struct s_method *result;
	int index;
	d_foreach(&(object->virtual_tables), singleton, struct s_virtual_table)
		for (index = 0; singleton->virtual_table[index].symbol; index++)
			if (singleton->virtual_table[index].symbol == symbol) {
				if ((singleton->virtual_table[index].flag == e_flag_private) && (singleton->virtual_table[index].file != file))
					d_throw(v_exception_private_method, "this method is private and you're out of context");
				else {
					result = &(singleton->virtual_table[index]);
					break;
				}
			}
	if (!result)
		d_throw(v_exception_undefined_method, "this symbol is undefined or is a member of another class");
	return result;
}

struct s_object *p_object_malloc(const char *file, int line, const char *type, int flags) {
	struct s_object *result;
	if ((result = p_malloc(sizeof(struct s_object), file, line))) {
		result->type = type;
		result->file = file;
		result->line = line;
		memset(&(result->virtual_tables), 0, sizeof(struct s_list));
		memset(&(result->attributes), 0, sizeof(struct s_list));
		result->flags = flags;
	} else
		d_die(d_error_malloc);
	return result;
}

struct s_attributes *p_object_attributes_malloc(size_t size, const char *type) {
	struct s_attributes *result;
	if ((result = (struct s_attributes *)d_malloc(size)))
		result->type = type;
	else
		d_die(d_error_malloc);
	return result;
}

struct s_attributes *p_object_setup(struct s_object *object, struct s_method *virtual_table, struct s_attributes *attributes) {
	struct s_virtual_table *node;
	if ((node = (struct s_virtual_table *) d_malloc(sizeof(struct s_virtual_table)))) {
		node->virtual_table = virtual_table;
		f_list_append(&(object->virtual_tables), (struct s_list_node *)node, e_list_insert_head);
		f_list_append(&(object->attributes), (struct s_list_node *)attributes, e_list_insert_head);
	} else
		d_die(d_error_malloc);
	return attributes;
}

struct s_attributes *p_object_cast(struct s_object *object, const char *type) {
	struct s_attributes *result = NULL;
	if ((object->last_attributes) && (object->last_attributes->type == type))
		result = object->last_attributes;
	else {
		d_foreach(&(object->attributes), result, struct s_attributes)
			if (result->type == type) {
				object->last_attributes = result;
				break;
			}
	}
	if (!result)
		d_throw(v_exception_wrong_casting, "this object can't be casted in this way");
	return result;
}

void f_object_delete(struct s_object *object) {
	struct s_attributes *attributes;
	struct s_virtual_table *virtual_table;
	int index;
	while ((attributes = (struct s_attributes *)object->attributes.head) &&
			(virtual_table = (struct s_virtual_table *)object->virtual_tables.head)) {
		for (index = 0; virtual_table->virtual_table[index].symbol; ++index)
			if (virtual_table->virtual_table[index].symbol == m_object_delete) {
				virtual_table->virtual_table[index].method(object, attributes);
				break;
			}
		f_list_delete(&(object->attributes), (struct s_list_node *)attributes);
		f_list_delete(&(object->virtual_tables), (struct s_list_node *)virtual_table);
		d_free(attributes);
		d_free(virtual_table);
	}
	d_free(object);
}

t_hash_value f_object_hash(struct s_object *object) {
	struct s_virtual_table *virtual_table;
	int index;
	t_hash_value result = 0, current;
	if ((object->flags&e_flag_hashed) != e_flag_hashed) {
		virtual_table = (struct s_virtual_table *)object->virtual_tables.head;
		while (virtual_table) {
			for (index = 0; virtual_table->virtual_table[index].symbol; ++index)
				if (virtual_table->virtual_table[index].symbol == m_object_hash) {
					virtual_table->virtual_table[index].method(object, &current);
					result += current;
					break;
				}
			virtual_table = (struct s_virtual_table *)(((struct s_list_node *)virtual_table)->next);
		}
		object->hash_value = result;
		object->flags |= e_flag_hashed;
	} else
		result = object->hash_value;
	return result;
}

struct s_object *p_object_compare_single(struct s_object *object, struct s_object *other) {
	struct s_virtual_table *virtual_table;
	struct s_object *result = NULL;
	int index;
	virtual_table = (struct s_virtual_table *)object->virtual_tables.head;
	while ((!result) && (virtual_table)) {
		for (index = 0; virtual_table->virtual_table[index].symbol; ++index)
			if (virtual_table->virtual_table[index].symbol == m_object_compare)
				result = virtual_table->virtual_table[index].method(object, other);
		virtual_table = (struct s_virtual_table *)(((struct s_list_node *)virtual_table)->next);
	}
	return result;
}

struct s_object *f_object_compare(struct s_object *object, struct s_object *other) {
	struct s_object *result = NULL;
	if (!(result = p_object_compare_single(object, other)))
		result = p_object_compare_single(other, object);
	return result;
}

