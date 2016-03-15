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
#include "square.obj.h"
#include <math.h>
struct s_square_attributes *p_square_alloc(struct s_object *self) {
	struct s_square_attributes *result = d_prepare(self, square);
	f_memory_new(self);	/* inherit */
	return result;
}

struct s_object *f_square_new(struct s_object *self, double top_left_x, double top_left_y, double bottom_right_x, double bottom_right_y) {
	struct s_square_attributes *attributes = p_square_alloc(self);
	attributes->top_left_x = top_left_x;
	attributes->top_left_y = top_left_y;
	attributes->bottom_right_x = bottom_right_x;
	attributes->bottom_right_y = bottom_right_y;
	return self;
}

struct s_object *f_square_new_points(struct s_object *self, struct s_object *point_top_left, struct s_object *point_bottom_right) {
	struct s_point_attributes *top_left_attributes = d_cast(point_top_left, point), *bottom_right_attributes = d_cast(point_bottom_right, point);
	return f_square_new(self, top_left_attributes->x, top_left_attributes->y, bottom_right_attributes->x, bottom_right_attributes->y);
}

d_define_method(square, set_top_left_x)(struct s_object *self, double top_left_x, t_boolean keep_ratio) {
	d_using(square);
	if (keep_ratio)
		square_attributes->bottom_right_x = top_left_x + (square_attributes->bottom_right_x-square_attributes->top_left_x);
	square_attributes->top_left_x = top_left_x;
	square_attributes->normalized = d_false;
	return self;
}

d_define_method(square, set_top_left_y)(struct s_object *self, double top_left_y, t_boolean keep_ratio) {
	d_using(square);
	if (keep_ratio)
		square_attributes->bottom_right_y = top_left_y + (square_attributes->bottom_right_y-square_attributes->top_left_y);
	square_attributes->top_left_y = top_left_y;
	square_attributes->normalized = d_false;
	return self;
}

d_define_method(square, set_bottom_right_x)(struct s_object *self, double bottom_right_x) {
	d_using(square);
	square_attributes->bottom_right_x = bottom_right_x;
	square_attributes->normalized = d_false;
	return self;
}

d_define_method(square, set_bottom_right_y)(struct s_object *self, double bottom_right_y) {
	d_using(square);
	square_attributes->bottom_right_y = bottom_right_y;
	square_attributes->normalized = d_false;
	return self;
}

d_define_method(square, set_angle)(struct s_object *self, double angle) {
	d_using(square);
	square_attributes->angle = angle;
	square_attributes->normalized = d_false;
	return self;
}

d_define_method(square, set_center)(struct s_object *self, double center_x, double center_y) {
	d_using(square);
	square_attributes->center_x = center_x;
	square_attributes->center_y = center_y;
	square_attributes->normalized = d_false;
	return self;
}

d_define_method(square, normalize_coordinate)(struct s_object *self, double x, double y, double normalized_center_x, double normalized_center_y,
		double radians, double *normalized_x, double *normalized_y) {
	double support_x, support_y;
	support_x = x - normalized_center_x;
	support_y = y - normalized_center_y;
	*normalized_x = (support_x * cos(radians)) - (support_y * sin(radians));
	*normalized_y = (support_x * sin(radians)) + (support_y * cos(radians));
	*normalized_x += normalized_center_x;
	*normalized_y += normalized_center_y;
	return self;
}

d_define_method(square, normalize)(struct s_object *self) {
	d_using(square);
	double radians = (square_attributes->angle * d_math_pi)/180.0, normalized_center_x, normalized_center_y;
	if (!square_attributes->normalized) {
		normalized_center_x = square_attributes->top_left_x + square_attributes->center_x;
		normalized_center_y = square_attributes->top_left_y + square_attributes->center_y;
		d_call(self, m_square_normalize_coordinate, square_attributes->top_left_x, square_attributes->top_left_y, normalized_center_x,
				normalized_center_y, radians, &(square_attributes->normalized_top_left_x), &(square_attributes->normalized_top_left_y));
		d_call(self, m_square_normalize_coordinate, square_attributes->bottom_right_x, square_attributes->top_left_y, normalized_center_x,
				normalized_center_y, radians, &(square_attributes->normalized_top_right_x), &(square_attributes->normalized_top_right_y));
		d_call(self, m_square_normalize_coordinate, square_attributes->top_left_x, square_attributes->bottom_right_y, normalized_center_x,
				normalized_center_y, radians, &(square_attributes->normalized_bottom_left_x), &(square_attributes->normalized_bottom_left_y));
		d_call(self, m_square_normalize_coordinate, square_attributes->bottom_right_x, square_attributes->bottom_right_y, normalized_center_x,
				normalized_center_y, radians, &(square_attributes->normalized_bottom_right_x), &(square_attributes->normalized_bottom_right_y));
		square_attributes->normalized = d_true;
	}
	return self;
}

d_define_method(square, inside)(struct s_object *self, struct s_object *point) {
	struct s_point_attributes *point_attributes = d_cast(point, point);
	return d_call(self, m_square_inside_coordinates, point_attributes->x, point_attributes->y);
}

d_define_method(square, inside_coordinates)(struct s_object *self, double x, double y) {
	d_using(square);
	double radians = ((360.0 - square_attributes->angle) * d_math_pi)/180.0, normalized_center_x, normalized_center_y, normalized_x, normalized_y;
	t_boolean result = d_false;
	d_call(self, m_square_normalize, NULL);
	normalized_center_x = square_attributes->top_left_x + square_attributes->center_x;
	normalized_center_y = square_attributes->top_left_y + square_attributes->center_y;
	d_call(self, m_square_normalize_coordinate, x, y, normalized_center_x, normalized_center_y, radians, &normalized_x, &normalized_y);
	if (((normalized_x >= square_attributes->top_left_x) && (normalized_x <= square_attributes->bottom_right_x)) &&
			((normalized_y >= square_attributes->top_left_y) && (normalized_y <= square_attributes->bottom_right_y)))
		result = d_true;
	d_cast_return(result);
}

d_define_method(square, collision)(struct s_object *self, struct s_object *other) {
	d_using(square);
	struct s_square_attributes *square_attributes_other = d_cast(other, square);
	t_boolean result = d_false;
	d_call(self, m_square_normalize, NULL);
	d_call(other, m_square_normalize, NULL);
	if (!(result = (intptr_t)p_square_inside_coordinates(self, square_attributes_other->normalized_top_left_x,
					square_attributes_other->normalized_top_left_y)))
		if (!(result = (intptr_t)p_square_inside_coordinates(self, square_attributes_other->normalized_top_right_x,
						square_attributes_other->normalized_top_right_y)))
			if (!(result = (intptr_t)p_square_inside_coordinates(self, square_attributes_other->normalized_bottom_right_x,
							square_attributes_other->normalized_bottom_right_y)))
				if (!(result = (intptr_t)p_square_inside_coordinates(self, square_attributes_other->normalized_bottom_left_x,
								square_attributes_other->normalized_bottom_left_y)))
					if (!(result = (intptr_t)p_square_inside_coordinates(other, square_attributes->normalized_top_left_x,
									square_attributes->normalized_top_left_y)))
						if (!(result = (intptr_t)p_square_inside_coordinates(other, square_attributes->normalized_top_right_x,
										square_attributes->normalized_top_right_y)))
							if (!(result = (intptr_t)p_square_inside_coordinates(other, square_attributes->normalized_bottom_right_x,
											square_attributes->normalized_bottom_right_y)))
								result = (intptr_t)p_square_inside_coordinates(other, square_attributes->normalized_bottom_left_x,
										square_attributes->normalized_bottom_left_y);
	d_cast_return(result);
}

d_define_class(square) {
	d_hook_method(square, e_flag_public, set_top_left_x),
	d_hook_method(square, e_flag_public, set_top_left_y),
	d_hook_method(square, e_flag_public, set_bottom_right_x),
	d_hook_method(square, e_flag_public, set_bottom_right_y),
	d_hook_method(square, e_flag_public, set_angle),
	d_hook_method(square, e_flag_public, set_center),
	d_hook_method(square, e_flag_private, normalize_coordinate),
	d_hook_method(square, e_flag_public, normalize),
	d_hook_method(square, e_flag_public, inside),
	d_hook_method(square, e_flag_public, inside_coordinates),
	d_hook_method(square, e_flag_public, collision),
	d_hook_method_tail
};
