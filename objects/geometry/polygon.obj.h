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
#ifndef miranda_object_polygon_h
#define miranda_object_polygon_h
#include "../array.obj.h"
#include "point.obj.h"
#include "line.obj.h"
d_declare_class(polygon) {
  struct s_attributes head;
  struct s_object *array_points, *array_normalized_points;
  double sum_component_x, sum_component_y, origin_x, origin_y, center_x, center_y, angle;
  unsigned int entries;
  t_boolean normalized;
} d_declare_class_tail(polygon);
double p_polygon_relative_orientation(struct s_object *point_proposed, struct s_object *point_next, struct s_object *point_current);
struct s_polygon_attributes *p_polygon_alloc(struct s_object *self);
extern struct s_object *f_polygon_new(struct s_object *self, size_t size, ...);
d_declare_method(polygon, set_polygon)(struct s_object *self, struct s_object *source);
d_declare_method(polygon, clear)(struct s_object *self);
d_declare_method(polygon, push)(struct s_object *self, struct s_object *point);
d_declare_method(polygon, get)(struct s_object *self, size_t position);
d_declare_method(polygon, reset)(struct s_object *self);
d_declare_method(polygon, next)(struct s_object *self);
d_declare_method(polygon, set_center)(struct s_object *self, double center_x, double center_y);
d_declare_method(polygon, get_center)(struct s_object *self, double *center_x, double *center_y);
d_declare_method(polygon, get_centroid)(struct s_object *self, double *centroid_x, double *centroid_y);
d_declare_method(polygon, get_origin)(struct s_object *self, double *origin_x, double *origin_y);
d_declare_method(polygon, convex_hull)(struct s_object *self);
#define d_polygon_foreach(o, v) for(d_call((o),m_polygon_reset,NULL),(v)=d_call((o),m_polygon_next,NULL);(v);(v)=d_call((o),m_polygon_next,NULL))
d_declare_method(polygon, size)(struct s_object *self, size_t *size);
d_declare_method(polygon, set_angle)(struct s_object *self, double angle);
d_declare_method(polygon, normalize_coordinate)(struct s_object *self, double x, double y, double normalized_center_x, double normalized_center_y,
    double sin_radians, double cos_radians, double *normalized_x, double *normalized_y);
d_declare_method(polygon, normalize)(struct s_object *self);
d_declare_method(polygon, inside)(struct s_object *self, struct s_object *point);
d_declare_method(polygon, inside_coordinates)(struct s_object *self, double x, double y);
d_declare_method(polygon, intersect_line)(struct s_object *self, struct s_object *other);
d_declare_method(polygon, intersect_coordinates)(struct s_object *self, double starting_x_B, double starting_y_B, double ending_x_B, double ending_y_B,
    unsigned int *collisions);
d_declare_method(polygon, delete)(struct s_object *self, struct s_polygon_attributes *attributes);
#endif
