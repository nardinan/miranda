/*
 * miranda
 * Copyright (C) 2019 Andrea Nardinocchi (andrea@nardinan.it)
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
#include "shadows.obj.h"
struct s_shadows_attributes *p_shadows_alloc(struct s_object *self) {
  struct s_shadows_attributes *result = d_prepare(self, shadows);
  f_mutex_new(self);                             /* inherit */
  f_memory_new(self);                            /* inherit */
  f_drawable_new(self, e_drawable_kind_single);  /* inherit */
  return result;
}
struct s_object *f_shadows_new(struct s_object *self) {
  struct s_shadows_attributes *attributes = p_shadows_alloc(self);
  if (!(attributes->array_casters = f_array_new(d_new(array), d_array_bucket)))
    d_die(d_error_malloc);
  attributes->maximum_intensity = d_shadows_default_maximum_intensity;
  return self;
}
d_define_method(shadows, add_caster)(struct s_object *self, struct s_object *illuminable_bitmap) {
  d_using(shadows);
  return d_call(shadows_attributes->array_casters, m_array_push, illuminable_bitmap);
}
d_define_method(shadows, clear)(struct s_object *self) {
  d_using(shadows);
  return d_call(shadows_attributes->array_casters, m_array_clear, NULL);
}
d_define_method_override(shadows, draw)(struct s_object *self, struct s_object *environment) {
  d_using(shadows);
  struct s_environment_attributes *environmental_attributes = d_cast(environment, environment);
  struct s_drawable_attributes *drawable_attributes = d_cast(self, drawable);
  struct s_illuminable_bitmap_attributes *illuminable_bitmap_attributes;
  struct s_lights_emitter_description *lights_emitter;
  struct s_list affected_lights;
  struct s_object *illuminable_bitmap_caster;
  struct s_object *point_vertex;
  double projected_position_x, projected_position_y, point_x, point_y, previous_valid_point_x = NAN, previous_valid_point_y = NAN, current_position_x,
    current_position_y, distance, shadow_projection, intensity_alpha, screen_diagonal = f_math_sqrt(d_math_square(environmental_attributes->current_w) +
      d_math_square(environmental_attributes->current_h), d_math_default_precision);
  unsigned int collisions;
  ssize_t polygons, index_polygon;
  struct s_shadows_quadrilateral projected_polygons[d_shadows_maximum_polygons];
  d_array_foreach(shadows_attributes->array_casters, illuminable_bitmap_caster) {
    if ((illuminable_bitmap_attributes = d_cast(illuminable_bitmap_caster, illuminable_bitmap))) {
      if (illuminable_bitmap_attributes->lights) {
        memset(&(affected_lights), 0, sizeof(struct s_list));
        d_call(illuminable_bitmap_attributes->lights, m_lights_get_affecting_lights, illuminable_bitmap_caster, &(affected_lights), environment);
        d_call(illuminable_bitmap_caster, m_drawable_get_scaled_principal_point, &current_position_x, &current_position_y);
        d_foreach(&(affected_lights), lights_emitter, struct s_lights_emitter_description) {
          distance = f_math_sqrt(d_point_square_distance(current_position_x, current_position_y, lights_emitter->position_x, lights_emitter->position_y),
            d_math_default_precision);
          if (distance < lights_emitter->radius) {
            intensity_alpha = (shadows_attributes->maximum_intensity * (1.0 - (distance / lights_emitter->radius))) * 255;
            shadow_projection = (-1.0 * (distance / lights_emitter->radius));
            polygons = 0;
            d_polygon_foreach(illuminable_bitmap_attributes->polygon_shadow_caster_normalized, point_vertex) {
              d_call(point_vertex, m_point_get, &point_x, &point_y);
              projected_position_x = ((lights_emitter->position_x - point_x) * (-1.0 * screen_diagonal)) + point_x;
              projected_position_y = ((lights_emitter->position_y - point_y) * (-1.0 * screen_diagonal)) + point_y;
              d_call(illuminable_bitmap_attributes->polygon_shadow_caster_normalized, m_polygon_intersect_coordinates, point_x, point_y, projected_position_x,
                projected_position_y, &collisions);
              if (collisions < 2) {
                if ((drawable_attributes->flags & e_drawable_kind_contour) == e_drawable_kind_contour) {
                  SDL_SetRenderDrawColor(environmental_attributes->renderer, d_shadows_default_contour_color);
                  SDL_RenderDrawLine(environmental_attributes->renderer, point_x, point_y, projected_position_x, projected_position_y);
                }
                if ((previous_valid_point_x == previous_valid_point_x) && (previous_valid_point_y == previous_valid_point_y)) {
                  projected_polygons[polygons].point_A_x = previous_valid_point_x;
                  projected_polygons[polygons].point_A_y = previous_valid_point_y;
                  projected_polygons[polygons].point_B_x =
                    ((lights_emitter->position_x - previous_valid_point_x) * shadow_projection) + previous_valid_point_x;
                  projected_polygons[polygons].point_B_y =
                    ((lights_emitter->position_y - previous_valid_point_y) * shadow_projection) + previous_valid_point_y;
                  projected_polygons[polygons].point_C_x = ((lights_emitter->position_x - point_x) * shadow_projection) + point_x;
                  projected_polygons[polygons].point_C_y = ((lights_emitter->position_y - point_y) * shadow_projection) + point_y;
                  projected_polygons[polygons].point_D_x = point_x;
                  projected_polygons[polygons].point_D_y = point_y;
                  ++polygons;
                }
                previous_valid_point_x = point_x;
                previous_valid_point_y = point_y;
              } else {
                previous_valid_point_x = NAN;
                previous_valid_point_y = NAN;
              }
            }
            if (polygons > 0) {
              projected_polygons[polygons].point_A_x = projected_polygons[0].point_A_x;
              projected_polygons[polygons].point_A_y = projected_polygons[0].point_A_y;
              projected_polygons[polygons].point_B_x = projected_polygons[0].point_B_x;
              projected_polygons[polygons].point_B_y = projected_polygons[0].point_B_y;
              projected_polygons[polygons].point_C_x = projected_polygons[polygons - 1].point_C_x;
              projected_polygons[polygons].point_C_y = projected_polygons[polygons - 1].point_C_y;
              projected_polygons[polygons].point_D_x = projected_polygons[polygons - 1].point_D_x;
              projected_polygons[polygons].point_D_y = projected_polygons[polygons - 1].point_D_y;
            }
            ++polygons;
            /* check for clipping and removing all the clipping obstacles */
            d_miranda_lock(environment) {
              for (index_polygon = 0; index_polygon < polygons; ++index_polygon) {
                if (!d_call(illuminable_bitmap_attributes->polygon_shadow_caster_normalized, m_polygon_intersect_coordinates,
                            (double)projected_polygons[index_polygon].point_B_x, (double)projected_polygons[index_polygon].point_B_y,
                            (double)projected_polygons[index_polygon].point_C_x, (double)projected_polygons[index_polygon].point_C_y, NULL)) {
                  int vector_point_x[] = {
                    projected_polygons[index_polygon].point_A_x,
                    projected_polygons[index_polygon].point_B_x,
                    projected_polygons[index_polygon].point_C_x,
                    projected_polygons[index_polygon].point_D_x
                  }, vector_point_y[] = {
                    projected_polygons[index_polygon].point_A_y,
                    projected_polygons[index_polygon].point_B_y,
                    projected_polygons[index_polygon].point_C_y,
                    projected_polygons[index_polygon].point_D_y
                  };
                  if ((drawable_attributes->flags & e_drawable_kind_contour) == e_drawable_kind_contour) {
                    SDL_SetRenderDrawColor(environmental_attributes->renderer, d_shadows_default_polygon_color);
                    SDL_RenderDrawLine(environmental_attributes->renderer, vector_point_x[0], vector_point_y[0], vector_point_x[1], vector_point_y[1]);
                    SDL_RenderDrawLine(environmental_attributes->renderer, vector_point_x[1], vector_point_y[1], vector_point_x[2], vector_point_y[2]);
                    SDL_RenderDrawLine(environmental_attributes->renderer, vector_point_x[2], vector_point_y[2], vector_point_x[3], vector_point_y[3]);
                    SDL_RenderDrawLine(environmental_attributes->renderer, vector_point_x[3], vector_point_y[3], vector_point_x[0], vector_point_y[0]);
                  }
                  f_primitive_fill_polygon(environmental_attributes->renderer, vector_point_x, vector_point_y, 4, 0, 0, 0, intensity_alpha);
                }
              }
            } d_miranda_unlock(environment);
          }
        }
        while ((lights_emitter = (struct s_lights_emitter_description *)(affected_lights.head))) {
          f_list_delete(&(affected_lights), (struct s_list_node *)lights_emitter);
          d_free(lights_emitter);
        }
      }
    }
  }
  d_call(shadows_attributes->array_casters, m_array_clear, NULL);
  d_cast_return(d_drawable_return_last);
}
d_define_method_override(shadows, draw_contour)(struct s_object *self, struct s_object *environment) {
  /* contours are drawn into the draw routine */
  return self;
}
d_define_method_override(shadows, is_visible)(struct s_object *self, double current_w, double current_h) {
  /* is visible, every time */
  return self;
}
d_define_method(shadows, delete)(struct s_object *self, struct s_shadows_attributes *attributes) {
  d_delete(attributes->array_casters);
  return NULL;
}
d_define_class(shadows) {d_hook_method(shadows, e_flag_public, add_caster),
                         d_hook_method(shadows, e_flag_public, clear),
                         d_hook_method_override(shadows, e_flag_public, drawable, draw),
                         d_hook_method_override(shadows, e_flag_public, drawable, draw_contour),
                         d_hook_method_override(shadows, e_flag_public, drawable, is_visible),
                         d_hook_delete(shadows),
                         d_hook_method_tail};