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
  double projected_position_x, projected_position_y, point_x, point_y, current_position_x, current_position_y, distance, intensity_alpha,
    final_incident_angle[d_shadows_maximum_vertices], current_incident_angle, screen_diagonal =
    f_math_sqrt(d_math_square(environmental_attributes->current_w) + d_math_square(environmental_attributes->current_h), d_math_default_precision);
  unsigned int collisions;
  ssize_t raycasts, index_raycast, vertices, index_vertex, index_compensation;
  int vertices_x[d_shadows_maximum_vertices], vertices_y[d_shadows_maximum_vertices], real_vertices_x[d_shadows_maximum_vertices],
    real_vertices_y[d_shadows_maximum_vertices];
  t_boolean raycast_required[d_shadows_maximum_vertices];
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
            raycasts = 0;
            vertices = 0;
            d_polygon_foreach(illuminable_bitmap_attributes->polygon_shadow_caster_normalized, point_vertex) {
              d_call(point_vertex, m_point_get, &point_x, &point_y);
              projected_position_x = ((lights_emitter->position_x - point_x) * (-1.0 * screen_diagonal)) + point_x;
              projected_position_y = ((lights_emitter->position_y - point_y) * (-1.0 * screen_diagonal)) + point_y;
              d_call(illuminable_bitmap_attributes->polygon_shadow_caster_normalized, m_polygon_intersect_coordinates, point_x, point_y, projected_position_x,
                projected_position_y, &collisions);
              real_vertices_x[vertices] = point_x;
              real_vertices_y[vertices] = point_y;
              ++vertices;
              if (collisions < 2) {
                if ((drawable_attributes->flags & e_drawable_kind_contour) == e_drawable_kind_contour) {
                  SDL_SetRenderDrawColor(environmental_attributes->renderer, d_lights_default_contour_color);
                  SDL_RenderDrawLine(environmental_attributes->renderer, point_x, point_y, projected_position_x, projected_position_y);
                }
                current_incident_angle = d_point_angle(lights_emitter->position_x, lights_emitter->position_y, point_x, point_y);
                if (raycasts > 0) {
                  for (index_raycast = (raycasts - 1); index_raycast >= 0; --index_raycast)
                    if (current_incident_angle < final_incident_angle[index_raycast]) {
                      final_incident_angle[index_raycast + 1] = final_incident_angle[index_raycast];
                      vertices_x[index_raycast + 1] = vertices_x[index_raycast];
                      vertices_y[index_raycast + 1] = vertices_y[index_raycast];
                    } else
                      break;
                  final_incident_angle[index_raycast + 1] = current_incident_angle;
                  vertices_x[index_raycast + 1] = point_x;
                  vertices_y[index_raycast + 1] = point_y;

                } else {
                  final_incident_angle[0] = current_incident_angle;
                  vertices_x[0] = point_x;
                  vertices_y[0] = point_y;
                }
                ++raycasts;
              }
            }
            memset(raycast_required, d_true, raycasts);
            for (index_raycast = 0; index_raycast < (raycasts - 1); ++index_raycast) {
              for (index_vertex = 0; index_vertex < vertices; ++index_vertex)
                if ((real_vertices_x[index_vertex] == vertices_x[index_raycast]) && (real_vertices_y[index_vertex] == vertices_y[index_raycast])) {
                  ++index_vertex;
                  break;
                }
              while ((index_vertex < vertices) && (real_vertices_x[index_vertex] != vertices_x[index_raycast + 1]) &&
                     (real_vertices_y[index_vertex] != vertices_y[index_raycast + 1])) {
                memmove(&(vertices_x[index_raycast + 2]), &(vertices_x[index_raycast + 1]), (raycasts - (index_raycast + 1)) * sizeof(int));
                memmove(&(vertices_y[index_raycast + 2]), &(vertices_y[index_raycast + 1]), (raycasts - (index_raycast + 1)) * sizeof(int));
                ++raycasts;
                vertices_x[index_raycast + 1] = real_vertices_x[index_vertex];
                vertices_y[index_raycast + 1] = real_vertices_y[index_vertex];
                raycast_required[index_raycast + 1] = d_false;
                ++index_raycast;
                ++index_vertex;
              }
            }
            for (index_raycast = 0, index_compensation = 0; index_raycast < raycasts; ++index_raycast) {
              if (raycast_required[index_raycast]) {
                vertices_x[raycasts + index_raycast - index_compensation] =
                  ((lights_emitter->position_x - vertices_x[raycasts - 1 - index_raycast]) * (-1.0 * (distance / lights_emitter->radius))) +
                  vertices_x[raycasts - 1 - index_raycast];
                vertices_y[raycasts + index_raycast - index_compensation] =
                  ((lights_emitter->position_y - vertices_y[raycasts - 1 - index_raycast]) * (-1.0 * (distance / lights_emitter->radius))) +
                  vertices_y[raycasts - 1 - index_raycast];
              } else
                ++index_compensation;
            }
            d_miranda_lock(environment) {
              f_primitive_fill_polygon(environmental_attributes->renderer, vertices_x, vertices_y, (raycasts * 2), 0, 0, 0, intensity_alpha);
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