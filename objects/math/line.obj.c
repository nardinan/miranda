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
#include "line.obj.h"
struct s_line_attributes *p_line_alloc(struct s_object *self) {
	struct s_line_attributes *result = d_prepare(self, line);
	f_memory_new(self);	/* inherit */
	return result;
}

struct s_object *f_line_new(struct s_object *self, float starting_x, float starting_y, float ending_x, float ending_y) {
	struct s_line_attributes *attributes = p_line_alloc(self);
	attributes->starting_x = starting_x;
	attributes->starting_y = starting_y;
	attributes->ending_x = ending_x;
	attributes->ending_y = ending_y;
	return self;
}

struct s_object *f_line_new_points(struct s_object *self, struct s_object *point_starting, struct s_object *point_ending) {
	struct s_point_attributes *starting_attributes = d_cast(point_starting, point), *ending_attributes = d_cast(point_ending, point);
	return f_line_new(self, starting_attributes->x, starting_attributes->y, ending_attributes->x, ending_attributes->y);
}

d_define_method(line, set_starting_x)(struct s_object *self, float starting_x, t_boolean keep_ratio) {
	d_using(line);
	if (keep_ratio)
		line_attributes->ending_x = starting_x + (line_attributes->ending_x-line_attributes->starting_x);
	line_attributes->starting_x = starting_x;
	return self;
}

d_define_method(line, set_starting_y)(struct s_object *self, float starting_y, t_boolean keep_ratio) {
	d_using(line);
	if (keep_ratio)
		line_attributes->ending_y = starting_y + (line_attributes->ending_y-line_attributes->starting_y);
	line_attributes->starting_y = starting_y;
	return self;
}

d_define_method(line, set_ending_x)(struct s_object *self, float ending_x) {
	d_using(line);
	line_attributes->ending_x = ending_x;
	return self;
}

d_define_method(line, set_ending_y)(struct s_object *self, float ending_y) {
	d_using(line);
	line_attributes->ending_y = ending_y;
	return self;
}

d_define_method(line, get)(struct s_object *self, float *starting_x, float *starting_y, float *ending_x, float *ending_y) {
	d_using(line);
	if (starting_x)
		*starting_x = line_attributes->starting_x;
	if (starting_y)
		*starting_y = line_attributes->starting_y;
	if (ending_x)
		*ending_x = line_attributes->ending_x;
	if (ending_y)
		*ending_y = line_attributes->ending_y;
	return self;
}

d_define_method(line, intersect)(struct s_object *self, struct s_object *other) {
	return d_call(self, m_line_intersect_point, other, NULL);
}

d_define_method(line, intersect_point)(struct s_object *self, struct s_object *other, struct s_object *intersection) {
	d_using(line);
	struct s_line_attributes *other_attributes = d_cast(other, line);
	struct s_point_attributes *intersection_attributes;
	float projection_A_ending_x, projection_A_ending_y, projection_B_starting_x, projection_B_starting_y, projection_B_ending_x, projection_B_ending_y,
	      size_A, cos, sin, rotated_B_starting_x, rotated_B_starting_y, rotated_B_ending_x, rotated_B_ending_y, intersection_position;
	t_boolean result = d_false;
	projection_A_ending_x = line_attributes->ending_x - line_attributes->starting_x;
	projection_A_ending_y = line_attributes->ending_y - line_attributes->starting_y;
	projection_B_starting_x = other_attributes->starting_x - line_attributes->starting_x;
	projection_B_starting_y = other_attributes->starting_y - line_attributes->starting_y;
	projection_B_ending_x = other_attributes->ending_x - line_attributes->starting_x;
	projection_B_ending_y = other_attributes->ending_y - line_attributes->starting_y;
	size_A = f_math_sqrt((projection_A_ending_x*projection_A_ending_x) + (projection_B_ending_y*projection_B_ending_y), d_math_default_precision);
	cos = projection_A_ending_x/size_A;
	sin = projection_A_ending_y/size_A;
	rotated_B_starting_x = (projection_B_starting_x * cos) + (projection_B_starting_y * sin);
	rotated_B_starting_y = (projection_B_starting_y * cos) - (projection_B_starting_x * sin);
	rotated_B_ending_x = (projection_B_ending_x * cos) + (projection_B_ending_y * sin);
	rotated_B_ending_y = (projection_B_ending_y * cos) - (projection_B_ending_x * sin);
	if (((rotated_B_starting_y >= 0) && (rotated_B_ending_y < 0)) || ((rotated_B_starting_y < 0) && (rotated_B_ending_y >= 0))) {
		intersection_position = rotated_B_ending_x + (rotated_B_starting_x - rotated_B_ending_x) * rotated_B_ending_y /
			(rotated_B_ending_y - rotated_B_starting_y);
		if ((intersection_position >= 0) && (intersection_position <= size_A)) {
			if (intersection)
				if ((intersection_attributes = d_cast(intersection, point))) {
					intersection_attributes->x = line_attributes->starting_x + (intersection_position * cos);
					intersection_attributes->y = line_attributes->starting_y + (intersection_position * sin);
				}
			result = d_true;
		}
	}
	d_cast_return(result);
}

d_define_class(line) {
	d_hook_method(line, e_flag_public, set_starting_x),
	d_hook_method(line, e_flag_public, set_starting_y),
	d_hook_method(line, e_flag_public, set_ending_x),
	d_hook_method(line, e_flag_public, set_ending_y),
	d_hook_method(line, e_flag_public, get),
	d_hook_method(line, e_flag_public, intersect),
	d_hook_method(line, e_flag_public, intersect_point),
	d_hook_method_tail
};
