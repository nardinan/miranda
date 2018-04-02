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
#ifndef miranda_drawable_chaser_h
#define miranda_drawable_chaser_h
#include "../camera_controller.obj.h"
#define d_drawable_chaser_default_minimum_speed 2.0           /* pixels per second */
#define d_drawable_chaser_default_maximum_speed 8.0           /* pixels per second */
#define d_drawable_chaser_default_acceleration_distance 50.0  /* pixels per second */
d_declare_class(drawable_chaser) {
  struct s_attributes head;
  double offset_x, offset_y, offset_z, destination_x, destination_y, destination_z, minimum_speed, maximum_speed, acceleration_distance;
  t_boolean change_position;
  struct s_object *reference;
} d_declare_class_tail(drawable_chaser);
struct s_drawable_chaser_attributes *p_drawable_chaser_alloc(struct s_object *self, unsigned short int affected_axis);
extern struct s_object *f_drawable_chaser_new(struct s_object *self, unsigned short int affected_axis);
d_declare_method(drawable_chaser, move_destination)(struct s_object *self, double destination_x, double destination_y, double destination_z);
d_declare_method(drawable_chaser, chase_destination)(struct s_object *self, struct s_object *environment, double destination_x, double destination_y,
                                                       double destination_z);
d_declare_method(drawable_chaser, chase_reference)(struct s_object *self, struct s_object *reference);
d_declare_method(drawable_chaser, remove_reference)(struct s_object *self);
d_declare_method(drawable_chaser, set_offset)(struct s_object *self, double offset_x, double offset_y, double offset_z);
d_declare_method(drawable_chaser, set_speed)(struct s_object *self, double minimum_speed, double maximum_speed, double acceleration_distance);
d_declare_method(drawable_chaser, update)(struct s_object *self, double *screen_position_x, double *screen_position_y, double *screen_w, double *screen_h,
                                            double *scene_reference_w, double *scene_reference_h, double *scene_offset_x, double *scene_offset_y,
                                            double *scene_center_x, double *scene_center_y, double *camera_angle, double *scene_zoom);
d_declare_method(drawable_chaser, delete)(struct s_object *self, struct s_drawable_chaser_attributes *attributes);
#endif