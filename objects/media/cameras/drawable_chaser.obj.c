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
#include "drawable_chaser.obj.h"
struct s_drawable_chaser_attributes *p_drawable_chaser_alloc(struct s_object *self, unsigned short int affected_axis) {
  struct s_drawable_chaser_attributes *result = d_prepare(self, drawable_chaser);
  f_mutex_new(self);                              /* inherit */
  f_memory_new(self);                             /* inherit */
  f_camera_controller_new(self, affected_axis);   /* inherit */
  return result;
}
struct s_object *f_drawable_chaser_new(struct s_object *self, unsigned short int affected_axis) {
  struct s_drawable_chaser_attributes *attributes = p_drawable_chaser_alloc(self, affected_axis);
  attributes->minimum_speed = d_drawable_chaser_default_minimum_speed;
  attributes->maximum_speed = d_drawable_chaser_default_maximum_speed;
  attributes->acceleration_distance = d_drawable_chaser_default_acceleration_distance;
  attributes->offset_z = 1.0;
  attributes->destination_z = 1.0;
  return self;
}
d_define_method(drawable_chaser, move_destination)(struct s_object *self, double destination_x, double destination_y, double destination_z) {
  d_using(drawable_chaser);
  drawable_chaser_attributes->destination_x = destination_x;
  drawable_chaser_attributes->destination_y = destination_y;
  drawable_chaser_attributes->destination_z = destination_z;
  drawable_chaser_attributes->change_position = d_true;
  return self;
}
d_define_method(drawable_chaser, chase_destination)(struct s_object *self, struct s_object *environment, double destination_x, double destination_y,
  double destination_z) {
  d_using(drawable_chaser);
  if (drawable_chaser_attributes->reference) {
    d_delete(drawable_chaser_attributes->reference);
    drawable_chaser_attributes->reference = NULL;
  }
  if (destination_z == destination_z)
    drawable_chaser_attributes->destination_z = destination_z;
  if (destination_x == destination_x)
    drawable_chaser_attributes->destination_x = destination_x;
  if (destination_y == destination_y)
    drawable_chaser_attributes->destination_y = destination_y;
  return self;
}
d_define_method(drawable_chaser, chase_depth)(struct s_object *self, double destination_z) {
  d_using(drawable_chaser);
  drawable_chaser_attributes->destination_z = destination_z;
  return self;
}
d_define_method(drawable_chaser, chase_reference)(struct s_object *self, struct s_object *reference) {
  d_using(drawable_chaser);
  if (drawable_chaser_attributes->reference)
    d_delete(drawable_chaser_attributes->reference);
  drawable_chaser_attributes->reference = d_retain(reference);
  return self;
}
d_define_method(drawable_chaser, set_offset)(struct s_object *self, double offset_x, double offset_y, double offset_z) {
  d_using(drawable_chaser);
  drawable_chaser_attributes->offset_x = offset_x;
  drawable_chaser_attributes->offset_y = offset_y;
  drawable_chaser_attributes->offset_z = offset_z;
  return self;
}
d_define_method(drawable_chaser, remove_reference)(struct s_object *self) {
  d_using(drawable_chaser);
  if (drawable_chaser_attributes->reference) {
    d_delete(drawable_chaser_attributes->reference);
    drawable_chaser_attributes->reference = NULL;
  }
  return self;
}
d_define_method(drawable_chaser, set_speed)(struct s_object *self, double minimum_speed, double maximum_speed, double acceleration_distance) {
  d_using(drawable_chaser);
  drawable_chaser_attributes->minimum_speed = minimum_speed;
  drawable_chaser_attributes->maximum_speed = maximum_speed;
  drawable_chaser_attributes->acceleration_distance = acceleration_distance;
  return self;
}
d_define_method_override(drawable_chaser, update)(struct s_object *self, double *screen_position_x, double *screen_position_y, double *screen_w,
  double *screen_h, double *scene_reference_w, double *scene_reference_h, double *scene_offset_x, double *scene_offset_y, double *scene_center_x,
  double *scene_center_y, double *camera_angle, double *scene_zoom) {
  d_using(drawable_chaser);
  d_using(camera_controller);
  struct timeval current, difference;
  double elapsed_time, current_distance, current_speed, reference_position_x, reference_position_y, final_position_x = *scene_offset_x,
    final_position_y = *scene_offset_y, final_position_z = *scene_zoom;
  gettimeofday(&current, NULL);
  if (drawable_chaser_attributes->change_position) {
    final_position_x = drawable_chaser_attributes->destination_x;
    final_position_y = drawable_chaser_attributes->destination_y;
    final_position_z = drawable_chaser_attributes->destination_z;
    drawable_chaser_attributes->change_position = d_false;
  } else {
    timersub(&current, &(camera_controller_attributes->last_refresh), &difference);
    if ((elapsed_time = difference.tv_sec + (difference.tv_usec / 1000000.0)) > 0) {
      if (drawable_chaser_attributes->reference) {
        /* normalize the drawable component using the incoming details ... */
        d_call(drawable_chaser_attributes->reference, m_drawable_normalize_scale, *scene_reference_w, *scene_reference_h, *scene_offset_x, *scene_offset_y,
          *scene_center_x, *scene_center_y, *screen_w, *screen_h, *scene_zoom);
        /* ... and now we could access the normalized position */
        d_call(drawable_chaser_attributes->reference, m_drawable_get_scaled_principal_point, &reference_position_x, &reference_position_y);
        drawable_chaser_attributes->destination_x = ((reference_position_x + *scene_offset_x) - (*screen_w / 2.0));
        drawable_chaser_attributes->destination_y = ((reference_position_y + *scene_offset_y) - (*screen_h / 2.0));
      }
      if ((camera_controller_attributes->affected_axis & e_camera_controller_axis_x) == e_camera_controller_axis_x) {
        if ((current_distance = (drawable_chaser_attributes->destination_x + drawable_chaser_attributes->offset_x) - (*scene_offset_x)) != 0.0) {
          current_speed = drawable_chaser_attributes->minimum_speed + ((drawable_chaser_attributes->maximum_speed - drawable_chaser_attributes->minimum_speed) *
                                                                       (fabs(current_distance) / drawable_chaser_attributes->acceleration_distance));
          current_speed = d_math_max(d_math_min(current_speed, drawable_chaser_attributes->maximum_speed), drawable_chaser_attributes->minimum_speed);
          current_speed *= *scene_zoom;
          if (current_distance < 0)
            current_speed = -current_speed;
          final_position_x = (current_speed * elapsed_time) + (*scene_offset_x);
          if ((current_speed > 0) && (final_position_x > (drawable_chaser_attributes->destination_x + drawable_chaser_attributes->offset_x)))
            final_position_x = drawable_chaser_attributes->destination_x + drawable_chaser_attributes->offset_x;
          else if ((current_speed < 0) && (final_position_x < (drawable_chaser_attributes->destination_x + drawable_chaser_attributes->offset_x)))
            final_position_x = drawable_chaser_attributes->destination_x + drawable_chaser_attributes->offset_x;
        }
      }
      if ((camera_controller_attributes->affected_axis & e_camera_controller_axis_y) == e_camera_controller_axis_y) {
        if ((current_distance = (drawable_chaser_attributes->destination_y + drawable_chaser_attributes->offset_y) - (*scene_offset_y)) != 0.0) {
          current_speed = drawable_chaser_attributes->minimum_speed + ((drawable_chaser_attributes->maximum_speed - drawable_chaser_attributes->minimum_speed) *
                                                                       (fabs(current_distance) / drawable_chaser_attributes->acceleration_distance));
          current_speed = d_math_max(d_math_min(current_speed, drawable_chaser_attributes->maximum_speed), drawable_chaser_attributes->minimum_speed);
          current_speed *= *scene_zoom;
          if (current_distance < 0)
            current_speed = -current_speed;
          final_position_y = (current_speed * elapsed_time) + (*scene_offset_y);
          if ((current_speed > 0) && (final_position_y > (drawable_chaser_attributes->destination_y + drawable_chaser_attributes->offset_y)))
            final_position_y = drawable_chaser_attributes->destination_y + drawable_chaser_attributes->offset_y;
          else if ((current_speed < 0) && (final_position_y < (drawable_chaser_attributes->destination_y + drawable_chaser_attributes->offset_y)))
            final_position_y = drawable_chaser_attributes->destination_y + drawable_chaser_attributes->offset_y;
        }
      }
      if ((camera_controller_attributes->affected_axis & e_camera_controller_axis_z) == e_camera_controller_axis_z) {
        if ((current_distance = (drawable_chaser_attributes->destination_z + drawable_chaser_attributes->offset_z) - (*scene_zoom)) != 0.0) {
          current_speed = drawable_chaser_attributes->minimum_speed + ((drawable_chaser_attributes->maximum_speed - drawable_chaser_attributes->minimum_speed) *
                                                                       (fabs(current_distance) / drawable_chaser_attributes->acceleration_distance));
          current_speed = d_math_max(d_math_min(current_speed, drawable_chaser_attributes->maximum_speed), drawable_chaser_attributes->minimum_speed);
          if (current_distance < 0)
            current_speed = -current_speed;
          final_position_z = (current_speed * elapsed_time) + (*scene_zoom);
          if ((current_speed > 0) && (final_position_z > (drawable_chaser_attributes->destination_z + drawable_chaser_attributes->offset_z)))
            final_position_z = drawable_chaser_attributes->destination_z + drawable_chaser_attributes->offset_z;
          else if ((current_speed < 0) && (final_position_z < (drawable_chaser_attributes->destination_z + drawable_chaser_attributes->offset_z)))
            final_position_z = drawable_chaser_attributes->destination_z + drawable_chaser_attributes->offset_z;
        }
      }
    }
  }
  *scene_offset_x = final_position_x;
  *scene_offset_y = final_position_y;
  *scene_zoom = final_position_z;
  memcpy(&(camera_controller_attributes->last_refresh), &current, sizeof(struct timeval));
  return self;
}
d_define_method(drawable_chaser, delete)(struct s_object *self, struct s_drawable_chaser_attributes *attributes) {
  if (attributes->reference)
    d_delete(attributes->reference);
  return NULL;
}
d_define_class(drawable_chaser) {d_hook_method(drawable_chaser, e_flag_public, move_destination),
                                 d_hook_method(drawable_chaser, e_flag_public, chase_destination),
                                 d_hook_method(drawable_chaser, e_flag_public, chase_reference),
                                 d_hook_method(drawable_chaser, e_flag_public, chase_depth),
                                 d_hook_method(drawable_chaser, e_flag_public, remove_reference),
                                 d_hook_method(drawable_chaser, e_flag_public, set_offset),
                                 d_hook_method(drawable_chaser, e_flag_public, set_speed),
                                 d_hook_method_override(drawable_chaser, e_flag_public, camera_controller, update),
                                 d_hook_delete(drawable_chaser),
                                 d_hook_method_tail};