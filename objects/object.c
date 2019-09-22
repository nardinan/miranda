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
#include "object.h"
const char v_undefined_type[] = "undefined";
const char m_object_delete[] = "delete";
const char m_object_hash[] = "hash";
const char m_object_compare[] = "compare";
struct s_memory_buckets *v_memory_bucket = NULL;
d_exception_define(undefined_method, 1, "undefined method exception");
d_exception_define(private_method, 2, "private method exception");
const struct s_method *p_object_recall(const char *file, int line, struct s_object *object, const char *symbol, const char *type) {
  struct s_virtual_table *singleton;
  char buffer[d_string_buffer_size];
  struct s_method_cache swap_cache;
  const struct s_method *result = NULL;
  int index;
  pthread_mutex_lock(&(object->lock));
  {
    if ((object->cache_calls.first.type == type) && (object->cache_calls.first.entry->symbol == symbol))
      result = object->cache_calls.first.entry;
    else if ((object->cache_calls.second.type == type) && (object->cache_calls.second.entry->symbol == symbol)) {
      swap_cache = object->cache_calls.first;
      object->cache_calls.first = object->cache_calls.second;
      object->cache_calls.second = swap_cache;
      result = object->cache_calls.first.entry;
    } else {
      d_reverse_foreach(&(object->virtual_tables), singleton, struct s_virtual_table) {
        if ((type == v_undefined_type) || (type == singleton->type))
          for (index = 0; singleton->virtual_table[index].symbol; ++index)
            if (singleton->virtual_table[index].symbol == symbol) {
              object->cache_calls.second = object->cache_calls.first;
              object->cache_calls.first.type = type;
              object->cache_calls.first.entry = &(singleton->virtual_table[index]);
              result = object->cache_calls.first.entry;
            }
        if (result)
          break;
      }
    }
  }
  pthread_mutex_unlock(&(object->lock));
  if (result) {
    if ((result->flag == e_flag_private) && (result->file != file)) {
      snprintf(buffer, d_string_buffer_size, "method '%s' is private in object %s and you are out of context (%s, %d)", symbol, object->type, file, line);
      d_throw(v_exception_private_method, buffer);
    }
  } else {
    snprintf(buffer, d_string_buffer_size, "symbol '%s' is undefined in object %s or it is a member of another class (%s, %d)", symbol,
      object->type, file, line);
    d_throw(v_exception_undefined_method, buffer);
  }
  return result;
}
struct s_object *p_object_prepare(struct s_object *provided, const char *file, int line, const char *type, int flags) {
  pthread_mutexattr_t attributes_mutex;
  if ((flags & e_flag_recovered) != e_flag_recovered) {
    memset(provided, 0, sizeof(struct s_object));
    pthread_mutexattr_init(&attributes_mutex);
    pthread_mutexattr_settype(&attributes_mutex, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&(provided->lock), &attributes_mutex);
    pthread_mutexattr_destroy(&attributes_mutex);
  }
  provided->type = type;
  provided->file = file;
  provided->line = line;
  provided->flags = flags;
  return provided;
}
struct s_object *p_object_malloc(const char *file, int line, const char *type, int flags) {
  struct s_object *result = NULL;
  if (v_memory_bucket)
    result = f_memory_bucket_query(v_memory_bucket, type);
  if (result)
    p_object_prepare(result, file, line, type, (flags | e_flag_allocated | e_flag_recovered));
  else if ((result = d_malloc_explicit(sizeof(struct s_object), file, line)))
    p_object_prepare(result, file, line, type, (flags | e_flag_allocated));
  else
    d_die(d_error_malloc);
  return result;
}
struct s_attributes *p_object_setup(struct s_object *object, struct s_method *virtual_table, size_t attributes_size, const char *attributes_type) {
  struct s_virtual_table *node;
  struct s_attributes *attributes = NULL;
  if ((object->flags & e_flag_recovered) != e_flag_recovered) {
    if ((attributes = (struct s_attributes *)d_malloc(attributes_size))) {
      attributes->type = attributes_type;
      if ((node = (struct s_virtual_table *)d_malloc(sizeof(struct s_virtual_table)))) {
        node->virtual_table = virtual_table;
        node->type = attributes_type;
        f_list_append(&(object->virtual_tables), (struct s_list_node *)node, e_list_insert_head);
        f_list_append(&(object->attributes), (struct s_list_node *)attributes, e_list_insert_head);
      } else
        d_die(d_error_malloc);
    } else
      d_die(d_error_malloc);
  } else
    d_foreach(&(object->attributes), attributes, struct s_attributes)
      if (attributes->type == attributes_type)
        break;
  return attributes;
}
struct s_attributes *p_object_cast(const char *file, int line, struct s_object *object, const char *type) {
  struct s_attributes *swap_cache, *result = NULL;
  pthread_mutex_lock(&(object->lock));
  {
    if ((object->cache_attributes.first) && (object->cache_attributes.first->type == type))
      result = object->cache_attributes.first;
    else if ((object->cache_attributes.second) && (object->cache_attributes.second->type == type)) {
      swap_cache = object->cache_attributes.second;
      object->cache_attributes.second = object->cache_attributes.first;
      object->cache_attributes.first = swap_cache;
      result = object->cache_attributes.first;
    } else {
      d_reverse_foreach(&(object->attributes), result, struct s_attributes)
        if (result->type == type) {
          object->cache_attributes.second = object->cache_attributes.first;
          object->cache_attributes.first = result;
          break;
        }
    }
  }
  pthread_mutex_unlock(&(object->lock));
  return result;
}
void p_object_residual_delete(struct s_object *object) {
  struct s_attributes *attributes;
  struct s_virtual_table *virtual_table;
  while ((attributes = (struct s_attributes *)object->attributes.tail) && (virtual_table = (struct s_virtual_table *)object->virtual_tables.tail)) {
    f_list_delete(&(object->attributes), (struct s_list_node *)attributes);
    f_list_delete(&(object->virtual_tables), (struct s_list_node *)virtual_table);
    d_free(attributes);
    d_free(virtual_table);
  }
  pthread_mutex_destroy(&(object->lock));
  if ((object->flags & e_flag_allocated) == e_flag_allocated)
    d_free(object);
}
void f_object_delete(struct s_object *object) {
  struct s_object *destroyable = object;
  struct s_attributes *attributes = (struct s_attributes *)object->attributes.head;
  struct s_virtual_table *virtual_table = (struct s_virtual_table *)object->virtual_tables.head;
  int index;
  while ((attributes) && (virtual_table)) {
    for (index = 0; virtual_table->virtual_table[index].symbol; ++index)
      if (virtual_table->virtual_table[index].symbol == m_object_delete) {
        virtual_table->virtual_table[index].method(object, attributes);
        break;
      }
    attributes = (struct s_attributes *)(((struct s_list_node *)attributes)->next);
    virtual_table = (struct s_virtual_table *)(((struct s_list_node *)virtual_table)->next);
  }
  if (!v_memory_bucket)
    f_memory_bucket_init(&v_memory_bucket, (t_memory_bucket_delete *)&p_object_residual_delete);
  if (((object->flags & e_flag_allocated) != e_flag_allocated) || ((destroyable = f_memory_bucket_push(v_memory_bucket, object->type, object))))
    p_object_residual_delete(destroyable);
}
t_hash_value f_object_hash(struct s_object *object) {
  struct s_virtual_table *virtual_table;
  int index;
  t_hash_value result = 0, current;
  pthread_mutex_lock(&(object->lock));
  {
    if ((object->flags & e_flag_hashed) != e_flag_hashed) {
      d_reverse_foreach(&(object->virtual_tables), virtual_table, struct s_virtual_table) {
        for (index = 0; virtual_table->virtual_table[index].symbol; ++index) {
          if (virtual_table->virtual_table[index].symbol == m_object_hash) {
            virtual_table->virtual_table[index].method(object, &current);
            result += current;
            break;
          }
        }
      }
      object->hash_value = result;
      object->flags |= e_flag_hashed;
    } else
      result = object->hash_value;
  }
  pthread_mutex_unlock(&(object->lock));
  return result;
}
struct s_object *p_object_compare_single(struct s_object *object, struct s_object *other) {
  struct s_virtual_table *virtual_table;
  struct s_object *result = NULL;
  int index;
  d_reverse_foreach(&(object->virtual_tables), virtual_table, struct s_virtual_table) {
    for (index = 0; virtual_table->virtual_table[index].symbol; ++index)
      if (virtual_table->virtual_table[index].symbol == m_object_compare)
        result = virtual_table->virtual_table[index].method(object, other);
    if (result)
      break;
  }
  return result;
}
struct s_object *f_object_compare(struct s_object *object, struct s_object *other) {
  struct s_object *result = NULL;
  if (!(result = p_object_compare_single(object, other)))
    result = p_object_compare_single(other, object);
  return result;
}
