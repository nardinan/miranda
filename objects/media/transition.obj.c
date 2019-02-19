/*
 * miranda
 * Copyright (C) 2017 Andrea Nardinocchi (andrea@nardinan.it)
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
#include "transition.obj.h"
#include "camera.obj.h"
struct s_transition_attributes *p_transition_alloc(struct s_object *self) {
  struct s_transition_attributes *result = d_prepare(self, transition);
  f_mutex_new(self);                                                                  /* inherit */
  f_memory_new(self);                                                                 /* inherit */
  f_drawable_new(self, (e_drawable_kind_single | e_drawable_kind_force_visibility));  /* inherit */
  return result;
}
struct s_object *f_transition_new(struct s_object *self, struct s_object *drawable, double time_ratio) {
  struct s_transition_attributes *attributes = p_transition_alloc(self);
  struct s_drawable_attributes *drawable_attributes_core;
  double dimension_w, dimension_h;
  memset(&(attributes->keys), 0, sizeof(struct s_list));
  if ((attributes->drawable = d_retain(drawable))) {
    drawable_attributes_core = d_cast(attributes->drawable, drawable);
    d_call(&(drawable_attributes_core->point_dimension), m_point_get, &dimension_w, &dimension_h);
    d_call(self, m_drawable_set_dimension, dimension_w, dimension_h);
  }
  attributes->time_ratio = time_ratio;
  attributes->status = e_transition_direction_stop;
  return self;
}
d_define_method(transition, append_key)(struct s_object *self, struct s_transition_key transition) {
  d_using(transition);
  struct s_transition_key *current_key;
  if ((current_key = (struct s_transition_key *)d_malloc(sizeof(struct s_transition_key)))) {
    memcpy(current_key, &transition, sizeof(struct s_transition_key));
    f_list_append(&(transition_attributes->keys), (struct s_list_node *)current_key, e_list_insert_tail);
  } else
    d_die(d_error_malloc);
  return self;
}
d_define_method(transition, set_status)(struct s_object *self, enum e_transition_directions status) {
  d_using(transition);
  transition_attributes->status = status;
  transition_attributes->current_key = NULL;
  transition_attributes->next_key = NULL;
  return self;
}
d_define_method(transition, get_status)(struct s_object *self) {
  d_using(transition);
  d_cast_return(transition_attributes->status);
}
d_define_method_override(transition, draw)(struct s_object *self, struct s_object *environment) {
  d_using(transition);
  struct s_drawable_attributes *drawable_attributes_self = d_cast(self, drawable), *drawable_attributes_core;
  struct s_environment_attributes *environment_attributes = d_cast(environment, environment);
  struct s_camera_attributes *camera_attributes = d_cast(environment_attributes->current_camera, camera);
  struct timeval current, elapsed_update;
  double real_elapsed_update, fraction_elapsed_update = 0.0, local_position_x, local_position_y, local_center_x, local_center_y, position_x, position_y,
    center_x, center_y, angle, zoom, mask_R, mask_G, mask_B, mask_A;
  gettimeofday(&current, NULL);
  d_call(&(drawable_attributes_self->point_destination), m_point_get, &local_position_x, &local_position_y);
  d_call(&(drawable_attributes_self->point_center), m_point_get, &local_center_x, &local_center_y);
  if (transition_attributes->drawable) {
    drawable_attributes_core = d_cast(transition_attributes->drawable, drawable);
    if ((transition_attributes->status == e_transition_direction_forward) || (transition_attributes->status == e_transition_direction_rewind)) {
      if ((transition_attributes->current_key) && (transition_attributes->next_key)) {
        timersub(&current, &(transition_attributes->last_update), &elapsed_update);
        real_elapsed_update = elapsed_update.tv_sec + ((double)(elapsed_update.tv_usec) / 1000000.0);
        if (real_elapsed_update > (transition_attributes->next_key->time * transition_attributes->time_ratio)) {
          transition_attributes->current_key = transition_attributes->next_key;
          switch (transition_attributes->status) {
            case e_transition_direction_forward:
              transition_attributes->next_key = (struct s_transition_key *)((struct s_list_node *)transition_attributes->next_key)->next;
              break;
            case e_transition_direction_rewind:
              transition_attributes->next_key = (struct s_transition_key *)((struct s_list_node *)transition_attributes->next_key)->back;
            default:
              break;
          }
          memcpy(&(transition_attributes->last_update), &current, sizeof(struct timeval));
        } else
          fraction_elapsed_update = (real_elapsed_update / (transition_attributes->next_key->time * transition_attributes->time_ratio));
      } else if (!transition_attributes->current_key) {
        switch (transition_attributes->status) {
          case e_transition_direction_forward:
            if ((transition_attributes->current_key = (struct s_transition_key *)(transition_attributes->keys.head)))
              transition_attributes->next_key = (struct s_transition_key *)((struct s_list_node *)transition_attributes->current_key)->next;
            break;
          case e_transition_direction_rewind:
            if ((transition_attributes->current_key = (struct s_transition_key *)(transition_attributes->keys.tail)))
              transition_attributes->next_key = (struct s_transition_key *)((struct s_list_node *)transition_attributes->current_key)->back;
          default:
            break;
        }
        memcpy(&(transition_attributes->last_update), &current, sizeof(struct timeval));
      }
    } else if ((transition_attributes->status == e_transition_direction_stop) || (!transition_attributes->current_key))
      transition_attributes->current_key = (struct s_transition_key *)(transition_attributes->keys.head);
    if (transition_attributes->current_key) {
      if (transition_attributes->next_key) {
        center_x = local_center_x + d_transition_factor_value(transition_attributes->current_key->position_x, transition_attributes->next_key->position_x,
          fraction_elapsed_update);
        center_y = local_center_y + d_transition_factor_value(transition_attributes->current_key->position_y, transition_attributes->next_key->position_y,
          fraction_elapsed_update);
        position_x = local_position_x + d_transition_factor_value(transition_attributes->current_key->position_x, transition_attributes->next_key->position_x,
          fraction_elapsed_update);
        position_y = local_position_y + d_transition_factor_value(transition_attributes->current_key->position_y, transition_attributes->next_key->position_y,
          fraction_elapsed_update);
        zoom = drawable_attributes_self->zoom *
               d_transition_factor_value(transition_attributes->current_key->zoom, transition_attributes->next_key->zoom, fraction_elapsed_update);
        angle = drawable_attributes_self->angle +
                d_transition_factor_value(transition_attributes->current_key->angle, transition_attributes->next_key->angle, fraction_elapsed_update);
        mask_R = (drawable_attributes_self->last_mask_R / 255.0) *
                 d_transition_factor_value(transition_attributes->current_key->mask_R, transition_attributes->next_key->mask_R, fraction_elapsed_update);
        mask_G = (drawable_attributes_self->last_mask_G / 255.0) *
                 d_transition_factor_value(transition_attributes->current_key->mask_G, transition_attributes->next_key->mask_G, fraction_elapsed_update);
        mask_B = (drawable_attributes_self->last_mask_B / 255.0) *
                 d_transition_factor_value(transition_attributes->current_key->mask_B, transition_attributes->next_key->mask_B, fraction_elapsed_update);
        mask_A = (drawable_attributes_self->last_mask_A / 255.0) *
                 d_transition_factor_value(transition_attributes->current_key->mask_A, transition_attributes->next_key->mask_A, fraction_elapsed_update);
      } else {
        center_x = local_center_x + transition_attributes->current_key->position_x;
        center_y = local_center_y + transition_attributes->current_key->position_y;
        position_x = local_position_x + transition_attributes->current_key->position_x;
        position_y = local_position_y + transition_attributes->current_key->position_y;
        zoom = drawable_attributes_self->zoom * transition_attributes->current_key->zoom;
        angle = drawable_attributes_self->angle + transition_attributes->current_key->angle;
        mask_R = (drawable_attributes_self->last_mask_R / 255.0) * transition_attributes->current_key->mask_R;
        mask_G = (drawable_attributes_self->last_mask_G / 255.0) * transition_attributes->current_key->mask_G;
        mask_B = (drawable_attributes_self->last_mask_B / 255.0) * transition_attributes->current_key->mask_B;
        mask_A = (drawable_attributes_self->last_mask_A / 255.0) * transition_attributes->current_key->mask_A;
      }
      d_call(transition_attributes->drawable, m_drawable_set_position, position_x, position_y);
      d_call(transition_attributes->drawable, m_drawable_set_center, center_x, center_y);
      d_call(transition_attributes->drawable, m_drawable_set_maskRGB, (unsigned int)mask_R, (unsigned int)mask_G, (unsigned int)mask_B);
      d_call(transition_attributes->drawable, m_drawable_set_maskA, (unsigned int)mask_A);
      drawable_attributes_core->zoom = zoom;
      drawable_attributes_core->angle = angle;
      drawable_attributes_core->flip = drawable_attributes_self->flip;
      if ((d_call(transition_attributes->drawable, m_drawable_normalize_scale, camera_attributes->scene_reference_w, camera_attributes->scene_reference_h,
        camera_attributes->scene_offset_x, camera_attributes->scene_offset_y, camera_attributes->scene_center_x, camera_attributes->scene_center_y,
        camera_attributes->screen_w, camera_attributes->screen_h, camera_attributes->scene_zoom)))
        while (((intptr_t)d_call(transition_attributes->drawable, m_drawable_draw, environment)) == d_drawable_return_continue);
    }
  }
  if ((drawable_attributes_self->flags & e_drawable_kind_contour) == e_drawable_kind_contour)
    d_call(self, m_drawable_draw_contour, environment);
  d_cast_return(d_drawable_return_last);
}
d_define_method_override(transition, set_maskRGB)(struct s_object *self, unsigned int red, unsigned int green, unsigned int blue) {
  struct s_drawable_attributes *drawable_attributes = d_cast(self, drawable);
  drawable_attributes->last_mask_R = red;
  drawable_attributes->last_mask_G = green;
  drawable_attributes->last_mask_B = blue;
  return self;
}
d_define_method_override(transition, set_maskA)(struct s_object *self, unsigned int alpha) {
  struct s_drawable_attributes *drawable_attributes = d_cast(self, drawable);
  drawable_attributes->last_mask_A = alpha;
  return self;
}
d_define_method_override(transition, set_blend)(struct s_object *self, enum e_drawable_blends blend) {
  d_using(transition);
  struct s_drawable_attributes *drawable_attributes = d_cast(self, drawable);
  drawable_attributes->last_blend = blend;
  if (transition_attributes->drawable)
    d_call(transition_attributes->drawable, m_drawable_set_blend, blend);
  return self;
}
d_define_method_override(transition, get_scaled_position)(struct s_object *self, double *x, double *y) {
  d_using(transition);
  *x = NAN;
  *y = NAN;
  if (transition_attributes->drawable)
    d_call(transition_attributes->drawable, m_drawable_get_scaled_position, x, y);
  return self;
}
d_define_method_override(transition, get_scaled_center)(struct s_object *self, double *x, double *y) {
  d_using(transition);
  *x = NAN;
  *y = NAN;
  if (transition_attributes->drawable)
    d_call(transition_attributes->drawable, m_drawable_get_scaled_center, x, y);
  return self;
}
d_define_method_override(transition, get_dimension)(struct s_object *self, double *w, double *h) {
  d_using(transition);
  *w = NAN;
  *h = NAN;
  if (transition_attributes->drawable)
    d_call(transition_attributes->drawable, m_drawable_get_dimension, w, h);
  return self;
}
d_define_method_override(transition, get_scaled_dimension)(struct s_object *self, double *w, double *h) {
  d_using(transition);
  *w = NAN;
  *h = NAN;
  if (transition_attributes->drawable)
    d_call(transition_attributes->drawable, m_drawable_get_scaled_dimension, w, h);
  return self;
}
d_define_method(transition, delete)(struct s_object *self, struct s_transition_attributes *attributes) {
  struct s_transition_key *current_key;
  if (attributes->drawable)
    d_delete(attributes->drawable);
  while ((current_key = (struct s_transition_key *)attributes->keys.head)) {
    f_list_delete(&(attributes->keys), (struct s_list_node *)current_key);
    d_free(current_key);
  }
  return NULL;
}
d_define_class(transition) {d_hook_method(transition, e_flag_public, append_key),
                            d_hook_method(transition, e_flag_public, set_status),
                            d_hook_method(transition, e_flag_public, get_status),
                            d_hook_method_override(transition, e_flag_public, drawable, draw),
                            d_hook_method_override(transition, e_flag_public, drawable, set_maskRGB),
                            d_hook_method_override(transition, e_flag_public, drawable, set_maskA),
                            d_hook_method_override(transition, e_flag_public, drawable, set_blend),
                            d_hook_method_override(transition, e_flag_public, drawable, get_scaled_position),
                            d_hook_method_override(transition, e_flag_public, drawable, get_scaled_center),
                            d_hook_method_override(transition, e_flag_public, drawable, get_dimension),
                            d_hook_method_override(transition, e_flag_public, drawable, get_scaled_dimension),
                            d_hook_delete(transition),
                            d_hook_method_tail};
