/*
 * miranda
 * Copyright (C) 2016 Andrea Nardinocchi (andrea@nardinan.it)
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
#include "morphable.obj.h"
#include "camera.obj.h"
struct s_morphable_attributes *p_morphable_alloc(struct s_object *self) {
  struct s_morphable_attributes *result = d_prepare(self, morphable);
  /* abstract (no memory inheritance) */
  f_eventable_new(self);  /* inherit */
  return result;
}
struct s_object *f_morphable_new(struct s_object *self) {
  struct s_morphable_attributes *attributes = p_morphable_alloc(self);
  attributes = attributes;
  return self;
}
d_define_method(morphable, set_freedom_x)(struct s_object *self, t_boolean free) {
  d_using(morphable);
  morphable_attributes->freedom_x = free;
  return self;
}
d_define_method(morphable, set_freedom_y)(struct s_object *self, t_boolean free) {
  d_using(morphable);
  morphable_attributes->freedom_y = free;
  return self;
}
d_define_method(morphable, set_freedom_z)(struct s_object *self, t_boolean free) {
  d_using(morphable);
  morphable_attributes->freedom_z = free;
  return self;
}
d_define_method(morphable, set_visibility)(struct s_object *self, t_boolean visibility) {
  d_using(morphable);
  morphable_attributes->visible = visibility;
  return self;
}
d_define_method(morphable, update)(struct s_object *self, struct s_object *environment) {
  d_using(morphable);
  struct s_drawable_attributes *drawable_attributes = d_cast(self, drawable);
  struct s_environment_attributes *environment_attributes = d_cast(environment, environment);
  struct s_camera_attributes *camera_attributes;
  double position_x_self, position_y_self, position_x, position_y;
  int mouse_x, mouse_y;
  if (environment_attributes->current_camera) {
    camera_attributes = d_cast(environment_attributes->current_camera, camera);
    if (morphable_attributes->grabbed) {
      d_call(&(drawable_attributes->point_destination), m_point_get, &position_x_self, &position_y_self);
      SDL_GetMouseState(&mouse_x, &mouse_y);
      mouse_x = (mouse_x * camera_attributes->scene_reference_w) / camera_attributes->screen_w;
      mouse_y = (mouse_y * camera_attributes->scene_reference_h) / camera_attributes->screen_h;
      mouse_x = (mouse_x - camera_attributes->scene_offset_x);
      mouse_y = (mouse_y - camera_attributes->scene_offset_y);
      mouse_x = (mouse_x / camera_attributes->scene_zoom);
      mouse_y = (mouse_y / camera_attributes->scene_zoom);
      if ((morphable_attributes->offset_x != morphable_attributes->offset_x) && (morphable_attributes->offset_y != morphable_attributes->offset_y) &&
          (morphable_attributes->offset_z != morphable_attributes->offset_z)) {
        morphable_attributes->offset_x = (mouse_x - position_x_self);
        morphable_attributes->offset_y = (mouse_y - position_y_self);
        morphable_attributes->offset_z = drawable_attributes->zoom;
      } else {
        position_x = (mouse_x - morphable_attributes->offset_x);
        position_y = (mouse_y - morphable_attributes->offset_y);
        if (morphable_attributes->freedom_x)
          d_call(self, m_drawable_set_position_x, position_x);
        if (morphable_attributes->freedom_y)
          d_call(self, m_drawable_set_position_y, position_y);
        if (morphable_attributes->freedom_z)
          d_call(self, m_drawable_set_zoom, morphable_attributes->offset_z);
      }
    }
  }
  d_cast_return(d_drawable_return_last);
}
d_define_method_override(morphable, event)(struct s_object *self, struct s_object *environment, SDL_Event *current_event) {
  d_using(morphable);
  struct s_drawable_attributes *drawable_attributes = d_cast(self, drawable);
  struct s_exception *exception;
  int mouse_x, mouse_y;
  t_boolean mouse_inside = d_false;
  SDL_GetMouseState(&mouse_x, &mouse_y);
  if (morphable_attributes->visible) {
    drawable_attributes->flags &= ~e_drawable_kind_contour;
    if ((intptr_t) d_call(&(drawable_attributes->square_collision_box), m_square_inside_coordinates, (double) mouse_x, (double) mouse_y)) {
      mouse_inside = d_true;
      drawable_attributes->flags |= e_drawable_kind_contour;
    }
  }
  d_try
      {
        switch (current_event->type) {
          case SDL_MOUSEWHEEL:
            if (morphable_attributes->grabbed)
              morphable_attributes->offset_z += (current_event->wheel.y * d_morphable_z_factor);
            break;
          case SDL_MOUSEBUTTONUP:
            if (current_event->button.button == SDL_BUTTON_LEFT)
              morphable_attributes->grabbed = d_false;
            break;
          case SDL_MOUSEBUTTONDOWN:
            if (current_event->button.button == SDL_BUTTON_LEFT) {
              if ((mouse_inside) /* has been already calculated */||
                  ((intptr_t) d_call(&(drawable_attributes->square_collision_box), m_square_inside_coordinates, (double) mouse_x, (double) mouse_y))) {
                morphable_attributes->grabbed = d_true;
                morphable_attributes->offset_x = NAN;
                morphable_attributes->offset_y = NAN;
                morphable_attributes->offset_z = NAN;
              }
            }
        }
      }
    d_catch(exception)
      {
        d_exception_dump(stderr, exception);
        d_raise;
      }d_endtry;
  return self;
}
d_define_method(morphable, delete)(struct s_object *self, struct s_morphable_attributes *attributes) {
  return NULL;
}
d_define_class(morphable) {d_hook_method(morphable, e_flag_public, set_freedom_x),
                           d_hook_method(morphable, e_flag_public, set_freedom_y),
                           d_hook_method(morphable, e_flag_public, set_freedom_z),
                           d_hook_method(morphable, e_flag_public, set_visibility),
                           d_hook_method(morphable, e_flag_public, update),
                           d_hook_method_override(morphable, e_flag_public, eventable, event),
                           d_hook_delete(morphable),
                           d_hook_method_tail};
