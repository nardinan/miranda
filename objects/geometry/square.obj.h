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
#ifndef miranda_object_square_h
#define miranda_object_square_h
#include "line.obj.h"
d_declare_class(square) {
  struct s_attributes head;
  double top_left_x, top_left_y, bottom_right_x, bottom_right_y, center_x, center_y, angle, normalized_top_left_x, normalized_top_left_y,
         normalized_top_right_x, normalized_top_right_y, normalized_bottom_left_x, normalized_bottom_left_y, normalized_bottom_right_x, normalized_bottom_right_y;
  t_boolean normalized;
} d_declare_class_tail(square);
struct s_square_attributes *p_square_alloc(struct s_object *self);
extern struct s_object *f_square_new(struct s_object *self, double top_left_x, double top_left_y, double bottom_right_x, double bottom_right_y);
extern struct s_object *f_square_new_points(struct s_object *self, struct s_object *point_top_left, struct s_object *point_bottom_right);
d_declare_method(square, set_square)(struct s_object *self, struct s_object *source);
d_declare_method(square, set_top_left)(struct s_object *self, double top_left_x, double top_left_y);
d_declare_method(square, set_bottom_right)(struct s_object *self, double bottom_right_x, double bottom_right_y);
d_declare_method(square, set_angle)(struct s_object *self, double angle);
d_declare_method(square, set_center)(struct s_object *self, double center_x, double center_y);
d_declare_method(square, normalize_coordinate)(struct s_object *self, double x, double y, double normalized_center_x, double normalized_center_y,
    double sin_radians, double cos_radians, double *normalized_x, double *normalized_y);
d_declare_method(square, normalize)(struct s_object *self);
d_declare_method(square, get_normalized_coordinates)(struct s_object *self, double *top_left_x, double *top_left_y,
    double *bottom_right_x, double *bottom_right_y);
d_declare_method(square, inside)(struct s_object *self, struct s_object *point);
d_declare_method(square, inside_coordinates)(struct s_object *self, double x, double y);
d_declare_method(square, intersect_coordinates)(struct s_object *self, double starting_x_B, double starting_y_B, double ending_x_B, double ending_y_B,
    double *collision_x, double *collision_y);
d_declare_method(square, collision)(struct s_object *self, struct s_object *other);
#endif

