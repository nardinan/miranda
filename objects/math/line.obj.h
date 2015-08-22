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
#ifndef miranda_object_line_h
#define miranda_object_line_h
#include "point.obj.h"
d_declare_class(line) {
	struct s_attributes head;
	float starting_x, starting_y, ending_x, ending_y;
} d_declare_class_tail(line);
struct s_line_attributes *p_line_alloc(struct s_object *self);
extern struct s_object *f_line_new(struct s_object *self, float starting_x, float starting_y, float ending_x, float ending_y);
extern struct s_object *f_line_new_points(struct s_object *self, struct s_object *point_starting, struct s_object *point_ending);
d_declare_method(line, set_starting_x)(struct s_object *self, float starting_x, t_boolean keep_ratio);
d_declare_method(line, set_starting_y)(struct s_object *self, float starting_y, t_boolean keep_ratio);
d_declare_method(line, set_ending_x)(struct s_object *self, float ending_x);
d_declare_method(line, set_ending_y)(struct s_object *self, float ending_y);
d_declare_method(line, get)(struct s_object *self, float *starting_x, float *starting_y, float *ending_x, float *ending_y);
d_declare_method(line, intersect)(struct s_object *self, struct s_object *other);
d_declare_method(line, intersect_point)(struct s_object *self, struct s_object *other, struct s_object *intersect);
#endif
