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
#include "camera_controller.obj.h"
struct s_camera_controller_attributes *p_camera_controller_alloc(struct s_object *self) {
  struct s_camera_controller_attributes *result = d_prepare(self, camera_controller);
  /* abstract (no memory inheritance) */
  return result;
}
struct s_object *f_camera_controller_new(struct s_object *self, unsigned short int affected_axis) {
  struct s_camera_controller_attributes *attributes = p_camera_controller_alloc(self);
  attributes->affected_axis = affected_axis;
  gettimeofday(&(attributes->last_refresh), NULL);
  return self;
}
d_define_method(camera_controller, update)(struct s_object *self, double *screen_position_x, double *screen_position_y, double *screen_w, double *screen_h,
                                           double *scene_reference_w, double *scene_reference_h, double *scene_offset_x, double *scene_offset_y,
                                           double *scene_center_x, double *scene_center_y, double *camera_angle, double *scene_zoom) {
  d_war(e_log_level_ever, "'update' method has not been implemented yet");
  return self;
}
d_define_class(camera_controller) {d_hook_method(camera_controller, e_flag_public, update),
                                   d_hook_method_tail};