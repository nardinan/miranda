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
#include "entity.obj.h"
#include "camera.obj.h"
struct s_entity_attributes *p_entity_alloc(struct s_object *self) {
  struct s_entity_attributes *result = d_prepare(self, entity);
  /* abstract (no memory inheritance) */
  f_drawable_new(self, (e_drawable_kind_single | e_drawable_kind_force_visibility));    /* inherit */
  return result;
}
struct s_object *f_entity_new(struct s_object *self, const char *key, t_entity_validator validator) {
  struct s_entity_attributes *attributes = p_entity_alloc(self);
  strncpy(attributes->label, key, d_entity_label_size);
  attributes->factor_z = 1.0;
  attributes->validator = validator;
  return self;
}
d_define_method(entity, get_component)(struct s_object *self, char *label) {
  d_using(entity);
  struct s_entity_component *current_component = NULL;
  d_foreach(&(entity_attributes->components), current_component, struct s_entity_component)
    if (f_string_strcmp(current_component->label, label) == 0)
      break;
  d_cast_return(current_component);
}
d_define_method(entity, add_component)(struct s_object *self, char *label, double speed_x, double speed_y, double speed_z, double offset_point_x,
    double offset_point_y) {
  d_using(entity);
  struct s_entity_component *current_component = NULL;
  if (!d_call(self, m_entity_get_component, label)) {
    if ((current_component = (struct s_entity_component *)d_malloc(sizeof(struct s_entity_component)))) {
      strncpy(current_component->label, label, d_entity_label_size);
      current_component->speed_x = speed_x;
      current_component->speed_y = speed_y;
      current_component->speed_z = speed_z;
      current_component->offset_point_x = offset_point_x;
      current_component->offset_point_y = offset_point_y;
      f_list_append(&(entity_attributes->components), (struct s_list_node *)current_component, e_list_insert_head);
    } else
      d_die(d_error_malloc)
  }
  return self;
}
d_define_method(entity, add_element)(struct s_object *self, char *label, double offset_x, double offset_y, struct s_object *drawable) {
  struct s_drawable_attributes *drawable_attributes = d_cast(self, drawable);
  struct s_entity_component *current_component = NULL;
  struct s_entity_element *current_element = NULL;
  double current_width, current_height, drawable_width, drawable_height;
  if ((current_component = (struct s_entity_component *)d_call(self, m_entity_get_component, label))) {
    if ((current_element = (struct s_entity_element *)d_malloc(sizeof(struct s_entity_element)))) {
      current_element->offset_x = offset_x;
      current_element->offset_y = offset_y;
      current_element->drawable = d_retain(drawable);
      f_list_append(&(current_component->elements), (struct s_list_node *)current_element, e_list_insert_head);
      if (drawable_attributes->last_blend != e_drawable_blend_undefined)
        d_call(current_element->drawable, m_drawable_set_blend, drawable_attributes->last_blend);
      d_call(current_element->drawable, m_drawable_set_maskRGB, (unsigned int)drawable_attributes->last_mask_R, (unsigned int)drawable_attributes->last_mask_G,
          (unsigned int)drawable_attributes->last_mask_B);
      d_call(current_element->drawable, m_drawable_set_maskA, (unsigned int)drawable_attributes->last_mask_A);
      d_call(current_element->drawable, m_drawable_get_dimension, &drawable_width, &drawable_height);
      d_call(&(drawable_attributes->point_dimension), m_point_get, &current_width, &current_height);
      if (current_width < (offset_x + drawable_width))
        current_width = offset_x + drawable_width;
      if (current_height < (offset_y + drawable_height))
        current_height = offset_y + drawable_height;
      d_call(&(drawable_attributes->point_dimension), m_point_set_x, current_width);
      d_call(&(drawable_attributes->point_dimension), m_point_set_y, current_height);
    } else
      d_die(d_error_malloc);
  }
  return self;
}
d_define_method(entity, set_component)(struct s_object *self, char *label) {
  d_using(entity);
  struct s_entity_component *current_component = NULL;
  struct timeval current_refresh;
  if ((current_component = (struct s_entity_component *)d_call(self, m_entity_get_component, label)))
    if (entity_attributes->current_component != current_component) {
      entity_attributes->current_component = current_component;
      gettimeofday(&current_refresh, NULL);
      entity_attributes->last_refresh_x = current_refresh;
      entity_attributes->last_refresh_y = current_refresh;
      entity_attributes->last_refresh_zoom = current_refresh;
    }
  return self;
}
d_define_method(entity, collision)(struct s_object *self, struct s_object *entity) {
  struct s_drawable_attributes *drawable_attributes_self = d_cast(self, drawable), *drawable_attributes_core = d_cast(entity, drawable);
  t_boolean collision = (intptr_t)d_call(&(drawable_attributes_self->square_collision_box), m_square_collision,
      &(drawable_attributes_core->square_collision_box));
  d_cast_return(collision);
}
d_define_method(entity, interact)(struct s_object *self, struct s_object *entity) {
  d_using(entity);
  struct s_drawable_attributes *drawable_attributes_self = d_cast(self, drawable), *drawable_attributes_core = d_cast(entity, drawable);
  struct s_square_attributes *square_attributes = d_cast(&(drawable_attributes_self->square_collision_box), square);
  double position_x, position_y, offset_x = 0.0, offset_y = 0.0;
  t_boolean collision;
  if (entity_attributes->current_component) {
    offset_x = entity_attributes->current_component->offset_point_x;
    offset_y = entity_attributes->current_component->offset_point_y;
  }
  position_x = (square_attributes->top_left_x + ((square_attributes->bottom_right_x - square_attributes->top_left_x) / 2.0)) + offset_x;
  position_y = (square_attributes->top_left_y + ((square_attributes->bottom_right_y - square_attributes->top_left_y) / 2.0)) + offset_y;
  collision = (intptr_t)d_call(&(drawable_attributes_core->square_collision_box), m_square_inside_coordinates, position_x, position_y);
  d_cast_return(collision);
}
d_define_method_override(entity, draw)(struct s_object *self, struct s_object *environment) {
  d_using(entity);
  struct s_drawable_attributes *drawable_attributes_self = d_cast(self, drawable), *drawable_attributes_core;
  struct s_environment_attributes *environment_attributes = d_cast(environment, environment);
  struct s_camera_attributes *camera_attributes = d_cast(environment_attributes->current_camera, camera);
  double local_position_x, local_position_y, local_position_z, local_center_x, local_center_y, local_dimension_w, local_dimension_h, position_x, position_y,
         center_x, center_y, dimension_w = 0.0, dimension_h = 0.0, final_dimension_w = 0.0, final_dimension_h = 0.0, difference_x_seconds, difference_y_seconds,
         difference_zoom_seconds, movement_x, movement_y, movement_zoom, new_x, new_y, new_z;
  struct s_entity_element *current_element;
  struct s_exception *exception;
  struct timeval current_refresh, difference_x, difference_y, difference_zoom;
  if (entity_attributes->current_component) {
    d_call(&(drawable_attributes_self->point_destination), m_point_get, &local_position_x, &local_position_y);
    local_position_z = entity_attributes->factor_z;
    new_x = local_position_x;
    new_y = local_position_y;
    new_z = local_position_z;
    gettimeofday(&current_refresh, NULL);
    timersub(&current_refresh, &(entity_attributes->last_refresh_x), &difference_x);
    timersub(&current_refresh, &(entity_attributes->last_refresh_y), &difference_y);
    timersub(&current_refresh, &(entity_attributes->last_refresh_zoom), &difference_zoom);
    difference_x_seconds = ((double)difference_x.tv_sec + (difference_x.tv_usec / 1000000.0));
    difference_y_seconds = ((double)difference_y.tv_sec + (difference_y.tv_usec / 1000000.0));
    difference_zoom_seconds = ((double)difference_zoom.tv_sec + (difference_zoom.tv_usec / 1000000.0));
    if (fabs(movement_x = (difference_x_seconds * entity_attributes->current_component->speed_x) * drawable_attributes_self->zoom) >
        d_entity_minimum_movement) {
      entity_attributes->last_refresh_x = current_refresh;
      new_x += movement_x;
    }
    if (fabs(movement_y = (difference_y_seconds * entity_attributes->current_component->speed_y) * drawable_attributes_self->zoom) >
        d_entity_minimum_movement) {
      entity_attributes->last_refresh_y = current_refresh;
      new_y += movement_y;
    }
    if (fabs(movement_zoom = (difference_zoom_seconds * entity_attributes->current_component->speed_z)) > d_entity_minimum_zoom) {
      entity_attributes->last_refresh_zoom = current_refresh;
      if ((new_z + movement_zoom) > 0)
        new_z += movement_zoom;
    }
    if (entity_attributes->validator)
      entity_attributes->validator(self, local_position_x, local_position_y, entity_attributes->factor_z, &new_x, &new_y, &new_z);
    d_call(self, m_drawable_set_position, new_x, new_y);
    entity_attributes->factor_z = new_z;
    local_position_x = new_x;
    local_position_y = new_y;
    local_position_z = new_z;
    d_try
    {
      d_foreach(&(entity_attributes->current_component->elements), current_element, struct s_entity_element)
        if (d_cast(current_element->drawable, drawable)) {
          d_call(current_element->drawable, m_drawable_get_dimension, &dimension_w, &dimension_h);
          if ((local_dimension_w = (dimension_w + current_element->offset_x)) > final_dimension_w)
            final_dimension_w = local_dimension_w;
          if ((local_dimension_h = (dimension_h + current_element->offset_y)) > final_dimension_h)
            final_dimension_h = local_dimension_h;
        }
      local_center_x = (final_dimension_w / 2.0);
      local_center_y = (final_dimension_h / 2.0);
      d_call(self, m_drawable_set_dimension, final_dimension_w, final_dimension_h);
      d_call(self, m_drawable_set_center, local_center_x, local_center_y);
      d_foreach(&(entity_attributes->current_component->elements), current_element, struct s_entity_element)
        if ((drawable_attributes_core = d_cast(current_element->drawable, drawable))) {
          position_x = local_position_x + current_element->offset_x;
          position_y = local_position_y + current_element->offset_y;
          center_x = local_center_x - current_element->offset_x;
          center_y = local_center_y - current_element->offset_y;
          d_call(current_element->drawable, m_drawable_set_position, position_x, position_y);
          d_call(current_element->drawable, m_drawable_set_center, center_x, center_y);
          drawable_attributes_core->zoom = (drawable_attributes_self->zoom * local_position_z);
          drawable_attributes_core->angle = drawable_attributes_self->angle;
          if ((d_call(current_element->drawable, m_drawable_normalize_scale, camera_attributes->scene_reference_w, camera_attributes->scene_reference_h,
                  camera_attributes->scene_offset_x, camera_attributes->scene_offset_y, camera_attributes->scene_center_x, camera_attributes->scene_center_y,
                  camera_attributes->screen_w, camera_attributes->screen_h, camera_attributes->scene_zoom)))
            while (((intptr_t)d_call(current_element->drawable, m_drawable_draw, environment)) == d_drawable_return_continue);
        }
    }
    d_catch(exception)
    {
      d_exception_dump(stderr, exception);
      d_raise;
    }
    d_endtry;
  }
  if ((drawable_attributes_self->flags & e_drawable_kind_contour) == e_drawable_kind_contour)
    d_call(self, m_drawable_draw_contour, environment);
  d_cast_return(d_drawable_return_last);
}
d_define_method_override(entity, set_maskRGB)(struct s_object *self, unsigned int red, unsigned int green, unsigned int blue) {
  d_using(entity);
  struct s_drawable_attributes *drawable_attributes = d_cast(self, drawable);
  struct s_entity_component *current_component;
  struct s_entity_element *current_element;
  drawable_attributes->last_mask_R = red;
  drawable_attributes->last_mask_G = green;
  drawable_attributes->last_mask_B = blue;
  d_foreach(&(entity_attributes->components), current_component, struct s_entity_component)
    d_foreach(&(current_component->elements), current_element, struct s_entity_element)
    d_call(current_element->drawable, m_drawable_set_maskRGB, red, green, blue);
  return self;
}
d_define_method_override(entity, set_maskA)(struct s_object *self, unsigned int alpha) {
  d_using(entity);
  struct s_drawable_attributes *drawable_attributes = d_cast(self, drawable);
  struct s_entity_component *current_component;
  struct s_entity_element *current_element;
  drawable_attributes->last_mask_A = alpha;
  d_foreach(&(entity_attributes->components), current_component, struct s_entity_component)
    d_foreach(&(current_component->elements), current_element, struct s_entity_element)
    d_call(current_element->drawable, m_drawable_set_maskA, alpha);
  return self;
}
d_define_method_override(entity, set_blend)(struct s_object *self, enum e_drawable_blends blend) {
  d_using(entity);
  struct s_drawable_attributes *drawable_attributes = d_cast(self, drawable);
  struct s_entity_component *current_component;
  struct s_entity_element *current_element;
  drawable_attributes->last_blend = blend;
  d_foreach(&(entity_attributes->components), current_component, struct s_entity_component)
    d_foreach(&(current_component->elements), current_element, struct s_entity_element)
    d_call(current_element->drawable, m_drawable_set_blend, blend);
  return self;
}
d_define_method(entity, delete)(struct s_object *self, struct s_entity_attributes *attributes) {
  struct s_entity_component *current_component;
  struct s_entity_element *current_element;
  while ((current_component = (struct s_entity_component *)attributes->components.head)) {
    f_list_delete(&(attributes->components), (struct s_list_node *)current_component);
    while ((current_element = (struct s_entity_element *)current_component->elements.head)) {
      f_list_delete(&(current_component->elements), (struct s_list_node *)current_element);
      if (current_element->drawable)
        d_delete(current_element->drawable);
      d_free(current_element);
    }
    d_free(current_component);
  }
  return NULL;
}
d_define_class(entity) {d_hook_method(entity, e_flag_public, add_component),
  d_hook_method(entity, e_flag_private, get_component),
  d_hook_method(entity, e_flag_public, add_element),
  d_hook_method(entity, e_flag_public, set_component),
  d_hook_method(entity, e_flag_public, collision),
  d_hook_method(entity, e_flag_public, interact),
  d_hook_method_override(entity, e_flag_public, drawable, draw),
  d_hook_method_override(entity, e_flag_public, drawable, set_maskRGB),
  d_hook_method_override(entity, e_flag_public, drawable, set_maskA),
  d_hook_method_override(entity, e_flag_public, drawable, set_blend),
  d_hook_delete(entity),
  d_hook_method_tail};
