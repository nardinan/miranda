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
#ifndef miranda_object_point_h
#define miranda_object_point_h
#include "../memory.obj.h"
#include "../mutex.obj.h"
#include "../../math.local.h"
#define d_point_precision_default 5
#define d_point_square_distance(x1, y1, x2, y2) ((((x1)-(x2))*((x1)-(x2)))+(((y1)-(y2))*((y1)-(y2))))
d_declare_class(point) {
  struct s_attributes head;
  double x, y;
} d_declare_class_tail(point);
struct s_point_attributes *p_point_alloc(struct s_object *self);
extern struct s_object *f_point_new(struct s_object *self, double x, double y);
d_declare_method(point, set_point)(struct s_object *self, struct s_object *source);
d_declare_method(point, set_x)(struct s_object *self, double x);
d_declare_method(point, set_y)(struct s_object *self, double y);
d_declare_method(point, get)(struct s_object *self, double *x, double *y);
d_declare_method(point, add)(struct s_object *self, double x, double y);
d_declare_method(point, subtract)(struct s_object *self, double x, double y);
d_declare_method(point, distance)(struct s_object *self, struct s_object *other, double *distance, double *distance_square);
d_declare_method(point, angle)(struct s_object *self, struct s_object *other, double *tilt_angle);
d_declare_method(point, rotate)(struct s_object *self, double angle);
d_declare_method(point, rotate_pivot)(struct s_object *self, double angle, struct s_object *pivot);
#endif

