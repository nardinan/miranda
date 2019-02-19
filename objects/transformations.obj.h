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
#ifndef miranda_object_transformations_h
#define miranda_object_transformations_h
#include "string.obj.h"
#include "../hash.h"
typedef double (*t_transformation_function)(double, double, double);
d_declare_class(transformations) {
  struct s_attributes head;
  struct s_hash_table *hash;
} d_declare_class_tail(transformations);
struct s_transformations_attributes *p_transformations_alloc(struct s_object *self);
extern struct s_object *f_transformations_new(struct s_object *self);
d_declare_method(transformations, insert)(struct s_object *self, struct s_object *function, t_transformation_function pointer);
d_declare_method(transformations, run)(struct s_object *self, struct s_object *function, double normalized_time, double minimum, double maximum,
  double *normalized_value);
d_declare_method(transformations, run_mapped)(struct s_object *self, struct s_object *function, double time, double minimum_time, double maximum_time,
  double minimum, double maximum, double *normalized_value);
d_declare_method(transformations, run_mixed)(struct s_object *self, struct s_object *function_left, struct s_object *function_right, double weight,
  double normalized_time, double minimum, double maximum, double *normalized_value);
d_declare_method(transformations, run_crossfaded)(struct s_object *self, struct s_object *function_left, struct s_object *function_right,
  double normalized_time, double minimum, double maximum, double *normalized_value);
d_declare_method(transformations, delete)(struct s_object *self, struct s_transformations_attributes *attributes);
#endif
