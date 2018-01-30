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
#include "point.obj.h"
struct s_point_attributes *p_point_alloc(struct s_object *self) {
    struct s_point_attributes *result = d_prepare(self, point);
    f_memory_new(self);	/* inherit */
    f_mutex_new(self);	/* inherit */
    return result;
}

struct s_object *f_point_new(struct s_object *self, double x, double y) {
    struct s_point_attributes *attributes = p_point_alloc(self);
    attributes->x = x;
    attributes->y = y;
    return self;
}

d_define_method(point, set_point)(struct s_object *self, struct s_object *source) {
    d_using(point);
    struct s_point_attributes *point_attributes_source = d_cast(source, point);
    point_attributes->x = point_attributes_source->x;
    point_attributes->y = point_attributes_source->y;
    return self;
}

d_define_method(point, set_x)(struct s_object *self, double x) {
    d_using(point);
    point_attributes->x = x;
    return self;
}

d_define_method(point, set_y)(struct s_object *self, double y) {
    d_using(point);
    point_attributes->y = y;
    return self;
}

d_define_method(point, get)(struct s_object *self, double *x, double *y) {
    d_using(point);
    if (x)
        *x = point_attributes->x;
    if (y)
        *y = point_attributes->y;
    return self;
}

d_define_method(point, add)(struct s_object *self, double x, double y) {
    d_using(point);
    point_attributes->x += x;
    point_attributes->y += y;
    return self;
}

d_define_method(point, subtract)(struct s_object *self, double x, double y) {
    d_using(point);
    point_attributes->x -= x;
    point_attributes->y -= y;
    return self;
}

d_define_method(point, distance)(struct s_object *self, struct s_object *other, double *distance, double *distance_square) {
    d_using(point);
    struct s_point_attributes *point_attributes_other = d_cast(other, point);
    double current_distance_square;
    current_distance_square = ((point_attributes->x - point_attributes_other->x) * (point_attributes->x - point_attributes_other->x)) +
        ((point_attributes->y - point_attributes_other->y) * (point_attributes->y - point_attributes_other->y));
    if (distance_square)
        *distance_square = current_distance_square;
    if (distance)
        *distance = f_math_sqrt(current_distance_square, d_point_precision_default);
    return self;
}

d_define_method(point, angle)(struct s_object *self, struct s_object *other, double *tilt_angle) {
    d_using(point);
    struct s_point_attributes *point_attributes_other = d_cast(other, point);
    double delta_x = point_attributes_other->x - point_attributes->x, delta_y = point_attributes_other->y - point_attributes->y;
    if (tilt_angle)
        *tilt_angle = (atan2(delta_x, delta_y) / d_math_radians_conversion);
    return self;
}

d_define_method(point, rotate)(struct s_object *self, double angle) {
    d_using(point);
    double radians = angle * d_math_radians_conversion, cosine = cos(radians), sine = sin(radians);
    point_attributes->x = (point_attributes->x * cosine) - (point_attributes->y * sine);
    point_attributes->y = (point_attributes->x * sine) - (point_attributes->y * cosine);
    return self;
}

d_define_method(point, rotate_pivot)(struct s_object *self, double angle, struct s_object *pivot) {
    d_using(point);
    struct s_point_attributes *point_attributes_pivot = d_cast(pivot, point);
    double radians = angle * d_math_radians_conversion, cosine = cos(radians), sine = sin(radians), new_x = point_attributes->x - point_attributes_pivot->x, 
           new_y = point_attributes->y - point_attributes_pivot->y;
    new_x = (new_x * cosine) - (new_y * sine);
    new_y = (new_x * sine) + (new_y * cosine);
    point_attributes->x = new_x + point_attributes_pivot->x;
    point_attributes->y = new_y + point_attributes_pivot->y;
    return self;
}

d_define_class(point) {
    d_hook_method(point, e_flag_public, set_point),
        d_hook_method(point, e_flag_public, set_x),
        d_hook_method(point, e_flag_public, set_y),
        d_hook_method(point, e_flag_public, get),
        d_hook_method(point, e_flag_public, add),
        d_hook_method(point, e_flag_public, subtract),
        d_hook_method(point, e_flag_public, distance),
        d_hook_method(point, e_flag_public, angle),
        d_hook_method(point, e_flag_public, rotate),
        d_hook_method(point, e_flag_public, rotate_pivot),
        d_hook_method_tail
};
