/*
 * miranda
 * Copyright (C) 2015 Andrea Nardinocchi (andrea@nardinan.it)
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
#ifndef miranda_object_emitter_h
#define miranda_object_emitter_h
#include "object.h"
#define d_emitter_name_size 16
typedef void *(*t_emitter_action)(struct s_object *, void **, size_t);
typedef struct s_signal {
  d_list_node_head;
  char id[d_emitter_name_size];
  void **parameters;
  size_t parameters_size;
  t_emitter_action action;
} s_signal;
d_declare_class(emitter) {
  struct s_attributes head;
  struct s_list signals;
} d_declare_class_tail(emitter);
extern struct s_object *f_emitter_new(struct s_object *self);
d_declare_method(emitter, record)(struct s_object *self, const char *id);
d_declare_method(emitter, get)(struct s_object *self, const char *id);
d_declare_method(emitter, embed_parameter)(struct s_object *self, const char *id, void *parameter);
d_declare_method(emitter, embed_function)(struct s_object *self, const char *id, t_emitter_action action);
d_declare_method(emitter, raise)(struct s_object *self, const char *id);
d_declare_method(emitter, delete)(struct s_object *self, struct s_emitter_attributes *attributes);
#endif

