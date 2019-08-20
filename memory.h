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
#include "log.h"
#include "logs.h"
#ifdef d_miranda_debug
#define d_malloc(siz) p_malloc((siz),__FILE__,__LINE__)
#define d_malloc_explicit(siz, f, l) p_malloc((siz),(f),(l))
#define d_realloc(ptr, siz) p_realloc((ptr),(siz),__FILE__,__LINE__)
#define d_free(ptr) p_free((ptr),__FILE__,__LINE__)
#else
#define d_malloc(siz) calloc(1,(siz))
#define d_malloc_explicit(siz, f, l) calloc(1,(siz))
#define d_realloc(p,siz) ((p)?realloc((p),(siz)):calloc(1,(siz)))
#define d_free(ptr) free(ptr)
#endif
#define d_memory_checksum 0xfacefeed
#define d_explicit_result __attribute__ ((warn_unused_result))
#define d_explicit_malloc d_explicit_result __attribute__ ((malloc))
typedef struct s_memory_tail {
  unsigned int checksum, line;
  const char *file;
} s_memory_tail;
typedef struct s_memory_head {
  struct s_memory_head *next, *back;
  size_t dimension;
  unsigned int checksum;
} s_memory_head;
extern struct s_memory_head *v_memory_root;
extern void f_memory_destroy(void);
extern __attribute__ ((warn_unused_result)) __attribute__ ((malloc)) void *p_malloc(size_t dimension, const char *file, unsigned int line);
extern __attribute__ ((warn_unused_result)) void *p_realloc(void *pointer, size_t dimension, const char *file, unsigned int line);
extern void p_free(void *pointer, const char *file, unsigned int line);
#endif

