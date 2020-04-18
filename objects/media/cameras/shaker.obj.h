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
#ifndef miranda_shaker_h
#define miranda_shaker_h
#include "../camera_controller.obj.h"
#define d_shaker_default_maximum_translation 5
#define d_shaker_default_maximum_angle 2
d_declare_class(shaker) {
  struct s_attributes head;
  double trauma_level, maximum_translation_x, maximum_translation_y, maximum_angle;
} d_declare_class_tail(shaker);
struct s_shaker_attributes *p_shaker_alloc(struct s_object *self, unsigned short int affected_axis);
extern struct s_object *f_shaker_new(struct s_object *self, unsigned short int affected_axis);
d_declare_method(shaker, set_maximum_translation_x)(struct s_object *self, double translation_x);
d_declare_method(shaker, set_maximum_translation_y)(struct s_object *self, double translation_y);
d_declare_method(shaker, set_maximum_angle)(struct s_object *self, double angle);
d_declare_method(shaker, set_trauma_level)(struct s_object *self, double trauma_level);
d_declare_method(shaker, update)(struct s_object *self, double *screen_position_x, double *screen_position_y, double *screen_w, double *screen_h,
    double *scene_reference_w, double *scene_reference_h, double *scene_offset_x, double *scene_offset_y, double *scene_center_x, double *scene_center_y,
    double *camera_angle, double *scene_zoom);
#endif
