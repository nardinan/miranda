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
#include <sys/time.h>
#define d_entity_label_size 64
#define d_entity_minimum_movement 0.1
#define d_entity_minimum_zoom 0.01
typedef t_boolean(*t_entity_validator)(struct s_object *self, double current_x, double current_y, double current_zoom, double *new_x, double *new_y,
        double *new_zoom);
typedef struct s_entity_element { d_list_node_head;
    double offset_x, offset_y;
    struct s_object *drawable;
} s_entity_element;
typedef struct s_entity_component { d_list_node_head;
    char label[d_string_buffer_size];
    double speed_x, speed_y, speed_z;
    struct s_list elements;
} s_entity_component;
d_declare_class(entity) {
    struct s_attributes head;
    char label[d_string_buffer_size];
    struct s_list components;
    struct s_entity_component *current_component;
    struct timeval last_refresh_x, last_refresh_y, last_refresh_zoom;
    double factor_z;
    t_entity_validator validator;
} d_declare_class_tail(entity);
struct s_entity_attributes *p_entity_alloc(struct s_object *self);
extern struct s_object *f_entity_new(struct s_object *self, const char *key, t_entity_validator validator);
d_declare_method(entity, add_component)(struct s_object *self, char *label, double speed_x, double speed_y, double speed_z);
d_declare_method(entity, get_component)(struct s_object *self, char *label);
d_declare_method(entity, add_element)(struct s_object *self, char *label, double offset_x, double offset_y, struct s_object *drawable);
d_declare_method(entity, set_component)(struct s_object *self, char *label);
d_declare_method(entity, collision)(struct s_object *self, struct s_object *entity);
d_declare_method(entity, draw)(struct s_object *self, struct s_object *environment);
d_declare_method(entity, delete)(struct s_object *self, struct s_entity_attributes *attributes);
#endif
