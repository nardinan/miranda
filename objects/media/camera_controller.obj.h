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
#ifndef miranda_media_camera_controller_h
#define miranda_media_camera_controller_h
#include "camera.obj.h"
#include <sys/time.h>
typedef enum e_camera_controller_axis {
  e_camera_controller_axis_x = 0x01,
  e_camera_controller_axis_y = 0x02,
  e_camera_controller_axis_z = 0x04
} e_camera_controller_axis;
d_declare_class(camera_controller) {
  struct s_attributes head;
  struct timeval last_refresh;
  unsigned short int affected_axis;
} d_declare_class_tail(camera_controller);
struct s_camera_controller_attributes *p_camera_controller_alloc(struct s_object *self);
extern struct s_object *f_camera_controller_new(struct s_object *self, unsigned short int affected_axis);
d_declare_method(camera_controller, update)(struct s_object *self, double *screen_position_x, double *screen_position_y, double *screen_w, double *screen_h,
  double *scene_reference_w, double *scene_reference_h, double *scene_offset_x, double *scene_offset_y, double *scene_center_x, double *scene_center_y,
  double *camera_angle, double *scene_zoom);
#endif
