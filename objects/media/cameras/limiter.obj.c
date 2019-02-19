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
#include "limiter.obj.h"
struct s_limiter_attributes *p_limiter_alloc(struct s_object *self, unsigned short int affected_axis) {
  struct s_limiter_attributes *result = d_prepare(self, limiter);
  f_mutex_new(self);                              /* inherit */
  f_memory_new(self);                             /* inherit */
  f_camera_controller_new(self, affected_axis);   /* inherit */
  return result;
}
struct s_object *f_limiter_new(struct s_object *self, unsigned short int affected_axis) {
  struct s_limiter_attributes *attributes = p_limiter_alloc(self, affected_axis);
  attributes = attributes;
  return self;
}
d_define_method(limiter, set_limit_x)(struct s_object *self, double low_limit_x, double high_limit_x) {
  d_using(limiter);
  limiter_attributes->low_limit_x = low_limit_x;
  limiter_attributes->high_limit_x = high_limit_x;
  return self;
}
d_define_method(limiter, set_limit_y)(struct s_object *self, double low_limit_y, double high_limit_y) {
  d_using(limiter);
  limiter_attributes->low_limit_y = low_limit_y;
  limiter_attributes->high_limit_y = high_limit_y;
  return self;
}
d_define_method(limiter, set_limit_z)(struct s_object *self, double low_limit_z, double high_limit_z) {
  d_using(limiter);
  limiter_attributes->low_limit_z = low_limit_z;
  limiter_attributes->high_limit_z = high_limit_z;
  return self;
}
d_define_method(limiter, update)(struct s_object *self, double *screen_position_x, double *screen_position_y, double *screen_w, double *screen_h,
  double *scene_reference_w, double *scene_reference_h, double *scene_offset_x, double *scene_offset_y, double *scene_center_x, double *scene_center_y,
  double *camera_angle, double *scene_zoom) {
  d_using(limiter);
  d_using(camera_controller);
  if ((camera_controller_attributes->affected_axis & e_camera_controller_axis_x) == e_camera_controller_axis_x) {
    if (*scene_offset_x > limiter_attributes->high_limit_x)
      *scene_offset_x = limiter_attributes->high_limit_x;
    if (*scene_offset_x < limiter_attributes->low_limit_x)
      *scene_offset_x = limiter_attributes->low_limit_x;
  }
  if ((camera_controller_attributes->affected_axis & e_camera_controller_axis_y) == e_camera_controller_axis_y) {
    if (*scene_offset_y > limiter_attributes->high_limit_y)
      *scene_offset_y = limiter_attributes->high_limit_y;
    if (*scene_offset_y < limiter_attributes->low_limit_y)
      *scene_offset_y = limiter_attributes->low_limit_y;
  }
  if ((camera_controller_attributes->affected_axis & e_camera_controller_axis_z) == e_camera_controller_axis_z) {
    if (*scene_zoom > limiter_attributes->high_limit_z)
      *scene_zoom = limiter_attributes->high_limit_z;
    if (*scene_zoom < limiter_attributes->low_limit_z)
      *scene_zoom = limiter_attributes->low_limit_z;
  }
  return self;
}
d_define_class(limiter) {d_hook_method(limiter, e_flag_public, set_limit_x),
                         d_hook_method(limiter, e_flag_public, set_limit_y),
                         d_hook_method(limiter, e_flag_public, set_limit_z),
                         d_hook_method_override(limiter, e_flag_public, camera_controller, update),
                         d_hook_method_tail};