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
#ifndef miranda_object_memory_h
#define miranda_object_memory_h
#include "object.h"
d_declare_class(memory) {
  struct s_attributes head;
  int references;
} d_declare_class_tail(memory);
extern struct s_object *f_memory_new(struct s_object *self);
#define d_residual_references(mem) ((d_cast((mem),memory))->references)
#define d_retain(mem) ((mem)?d_call((mem),m_memory_retain,NULL):NULL)
d_declare_method(memory, retain)(struct s_object *self);
#define d_delete(mem) \
  do{\
    if(!d_call((mem),m_memory_release,NULL))\
      f_object_delete(mem,__FILE__,__LINE__);\
  }while(0)
d_declare_method(memory, release)(struct s_object *self);
#endif

