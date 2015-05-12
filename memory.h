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
#ifndef miranda_memory_h
#define miranda_memory_h
#include "types.h"
#include "list.h"
#define d_memory_checksum 0xfacefeed
#define d_malloc(siz) p_malloc((siz),__FILE__,__LINE__)
#define d_realloc(ptr,siz) p_realloc((ptr),(siz),__FILE__,__LINE__)
#define d_free(ptr) p_free((ptr),__FILE__,__LINE__)
typedef struct s_memory_tail {
	unsigned int checksum, line;
	const char *file;
	struct s_memory_node *node;
} s_memory_tail;
typedef struct s_memory_head {
	size_t dimension;
	unsigned int checksum;
} s_memory_head;
typedef struct s_memory_node {
	d_list_node_head;
	void *pointer;
} s_memory_node;
extern struct s_list *v_memory;
extern void f_memory_init(void);
extern void f_memory_destroy(void);
extern __attribute__ ((warn_unused_result)) __attribute__ ((malloc)) void *p_malloc(size_t dimension, const char *file, unsigned int line);
extern __attribute__ ((warn_unused_result)) void *p_realloc(void *pointer, size_t dimension, const char *file, unsigned int line);
extern void p_free(void *pointer, const char *file, unsigned int line);
#endif

