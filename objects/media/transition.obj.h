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
#ifndef miranda_media_transition_h
#define miranda_media_transition_h
#include <sys/time.h>
#include "drawable.obj.h"
#define d_transition_factor_value(start, end, f) (start+((end-start)*f))
typedef enum e_transition_directions {
  e_transition_direction_forward,
  e_transition_direction_rewind,
  e_transition_direction_stop
} e_transition_directions;
typedef struct s_transition_key {
  d_list_node_head;
  double position_x, position_y, zoom, angle, mask_R, mask_G, mask_B, mask_A, time;
} s_transition_key;
d_declare_class(transition) {
  struct s_attributes head;
  struct s_object *drawable;
  struct s_list keys;
  struct s_transition_key *current_key, *next_key;
  struct timeval last_update;
  enum e_transition_directions status;
  double time_ratio;
} d_declare_class_tail(transition);
struct s_transition_attributes *p_transition_alloc(struct s_object *self);
extern struct s_object *f_transition_new(struct s_object *self, struct s_object *drawable, double time_ratio);
d_declare_method(transition, append_key)(struct s_object *self, struct s_transition_key transition);
d_declare_method(transition, set_status)(struct s_object *self, enum e_transition_directions status);
d_declare_method(transition, get_status)(struct s_object *self);
d_declare_method(transition, draw)(struct s_object *self, struct s_object *environment);
d_declare_method(transition, set_maskRGB)(struct s_object *self, unsigned int red, unsigned int green, unsigned int blue);
d_declare_method(transition, set_maskA)(struct s_object *self, unsigned int alpha);
d_declare_method(transition, set_blend)(struct s_object *self, enum e_drawable_blends blend);
d_declare_method(transition, get_scaled_position)(struct s_object *self, double *x, double *y);
d_declare_method(transition, get_scaled_center)(struct s_object *self, double *x, double *y);
d_declare_method(transition, get_dimension)(struct s_object *self, double *w, double *h);
d_declare_method(transition, get_scaled_dimension)(struct s_object *self, double *w, double *h);
d_declare_method(transition, delete)(struct s_object *self, struct s_transition_attributes *attributes);
#endif
