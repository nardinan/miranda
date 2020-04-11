/*
 * miranda
 * Copyright (C) 2014-2020 Andrea Nardinocchi (andrea@nardinan.it)
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
#include <stdio.h>
#include <execinfo.h>
struct s_memory_head *v_memory_root;
pthread_mutex_t v_memory_mutex = PTHREAD_MUTEX_INITIALIZER;
void f_memory_destroy(t_boolean show_only_signed) {
  struct s_memory_tail *tail;
  struct s_memory_head *head;
  unsigned int elements = 0;
  while (v_memory_root) {
    head = v_memory_root;
    tail = (struct s_memory_tail *)((void *)v_memory_root + sizeof(struct s_memory_head) + head->dimension);
    if (head->internal_block == 0) {
      if ((!show_only_signed) || (strlen(head->signature) > 0))
        d_log(e_log_level_high, "pointer %p (%hu bytes) is still here (allocated in %s::%d) [0x%x-0x%x] signed {%s}", 
            ((void *)head + sizeof(struct s_memory_head)), head->dimension, tail->file, tail->line, head->checksum, tail->checksum, 
            head->signature);
      ++elements;
    }
    v_memory_root = head->next;
    free(head);
  }
  if (elements > 0)
    d_log(e_log_level_medium, "%d pointer(s) %s still here", elements, ((elements == 1) ? "was" : "were"));
}
void *p_malloc(size_t dimension, unsigned char internal_block, const char *file, unsigned int line) {
  struct s_memory_tail *tail;
  struct s_memory_head *head;
  size_t total_dimension = sizeof(struct s_memory_head) + dimension + sizeof(struct s_memory_tail);
  void *pointer;
  if ((pointer = (struct s_memory_head *)calloc(1, total_dimension))) {
    head = (struct s_memory_head *)pointer;
    tail = (struct s_memory_tail *)(pointer + sizeof(struct s_memory_head) + dimension);
    pointer = (void *)(pointer + sizeof(struct s_memory_head));
    pthread_mutex_lock(&(v_memory_mutex));
    {
      head->next = v_memory_root;
      if (v_memory_root)
        v_memory_root->back = head;
      v_memory_root = head;
    }
    pthread_mutex_unlock(&(v_memory_mutex));
    head->dimension = dimension;
    head->internal_block = internal_block;
    head->checksum = (unsigned int)d_memory_checksum;
    tail->checksum = (unsigned int)d_memory_checksum;
    tail->line = line;
    tail->file = file;
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
      if ((backup_pointer = p_malloc(dimension, 0, file, line)))
        memcpy(backup_pointer, pointer, head->dimension);
      p_free(pointer, file, line);
    } else
      backup_pointer = pointer;
  } else
    backup_pointer = p_malloc(dimension, 0, file, line);
  return backup_pointer;
}
void p_free(void *pointer, const char *file, unsigned int line) {
  struct s_memory_head *head = (struct s_memory_head *)(pointer - sizeof(struct s_memory_head));
  struct s_memory_tail *tail;
  if (head->checksum == (unsigned int)d_memory_checksum) {
    tail = (struct s_memory_tail *)(pointer + head->dimension);
    if (tail->checksum == (unsigned int)d_memory_checksum) {
      pthread_mutex_lock(&(v_memory_mutex));
      {
        if (head->next)
          head->next->back = head->back;
        if (head->back)
          head->back->next = head->next;
        else
          v_memory_root = head->next;
      }
      pthread_mutex_unlock(&(v_memory_mutex));
      head->back = NULL;
      head->next = NULL;
      free(head);
    } else
      d_err(e_log_level_ever, "wrong tail checksum with %p (%s::%d) signature {%s}", pointer, file, line, 
          head->signature);
  } else
    d_err(e_log_level_ever, "wrong head checksum with %p (%s::%d) signature {%s}", pointer, file, line,
        head->signature);
}
void p_set_signature(void *pointer, const char *signature, const char *file, unsigned int line) {
  struct s_memory_head *head = (struct s_memory_head *)(pointer - sizeof(struct s_memory_head));
  size_t signature_length;
  const char *pointer_signature = signature;
  if (signature) {
    if ((signature_length = strlen(signature)) > d_memory_signature_size)
      pointer_signature = (signature + (signature_length - d_memory_signature_size) + 1);
    strncpy(head->signature, pointer_signature, (d_memory_signature_size - 1));
    head->signature[d_memory_signature_size - 1] = 0;
  } else
    memset(head->signature, 0, d_memory_signature_size - 1);
}

