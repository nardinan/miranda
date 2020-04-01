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
#include "illuminable_bitmap.obj.h"
struct s_illuminable_bitmap_attributes *p_illuminable_bitmap_alloc(struct s_object *self, struct s_object *stream, struct s_object *environment) {
  struct s_illuminable_bitmap_attributes *result = d_prepare(self, illuminable_bitmap);
  f_bitmap_new(self, stream, environment); /* inherit */
  return result;
}
struct s_object *f_illuminable_bitmap_new(struct s_object *self, struct s_object *stream, struct s_object *environment) {
  struct s_illuminable_bitmap_attributes *attributes = p_illuminable_bitmap_alloc(self, stream, environment);
  struct s_stream_attributes *stream_attributes;
  unsigned int index;
  if ((self->flags & e_flag_allocated) == e_flag_allocated)
    if ((stream_attributes = d_cast(stream, stream)))
      d_sign_memory(self, d_string_cstring(stream_attributes->string_name));
  for (index = 0; index < e_illuminable_bitmap_side_NULL; ++index)
    attributes->drawable_mask[index] = NULL;
  if ((attributes->polygon_shadow_caster = f_polygon_new(d_new(polygon), 0, NULL))) {
    if (!(attributes->polygon_shadow_caster_normalized = f_polygon_new(d_new(polygon), 0, NULL)))
      d_die(d_error_malloc);
  } else
    d_die(d_error_malloc);
  return self;
}
d_define_method(illuminable_bitmap, set_light_mask)(struct s_object *self, struct s_object *drawable_mask, enum e_illuminable_bitmap_sides side) {
  d_using(illuminable_bitmap);
  double mask_width, mask_height, image_width, image_height;
  if (illuminable_bitmap_attributes->drawable_mask[side])
    d_delete(illuminable_bitmap_attributes->drawable_mask[side]);
  illuminable_bitmap_attributes->drawable_mask[side] = d_retain(drawable_mask);
  d_call(illuminable_bitmap_attributes->drawable_mask[side], m_drawable_set_blend, e_drawable_blend_add);
  d_call(illuminable_bitmap_attributes->drawable_mask[side], m_drawable_get_dimension, &mask_width, &mask_height);
  d_call(self, m_drawable_get_dimension, &image_width, &image_height);
  d_assert((image_width == mask_width));
  d_assert((image_height == mask_height));
  return self;
}
d_define_method(illuminable_bitmap, set_lights)(struct s_object *self, struct s_object *lights) {
  d_using(illuminable_bitmap);
  if (illuminable_bitmap_attributes->lights)
    d_delete(illuminable_bitmap_attributes->lights);
  illuminable_bitmap_attributes->lights = d_retain(lights);
  return self;
}
d_define_method(illuminable_bitmap, set_shadows)(struct s_object *self, struct s_object *shadows) {
  d_using(illuminable_bitmap);
  if (illuminable_bitmap_attributes->shadows)
    d_delete(illuminable_bitmap_attributes->shadows);
  illuminable_bitmap_attributes->shadows = d_retain(shadows);
  return self;
}
d_define_method(illuminable_bitmap, set_shadow_caster_points_args)(struct s_object *self, size_t size, ...) {
  d_using(illuminable_bitmap);
  va_list parameters;
  struct s_object *current_point;
  unsigned int index;
  d_call(illuminable_bitmap_attributes->polygon_shadow_caster, m_polygon_clear, NULL);
  va_start(parameters, size);
  for (index = 0; index < size; ++index)
    if ((current_point = va_arg(parameters, struct s_object *)))
      d_call(illuminable_bitmap_attributes->polygon_shadow_caster, m_polygon_push, current_point);
  va_end(parameters);
  return self;
}
d_define_method(illuminable_bitmap, set_shadow_caster_points_list)(struct s_object *self, size_t size, struct s_object *points[]) {
  d_using(illuminable_bitmap);
  unsigned int index;
  d_call(illuminable_bitmap_attributes->polygon_shadow_caster, m_polygon_clear, NULL);
  for (index = 0; index < size; ++index)
    d_call(illuminable_bitmap_attributes->polygon_shadow_caster, m_polygon_push, points[index]);
  return self;
}
d_define_method_override(illuminable_bitmap, draw)(struct s_object *self, struct s_object *environment) {
  d_using(illuminable_bitmap);
  struct s_drawable_attributes *drawable_attributes = d_cast(self, drawable);
  struct s_lights_emitter_description *lights_emitter;
  struct s_list affected_lights;
  struct s_object *result = d_call_owner(self, bitmap, m_drawable_draw, environment); /* recall the father's draw method */
  double image_x, image_y, image_center_x, image_center_y, image_width, image_height, image_principal_point_x, image_principal_point_y, new_x, new_y,
         radians_rotation = -(drawable_attributes->angle * d_math_radians_conversion), radians_incident, cosine = cos(radians_rotation),
         sine = sin(radians_rotation), light_normalized_percentage_R[e_illuminable_bitmap_side_NULL], 
         light_normalized_percentage_B[e_illuminable_bitmap_side_NULL], light_normalized_percentage_G[e_illuminable_bitmap_side_NULL], 
         light_final_mask_R[e_illuminable_bitmap_side_NULL], light_final_mask_G[e_illuminable_bitmap_side_NULL], 
         light_final_mask_B[e_illuminable_bitmap_side_NULL], light_radius_component[e_illuminable_bitmap_side_NULL], center_factor, side_factor, 
         real_light_x, real_light_y, collision_x = 0, collision_y = 0, distance_collision, diagonal_size_squared, distance_center_squared;
  size_t vertices;
  unsigned int index_side;
  if (illuminable_bitmap_attributes->lights) {
    memset(&(affected_lights), 0, sizeof(struct s_list));
    for (index_side = 0; index_side < e_illuminable_bitmap_side_NULL; ++index_side) {
      light_final_mask_R[index_side] = 0;
      light_final_mask_G[index_side] = 0;
      light_final_mask_B[index_side] = 0;
    }
    d_call(self, m_drawable_get_scaled_position, &image_x, &image_y);
    d_call(self, m_drawable_get_scaled_center, &image_center_x, &image_center_y);
    d_call(self, m_drawable_get_scaled_dimension, &image_width, &image_height);
    d_call(self, m_drawable_get_scaled_principal_point, &image_principal_point_x, &image_principal_point_y);
    d_call(illuminable_bitmap_attributes->lights, m_lights_get_affecting_lights, self, &(affected_lights), environment);
    d_foreach(&(affected_lights), lights_emitter, struct s_lights_emitter_description) {
      for (index_side = 0; index_side < e_illuminable_bitmap_side_NULL; ++index_side) {
        light_normalized_percentage_R[index_side] = 0;
        light_normalized_percentage_G[index_side] = 0;
        light_normalized_percentage_B[index_side] = 0;
        light_radius_component[index_side] = 0;
      }
      /* we need to re-calculate the coordinates of the light in respect of the angle generated by this image */
      real_light_x = lights_emitter->position_x;
      real_light_y = lights_emitter->position_y;
      new_x = lights_emitter->position_x - (image_x + image_center_x);
      new_y = lights_emitter->position_y - (image_y + image_center_y);
      lights_emitter->position_x = (new_x * cosine) - (new_y * sine) + (image_x + image_center_x);
      lights_emitter->position_y = (new_x * sine) + (new_y * cosine) + (image_y + image_center_y);
      center_factor = 0.0;
      side_factor = 0.0;
      if (lights_emitter->distance < lights_emitter->radius)
        center_factor = (1.0 - (lights_emitter->distance / lights_emitter->radius));
      light_normalized_percentage_R[e_illuminable_bitmap_side_front] = (center_factor * (lights_emitter->mask_R / 255.0)) * (lights_emitter->intensity / 255.0);
      light_normalized_percentage_G[e_illuminable_bitmap_side_front] = (center_factor * (lights_emitter->mask_G / 255.0)) * (lights_emitter->intensity / 255.0);
      light_normalized_percentage_B[e_illuminable_bitmap_side_front] = (center_factor * (lights_emitter->mask_B / 255.0)) * (lights_emitter->intensity / 255.0);
      /* we need to calculate now the distance from the light source to the collision box of the object. This means we are temporary ignoring the real center
       * of the image, and we are using it only as target of our caster. We'll draw a line that goes from the light source to the center and we detect where
       * we hit the square
       */
      if (d_call(&(drawable_attributes->square_collision_box), m_square_intersect_coordinates, real_light_x, real_light_y, (image_x + image_center_x),
            (image_y + image_center_y), &collision_x, &collision_y)) {
        distance_collision = f_math_sqrt(d_point_square_distance(collision_x, collision_y, real_light_x, real_light_y), d_math_default_precision);
        if (distance_collision < lights_emitter->radius)
          side_factor = 1.0 - (distance_collision / lights_emitter->radius);
      } else {
        diagonal_size_squared = (image_width * image_width) + (image_height * image_height);
        distance_center_squared = d_point_square_distance(real_light_x, real_light_y, (image_x + image_center_x),(image_y + image_center_y));
        if (d_call(&(drawable_attributes->square_collision_box), m_square_intersect_coordinates, real_light_x, real_light_y,
              (real_light_x + ((real_light_x - (image_x + image_center_x)) * (diagonal_size_squared/distance_center_squared))),
              (real_light_y + ((real_light_y - (image_y + image_center_y)) * (diagonal_size_squared/distance_center_squared))), &collision_x, &collision_y)) {
          distance_collision = f_math_sqrt(d_point_square_distance(collision_x, collision_y, (image_x + image_center_x), (image_y + image_center_y)),
              d_math_default_precision);
          side_factor = (f_math_sqrt(distance_center_squared, d_math_default_precision) / distance_collision);
        }
      }
      /* we need to calculate the angle between the center of the object and the center of the light source (and we have to have that angle
       * expressed as a positive value).
       */
      radians_incident = fmod(atan2(lights_emitter->position_y - image_principal_point_y, lights_emitter->position_x - image_principal_point_x), d_math_two_pi);
      if (radians_incident < 0)
        radians_incident = d_math_two_pi + radians_incident;
      /*
       * The radians_incident is calculated as the angle created by the line that goes from the center
       * of the object hit by the light caster and the light caster itself.
       *
       *       (pi/2) + pi
       *
       *            |
       *    (3)    _|_    (4)
       *         .  |  .
       * pi ----:-(obj)-:---- 0pi or 2pi
       *         .  |  .
       *           -|-
       *    (2)     |     (1)
       *
       *           pi/2
       */
      if (radians_incident <= d_math_half_pi) {
        light_radius_component[e_illuminable_bitmap_side_right] = 1.0 - (radians_incident/d_math_half_pi);
        light_radius_component[e_illuminable_bitmap_side_bottom] = radians_incident/d_math_half_pi;
      } else if ((radians_incident > d_math_half_pi) && (radians_incident <= d_math_pi)) {
        light_radius_component[e_illuminable_bitmap_side_bottom] = 1.0 - (radians_incident - d_math_half_pi)/d_math_half_pi;
        light_radius_component[e_illuminable_bitmap_side_left] = (radians_incident - d_math_half_pi)/d_math_half_pi;
      } else if ((radians_incident > d_math_pi) && (radians_incident <= (d_math_half_pi + d_math_pi))) {
        light_radius_component[e_illuminable_bitmap_side_left] = 1.0 - (radians_incident - d_math_pi)/d_math_half_pi;
        light_radius_component[e_illuminable_bitmap_side_top] = (radians_incident - d_math_pi)/d_math_half_pi;
      } else {
        light_radius_component[e_illuminable_bitmap_side_top] = 1.0 - (radians_incident - (d_math_pi + d_math_half_pi))/d_math_half_pi;
        light_radius_component[e_illuminable_bitmap_side_right] = (radians_incident - (d_math_pi + d_math_half_pi))/d_math_half_pi;
      }
      for (index_side = 0; index_side < e_illuminable_bitmap_side_NULL; ++index_side) {
        if (index_side != e_illuminable_bitmap_side_front) {
          light_normalized_percentage_R[index_side] = (side_factor * (lights_emitter->mask_R / 255.0) * light_radius_component[index_side]) * 
            (lights_emitter->intensity / 255.0);
          light_normalized_percentage_G[index_side] = (side_factor * (lights_emitter->mask_G / 255.0) * light_radius_component[index_side]) *
            (lights_emitter->intensity / 255.0);
          light_normalized_percentage_B[index_side] = (side_factor * (lights_emitter->mask_B / 255.0) * light_radius_component[index_side]) *
            (lights_emitter->intensity / 255.0);
        }
        if ((light_final_mask_R[index_side] += (light_normalized_percentage_R[index_side] * 255.0)) > 255.0)
          light_final_mask_R[index_side] = 255.0;
        if ((light_final_mask_G[index_side] += (light_normalized_percentage_G[index_side] * 255.0)) > 255.0)
          light_final_mask_G[index_side] = 255.0;
        if ((light_final_mask_B[index_side] += (light_normalized_percentage_B[index_side] * 255.0)) > 255.0)
          light_final_mask_B[index_side] = 255.0;
      }
    }
    for (index_side = 0; index_side < e_illuminable_bitmap_side_NULL; ++index_side) {
      if (illuminable_bitmap_attributes->drawable_mask[index_side]) {
        /* we don't need to check the visibility because, if we are in this function, means that the visibility of the object has been already confirmed by
         * the called drawable
         */
        d_call(illuminable_bitmap_attributes->drawable_mask[index_side], m_drawable_set_maskRGB, (unsigned int)light_final_mask_R[index_side],
            (unsigned int)light_final_mask_G[index_side], (unsigned int)light_final_mask_B[index_side]);
        while (((intptr_t)d_call(illuminable_bitmap_attributes->drawable_mask[index_side], m_drawable_draw, environment)) == d_drawable_return_continue);
      }
    }
    if (illuminable_bitmap_attributes->shadows) {
      d_call(illuminable_bitmap_attributes->polygon_shadow_caster_normalized, m_polygon_size, &vertices);
      if (vertices >= 3)
        d_call(illuminable_bitmap_attributes->shadows, m_shadows_add_caster, self);
    }
    while ((lights_emitter = (struct s_lights_emitter_description *)(affected_lights.head))) {
      f_list_delete(&(affected_lights), (struct s_list_node *)lights_emitter);
      d_free(lights_emitter);
    }
  }
  return result;
}
d_define_method_override(illuminable_bitmap, draw_contour)(struct s_object *self, struct s_object *environment) {
  d_using(illuminable_bitmap);
  struct s_environment_attributes *environment_attributes = d_cast(environment, environment);
  struct s_object *result = d_call_owner(self, drawable, m_drawable_draw_contour, environment);
  struct s_object *point_current, *point_previous = NULL, *point_first = NULL;
  double origin_x, origin_y, destination_x, destination_y;
  d_call(illuminable_bitmap_attributes->polygon_shadow_caster_normalized, m_polygon_normalize, NULL);
  d_miranda_lock(environment) {
    SDL_SetRenderDrawColor(environment_attributes->renderer, d_illuminable_bitmap_default_contour_color);
    d_polygon_foreach(illuminable_bitmap_attributes->polygon_shadow_caster_normalized, point_current) {
      if (point_previous) {
        d_call(point_previous, m_point_get, &origin_x, &origin_y);
        d_call(point_current, m_point_get, &destination_x, &destination_y);
        SDL_RenderDrawLine(environment_attributes->renderer, origin_x, origin_y, destination_x, destination_y);
      }
      if (!point_first)
        point_first = point_current;
      point_previous = point_current;
    }
    if ((point_first) && (point_previous) && (point_first != point_previous)) {
      d_call(point_first, m_point_get, &origin_x, &origin_y);
      d_call(point_previous, m_point_get, &destination_x, &destination_y);
      SDL_RenderDrawLine(environment_attributes->renderer, origin_x, origin_y, destination_x, destination_y);
    }
  } d_miranda_unlock(environment);
  return result;
}
d_define_method_override(illuminable_bitmap, normalize_scale)(struct s_object *self, double reference_w, double reference_h, double offset_x, double offset_y,
    double focus_x, double focus_y, double current_w, double current_h, double zoom) {
  d_using(illuminable_bitmap);
  struct s_drawable_attributes *drawable_core_attributes = d_cast(self, drawable), *drawable_other_attributes;
  struct s_object *point_selected, *point_new;
  struct s_object *result =
    d_call_owner(self, drawable, m_drawable_normalize_scale, reference_w, reference_h, offset_x, offset_y, focus_x, focus_y, current_w, current_h, zoom);
  double position_x, position_y, new_x, new_y, center_x, center_y, new_center_x, new_center_y, local_center_x, local_center_y, image_position_x,
         image_position_y, normalized_center_x, normalized_center_y, normalized_image_position_x, normalized_image_position_y;
  int index_side;
  for (index_side = 0; index_side < e_illuminable_bitmap_side_NULL; ++index_side)
    if (illuminable_bitmap_attributes->drawable_mask[index_side]) {
      d_assert((drawable_other_attributes = d_cast(illuminable_bitmap_attributes->drawable_mask[index_side], drawable)));
      d_call(&(drawable_other_attributes->point_normalized_destination), m_point_set_point, &(drawable_core_attributes->point_normalized_destination));
      d_call(&(drawable_other_attributes->point_normalized_dimension), m_point_set_point, &(drawable_core_attributes->point_normalized_dimension));
      d_call(&(drawable_other_attributes->point_normalized_center), m_point_set_point, &(drawable_core_attributes->point_normalized_center));
      d_call(&(drawable_other_attributes->square_collision_box), m_square_set_square, &(drawable_core_attributes->square_collision_box));
      drawable_other_attributes->angle = drawable_core_attributes->angle;
    }
  /* I need to normalize the scale of the polygon */
  d_call(self, m_drawable_get_position, &image_position_x, &image_position_y);
  d_call(self, m_drawable_get_center, &center_x, &center_y);
  d_call(self, m_drawable_get_scaled_position, &normalized_image_position_x, &normalized_image_position_y);
  d_call(self, m_drawable_get_scaled_center, &normalized_center_x, &normalized_center_y);
  d_call(illuminable_bitmap_attributes->polygon_shadow_caster_normalized, m_polygon_clear, NULL);
  d_polygon_foreach(illuminable_bitmap_attributes->polygon_shadow_caster, point_selected) {
    d_call(point_selected, m_point_get, &position_x, &position_y);
    position_x += image_position_x;
    position_y += image_position_y;
    local_center_x = ((image_position_x + center_x) - position_x);
    local_center_y = ((image_position_y + center_y) - position_y);
    if ((drawable_core_attributes->flags & e_drawable_kind_do_not_normalize_environment_zoom) != e_drawable_kind_do_not_normalize_environment_zoom) {
      /* global zoom */
      new_x = focus_x - ((focus_x - position_x) * zoom);
      new_y = focus_y - ((focus_y - position_y) * zoom);
      new_center_x = local_center_x * zoom;
      new_center_y = local_center_y * zoom;
    } else {
      new_x = position_x;
      new_y = position_y;
      new_center_x = local_center_x;
      new_center_y = local_center_y;
    }
    if ((drawable_core_attributes->flags & e_drawable_kind_do_not_normalize_local_zoom) != e_drawable_kind_do_not_normalize_local_zoom) {
      /* local zoom */
      new_x = (new_x + new_center_x) - (new_center_x * drawable_core_attributes->zoom);
      new_y = (new_y + new_center_y) - (new_center_y * drawable_core_attributes->zoom);
      new_center_x = new_center_x * drawable_core_attributes->zoom;
      new_center_y = new_center_y * drawable_core_attributes->zoom;
    }
    if ((drawable_core_attributes->flags & e_drawable_kind_do_not_normalize_reference_ratio) != e_drawable_kind_do_not_normalize_reference_ratio) {
      /* screen scale, in respect of the applied reference */
      new_center_x = (new_center_x * current_w) / reference_w;
      new_center_y = (new_center_y * current_h) / reference_h;
      new_x = (new_x * current_w) / reference_w;
      new_y = (new_y * current_h) / reference_h;
    }
    if ((drawable_core_attributes->flags & e_drawable_kind_do_not_normalize_camera) != e_drawable_kind_do_not_normalize_camera) {
      /* camera offset */
      new_x = new_x - offset_x;
      new_y = new_y - offset_y;
    }
    point_new = f_point_new(d_new(point), new_x, new_y);
    d_call(illuminable_bitmap_attributes->polygon_shadow_caster_normalized, m_polygon_push, point_new);
    d_delete(point_new);
  }
  d_call(illuminable_bitmap_attributes->polygon_shadow_caster_normalized, m_polygon_set_center, (normalized_image_position_x + normalized_center_x),
      (normalized_image_position_y + normalized_center_y));
  d_call(illuminable_bitmap_attributes->polygon_shadow_caster_normalized, m_polygon_set_angle, drawable_core_attributes->angle);
  d_call(illuminable_bitmap_attributes->polygon_shadow_caster_normalized, m_polygon_normalize, NULL);
  return result;
}
d_define_method_override(illuminable_bitmap, keep_scale)(struct s_object *self, double current_w, double current_h) {
  d_using(illuminable_bitmap);
  struct s_drawable_attributes *drawable_core_attributes = d_cast(self, drawable), *drawable_other_attributes;
  struct s_object *result = d_call_owner(self, drawable, m_drawable_keep_scale, current_w, current_h);
  int index_side;
  for (index_side = 0; index_side < e_illuminable_bitmap_side_NULL; ++index_side)
    if (illuminable_bitmap_attributes->drawable_mask[index_side]) {
      d_assert((drawable_other_attributes = d_cast(illuminable_bitmap_attributes->drawable_mask[index_side], drawable)));
      d_call(&(drawable_other_attributes->point_normalized_destination), m_point_set_point, &(drawable_core_attributes->point_normalized_destination));
      d_call(&(drawable_other_attributes->point_normalized_dimension), m_point_set_point, &(drawable_core_attributes->point_normalized_dimension));
      d_call(&(drawable_other_attributes->point_normalized_center), m_point_set_point, &(drawable_core_attributes->point_normalized_center));
      d_call(&(drawable_other_attributes->square_collision_box), m_square_set_square, &(drawable_core_attributes->square_collision_box));
      drawable_other_attributes->angle = drawable_core_attributes->angle;
    }
  d_call(illuminable_bitmap_attributes->polygon_shadow_caster_normalized, m_polygon_set_polygon, &(illuminable_bitmap_attributes->polygon_shadow_caster));
  return result;
}
d_declare_method(illuminable_bitmap, delete)(struct s_object *self, struct s_illuminable_bitmap_attributes *attributes) {
  unsigned int index;
  for (index = 0; index < e_illuminable_bitmap_side_NULL; ++index) {
    if (attributes->drawable_mask[index]) {
      d_delete(attributes->drawable_mask[index]);
      attributes->drawable_mask[index] = NULL;
    }
  }
  d_delete(attributes->polygon_shadow_caster);
  d_delete(attributes->polygon_shadow_caster_normalized);
  d_delete(attributes->shadows);
  return NULL;
}
d_define_class(illuminable_bitmap) {d_hook_method(illuminable_bitmap, e_flag_public, set_light_mask),
  d_hook_method(illuminable_bitmap, e_flag_public, set_lights),
  d_hook_method(illuminable_bitmap, e_flag_public, set_shadows),
  d_hook_method(illuminable_bitmap, e_flag_public, set_shadow_caster_points_args),
  d_hook_method(illuminable_bitmap, e_flag_public, set_shadow_caster_points_list),
  d_hook_method_override(illuminable_bitmap, e_flag_public, drawable, draw),
  d_hook_method_override(illuminable_bitmap, e_flag_public, drawable, draw_contour),
  d_hook_method_override(illuminable_bitmap, e_flag_public, drawable, normalize_scale),
  d_hook_method_override(illuminable_bitmap, e_flag_public, drawable, keep_scale),
  d_hook_delete(illuminable_bitmap),
  d_hook_method_tail};
