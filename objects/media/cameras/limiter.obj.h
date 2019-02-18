/*
 * miranda
 * Copyright (C) 2018 Andrea Nardinocchi (andrea@nardinan.it)
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
#ifndef miranda_limiter_h
#define miranda_limiter_h
#include "../camera_controller.obj.h"
d_declare_class(limiter) {
  struct s_attributes head;
  double low_limit_x, high_limit_x, low_limit_y, high_limit_y, low_limit_z, high_limit_z;
} d_declare_class_tail(limiter);
struct s_limiter_attributes *p_limiter_alloc(struct s_object *self, unsigned short int affected_axis);
extern struct s_object *f_limiter_new(struct s_object *self, unsigned short int affected_axis);
d_declare_method(limiter, set_limit_x)(struct s_object *self, double low_limit_x, double high_limit_x);
d_declare_method(limiter, set_limit_y)(struct s_object *self, double low_limit_y, double high_limit_y);
d_declare_method(limiter, set_limit_z)(struct s_object *self, double low_limit_z, double high_limit_z);
d_declare_method(limiter, update)(struct s_object *self, double *screen_position_x, double *screen_position_y, double *screen_w, double *screen_h,
  double *scene_reference_w, double *scene_reference_h, double *scene_offset_x, double *scene_offset_y, double *scene_center_x, double *scene_center_y,
  double *camera_angle, double *scene_zoom);
#endif