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
#include "lights.obj.h"
#include "camera.obj.h"
void p_lights_modulator_flickering(struct s_lights_emitter *emitter) {
  clock_t current_clock = clock();
  double default_factor = 0.1, new_intensity = ((sin(((double)current_clock) * default_factor) + 1.0) / 2.0) * emitter->original_intensity;
  emitter->current_intensity = new_intensity;
}
void p_lights_modulator_pulsing_fast(struct s_lights_emitter *emitter) {
  struct timespec current_time;
  int seconds_modulator = 1; /* this determine the amount of seconds the pulse should last */
  double new_intensity, factor_component;
  clock_gettime(CLOCK_MONOTONIC_RAW, &current_time);
  factor_component = ((((double)(current_time.tv_sec % seconds_modulator)) * 1000000.0) + 
    (current_time.tv_nsec / 1000.0))/(1000000.0 + ((seconds_modulator - 1) * 1000000.0));
  new_intensity = ((sin((360.0 * factor_component) * d_math_radians_conversion) + 1.0)/2.0) * (double)emitter->original_intensity;
  emitter->current_intensity = new_intensity;
}
void p_lights_modulator_pulsing_normal(struct s_lights_emitter *emitter) {
  struct timespec current_time;
  int seconds_modulator = 2; /* this determine the amount of seconds the pulse should last */
  double new_intensity, factor_component;
  clock_gettime(CLOCK_MONOTONIC_RAW, &current_time);
  factor_component = ((((double)(current_time.tv_sec % seconds_modulator)) * 1000000.0) + 
    (current_time.tv_nsec / 1000.0))/(1000000.0 + ((seconds_modulator - 1) * 1000000.0));
  new_intensity = ((sin((360.0 * factor_component) * d_math_radians_conversion) + 1.0)/2.0) * (double)emitter->original_intensity;
  emitter->current_intensity = new_intensity;
}
void p_lights_modulator_pulsing_slow(struct s_lights_emitter *emitter) {
  struct timespec current_time;
  int seconds_modulator = 4; /* this determine the amount of seconds the pulse should last */
  double new_intensity, factor_component;
  clock_gettime(CLOCK_MONOTONIC_RAW, &current_time);
  factor_component = ((((double)(current_time.tv_sec % seconds_modulator)) * 1000000.0) + 
    (current_time.tv_nsec / 1000.0))/(1000000.0 + ((seconds_modulator - 1) * 1000000.0));
  new_intensity = ((sin((360.0 * factor_component) * d_math_radians_conversion) + 1.0)/2.0) * (double)emitter->original_intensity;
  emitter->current_intensity = new_intensity;
}
struct s_lights_attributes *p_lights_alloc(struct s_object *self) {
  struct s_lights_attributes *result = d_prepare(self, lights);
  f_mutex_new(self);                             /* inherit */
  f_memory_new(self);                            /* inherit */
  f_drawable_new(self, e_drawable_kind_single);  /* inherit */
  return result;
}
struct s_object *f_lights_new(struct s_object *self, unsigned char intensity, struct s_object *environment) {
  struct s_lights_attributes *attributes = p_lights_alloc(self);
  struct s_environment_attributes *environment_attributes = d_cast(environment, environment);
  size_t size = (environment_attributes->current_w * environment_attributes->current_h * 4 /* RGBA */);
  char buffer[d_string_buffer_size];
  attributes->intensity = intensity;
  if ((attributes->memblock = (unsigned char *)d_malloc(size))) {
    memset(attributes->memblock, intensity, size);
    if ((attributes->background =
          SDL_CreateTexture(environment_attributes->renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, environment_attributes->current_w,
            environment_attributes->current_h))) {
      attributes->current_w = environment_attributes->current_w;
      attributes->current_h = environment_attributes->current_h;
      attributes->memblock_size = size;
      SDL_UpdateTexture(attributes->background, NULL, attributes->memblock, (environment_attributes->current_w * 4 /* RGBA */));
      SDL_SetTextureBlendMode(attributes->background, SDL_BLENDMODE_MOD);
    } else {
      snprintf(buffer, d_string_buffer_size, "ungenerable texture from empty bitmap (size %.02f %.02f)", environment_attributes->current_w,
          environment_attributes->current_h);
      d_throw(v_exception_texture, buffer);
    }
  } else
    d_die(d_error_malloc);
  return self;
}
d_define_method(lights, add_light)(struct s_object *self, unsigned char intensity, unsigned char mask_R, unsigned char mask_G, unsigned char mask_B,
    double radius, t_lights_intensity_modulator modulator, struct s_object *mask, struct s_object *reference, enum e_drawable_alignments alignment,
    t_boolean project_shadows) {
  d_using(lights);
  struct s_lights_emitter *current_emitter = NULL;
  if ((current_emitter = (struct s_lights_emitter *)d_malloc(sizeof(struct s_lights_emitter)))) {
    current_emitter->current_intensity = current_emitter->original_intensity = intensity;
    current_emitter->current_mask_R = current_emitter->original_mask_R = mask_R;
    current_emitter->current_mask_G = current_emitter->original_mask_G = mask_G;
    current_emitter->current_mask_B = current_emitter->original_mask_B = mask_B;
    current_emitter->current_radius = radius;
    current_emitter->modulator = modulator;
    current_emitter->alignment = alignment;
    current_emitter->reference = d_retain(reference);
    current_emitter->mask = d_retain(mask);
    current_emitter->project_shadows = project_shadows;
    d_call(current_emitter->mask, m_drawable_set_blend, e_drawable_blend_add);
    d_call(current_emitter->mask, m_drawable_add_flags,
        (e_drawable_kind_do_not_normalize_environment_zoom | e_drawable_kind_do_not_normalize_camera | e_drawable_kind_do_not_normalize_reference_ratio));
    f_list_append(&(lights_attributes->emitters), (struct s_list_node *)current_emitter, e_list_insert_head);
  } else
    d_die(d_error_malloc);
  d_cast_return(current_emitter);
}
d_define_method(lights, del_light)(struct s_object *self, struct s_lights_emitter *emitter) {
  d_using(lights);
  if (d_list_node_in(&(lights_attributes->emitters), (struct s_list_node *)emitter)) {
    f_list_delete(&(lights_attributes->emitters), (struct s_list_node *)emitter);
    if (emitter->mask)
      d_delete(emitter->mask);
    if (emitter->reference)
      d_delete(emitter->reference);
    d_free(emitter);
  }
  return self;
}
d_define_method(lights, get_light)(struct s_object *self, struct s_object *reference) {
  d_using(lights);
  struct s_lights_emitter *current_emitter, *result = NULL;
  d_foreach(&(lights_attributes->emitters), current_emitter, struct s_lights_emitter) {
    if (current_emitter->reference == reference) {
      result = current_emitter;
      break;
    }
  }
  d_cast_return(result);
}
d_define_method(lights, clear)(struct s_object *self) {
  d_using(lights);
  struct s_lights_emitter *current_emitter;
  while ((current_emitter = (struct s_lights_emitter *)lights_attributes->emitters.head)) {
    f_list_delete(&(lights_attributes->emitters), (struct s_list_node *)lights_attributes->emitters.head);
    if (current_emitter->mask)
      d_delete(current_emitter->mask);
    if (current_emitter->reference)
      d_delete(current_emitter->reference);
    d_free(current_emitter);
  }
  return self;
}
d_define_method(lights, set_intensity)(struct s_object *self, unsigned char intensity) {
  d_using(lights);
  lights_attributes->intensity = intensity;
  if (lights_attributes->memblock)
    memset(lights_attributes->memblock, intensity, lights_attributes->memblock_size);
  return self;
}
d_define_method(lights, get_intensity)(struct s_object *self) {
  d_using(lights);
  d_cast_return(lights_attributes->intensity);
}
d_define_method(lights, get_affecting_lights)(struct s_object *self, struct s_object *drawable, struct s_list *container, struct s_object *environment,
    t_boolean shadows_generation) {
  d_using(lights);
  struct s_environment_attributes *environment_attributes = d_cast(environment, environment);
  struct s_drawable_attributes *drawable_other_attributes = d_cast(drawable, drawable), *drawable_core_attributes;
  struct s_camera_attributes *camera_attributes = d_cast(environment_attributes->current_camera, camera);
  struct s_lights_emitter *current_emitter;
  struct s_lights_emitter_description *selected_emitter;
  double position_x, position_y, dimension_w, dimension_h, light_position_x, light_position_y, light_width, light_height, drawable_principal_point_x,
         drawable_principal_point_y;
  d_call(drawable, m_drawable_get_scaled_principal_point, &drawable_principal_point_x, &drawable_principal_point_y);
  d_foreach(&(lights_attributes->emitters), current_emitter, struct s_lights_emitter)
    if ((current_emitter->mask) && (current_emitter->reference) && ((!shadows_generation) || (current_emitter->project_shadows))) {
      drawable_core_attributes = d_cast(current_emitter->mask, drawable);
      d_call(current_emitter->mask, m_drawable_copy_geometry, current_emitter->reference, current_emitter->alignment);
      d_call(current_emitter->mask, m_drawable_get_position, &position_x, &position_y);
      d_call(current_emitter->mask, m_drawable_get_dimension, &dimension_w, &dimension_h);
      position_x -= (dimension_w / 2.0);
      position_y -= (dimension_h / 2.0);
      d_call(current_emitter->mask, m_drawable_set_position, position_x, position_y);
      d_call(current_emitter->mask, m_drawable_set_center_alignment, e_drawable_alignment_centered);
      d_call(current_emitter->mask, m_drawable_set_zoom, (current_emitter->current_radius * camera_attributes->scene_zoom));
      d_call(current_emitter->mask, m_drawable_normalize_scale, camera_attributes->scene_reference_w, camera_attributes->scene_reference_h,
          camera_attributes->scene_offset_x, camera_attributes->scene_offset_y, camera_attributes->scene_center_x, camera_attributes->scene_center_y,
          camera_attributes->screen_w, camera_attributes->screen_h, camera_attributes->scene_zoom);
      d_call(current_emitter->mask, m_drawable_get_scaled_position, &light_position_x, &light_position_y);
      d_call(current_emitter->mask, m_drawable_get_scaled_dimension, &light_width, &light_height);
      /* now we need to check the distance between the center of the light and the target, to see if it is less or the same of the zoom */
      if (((intptr_t)d_call(&(drawable_core_attributes->square_collision_box), m_square_collision, &(drawable_other_attributes->square_collision_box)))) {
        if ((selected_emitter = (struct s_lights_emitter_description *)d_malloc(sizeof(struct s_lights_emitter_description)))) {
          selected_emitter->position_x = (light_position_x + (light_width / 2.0));
          selected_emitter->position_y = (light_position_y + (light_height / 2.0));
          selected_emitter->radius = (d_math_max(light_width, light_height)/2.0);
          selected_emitter->intensity = current_emitter->current_intensity;
          selected_emitter->mask_R = current_emitter->original_mask_R;
          selected_emitter->mask_G = current_emitter->original_mask_G;
          selected_emitter->mask_B = current_emitter->original_mask_B;
          selected_emitter->distance = f_math_sqrt(d_point_square_distance(drawable_principal_point_x, drawable_principal_point_y,
                selected_emitter->position_x, selected_emitter->position_y), d_math_default_precision);
          f_list_append(container, (struct s_list_node *)selected_emitter, e_list_insert_head);
        } else
          d_die(d_error_malloc);
      }
    }
  return self;
}
d_define_method_override(lights, draw)(struct s_object *self, struct s_object *environment) {
  d_using(lights);
  struct s_environment_attributes *environment_attributes = d_cast(environment, environment);
  struct s_drawable_attributes *drawable_attributes = d_cast(self, drawable);
  struct s_camera_attributes *camera_attributes = d_cast(environment_attributes->current_camera, camera);
  struct s_lights_emitter *current_emitter;
  size_t size;
  double intensity_modifier, position_x, position_y, dimension_w, dimension_h;
  SDL_Texture *previous_target;
  SDL_Rect source, destination;
  SDL_Point center;
  if ((environment_attributes->current_w != lights_attributes->current_w) || (environment_attributes->current_h != lights_attributes->current_h)) {
    d_free(lights_attributes->memblock);
    size = (environment_attributes->current_w * environment_attributes->current_h * 4 /* RGBA */);
    if ((lights_attributes->memblock = (unsigned char *)d_malloc(size))) {
      memset(lights_attributes->memblock, lights_attributes->intensity, size);
      lights_attributes->current_w = environment_attributes->current_w;
      lights_attributes->current_h = environment_attributes->current_h;
      lights_attributes->memblock_size = size;
    } else
      d_die(d_error_malloc);
  }
  source.x = 0;
  source.y = 0;
  source.w = lights_attributes->current_w;
  source.h = lights_attributes->current_h;
  destination.x = 0;
  destination.y = 0;
  destination.w = lights_attributes->current_w;
  destination.h = lights_attributes->current_h;
  center.x = (lights_attributes->current_w / 2.0);
  center.y = (lights_attributes->current_h / 2.0);
  SDL_UpdateTexture(lights_attributes->background, NULL, lights_attributes->memblock, (environment_attributes->current_w * 4 /* RGBA */));
  SDL_SetTextureAlphaMod(lights_attributes->background, 255);
  d_miranda_lock(environment) {
    previous_target = SDL_GetRenderTarget(environment_attributes->renderer);
    SDL_SetRenderTarget(environment_attributes->renderer, lights_attributes->background);
  } d_miranda_unlock(environment);
  d_foreach(&(lights_attributes->emitters), current_emitter, struct s_lights_emitter)
    if ((current_emitter->mask) && (current_emitter->reference)) {
      /* since the mask is a single object shared between all the emitters stored into the list, we should perform the normalization here, in the draw method */
      d_call(current_emitter->mask, m_drawable_copy_geometry, current_emitter->reference, current_emitter->alignment);
      d_call(current_emitter->mask, m_drawable_get_position, &position_x, &position_y);
      d_call(current_emitter->mask, m_drawable_get_dimension, &dimension_w, &dimension_h);
      position_x -= (dimension_w / 2.0);
      position_y -= (dimension_h / 2.0);
      d_call(current_emitter->mask, m_drawable_set_position, position_x, position_y);
      d_call(current_emitter->mask, m_drawable_set_center_alignment, e_drawable_alignment_centered);
      d_call(current_emitter->mask, m_drawable_set_zoom, (current_emitter->current_radius * camera_attributes->scene_zoom));
      if (d_call(current_emitter->mask, m_drawable_normalize_scale, camera_attributes->scene_reference_w, camera_attributes->scene_reference_h,
            camera_attributes->scene_offset_x, camera_attributes->scene_offset_y, camera_attributes->scene_center_x, camera_attributes->scene_center_y,
            camera_attributes->screen_w, camera_attributes->screen_h, camera_attributes->scene_zoom)) {
        d_call(current_emitter->mask, m_drawable_get_scaled_position, &(current_emitter->last_normalized_x), &(current_emitter->last_normalized_y));
        d_call(current_emitter->mask, m_drawable_get_scaled_dimension, &(current_emitter->last_normalized_w), &(current_emitter->last_normalized_h));
        if (current_emitter->modulator)
          current_emitter->modulator(current_emitter);
        intensity_modifier = (current_emitter->current_intensity / 255.0);
        d_call(current_emitter->mask, m_drawable_set_maskRGB, (unsigned int)(current_emitter->current_mask_R * intensity_modifier),
            (unsigned int)(current_emitter->current_mask_G * intensity_modifier), (unsigned int)(current_emitter->current_mask_B * intensity_modifier));
        while (d_call(current_emitter->mask, m_drawable_draw, environment) != d_drawable_return_last);
      }
    }
  d_miranda_lock(environment) {
    SDL_SetRenderTarget(environment_attributes->renderer, previous_target);
    SDL_RenderCopyEx(environment_attributes->renderer, lights_attributes->background, &source, &destination, 0.0, &center, SDL_FLIP_NONE);
  } d_miranda_unlock(environment);
  if ((drawable_attributes->flags & e_drawable_kind_contour) == e_drawable_kind_contour)
    d_call(self, m_drawable_draw_contour, environment);
  d_cast_return(d_drawable_return_last);
}
d_define_method_override(lights, draw_contour)(struct s_object *self, struct s_object *environment) {
  d_using(lights);
  SDL_Rect rectangle_forward, rectangle_background;
  struct s_environment_attributes *environment_attributes = d_cast(environment, environment);
  double position_x, position_y;
  struct s_lights_emitter *current_emitter;
  d_foreach(&(lights_attributes->emitters), current_emitter, struct s_lights_emitter) {
    position_x = current_emitter->last_normalized_x + (current_emitter->last_normalized_w / 2.0);
    position_y = current_emitter->last_normalized_y + (current_emitter->last_normalized_h / 2.0);
    rectangle_forward.x = (position_x - (d_lights_default_contour_width / 2.0));
    rectangle_forward.y = (position_y + d_lights_default_contour_radius);
    rectangle_forward.w = d_lights_default_contour_width;
    rectangle_forward.h = d_lights_default_contour_height;
    rectangle_background.x = rectangle_forward.x - 1;
    rectangle_background.y = rectangle_forward.y - 1;
    rectangle_background.w = rectangle_forward.w + 2;
    rectangle_background.h = rectangle_forward.h + 2;
    d_miranda_lock(environment) {
      SDL_SetRenderDrawColor(environment_attributes->renderer, d_lights_default_contour_color_background);
      SDL_RenderFillRect(environment_attributes->renderer, &rectangle_background);
      SDL_SetRenderDrawColor(environment_attributes->renderer, d_lights_default_contour_color_body);
      SDL_RenderFillRect(environment_attributes->renderer, &rectangle_forward);
      f_primitive_fill_circle(environment_attributes->renderer, position_x, position_y, (d_lights_default_contour_radius + 1),
          d_lights_default_contour_color_background);
      f_primitive_fill_circle(environment_attributes->renderer, position_x, position_y, d_lights_default_contour_radius, current_emitter->current_mask_R,
          current_emitter->current_mask_G, current_emitter->current_mask_B, 255.0);
    } d_miranda_unlock(environment);
  }
  return self;
}
d_define_method_override(lights, is_visible)(struct s_object *self, double current_w, double current_h) {
  /* is visible, every time */
  return self;
}
d_define_method(lights, delete)(struct s_object *self, struct s_lights_attributes *attributes) {
  struct s_lights_emitter *current_emitter;
  while ((current_emitter = (struct s_lights_emitter *)attributes->emitters.head)) {
    f_list_delete(&(attributes->emitters), (struct s_list_node *)attributes->emitters.head);
    if (current_emitter->mask)
      d_delete(current_emitter->mask);
    if (current_emitter->reference)
      d_delete(current_emitter->reference);
    d_free(current_emitter);
  }
  if (attributes->memblock)
    d_free(attributes->memblock);
  SDL_DestroyTexture(attributes->background);
  return NULL;
}
d_define_class(lights) {d_hook_method(lights, e_flag_public, add_light),
  d_hook_method(lights, e_flag_public, del_light),
  d_hook_method(lights, e_flag_public, get_light),
  d_hook_method(lights, e_flag_public, clear),
  d_hook_method(lights, e_flag_public, set_intensity),
  d_hook_method(lights, e_flag_public, get_intensity),
  d_hook_method(lights, e_flag_public, get_affecting_lights),
  d_hook_method_override(lights, e_flag_public, drawable, draw),
  d_hook_method_override(lights, e_flag_public, drawable, draw_contour),
  d_hook_method_override(lights, e_flag_public, drawable, is_visible),
  d_hook_delete(lights),
  d_hook_method_tail};
