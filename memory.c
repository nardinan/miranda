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
struct s_memory_head *v_memory_root;
void f_memory_destroy(void) {
	struct s_memory_tail *tail;
	struct s_memory_head *head;
	while (v_memory_root) {
		head = v_memory_root;
		tail = (struct s_memory_tail *)((void *)v_memory_root + sizeof(struct s_memory_head) + head->dimension);
		d_log(e_log_level_ever, "pointer %p (%hu bytes) is still here (allocated in %s::%d) [0x%x-0x%x]", ((void *)head + sizeof(struct s_memory_head)),
				head->dimension, tail->file, tail->line, head->checksum, tail->checksum);
		v_memory_root = head->next;
		free(head);
	}
}

void *p_malloc(size_t dimension, const char *file, unsigned int line) {
	struct s_memory_tail *tail;
	struct s_memory_head *head;
	size_t total_dimension = sizeof(struct s_memory_head) + dimension + sizeof(struct s_memory_tail);
	void *pointer;
	if ((pointer = (struct s_memory_head *) calloc(1, total_dimension))) {
		memset(pointer, 0, total_dimension);
		head = (struct s_memory_head *)pointer;
		tail = (struct s_memory_tail *)(pointer + sizeof(struct s_memory_head) + dimension);
		pointer = (void *)(pointer + sizeof(struct s_memory_head));
		head->next = v_memory_root;
		v_memory_root = head;
		head->dimension = dimension;
		head->checksum = (unsigned int)d_memory_checksum;
		tail->checksum = (unsigned int)d_memory_checksum;
		tail->line = line;
		tail->file = file;
		d_log(e_log_level_high, "pointer %p (%hu bytes) has been allocated (in %s::%d) [0x%x-0x%x]", pointer, head->dimension, tail->file,
				tail->line, head->checksum, tail->checksum);
	} else
		d_die(d_error_malloc);
	return pointer;
}

void *p_realloc(void *pointer, size_t dimension, const char *file, unsigned int line) {
	struct s_memory_head *head;
	void *backup_pointer;
	if (pointer) {
		head = (struct s_memory_head *)(pointer - sizeof(struct s_memory_head));
		if (head->dimension < dimension) {
			if ((backup_pointer = p_malloc(dimension, file, line)))
				memcpy(backup_pointer, pointer, head->dimension);
			p_free(pointer, file, line);
		} else
			backup_pointer = pointer;
	} else
		backup_pointer = p_malloc(dimension, file, line);
	return backup_pointer;
}

void p_free(void *pointer, const char *file, unsigned int line) {
	struct s_memory_tail *tail;
	struct s_memory_head *head = v_memory_root, *previous = NULL;
	void *other_pointer;
	while (head) {
		other_pointer = ((void *)head + sizeof(struct s_memory_head));
		if (pointer == other_pointer) {
			tail = (struct s_memory_tail *)(pointer + head->dimension);
			if (tail->checksum == (unsigned int)d_memory_checksum) {
				if (previous)
					previous->next = head->next;
				else
					v_memory_root = head->next;
				free(head);
			} else
				d_err(e_log_level_ever, "wrong checksum with %p (%s::%d)", pointer, file, line);
			break;
		}
		previous = head;
		head = head->next;
	}
	if (!head)
		d_err(e_log_level_ever, "double free with %p (%s::%d)", pointer, file, line);
}
