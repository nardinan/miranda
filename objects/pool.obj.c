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
#include "pool.obj.h"
struct s_object *v_default_pool = NULL;
struct s_pool_attributes *p_pool_alloc(struct s_object *self) {
  struct s_pool_attributes *result = d_prepare(self, pool);
  f_memory_new(self);   /* inherit */
  f_mutex_new(self);    /* inherit */
  return result;
}
struct s_object *f_pool_new(struct s_object *self) {
  struct s_pool_attributes *attributes = p_pool_alloc(self);
  f_list_init(&(attributes->pool));
  return self;
}
d_define_method(pool, insert)(struct s_object *self, struct s_object *pointer) {
  d_using(pool);
  f_list_append(pool_attributes->pool, (struct s_list_node *)pointer, e_list_insert_head);
  pointer->flags |= e_flag_pooled;
  return pointer;
}
d_define_method(pool, clean)(struct s_object *self) {
  d_using(pool);
  struct s_object *next, *value = (struct s_object *)pool_attributes->pool->head;
  while (value) {
    next = (struct s_object *)value->head.next;
    f_list_delete(pool_attributes->pool, (struct s_list_node *)value);
    if ((value->flags & e_flag_placeholder) == e_flag_placeholder) {
      d_free(value);
      break;
    } else
      d_delete(value);
    value = next;
  }
  return self;
}
d_define_method(pool, delete)(struct s_object *self, struct s_pool_attributes *attributes) {
  f_list_destroy(&(attributes->pool));
  return NULL;
}
d_define_class(pool) {d_hook_method(pool, e_flag_public, insert),
                      d_hook_method(pool, e_flag_public, clean),
                      d_hook_delete(pool),
                      d_hook_method_tail};
