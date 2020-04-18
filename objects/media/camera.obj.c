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
#include "camera.obj.h"
#include "camera_controller.obj.h"
struct s_camera_attributes *p_camera_alloc(struct s_object *self) {
  struct s_camera_attributes *result = d_prepare(self, camera);
  f_mutex_new(self);    /* inherit */
  f_memory_new(self);   /* inherit */
  return result;
}
struct s_object *f_camera_new(struct s_object *self, double screen_offset_x, double screen_offset_y, double screen_width, double screen_height,
    enum e_environment_surfaces surface, struct s_object *environment) {
  struct s_camera_attributes *attributes = p_camera_alloc(self);
  if ((attributes->controllers = f_map_new(d_new(map)))) {
    d_call(self, m_camera_recalculate_texture, screen_offset_x, screen_offset_y, screen_width, screen_height, surface, environment);
    attributes->scene_reference_w = screen_width;
    attributes->scene_reference_h = screen_height;
    attributes->scene_zoom = 1.0;
    attributes->maximum_offset_x = NAN;
    attributes->maximum_offset_y = NAN;
    attributes->minimum_offset_x = NAN;
    attributes->minimum_offset_y = NAN;
  } else
    d_die(d_error_malloc);
  return self;
}
d_define_method(camera, set_size)(struct s_object *self, double width, double height) {
  d_using(camera);
  camera_attributes->screen_w = width;
  camera_attributes->screen_h = height;
  return self;
}
d_define_method(camera, get_size)(struct s_object *self, double *width, double *height) {
  d_using(camera);
  *width = camera_attributes->screen_w;
  *height = camera_attributes->screen_h;
  return self;
}
d_define_method(camera, set_reference)(struct s_object *self, double width, double height) {
  d_using(camera);
  camera_attributes->scene_reference_w = width;
  camera_attributes->scene_reference_h = height;
  return self;
}
d_define_method(camera, get_reference)(struct s_object *self, double *width, double *height) {
  d_using(camera);
  *width = camera_attributes->scene_reference_w;
  *height = camera_attributes->scene_reference_h;
  return self;
}
d_define_method(camera, set_position)(struct s_object *self, double position_x, double position_y) {
  d_using(camera);
  camera_attributes->screen_position_x = position_x;
  camera_attributes->screen_position_y = position_y;
  return self;
}
d_define_method(camera, get_position)(struct s_object *self, double *position_x, double *position_y) {
  d_using(camera);
  *position_x = camera_attributes->screen_position_x;
  *position_y = camera_attributes->screen_position_y;
  return self;
}
d_define_method(camera, set_boundaries)(struct s_object *self, double maximum_offset_x, double maximum_offset_y, 
    double minimum_offset_x, double minimum_offset_y) {
  d_using(camera);
  camera_attributes->maximum_offset_x = maximum_offset_x;
  camera_attributes->maximum_offset_y = maximum_offset_y;
  camera_attributes->minimum_offset_x = minimum_offset_x;
  camera_attributes->minimum_offset_y = minimum_offset_y;
  return self;
}
d_define_method(camera, set_offset)(struct s_object *self, double offset_x, double offset_y) {
  d_using(camera);
  camera_attributes->scene_offset_x = offset_x;
  camera_attributes->scene_offset_y = offset_y;
  camera_attributes->original_scene_offset_x = offset_x;
  camera_attributes->original_scene_offset_y = offset_y;
  return self;
}
d_define_method(camera, get_offset)(struct s_object *self, double *offset_x, double *offset_y) {
  d_using(camera);
  *offset_x = camera_attributes->scene_offset_x;
  *offset_y = camera_attributes->scene_offset_y;
  return self;
}
d_define_method(camera, set_center)(struct s_object *self, double center_x, double center_y) {
  d_using(camera);
  camera_attributes->scene_center_x = center_x;
  camera_attributes->scene_center_y = center_y;
  return self;
}
d_define_method(camera, get_center)(struct s_object *self, double *center_x, double *center_y) {
  d_using(camera);
  *center_x = camera_attributes->scene_center_x;
  *center_y = camera_attributes->scene_center_y;
  return self;
}
d_define_method(camera, set_angle)(struct s_object *self, double angle) {
  d_using(camera);
  camera_attributes->camera_angle = angle;
  camera_attributes->original_camera_angle = angle;
  return self;
}
d_define_method(camera, get_angle)(struct s_object *self, double *angle) {
  d_using(camera);
  *angle = camera_attributes->camera_angle;
  return self;
}
d_define_method(camera, set_zoom)(struct s_object *self, double zoom) {
  d_using(camera);
  camera_attributes->scene_zoom = zoom;
  camera_attributes->original_scene_zoom = zoom;
  return self;
}
d_define_method(camera, get_zoom)(struct s_object *self, double *zoom) {
  d_using(camera);
  *zoom = camera_attributes->scene_zoom;
  return self;
}
d_define_method(camera, add_controller)(struct s_object *self, struct s_object *label, struct s_object *camera_controller) {
  d_using(camera);
  d_call(camera_attributes->controllers, m_map_insert, label, camera_controller);
  return self;
}
d_define_method(camera, get_controller)(struct s_object *self, struct s_object *label) {
  d_using(camera);
  return d_call(camera_attributes->controllers, m_map_find, label);
}
d_define_method(camera, del_controller)(struct s_object *self, struct s_object *label) {
  d_using(camera);
  return d_call(camera_attributes->controllers, m_map_remove, label);
}
d_define_method(camera, get_surface)(struct s_object *self, enum e_environment_surfaces *surface) {
  d_using(camera);
  *surface = camera_attributes->surface;
  return self;
}
d_define_method(camera, validate)(struct s_object *self) {
  d_using(camera);
  if ((camera_attributes->maximum_offset_x == camera_attributes->maximum_offset_x) && 
      (camera_attributes->scene_offset_x > camera_attributes->maximum_offset_x))
    camera_attributes->scene_offset_x = camera_attributes->maximum_offset_x;
  if ((camera_attributes->maximum_offset_y == camera_attributes->maximum_offset_y) && 
      (camera_attributes->scene_offset_y > camera_attributes->maximum_offset_y))
    camera_attributes->scene_offset_y = camera_attributes->maximum_offset_y;
  if ((camera_attributes->minimum_offset_x == camera_attributes->minimum_offset_x) && 
      (camera_attributes->scene_offset_x < camera_attributes->minimum_offset_x))
    camera_attributes->scene_offset_x = camera_attributes->minimum_offset_x;
  if ((camera_attributes->minimum_offset_y == camera_attributes->minimum_offset_y) && 
      (camera_attributes->scene_offset_y < camera_attributes->minimum_offset_y))
    camera_attributes->scene_offset_y = camera_attributes->minimum_offset_y;
  return self;
}
d_define_method(camera, recalculate_texture)(struct s_object *self, double screen_offset_x, double screen_offset_y, double screen_width, double screen_height,
    enum e_environment_surfaces surface, struct s_object *environment) {
  d_using(camera);
  struct s_environment_attributes *environment_attributes = d_cast(environment, environment);
  size_t size = (environment_attributes->current_w * environment_attributes->current_h * 4 /* RGBA */);
  char buffer[d_string_buffer_size];
  double ratio_previous_x = 1.0, ratio_previous_y = 1.0;
  struct s_object *result = self;
  if ((environment_attributes->current_w != camera_attributes->environment_reference_w) ||
      (environment_attributes->current_h != camera_attributes->environment_reference_h)) { 
    if (camera_attributes->memblock)
      d_free(camera_attributes->memblock);
    if (camera_attributes->destination)
      SDL_DestroyTexture(camera_attributes->destination);
    if ((camera_attributes->memblock = (unsigned char *)d_malloc(size))) {
      if ((camera_attributes->destination = SDL_CreateTexture(environment_attributes->renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, 
              environment_attributes->current_w, environment_attributes->current_h))) {
        if ((camera_attributes->environment_reference_w > 0.0) && (camera_attributes->environment_reference_h > 0.0)) {
          ratio_previous_x = (environment_attributes->current_w / camera_attributes->environment_reference_w);
          ratio_previous_y = (environment_attributes->current_h / camera_attributes->environment_reference_h);
        }
        camera_attributes->environment_reference_w = environment_attributes->current_w;
        camera_attributes->environment_reference_h = environment_attributes->current_h;
        camera_attributes->screen_position_x = screen_offset_x;
        camera_attributes->screen_position_y = screen_offset_y;
        camera_attributes->screen_w = (screen_width * ratio_previous_x);
        camera_attributes->screen_h = (screen_height * ratio_previous_y);
        camera_attributes->surface = surface;
        SDL_UpdateTexture(camera_attributes->destination, NULL, camera_attributes->memblock, (environment_attributes->current_w * 4 /* RGBA */));
        SDL_SetTextureBlendMode(camera_attributes->destination, SDL_BLENDMODE_ADD);
      } else {
        snprintf(buffer, d_string_buffer_size, "ungenerable texture from empty bitmap (size %.02f %.02f)", environment_attributes->current_w,
            environment_attributes->current_h);
        d_throw(v_exception_texture, buffer);
      }
    } else
      d_die(d_error_malloc);
    result = NULL;
  }
  return result;
}
d_define_method(camera, initialize_context)(struct s_object *self, struct s_object *environment) {
  d_using(camera);
  struct s_environment_attributes *environment_attributes = d_cast(environment, environment);
  struct s_camera_controller_attributes *camera_controller_attributes;
  struct s_object *camera_controller_object;
  struct s_object *string_object;
  if (d_call(self, m_camera_recalculate_texture, camera_attributes->scene_offset_x, camera_attributes->scene_offset_y, camera_attributes->screen_w,
        camera_attributes->screen_h, camera_attributes->surface, environment)) {
    camera_attributes->scene_offset_x = camera_attributes->original_scene_offset_x;
    camera_attributes->scene_offset_y = camera_attributes->original_scene_offset_y;
    camera_attributes->scene_zoom = camera_attributes->original_scene_zoom;
    camera_attributes->camera_angle = camera_attributes->original_camera_angle;
    d_map_foreach(camera_attributes->controllers, camera_controller_object, string_object) {
      camera_controller_attributes = d_cast(camera_controller_object, camera_controller);
      if (camera_controller_attributes->permanent_change) 
        d_call(camera_controller_object, m_camera_controller_update, &(camera_attributes->screen_position_x), &(camera_attributes->screen_position_y),
            &(camera_attributes->screen_w), &(camera_attributes->screen_h), &(camera_attributes->scene_reference_w), &(camera_attributes->scene_reference_h),
            &(camera_attributes->scene_offset_x), &(camera_attributes->scene_offset_y), &(camera_attributes->scene_center_x), 
            &(camera_attributes->scene_center_y), &(camera_attributes->camera_angle), &(camera_attributes->scene_zoom));
    }
    camera_attributes->original_scene_offset_x = camera_attributes->scene_offset_x;
    camera_attributes->original_scene_offset_y = camera_attributes->scene_offset_y;
    camera_attributes->original_scene_zoom = camera_attributes->scene_zoom;
    camera_attributes->original_camera_angle = camera_attributes->camera_angle;
    d_map_foreach(camera_attributes->controllers, camera_controller_object, string_object) {
      camera_controller_attributes = d_cast(camera_controller_object, camera_controller);
      if (!camera_controller_attributes->permanent_change) 
        d_call(camera_controller_object, m_camera_controller_update, &(camera_attributes->screen_position_x), &(camera_attributes->screen_position_y),
            &(camera_attributes->screen_w), &(camera_attributes->screen_h), &(camera_attributes->scene_reference_w), &(camera_attributes->scene_reference_h),
            &(camera_attributes->scene_offset_x), &(camera_attributes->scene_offset_y), &(camera_attributes->scene_center_x), 
            &(camera_attributes->scene_center_y), &(camera_attributes->camera_angle), &(camera_attributes->scene_zoom));
    }
    d_call(self, m_camera_validate, NULL);
    d_miranda_lock(environment) {
      SDL_UpdateTexture(camera_attributes->destination, NULL, camera_attributes->memblock, (environment_attributes->current_w * 4 /* RGBA */));
      SDL_SetRenderTarget(environment_attributes->renderer, camera_attributes->destination);
    } d_miranda_unlock(environment);
  }
  return self;
}
d_define_method(camera, finalize_context)(struct s_object *self, struct s_object *environment) {
  d_using(camera);
  SDL_Rect source, destination;
  SDL_Point center;
  struct s_environment_attributes *environment_attributes = d_cast(environment, environment);
  source.x = 0;
  source.y = 0;
  source.w = camera_attributes->screen_w;
  source.h = camera_attributes->screen_h;
  destination.x = camera_attributes->screen_position_x;
  destination.y = camera_attributes->screen_position_y;
  destination.w = camera_attributes->screen_w;
  destination.h = camera_attributes->screen_h;
  center.x = (camera_attributes->screen_w / 2.0);
  center.y = (camera_attributes->screen_h / 2.0);
  d_miranda_lock(environment) {
    SDL_SetRenderTarget(environment_attributes->renderer, NULL);
    SDL_RenderCopyEx(environment_attributes->renderer, camera_attributes->destination, &source, &destination, camera_attributes->camera_angle, &center,
        (SDL_RendererFlip)e_drawable_flip_none);
  } d_miranda_unlock(environment);
  return self;
}
d_define_method(camera, delete)(struct s_object *self, struct s_camera_attributes *attributes) {
  d_delete(attributes->controllers);
  if (attributes->memblock)
    d_free(attributes->memblock);
  SDL_DestroyTexture(attributes->destination);
  return NULL;
}
d_define_class(camera) {d_hook_method(camera, e_flag_public, set_size),
  d_hook_method(camera, e_flag_public, get_size),
  d_hook_method(camera, e_flag_public, set_reference),
  d_hook_method(camera, e_flag_public, get_reference),
  d_hook_method(camera, e_flag_public, set_position),
  d_hook_method(camera, e_flag_public, get_position),
  d_hook_method(camera, e_flag_public, set_boundaries),
  d_hook_method(camera, e_flag_public, set_offset),
  d_hook_method(camera, e_flag_public, get_offset),
  d_hook_method(camera, e_flag_public, set_center),
  d_hook_method(camera, e_flag_public, get_center),
  d_hook_method(camera, e_flag_public, set_angle),
  d_hook_method(camera, e_flag_public, get_angle),
  d_hook_method(camera, e_flag_public, set_zoom),
  d_hook_method(camera, e_flag_public, get_zoom),
  d_hook_method(camera, e_flag_public, add_controller),
  d_hook_method(camera, e_flag_public, get_controller),
  d_hook_method(camera, e_flag_public, del_controller),
  d_hook_method(camera, e_flag_public, get_surface),
  d_hook_method(camera, e_flag_public, validate),
  d_hook_method(camera, e_flag_public, recalculate_texture),
  d_hook_method(camera, e_flag_public, initialize_context),
  d_hook_method(camera, e_flag_public, finalize_context),
  d_hook_delete(camera),
  d_hook_method_tail};
