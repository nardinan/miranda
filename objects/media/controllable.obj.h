/*
 * miranda
 * Copyright (C) 2016 Andrea Nardinocchi (andrea@nardinan.it)
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
#ifndef miranda_media_controllable_h
#define miranda_media_controllable_h
#include "eventable.obj.h"
struct s_controllable_entry;
typedef void *(*t_controllable_action)(struct s_object *, struct s_controllable_entry *, t_boolean);
typedef struct s_controllable_entry { d_list_node_head;
   int key;
   t_boolean enabled;
   t_controllable_action action_pressed, action_released;
} s_controllable_entry;
d_declare_class(controllable) {
    struct s_attributes head;
    struct s_list configurations;
    t_boolean enable;
} d_declare_class_tail(controllable);
struct s_controllable_attributes *p_controllable_alloc(struct s_object *self);
extern struct s_object *f_controllable_new(struct s_object *self);
d_declare_method(controllable, add_configuration)(struct s_object *self, int key, t_controllable_action action_pressed, t_controllable_action action_released);
d_declare_method(controllable, get_configuration)(struct s_object *self, int key);
d_declare_method(controllable, del_configuration)(struct s_object *self, int key);
d_declare_method(controllable, event)(struct s_object *self, struct s_object *environment, SDL_Event *current_event);
d_declare_method(controllable, delete)(struct s_object *self, struct s_controllable_attributes *attributes);
#endif
