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
#ifndef miranda_entity_h
#define miranda_entity_h
#include "drawable.obj.h"
#define d_entity_label_size 64
typedef struct s_entity_element { d_list_node_head;
    double offset_x, offset_y;
    struct s_object *drawable;
} s_entity_element;
typedef struct s_entity_component { d_list_node_head;
    char label[d_string_buffer_size];
    struct s_list elements;
} s_entity_component;
d_declare_class(entity) {
    struct s_attributes head;
    char label[d_string_buffer_size];
    struct s_list components;
    struct s_entity_component *current_component;
} d_declare_class_tail(entity);
struct s_entity_attributes *p_entity_alloc(struct s_object *self);
extern struct s_object *f_entity_new(struct s_object *self, const char *key);
d_declare_method(entity, add_component)(struct s_object *self, char *label);
d_declare_method(entity, get_component)(struct s_object *self, char *label);
d_declare_method(entity, add_element)(struct s_object *self, char *label, double offset_x, double offset_y, struct s_object *drawable);
d_declare_method(entity, set_component)(struct s_object *self, char *label);
d_declare_method(entity, collision)(struct s_object *self, struct s_object *entity);
d_declare_method(entity, draw)(struct s_object *self, struct s_object *environment);
d_declare_method(entity, delete)(struct s_object *self, struct s_entity_attributes *attributes);
#endif
