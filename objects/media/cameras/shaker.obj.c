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
#include "shaker.obj.h"
struct s_shaker_attributes *p_shaker_alloc(struct s_object *self, unsigned short int affected_axis) {
  struct s_shaker_attributes *result = d_prepare(self, shaker);
  f_mutex_new(self);                                        /* inherit */
  f_memory_new(self);                                       /* inherit */
  f_camera_controller_new(self, affected_axis, d_false);    /* inherit */
  return result;
}
struct s_object *f_shaker_new(struct s_object *self, unsigned short int affected_axis) {
  struct s_shaker_attributes *attributes = p_shaker_alloc(self, affected_axis);
  attributes->maximum_translation_x = d_shaker_default_maximum_translation;
  attributes->maximum_translation_y = d_shaker_default_maximum_translation;
  attributes->maximum_angle = d_shaker_default_maximum_angle;
  return self;
}
d_define_method(shaker, set_maximum_translation_x)(struct s_object *self, double translation_x) {
  d_using(shaker);
  shaker_attributes->maximum_translation_x = translation_x;
  return self;
}
d_define_method(shaker, set_maximum_translation_y)(struct s_object *self, double translation_y) {
  d_using(shaker);
  shaker_attributes->maximum_translation_y = translation_y;
  return self;
}
d_define_method(shaker, set_maximum_angle)(struct s_object *self, double angle) {
  d_using(shaker);
  shaker_attributes->maximum_angle = angle;
  return self;
}
d_define_method(shaker, set_trauma_level)(struct s_object *self, double trauma_level) {
  d_using(shaker);
  shaker_attributes->trauma_level = trauma_level;
  if (shaker_attributes->trauma_level > 1.0)
    shaker_attributes->trauma_level = 1.0;
  else if (shaker_attributes->trauma_level < 0.0)
    shaker_attributes->trauma_level = 0.0;
  return self; 
}

d_define_method(shaker, update)(struct s_object *self, double *screen_position_x, double *screen_position_y, double *screen_w, double *screen_h,
    double *scene_reference_w, double *scene_reference_h, double *scene_offset_x, double *scene_offset_y, double *scene_center_x, double *scene_center_y,
    double *camera_angle, double *scene_zoom) {
  d_using(shaker);
  d_using(camera_controller);
  if (shaker_attributes->trauma_level > 0) {
    if ((camera_controller_attributes->affected_axis & e_camera_controller_axis_x) == e_camera_controller_axis_x)
      *scene_offset_x = (*scene_offset_x + (shaker_attributes->maximum_translation_x * shaker_attributes->trauma_level * 
            ((rand()/((double)RAND_MAX/2.0))-1.0)));
    if ((camera_controller_attributes->affected_axis & e_camera_controller_axis_y) == e_camera_controller_axis_y)
      *scene_offset_y = (*scene_offset_y + (shaker_attributes->maximum_translation_y * shaker_attributes->trauma_level * 
            ((rand()/((double)RAND_MAX/2.0))-1.0)));
    *camera_angle = (*camera_angle + (shaker_attributes->maximum_angle * shaker_attributes->trauma_level * 
            ((rand()/((double)RAND_MAX/2.0))-1.0)));
  }
  return self;
}
d_define_class(shaker) {d_hook_method(shaker, e_flag_public, set_maximum_translation_x),
  d_hook_method(shaker, e_flag_public, set_maximum_translation_y),
  d_hook_method(shaker, e_flag_public, set_maximum_angle),
  d_hook_method(shaker, e_flag_public, set_trauma_level),
  d_hook_method_override(shaker, e_flag_public, camera_controller, update),
  d_hook_method_tail};
