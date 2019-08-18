/*
 * miranda
 * Copyright (C) 2019 Andrea Nardinocchi (andrea@nardinan.it)
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
#include "polygon.obj.h"
struct s_polygon_attributes *p_polygon_alloc(struct s_object *self) {
  struct s_polygon_attributes *result = d_prepare(self, polygon);
  f_memory_new(self);   /* inherit */
  f_mutex_new(self);    /* inherit */
  return result;
}
struct s_object *f_polygon_new(struct s_object *self, size_t size, ...) {
  struct s_polygon_attributes *attributes = p_polygon_alloc(self);
  struct s_object *current_point;
  double current_point_x, current_point_y;
  va_list parameters;
  f_array_new(d_use(&(attributes->array_normalized_points), array), size);
  va_start(parameters, size);
  f_array_new_args(d_use(&(attributes->array_points), array), size, parameters);
  va_end(parameters);
  d_array_foreach(&(attributes->array_points), current_point) {
    d_call(current_point, m_point_get, &current_point_x, &current_point_y);
    attributes->sum_component_x += current_point_x;
    attributes->sum_component_y += current_point_y;
    if (current_point_x < attributes->origin_x)
      attributes->origin_x = current_point_x;
    if (current_point_y < attributes->origin_y)
      attributes->origin_y = current_point_y;
    ++(attributes->entries);
  }
  return self;
}
d_define_method(polygon, set_polygon)(struct s_object *self, struct s_object *source) {
  d_using(polygon);
  struct s_polygon_attributes *polygon_source_attributes = d_cast(source, polygon);
  struct s_object *point_current;
  d_call(&(polygon_attributes->array_points), m_array_clear, NULL);
  d_call(&(polygon_attributes->array_normalized_points), m_array_clear, NULL);
  polygon_attributes->entries = polygon_source_attributes->entries;
  polygon_attributes->angle = polygon_source_attributes->angle;
  polygon_attributes->sum_component_x = polygon_source_attributes->sum_component_x;
  polygon_attributes->sum_component_y = polygon_source_attributes->sum_component_y;
  polygon_attributes->normalized = polygon_source_attributes->normalized;
  d_array_foreach(&(polygon_source_attributes->array_points), point_current) {
    /* the two polygons share the very same point object */
    d_call(&(polygon_attributes->array_points), m_polygon_push, point_current);
  }
  d_array_foreach(&(polygon_source_attributes->array_normalized_points), point_current) {
    /* the two polygons share the very same point object */
    d_call(&(polygon_attributes->array_normalized_points), m_polygon_push, point_current);
  }
  return self;
}
d_define_method(polygon, clear)(struct s_object *self) {
  d_using(polygon);
  d_call(&(polygon_attributes->array_points), m_array_clear, NULL);
  polygon_attributes->sum_component_x = 0;
  polygon_attributes->sum_component_y = 0;
  polygon_attributes->entries = 0;
  polygon_attributes->normalized = d_false;
  return self;
}
d_define_method(polygon, push)(struct s_object *self, struct s_object *point) {
  d_using(polygon);
  double current_point_x, current_point_y;
  d_call(point, m_point_get, &current_point_x, &current_point_y);
  polygon_attributes->sum_component_x += current_point_x;
  polygon_attributes->sum_component_y += current_point_y;
  if (current_point_x < polygon_attributes->origin_x)
    polygon_attributes->origin_x = current_point_x;
  if (current_point_y < polygon_attributes->origin_y)
    polygon_attributes->origin_y = current_point_y;
  ++(polygon_attributes->entries);
  polygon_attributes->normalized = d_false;
  return d_call(&(polygon_attributes->array_points), m_array_push, point);
}
d_define_method(polygon, reset)(struct s_object *self) {
  d_using(polygon);
  d_call(self, m_polygon_normalize, NULL);
  return d_call(&(polygon_attributes->array_normalized_points), m_array_reset, NULL);
}
d_define_method(polygon, next)(struct s_object *self) {
  d_using(polygon);
  return d_call(&(polygon_attributes->array_normalized_points), m_array_next, NULL);
}
d_define_method(polygon, set_center)(struct s_object *self, double center_x, double center_y) {
  d_using(polygon);
  polygon_attributes->center_x = center_x;
  polygon_attributes->center_y = center_y;
  return self;
}
d_define_method(polygon, get_center)(struct s_object *self, double *center_x, double *center_y) {
  d_using(polygon);
  if (center_x)
    *center_x = polygon_attributes->center_x;
  if (center_y)
    *center_y = polygon_attributes->center_y;
  return self;
}
d_define_method(polygon, get_centroid)(struct s_object *self, double *centroid_x, double *centroid_y) {
  d_using(polygon);
  if (centroid_x)
    *centroid_x = ((polygon_attributes->sum_component_x / (double)(polygon_attributes->entries)) - polygon_attributes->origin_x);
  if (centroid_y)
    *centroid_y = ((polygon_attributes->sum_component_y / (double)(polygon_attributes->entries)) - polygon_attributes->origin_y);
  return self;
}
d_define_method(polygon, get_origin)(struct s_object *self, double *origin_x, double *origin_y) {
  d_using(polygon);
  if (origin_x)
    *origin_x = polygon_attributes->origin_x;
  if (origin_y)
    *origin_y = polygon_attributes->origin_y;
  return self;
}
d_define_method(polygon, size)(struct s_object *self, size_t *size) {
  d_using(polygon);
  return d_call(&(polygon_attributes->array_points), m_array_size, size);
}
d_define_method(polygon, set_angle)(struct s_object *self, double angle) {
  d_using(polygon);
  polygon_attributes->angle = angle;
  polygon_attributes->normalized = d_false;
  return self;
}
d_define_method(polygon, normalize_coordinate)(struct s_object *self, double x, double y, double normalized_center_x, double normalized_center_y,
  double sin_radians, double cos_radians, double *normalized_x, double *normalized_y) {
  double support_x, support_y;
  support_x = x - normalized_center_x;
  support_y = y - normalized_center_y;
  *normalized_x = (support_x * cos_radians) - (support_y * sin_radians);
  *normalized_y = (support_x * sin_radians) + (support_y * cos_radians);
  *normalized_x += normalized_center_x;
  *normalized_y += normalized_center_y;
  return self;
}
d_define_method(polygon, normalize)(struct s_object *self) {
  d_using(polygon);
  struct s_object *current_point, *new_point;
  double radians = (polygon_attributes->angle * d_math_radians_conversion), sin_radians, cos_radians, normalized_center_x, normalized_center_y,
    current_position_x, current_position_y, normalized_position_x, normalized_position_y;
  if (!polygon_attributes->normalized) {
    d_call(&(polygon_attributes->array_normalized_points), m_array_clear, NULL);
    sin_radians = sin(radians);
    cos_radians = cos(radians);
    normalized_center_x = polygon_attributes->center_x;
    normalized_center_y = polygon_attributes->center_y;
    d_array_foreach(&(polygon_attributes->array_points), current_point) {
      d_call(current_point, m_point_get, &current_position_x, &current_position_y);
      d_call(self, m_polygon_normalize_coordinate, current_position_x, current_position_y, normalized_center_x, normalized_center_y, sin_radians,
        cos_radians, &normalized_position_x, &normalized_position_y);
      new_point = f_point_new(d_new(point), normalized_position_x, normalized_position_y);
      d_call(&(polygon_attributes->array_normalized_points), m_array_push, new_point);
      d_delete(new_point);
    }
    polygon_attributes->normalized = d_true;
  }
  return self;
}
d_define_method(polygon, delete)(struct s_object *self, struct s_polygon_attributes *attributes) {
  d_delete(&(attributes->array_points));
  d_delete(&(attributes->array_normalized_points));
  return NULL;
}
d_define_class(polygon) {d_hook_method(polygon, e_flag_public, set_polygon),
                         d_hook_method(polygon, e_flag_public, clear),
                         d_hook_method(polygon, e_flag_public, push),
                         d_hook_method(polygon, e_flag_public, reset),
                         d_hook_method(polygon, e_flag_public, next),
                         d_hook_method(polygon, e_flag_public, set_center),
                         d_hook_method(polygon, e_flag_public, get_center),
                         d_hook_method(polygon, e_flag_public, get_centroid),
                         d_hook_method(polygon, e_flag_public, get_origin),
                         d_hook_method(polygon, e_flag_public, size),
                         d_hook_method(polygon, e_flag_public, set_angle),
                         d_hook_method(polygon, e_flag_public, normalize_coordinate),
                         d_hook_method(polygon, e_flag_public, normalize),
                         d_hook_delete(polygon),
                         d_hook_method_tail};
