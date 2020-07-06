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
#ifndef miranda_entity_h
#define miranda_entity_h
#include "drawable.obj.h"
#include <sys/time.h>
#define d_entity_label_size 64
#define d_entity_minimum_movement 0.1
#define d_entity_minimum_zoom 0.01
typedef t_boolean(*t_entity_validator)(struct s_object *self, double current_x, double current_y, double current_zoom, double *new_x, 
    double *new_y, double *new_zoom);
typedef enum e_entity_locks {
  e_entity_lock_y_bottom = 0x01,
  e_entity_lock_y_top    = 0x02,
  e_entity_lock_y_center = 0x04,
  e_entity_lock_x_left   = 0x08,
  e_entity_lock_x_right  = 0x10,
  e_entity_lock_x_center = 0x20
} e_entity_locks;
typedef enum e_entity_alignments {
  e_entity_alignment_centered = e_drawable_alignment_centered,
  e_entity_alignment_top_left = e_drawable_alignment_top_left,
  e_entity_alignment_top_right = e_drawable_alignment_top_right,
  e_entity_alignment_bottom_left = e_drawable_alignment_bottom_left,
  e_entity_alignment_bottom_right = e_drawable_alignment_bottom_right,
  e_entity_alignment_inherited 
} e_entity_alignments;
typedef struct s_entity_element {
  d_list_node_head;
  char label[d_string_buffer_size];
  double offset_x, offset_y, offset_angle, offset_zoom;
  enum e_entity_alignments alignment;
  struct s_object *drawable;
} s_entity_element;
typedef struct s_entity_component {
  d_list_node_head;
  char label[d_string_buffer_size];
  double speed_x, speed_y, speed_z, offset_point_x, offset_point_y;
  struct s_list elements;
} s_entity_component;
d_declare_class(entity) {
  struct s_attributes head;
  char label[d_string_buffer_size];
  struct s_list components;
  struct s_entity_component *current_component;
  struct timeval last_refresh_x, last_refresh_y, last_refresh_zoom;
  unsigned int lock_rules;
  double factor_z;
  t_entity_validator validator;
} d_declare_class_tail(entity);
struct s_entity_attributes *p_entity_alloc(struct s_object *self);
extern struct s_object *f_entity_new(struct s_object *self, const char *key, t_entity_validator validator);
d_declare_method(entity, add_component)(struct s_object *self, char *label, double speed_x, double speed_y, double speed_z, double offset_point_x,
    double offset_point_y);
d_declare_method(entity, get_component)(struct s_object *self, char *label);
d_declare_method(entity, get_current_component)(struct s_object *self);
d_declare_method(entity, add_element)(struct s_object *self, char *label_component, char *label_element, double offset_x, double offset_y, 
    double offset_angle, double offset_zoom, struct s_object *drawable, enum e_entity_alignments alignment);
d_declare_method(entity, get_element)(struct s_object *self, char *label_component, char *label_element);
d_declare_method(entity, get_current_element)(struct s_object *self, char *label);
d_declare_method(entity, set_component)(struct s_object *self, char *label);
d_declare_method(entity, set_lock)(struct s_object *self, unsigned int lock_rules);
d_declare_method(entity, collision)(struct s_object *self, struct s_object *entity);
d_declare_method(entity, interact)(struct s_object *self, struct s_object *entity);
d_declare_method(entity, draw)(struct s_object *self, struct s_object *environment);
d_declare_method(entity, set_maskRGB)(struct s_object *self, unsigned int red, unsigned int green, unsigned int blue);
d_declare_method(entity, set_maskA)(struct s_object *self, unsigned int alpha);
d_declare_method(entity, set_blend)(struct s_object *self, enum e_drawable_blends blend);
d_declare_method(entity, set_mirrored)(struct s_object *self, enum e_drawable_flips flip);
d_declare_method(entity, set_mirrored_maskRGB)(struct s_object *self, unsigned int red, unsigned int green, unsigned int blue);
d_declare_method(entity, set_mirrored_maskA)(struct s_object *self, unsigned int alpha);
d_declare_method(entity, set_mirrored_position)(struct s_object *self, double x, double y);
d_declare_method(entity, delete)(struct s_object *self, struct s_entity_attributes *attributes);
#endif
