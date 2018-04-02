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
#ifndef miranda_media_camera_h
#define miranda_media_camera_h
#include "bitmap.obj.h"
d_declare_class(camera) {
  struct s_attributes head;
  enum e_environment_surfaces surface;
  double screen_position_x, screen_position_y, screen_w, screen_h, scene_reference_w, scene_reference_h, scene_offset_x, scene_offset_y, scene_center_x,
    scene_center_y, camera_angle, scene_zoom;
  struct s_object *controllers;
  unsigned char *memblock;
  SDL_Texture *destination;
} d_declare_class_tail(camera);
struct s_camera_attributes *p_camera_alloc(struct s_object *self);
extern struct s_object *f_camera_new(struct s_object *self, double screen_offset_x, double screen_offset_y, double screen_width, double screen_height,
                                     enum e_environment_surfaces surface, struct s_object *environment);
d_declare_method(camera, set_size)(struct s_object *self, double width, double height);
d_declare_method(camera, get_size)(struct s_object *self, double *width, double *height);
d_declare_method(camera, set_reference)(struct s_object *self, double width, double height);
d_declare_method(camera, get_reference)(struct s_object *self, double *width, double *height);
d_declare_method(camera, set_position)(struct s_object *self, double position_x, double position_y);
d_declare_method(camera, get_position)(struct s_object *self, double *position_x, double *position_y);
d_declare_method(camera, set_offset)(struct s_object *self, double offset_x, double offset_y);
d_declare_method(camera, get_offset)(struct s_object *self, double *offset_x, double *offset_y);
d_declare_method(camera, set_center)(struct s_object *self, double center_x, double center_y);
d_declare_method(camera, get_center)(struct s_object *self, double *center_x, double *center_y);
d_declare_method(camera, set_angle)(struct s_object *self, double angle);
d_declare_method(camera, get_angle)(struct s_object *self, double *angle);
d_declare_method(camera, set_zoom)(struct s_object *self, double zoom);
d_declare_method(camera, get_zoom)(struct s_object *self, double *zoom);
d_declare_method(camera, add_controller)(struct s_object *self, struct s_object *label, struct s_object *camera_controller);
d_declare_method(camera, get_controller)(struct s_object *self, struct s_object *label);
d_declare_method(camera, del_controller)(struct s_object *self, struct s_object *label);
d_declare_method(camera, get_surface)(struct s_object *self, enum e_environment_surfaces *surface);
d_declare_method(camera, initialize_context)(struct s_object *self, struct s_object *environment);
d_declare_method(camera, finalize_context)(struct s_object *self, struct s_object *environment);
d_declare_method(camera, delete)(struct s_object *self, struct s_camera_attributes *attributes);
#endif