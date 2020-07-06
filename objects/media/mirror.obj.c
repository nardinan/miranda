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
#include "mirror.obj.h"
#include "environment.obj.h"
#include "primitives.h"
struct s_mirror_attributes *p_mirror_alloc(struct s_object *self) {
  struct s_mirror_attributes *result = d_prepare(self, mirror);
  f_mutex_new(self);                              /* inherit */
  f_memory_new(self);                             /* inherit */
  f_drawable_new(self, e_drawable_kind_single);   /* inherit */
  return result;
}
struct s_object *f_mirror_new(struct s_object *self, double source_x, double source_y, double source_w, double source_h) {
  struct s_mirror_attributes *attributes = p_mirror_alloc(self);
  struct s_drawable_attributes *drawable_attributes = d_cast(self, drawable);
  f_point_new(d_use(&(attributes->point_source), point), 0, 0);
  f_point_new(d_use(&(attributes->point_normalized_source), point), 0, 0);
  d_call(&(attributes->point_source), m_point_set, source_x, source_y);
  d_call(&(drawable_attributes->point_dimension), m_point_set, source_w, source_h);
  return self;
}
d_define_method(mirror, set_source)(struct s_object *self, double x, double y) {
  d_using(mirror);
  d_call(&(mirror_attributes->point_source), m_point_set, x, y);
  return self;
}
d_define_method(mirror, set_source_x)(struct s_object *self, double x) {
  d_using(mirror);
  d_call(&(mirror_attributes->point_source), m_point_set_x, x);
  return self;
}
d_define_method(mirror, set_source_y)(struct s_object *self, double y) {
  d_using(mirror);
  d_call(&(mirror_attributes->point_source), m_point_set_y, y);
  return self;
}
d_define_method(mirror, update_texture)(struct s_object *self, struct s_object *environment) {
  d_using(mirror);
  struct s_drawable_attributes *drawable_attributes = d_cast(self, drawable);
  struct s_environment_attributes *environment_attributes = d_cast(environment, environment);
  if (!mirror_attributes->image)
    if (!(mirror_attributes->image = SDL_CreateTexture(environment_attributes->renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,
            environment_attributes->current_w, environment_attributes->current_h)))
      d_die(d_error_malloc);
  SDL_SetRenderTarget(environment_attributes->renderer, mirror_attributes->image);
      SDL_SetTextureColorMod(mirror_attributes->image, drawable_attributes->last_mask_R, 
          drawable_attributes->last_mask_G, drawable_attributes->last_mask_B);
    SDL_SetTextureAlphaMod(mirror_attributes->image, drawable_attributes->last_mask_A);
    SDL_SetTextureBlendMode(mirror_attributes->image, (SDL_BlendMode)drawable_attributes->last_blend);
  } else
    d_die(d_error_malloc);
}
d_define_method_override(mirror, draw)(struct s_object *self, struct s_object *environment) {
  d_using(mirror);
  double position_x, position_y, dimension_w, dimension_h, original_dimension_w, original_dimension_h, center_x, center_y;
  struct s_drawable_attributes *drawable_attributes = d_cast(self, drawable);
  struct s_environment_attributes *environment_attributes = d_cast(environment, environment);
  SDL_Rect source, destination;
  SDL_Point center;
  d_call(self, m_mirror_update_texture, environment);
  if (mirror_attributes->image) {
    d_call(&(drawable_attributes->point_normalized_destination), m_point_get, &position_x, &position_y);
    d_call(&(drawable_attributes->point_normalized_dimension), m_point_get, &dimension_w, &dimension_h);
    d_call(&(drawable_attributes->point_dimension), m_point_get, &original_dimension_w, &original_dimension_h);
    d_call(&(drawable_attributes->point_normalized_center), m_point_get, &center_x, &center_y);
    source.x = 0;
    source.y = 0;
    source.w = original_dimension_w;
    source.h = original_dimension_h;
    destination.x = position_x;
    destination.y = position_y;
    destination.w = dimension_w;
    destination.h = dimension_h;
    center.x = center_x;
    center.y = center_y;

    printf("print me print me %.02f %.02f (%.02f %.02f)\n", position_x, position_y, dimension_w, dimension_h);
    f_primitive_fill_circle(environment_attributes->renderer, position_x, position_y, 10, 255, 0, 0, 255);
    f_primitive_fill_circle(environment_attributes->renderer, position_x + dimension_w, position_y + dimension_h, 10, 0, 255, 0, 255);
    d_miranda_lock(environment) {
      SDL_RenderCopyEx(environment_attributes->renderer, mirror_attributes->image, &source, &destination, drawable_attributes->angle, &center,
          (SDL_RendererFlip)drawable_attributes->flip);
    } d_miranda_unlock(environment);
    if ((drawable_attributes->flags & e_drawable_kind_contour) == e_drawable_kind_contour)
      d_call(self, m_drawable_draw_contour, environment);
  }
  d_cast_return(d_drawable_return_last);
}
d_define_method_override(mirror, set_maskRGB)(struct s_object *self, unsigned int red, unsigned int green, unsigned int blue) {
  d_using(mirror);
  struct s_drawable_attributes *drawable_attributes = d_cast(self, drawable);
  drawable_attributes->last_mask_R = (double)red;
  drawable_attributes->last_mask_G = (double)green;
  drawable_attributes->last_mask_B = (double)blue;
  if (mirror_attributes->image)
    SDL_SetTextureColorMod(mirror_attributes->image, red, green, blue);
  return self;
}
d_define_method_override(mirror, set_maskA)(struct s_object *self, unsigned int alpha) {
  d_using(mirror);
  struct s_drawable_attributes *drawable_attributes = d_cast(self, drawable);
  drawable_attributes->last_mask_A = alpha;
  if (mirror_attributes->image)
    SDL_SetTextureAlphaMod(mirror_attributes->image, alpha);
  return self;
}
d_define_method_override(mirror, set_blend)(struct s_object *self, enum e_drawable_blends blend) {
  d_using(mirror);
  struct s_drawable_attributes *drawable_attributes = d_cast(self, drawable);
  drawable_attributes->last_blend = blend;
  if (mirror_attributes->image)
    SDL_SetTextureBlendMode(mirror_attributes->image, (SDL_BlendMode)blend);
  return self;
}
d_define_method(mirror, delete)(struct s_object *self, struct s_mirror_attributes *attributes) {
  d_delete(&(attributes->point_source));
  d_delete(&(attributes->point_normalized_source));
  if (attributes->image)
    SDL_DestroyTexture(attributes->image);
  return NULL;
}
d_define_class(mirror) { d_hook_method(mirror, e_flag_public, set_source),
  d_hook_method(mirror, e_flag_public, set_source_x),
  d_hook_method(mirror, e_flag_public, set_source_y),
  d_hook_method(mirror, e_flag_private, update_texture),
  d_hook_method_override(mirror, e_flag_public, drawable, draw),
  d_hook_method_override(mirror, e_flag_public, drawable, set_maskRGB),
  d_hook_method_override(mirror, e_flag_public, drawable, set_maskA),
  d_hook_method_override(mirror, e_flag_public, drawable, set_blend),
  d_hook_delete(mirror),
  d_hook_method_tail};
