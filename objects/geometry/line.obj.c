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
    f_mutex_new(self);  /* inherit */
    return result;
}

struct s_object *f_line_new(struct s_object *self, double starting_x, double starting_y, double ending_x, double ending_y) {
    struct s_line_attributes *attributes = p_line_alloc(self);
    attributes->starting_x = starting_x;
    attributes->starting_y = starting_y;
    attributes->ending_x = ending_x;
    attributes->ending_y = ending_y;
    return self;
}

struct s_object *f_line_new_points(struct s_object *self, struct s_object *point_starting, struct s_object *point_ending) {
    struct s_point_attributes *point_attributes_starting = d_cast(point_starting, point), *point_attributes_ending = d_cast(point_ending, point);
    return f_line_new(self, point_attributes_starting->x, point_attributes_starting->y, point_attributes_ending->x, point_attributes_ending->y);
}

d_define_method(line, set_starting)(struct s_object *self, double starting_x, double starting_y) {
    d_using(line);
    line_attributes->starting_x = starting_x;
    line_attributes->starting_y = starting_y;
    return self;
}

d_define_method(line, set_ending)(struct s_object *self, double ending_x, double ending_y) {
    d_using(line);
    line_attributes->ending_x = ending_x;
    line_attributes->ending_y = ending_y;
    return self;
}

d_define_method(line, add)(struct s_object *self, double x, double y) {
    d_using(line);
    line_attributes->starting_x += x;
    line_attributes->starting_y += y;
    line_attributes->ending_x += x;
    line_attributes->ending_y += y;
    return self;
}

d_define_method(line, subtract)(struct s_object *self, double x, double y) {
    d_using(line);
    line_attributes->starting_x -= x;
    line_attributes->starting_y -= y;
    line_attributes->ending_x -= x;
    line_attributes->ending_y -= y;
    return self;
}

d_define_method(line, get)(struct s_object *self, double *starting_x, double *starting_y, double *ending_x, double *ending_y) {
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
    d_using(line);
    struct s_line_attributes *line_attributes_other = d_cast(other, line);
    return d_call(self, m_line_intersect_coordinates, line_attributes->starting_x, line_attributes->starting_y, line_attributes->ending_x,
            line_attributes->ending_y, line_attributes_other->starting_x, line_attributes_other->starting_y, line_attributes_other->ending_x,
            line_attributes_other->ending_y);
}

d_define_method(line, intersect_coordinates)(struct s_object *self, double starting_x_A, double starting_y_A, double ending_x_A, double ending_y_A,
        double starting_x_B, double starting_y_B, double ending_x_B, double ending_y_B) {
    double length_x_A, length_y_A, length_x_B, length_y_B, s, t;
    t_boolean result = d_false;
    length_x_A = ending_x_A - starting_x_A;
    length_y_A = ending_y_A - starting_y_A;
    length_x_B = ending_x_B - starting_x_B;
    length_y_B = ending_y_B - starting_y_B;
    s = (-length_y_A * (starting_x_A - starting_x_B) + length_x_A * (starting_y_A - starting_y_B)) / (-length_x_B * length_y_A + length_x_A * length_y_B);
    t = (length_x_B * (starting_y_A - starting_y_B) - length_y_B * (starting_x_A - starting_x_B)) / (-length_x_B * length_y_A + length_x_A * length_y_B);
    if ((s >= 0) && (s <= 1) && (t >= 0) && (t <= 1))
        result = d_true;
    d_cast_return(result);
}

d_define_class(line) {
    d_hook_method(line, e_flag_public, set_starting),
    d_hook_method(line, e_flag_public, set_ending),
    d_hook_method(line, e_flag_public, add),
    d_hook_method(line, e_flag_public, subtract),
    d_hook_method(line, e_flag_public, get),
    d_hook_method(line, e_flag_public, intersect),
    d_hook_method(line, e_flag_public, intersect_coordinates),
    d_hook_method_tail
};
