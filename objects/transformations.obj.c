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
#include "transformations.obj.h"
double p_normalized_linear(double time, double minimum, double maximum) {
  return (minimum + ((maximum - minimum) * time));
}
double p_normalized_smooth_start_2(double time, double minimum, double maximum) {
  return (minimum + ((maximum - minimum) * (time * time)));
}
double p_normalized_smooth_start_3(double time, double minimum, double maximum) {
  return (minimum + ((maximum - minimum) * (time * time * time)));
}
double p_normalized_smooth_start_4(double time, double minimum, double maximum) {
  return (minimum + ((maximum - minimum) * (time * time * time * time)));
}
double p_normalized_inverted_linear(double time, double minimum, double maximum) {
  return (minimum + ((maximum - minimum) * (1 - time)));
}
double p_normalized_smooth_stop_2(double time, double minimum, double maximum) {
  return (minimum + ((maximum - minimum) * (1 - ((1 - time) * (1 - time)))));
}
double p_normalized_smooth_stop_3(double time, double minimum, double maximum) {
  return (minimum + ((maximum - minimum) * (1 - ((1 - time) * (1 - time) * (1 - time)))));
}
double p_normalized_smooth_stop_4(double time, double minimum, double maximum) {
  return (minimum + ((maximum - minimum) * (1 - ((1 - time) * (1 - time) * (1 - time) * (1 - time)))));
}
double p_normalized_crossfade(t_transformation_function left, t_transformation_function right, double time, double minimum, double maximum) {
  return ((1 - time) * left(time, minimum, maximum)) + (time * right(time, minimum, maximum));
}
double p_normalized_mix(t_transformation_function left, t_transformation_function right, double weight, double time, double minimum, double maximum) {
  return ((1 - weight) * left(time, minimum, maximum)) + (weight * right(time, minimum, maximum));
}
struct s_transformations_attributes *p_transformations_alloc(struct s_object *self) {
  struct s_transformations_attributes *result = d_prepare(self, transformations);
  f_memory_new(self);     /* inherit */
  f_mutex_new(self);      /* inherit */
  return result;
}
struct s_object *f_transformations_new(struct s_object *self) {
  struct s_scoped_container {
    struct s_object *key;
    t_transformation_function value;
  } scoped_container[] = {
    {d_kstr("linear"),          p_normalized_linear},
    {d_kstr("inverted_linear"), p_normalized_inverted_linear},
    {d_kstr("smooth_start_2"),  p_normalized_smooth_start_2},
    {d_kstr("smooth_start_3"),  p_normalized_smooth_start_3},
    {d_kstr("smooth_start_4"),  p_normalized_smooth_start_4},
    {d_kstr("smooth_stop_2"),   p_normalized_smooth_stop_2},
    {d_kstr("smooth_stop_3"),   p_normalized_smooth_stop_3},
    {d_kstr("smooth_stop_4"),   p_normalized_smooth_stop_4},
    {NULL, NULL}
  };
  struct s_transformations_attributes *attributes = p_transformations_alloc(self);
  unsigned int index;
  f_hash_init(&attributes->hash, (t_hash_compare *)f_object_compare, (t_hash_calculate *)f_object_hash);
  for (index = 0; (scoped_container[index].key); ++index) {
    p_transformations_insert(self, scoped_container[index].key, scoped_container[index].value);
    d_delete(scoped_container[index].key);
  }
  return self;
}
d_define_method(transformations, insert)(struct s_object *self, struct s_object *function, t_transformation_function pointer) {
  d_using(transformations);
  struct s_hash_bucket previous_content;
  struct s_object *retained_key = d_retain(function);
  if (f_hash_insert(transformations_attributes->hash, (void *)retained_key, (void *)pointer, d_true, &previous_content)) {
    /* if the value already exists, hash table keeps the previous key */
    d_delete(retained_key);
  }
  return self;
}
d_define_method(transformations, run)(struct s_object *self, struct s_object *function, double normalized_time, double minimum, double maximum,
                                       double *normalized_value) {
  d_using(transformations);
  struct s_object *result = NULL;
  t_transformation_function selected_function;
  if ((selected_function = f_hash_get(transformations_attributes->hash, function))) {
    *normalized_value = selected_function(normalized_time, minimum, maximum);
    result = self;
  }
  return result;
}
d_define_method(transformations, run_mapped)(struct s_object *self, struct s_object *function, double time, double minimum_time, double maximum_time,
                                              double minimum, double maximum, double *normalized_value) {
  d_using(transformations);
  struct s_object *result = NULL;
  t_transformation_function selected_function;
  double normalized_time;
  if ((selected_function = f_hash_get(transformations_attributes->hash, function))) {
    normalized_time = ((time - minimum_time) / (maximum_time - minimum_time));
    *normalized_value = selected_function(normalized_time, minimum, maximum);
    result = self;
  }
  return result;
}
d_define_method(transformations, run_mixed)(struct s_object *self, struct s_object *function_left, struct s_object *function_right, double weight,
                                             double normalized_time, double minimum, double maximum, double *normalized_value) {
  d_using(transformations);
  struct s_object *result = NULL;
  t_transformation_function selected_left_function, selected_right_function;
  if ((selected_left_function = f_hash_get(transformations_attributes->hash, function_left)) &&
    (selected_right_function = f_hash_get(transformations_attributes->hash, function_right))) {
    *normalized_value = p_normalized_mix(selected_left_function, selected_right_function, weight, normalized_time, minimum, maximum);
    result = self;
  }
  return result;
}
d_define_method(transformations, run_crossfaded)(struct s_object *self, struct s_object *function_left, struct s_object *function_right, double normalized_time,
                                                  double minimum, double maximum, double *normalized_value) {
  d_using(transformations);
  struct s_object *result = NULL;
  t_transformation_function selected_left_function, selected_right_function;
  if ((selected_left_function = f_hash_get(transformations_attributes->hash, function_left)) &&
      (selected_right_function = f_hash_get(transformations_attributes->hash, function_right))) {
    *normalized_value = p_normalized_crossfade(selected_left_function, selected_right_function, normalized_time, minimum, maximum);
    result = self;
  }
  return result;
}
d_define_method(transformations, delete)(struct s_object *self, struct s_transformations_attributes *attributes) {
  struct s_hash_bucket *current_table = attributes->hash->table, *item;
  t_hash_value elements = attributes->hash->mask;
  for (; elements >= 0; --elements) {
    item = &current_table[elements];
    if (item->kind != e_hash_kind_empty) {
      d_delete(item->key);
    }
  }
  f_hash_destroy(&(attributes->hash));
  return NULL;
}
d_define_class(transformations) {d_hook_method(transformations, e_flag_public, insert),
                                 d_hook_method(transformations, e_flag_public, run),
                                 d_hook_method(transformations, e_flag_public, run_mapped),
                                 d_hook_method(transformations, e_flag_public, run_mixed),
                                 d_hook_method(transformations, e_flag_public, run_crossfaded),
                                 d_hook_delete(transformations),
                                 d_hook_method_tail};