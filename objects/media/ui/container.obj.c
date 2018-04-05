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
#include "container.obj.h"
#include "../camera.obj.h"
struct s_container_attributes *p_container_alloc(struct s_object *self) {
  struct s_container_attributes *result = d_prepare(self, container);
  f_mutex_new(self);    /* inherit */
  f_memory_new(self);   /* inherit */
  f_uiable_new(self);   /* inherit */
  return result;
}
struct s_object *f_container_new(struct s_object *self, double border_top, double border_bottom, double border_left, double border_right, t_boolean floatable) {
  struct s_container_attributes *attributes = p_container_alloc(self);
  attributes->border_top = border_top;
  attributes->border_bottom = border_bottom;
  attributes->border_left = border_left;
  attributes->border_right = border_right;
  attributes->distributed_zoom = 1;
  if ((attributes->floatable = floatable)) {
    d_call(self, m_morphable_set_freedom_x, d_true);
    d_call(self, m_morphable_set_freedom_y, d_true);
  }
  return self;
}
d_define_method(container, add_drawable)(struct s_object *self, struct s_object *drawable, double position_x, double position_y) {
  d_using(container);
  struct s_container_drawable *current_container;
  if ((current_container = (struct s_container_drawable *)d_malloc(sizeof(struct s_container_drawable)))) {
    current_container->drawable = d_retain(drawable);
    current_container->position_x = position_x;
    current_container->position_y = position_y;
    f_list_append(&(container_attributes->entries), (struct s_list_node *)current_container, e_list_insert_tail);
  } else
    d_die(d_error_malloc);
  return self;
}
d_define_method(container, del_drawable)(struct s_object *self, struct s_object *drawable) {
  d_using(container);
  struct s_container_drawable *current_container;
  d_foreach(&(container_attributes->entries), current_container, struct s_container_drawable)
    if (current_container->drawable == drawable) {
      f_list_delete(&(container_attributes->entries), (struct s_list_node *)current_container);
      d_delete(current_container->drawable);
      d_free(current_container);
      break;
    }
  return self;
}
d_define_method(container, get_drawable)(struct s_object *self, struct s_object *drawable) {
  d_using(container);
  struct s_container_drawable *result = NULL;
  d_foreach(&(container_attributes->entries), result, struct s_container_drawable)
    if (result->drawable == drawable)
      break;
  d_cast_return(result);
}
d_define_method_override(container, event)(struct s_object *self, struct s_object *environment, SDL_Event *current_event) {
  d_using(container);
  struct s_eventable_attributes *eventable_attributes;
  struct s_uiable_attributes *uiable_attributes;
  struct s_container_drawable *current_container;
  struct s_object *result = d_call_owner(self, uiable, m_eventable_event, environment, current_event);
  struct s_exception *exception;
  d_try
      {
        d_foreach(&(container_attributes->entries), current_container, struct s_container_drawable) {
          if (((uiable_attributes = d_cast(current_container->drawable, uiable))) &&
              ((eventable_attributes = d_cast(current_container->drawable, eventable)))) {
            d_call_owner(current_container->drawable, uiable, m_eventable_event, environment, current_event);
            if ((uiable_attributes->is_selected) && (eventable_attributes->enable))
              d_call(current_container->drawable, m_eventable_event, environment, current_event);
          }
        }
      }
    d_catch(exception)
      {
        d_exception_dump(stderr, exception);
        d_raise;
      }
  d_endtry;
  return result;
}
d_define_method_override(container, draw)(struct s_object *self, struct s_object *environment) {
  d_using(container);
  struct s_drawable_attributes *drawable_attributes_self = d_cast(self, drawable), *drawable_attributes_entry;
  struct s_uiable_attributes *uiable_attributes_self = d_cast(self, uiable), *uiable_attributes_entry;
  struct s_square_attributes *square_attributes;
  struct s_environment_attributes *environment_attributes = d_cast(environment, environment);
  struct s_camera_attributes *camera_attributes = d_cast(environment_attributes->current_camera, camera);
  struct s_container_drawable *current_container;
  double position_x_self, position_y_self, normalized_position_x_self, normalized_position_y_self, position_x_entry, position_y_entry, center_x_self,
    center_y_self, center_x_entry, center_y_entry, max_w = container_attributes->border_left + container_attributes->border_right,
    max_h = container_attributes->border_top + container_attributes->border_bottom, current_w, current_h;
  int result = d_drawable_return_last;
  d_call(&(drawable_attributes_self->point_destination), m_point_get, &position_x_self, &position_y_self);
  d_call(&(drawable_attributes_self->point_normalized_destination), m_point_get, &normalized_position_x_self, &normalized_position_y_self);
  d_call(&(drawable_attributes_self->point_center), m_point_get, &center_x_self, &center_y_self);
  d_foreach(&(container_attributes->entries), current_container, struct s_container_drawable) {
    drawable_attributes_entry = d_cast(current_container->drawable, drawable);
    position_x_entry = position_x_self + current_container->position_x + container_attributes->border_left;
    position_y_entry = position_y_self + current_container->position_y + container_attributes->border_top;
    center_x_entry = (position_x_self + center_x_self) - position_x_entry;
    center_y_entry = (position_y_self + center_y_self) - position_y_entry;
    d_call(current_container->drawable, m_drawable_set_position, position_x_entry, position_y_entry);
    if (((drawable_attributes_entry->flags & e_drawable_kind_ui_no_attribute_angle) != e_drawable_kind_ui_no_attribute_angle) &&
        ((drawable_attributes_entry->flags & e_drawable_kind_ui_no_attribute_zoom) != e_drawable_kind_ui_no_attribute_zoom))
      d_call(current_container->drawable, m_drawable_set_center, center_x_entry, center_y_entry);
    if ((drawable_attributes_entry->flags & e_drawable_kind_ui_no_attribute_angle) != e_drawable_kind_ui_no_attribute_angle)
      drawable_attributes_entry->angle = drawable_attributes_self->angle;
    if ((drawable_attributes_entry->flags & e_drawable_kind_ui_no_attribute_zoom) != e_drawable_kind_ui_no_attribute_zoom)
      drawable_attributes_entry->zoom = container_attributes->distributed_zoom;
    if ((drawable_attributes_entry->flags & e_drawable_kind_ui_no_attribute_flip) != e_drawable_kind_ui_no_attribute_flip)
      drawable_attributes_entry->flip = drawable_attributes_self->flip;
    if ((d_call(current_container->drawable, m_drawable_normalize_scale, camera_attributes->scene_reference_w, camera_attributes->scene_reference_h,
                camera_attributes->scene_offset_x, camera_attributes->scene_offset_y, camera_attributes->scene_center_x, camera_attributes->scene_center_y,
                camera_attributes->screen_w, camera_attributes->screen_h, camera_attributes->scene_zoom))) {
      square_attributes = d_cast(&(drawable_attributes_entry->square_collision_box), square);
      current_w = d_math_max(d_math_max(square_attributes->normalized_top_left_x, square_attributes->normalized_top_right_x),
                             d_math_max(square_attributes->normalized_bottom_left_x, square_attributes->normalized_bottom_right_x)) -
                  normalized_position_x_self;
      current_h = d_math_max(d_math_max(square_attributes->normalized_top_left_y, square_attributes->normalized_top_right_y),
                             d_math_max(square_attributes->normalized_bottom_left_y, square_attributes->normalized_bottom_right_y)) -
                  normalized_position_y_self;
      if ((uiable_attributes_entry = d_cast(current_container->drawable, uiable))) {
        /* we need to take in consideration the border of the object that is not considered in the collision square */
        current_w += uiable_attributes_entry->border_w;
        current_h += uiable_attributes_entry->border_h;
      }
      /* normalization for ratio and environmental zoom */
      current_w = ((current_w * camera_attributes->scene_reference_w) / camera_attributes->screen_w) / camera_attributes->scene_zoom;
      current_h = ((current_h * camera_attributes->scene_reference_h) / camera_attributes->screen_h) / camera_attributes->scene_zoom;
      max_w = d_math_max(max_w, current_w);
      max_h = d_math_max(max_h, current_h);
    }
  }
  d_call(self, m_drawable_set_dimension, (max_w + uiable_attributes_self->border_w), (max_h + uiable_attributes_self->border_h));
  if ((d_call(self, m_drawable_normalize_scale, camera_attributes->scene_reference_w, camera_attributes->scene_reference_h, camera_attributes->scene_offset_x,
              camera_attributes->scene_offset_y, camera_attributes->scene_center_x, camera_attributes->scene_center_y, camera_attributes->screen_w,
              camera_attributes->screen_h, camera_attributes->scene_zoom))) {
    result = (intptr_t)d_call_owner(self, uiable, m_drawable_draw, environment); /* recall the father's draw method */
    d_foreach(&(container_attributes->entries), current_container, struct s_container_drawable)
      while (((intptr_t)d_call(current_container->drawable, m_drawable_draw, environment)) == d_drawable_return_continue);
  }
  if ((drawable_attributes_self->flags & e_drawable_kind_contour) == e_drawable_kind_contour)
    d_call(self, m_drawable_draw_contour, environment);
  d_cast_return(result);
}
d_define_method_override(container, set_zoom)(struct s_object *self, double zoom) {
  d_using(container);
  container_attributes->distributed_zoom = zoom;
  return self;
}
d_define_method(container, delete)(struct s_object *self, struct s_container_attributes *attributes) {
  struct s_container_drawable *current_container;
  while ((current_container = (struct s_container_drawable *)attributes->entries.head)) {
    f_list_delete(&(attributes->entries), (struct s_list_node *)current_container);
    d_delete(current_container->drawable);
    d_free(current_container);
  }
  return NULL;
}
d_define_class(container) {d_hook_method(container, e_flag_public, add_drawable),
                           d_hook_method(container, e_flag_public, del_drawable),
                           d_hook_method(container, e_flag_public, get_drawable),
                           d_hook_method_override(container, e_flag_public, eventable, event),
                           d_hook_method_override(container, e_flag_public, drawable, draw),
                           d_hook_method_override(container, e_flag_public, drawable, set_zoom),
                           d_hook_delete(container),
                           d_hook_method_tail};
