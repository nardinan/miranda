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
#include "memory.h"
struct s_list *v_memory = NULL;
void f_memory_init (void) {
	f_list_init(&v_memory);
}

void f_memory_destroy(void) {
	struct s_memory_node *node;
	struct s_memory_tail *tail;
	struct s_memory_head *head;
	if (v_memory) {
		d_foreach(v_memory, node, struct s_memory_node) {
			head = (struct s_memory_head *)(node->pointer-sizeof(struct s_memory_head));
			tail = (struct s_memory_tail *)(node->pointer+head->dimension);
			d_log(e_log_level_ever, "pointer %p (%hu bytes) is still here (allocated in %s::%d) [0x%x-0x%x]", node->pointer, head->dimension,
					tail->file, tail->line, head->checksum, tail->checksum);
			free((void *)(node->pointer-sizeof(struct s_memory_head)));
		}
		f_list_destroy(&v_memory);
	}
}

void *p_malloc(size_t dimension, const char *file, unsigned int line) {
	struct s_memory_node *node;
	struct s_memory_tail *tail;
	struct s_memory_head *head;
	size_t total_dimension = sizeof(struct s_memory_head)+dimension+sizeof(struct s_memory_tail);
	void *pointer;
	if ((node = (struct s_memory_node *) malloc(sizeof(struct s_memory_node))) && (pointer = (void *) malloc(total_dimension))) {
		memset(pointer, 0, total_dimension);
		f_list_append(v_memory, (struct s_list_node *)node, e_list_insert_head);
		head = (struct s_memory_head *)(pointer);
		head->dimension = dimension;
		head->checksum = (unsigned int)d_memory_checksum;
		node->pointer = pointer+sizeof(struct s_memory_head);
		tail = (struct s_memory_tail *)(node->pointer+dimension);
		tail->checksum = (unsigned int)d_memory_checksum;
		tail->line = line;
		tail->file = file;
		tail->node = node;
	} else
		d_die(d_error_malloc);
	return node->pointer;
}

void *p_realloc(void *pointer, size_t dimension, const char *file, unsigned int line) {
	struct s_memory_head *head;
	void *backup_pointer;
	size_t minimum_dimension;
	if ((backup_pointer = p_malloc(dimension, file, line))) {
		if ((pointer) && (head = (struct s_memory_head *)(pointer-sizeof(struct s_memory_head)))) {
			minimum_dimension = (dimension>head->dimension)?head->dimension:dimension;
			memcpy(backup_pointer, pointer, minimum_dimension);
			p_free(pointer, file, line);
		}
	} else
		d_die(d_error_malloc);
	return backup_pointer;
}

void p_free(void *pointer, const char *file, unsigned int line) {
	struct s_memory_node *node;
	struct s_memory_tail *tail;
	struct s_memory_head *head;
	int founded = d_false;
	d_foreach(v_memory, node, struct s_memory_node)
		if (node->pointer == pointer) {
			head = (struct s_memory_head *)(node->pointer-sizeof(struct s_memory_head));
			tail = (struct s_memory_tail *)(node->pointer+head->dimension);
			if (tail->checksum == (unsigned int)d_memory_checksum) {
				f_list_delete(v_memory, (struct s_list_node *)node);
				free((void *)(pointer-sizeof(struct s_memory_head)));
			} else
				d_err(e_log_level_ever, "wrong checksum with %p (%s::%d)", pointer, file, line);
			founded = d_true;
			break;
		}
	if (!founded)
		d_err(e_log_level_ever, "double free with %p (%s::%d)", pointer, file, line);
}

