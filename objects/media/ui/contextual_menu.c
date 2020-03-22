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
#include "contextual_menu.h"
#include "../camera.obj.h"
struct s_contextual_menu_attributes *p_contextual_menu_alloc(struct s_object *self) {
  struct s_contextual_menu_attributes *result = d_prepare(self, contextual_menu);
  f_mutex_new(self);    /* inherit */
  f_memory_new(self);   /* inherit */
  f_uiable_new(self);   /* inherit */
  return result;
}
extern struct s_object *f_contextual_menu_new(struct s_object *self) {
  struct s_contextual_menu_attributes *attributes = p_contextual_menu_alloc(self);
  struct s_uiable_attributes *uiable_attributes = d_cast(self, uiable);
  attributes->status = e_contextual_menu_status_hidden;
  uiable_attributes->inherit_visibility_from_parent = d_true;
  return self;
}
d_define_method(contextual_menu, set)(struct s_object *self, struct s_object *list) {
  d_using(contextual_menu);
  if (contextual_menu_attributes->list)
    d_delete(contextual_menu_attributes->list);
  contextual_menu_attributes->list = d_retain(list);
  d_call(contextual_menu_attributes->list, m_list_make_unscrollable, d_true);
  d_call(contextual_menu_attributes->list, m_list_make_multi_selection, d_false);
  return self;
}
d_define_method(contextual_menu, get_selected_uiable)(struct s_object *self) {
  d_using(contextual_menu);
  return contextual_menu_attributes->selected_element;
}
d_define_method_override(contextual_menu, event)(struct s_object *self, struct s_object *environment, SDL_Event *current_event) {
  d_using(contextual_menu);
  struct s_drawable_attributes *drawable_attributes_list;
  ssize_t *selection;
  int mouse_x, mouse_y, index;
  t_boolean changed = d_false;
  if (contextual_menu_attributes->list) {
    if (contextual_menu_attributes->status == e_contextual_menu_status_visible) {
      d_call(contextual_menu_attributes->list, m_eventable_event, environment, current_event);
      if ((selection = d_call(contextual_menu_attributes->list, m_list_get_selected_uiable, NULL))) {
        for (index = 0; (!contextual_menu_attributes->selected_element) && (index < d_list_max_selected); ++index)
          if (selection[index] != d_list_selected_null)
            if ((contextual_menu_attributes->selected_element = d_retain(d_call(contextual_menu_attributes->list, m_list_get_uiable,
                      (unsigned int)selection[index])))) {
              contextual_menu_attributes->status = e_contextual_menu_status_hidden;
              d_call(self, m_emitter_raise, v_uiable_signals[e_uiable_signal_selected]);
            }
      }
      changed = d_true;
    }
    if (current_event->type == SDL_MOUSEBUTTONUP) {
      SDL_GetMouseState(&mouse_x, &mouse_y);
      if (current_event->button.button == SDL_BUTTON_RIGHT) {
        if (contextual_menu_attributes->status == e_contextual_menu_status_hidden) {
          contextual_menu_attributes->status = e_contextual_menu_status_visible;
          d_call(contextual_menu_attributes->list, m_drawable_set_position, (double)mouse_x, (double)mouse_y);
          d_call(contextual_menu_attributes->list, m_list_reset_select, NULL);
          d_call(self, m_emitter_raise, v_uiable_signals[e_uiable_signal_unselected]);
          if (contextual_menu_attributes->selected_element) {
            d_delete(contextual_menu_attributes->selected_element);
            contextual_menu_attributes->selected_element = NULL;
          }
        } else
          contextual_menu_attributes->status = e_contextual_menu_status_hidden;
        d_call(self, m_emitter_raise, v_uiable_signals[e_uiable_signal_changed]);
        changed = d_true;
      } else if ((current_event->button.button == SDL_BUTTON_LEFT) && (contextual_menu_attributes->status == e_contextual_menu_status_visible)) {
        drawable_attributes_list = d_cast(contextual_menu_attributes->list, drawable);
        if (!((intptr_t)d_call(&(drawable_attributes_list->square_collision_box), m_square_inside_coordinates, (double)mouse_x, (double)mouse_y))) {
          contextual_menu_attributes->status = e_contextual_menu_status_hidden;
          d_call(self, m_emitter_raise, v_uiable_signals[e_uiable_signal_changed]);
          changed = d_true;
        }
      }
    }
  }
  d_cast_return(((changed)?e_eventable_status_captured:e_eventable_status_ignored));
}
d_define_method_override(contextual_menu, draw)(struct s_object *self, struct s_object *environment) {
  d_using(contextual_menu);
  struct s_drawable_attributes *drawable_attributes_self = d_cast(self, drawable), *drawable_attributes_list, *drawable_attributes_scroll,
                               *drawable_attributes_component;
  struct s_uiable_attributes *uiable_attributes_list;
  struct s_environment_attributes *environment_attributes = d_cast(environment, environment);
  struct s_camera_attributes *camera_attributes = d_cast(environment_attributes->current_camera, camera);
  struct s_list_attributes *list_attributes;
  struct s_object *current_node;
  int result = (intptr_t)d_call_owner(self, uiable, m_drawable_draw, environment); /* recall the father's draw method */
  double dimension_w_scroll = 0, dimension_h_scroll, dimension_h_components = 0, dimension_w_self, dimension_h_self, dimension_w_uiable, dimension_h_uiable;
  if ((contextual_menu_attributes->list) && (contextual_menu_attributes->status == e_contextual_menu_status_visible)) {
    list_attributes = d_cast(contextual_menu_attributes->list, list);
    drawable_attributes_list = d_cast(contextual_menu_attributes->list, drawable);
    drawable_attributes_scroll = d_cast(list_attributes->scroll, drawable);
    uiable_attributes_list = d_cast(contextual_menu_attributes->list, uiable);
    if (!list_attributes->unscrollable)
      d_call(&(drawable_attributes_scroll->point_dimension), m_point_get, &dimension_w_scroll, &dimension_h_scroll);
    d_call(&(drawable_attributes_self->point_dimension), m_point_get, &dimension_w_self, &dimension_h_self);
    d_foreach(&(list_attributes->uiables), current_node, struct s_object) {
      drawable_attributes_component = d_cast(current_node, drawable);
      d_call(&(drawable_attributes_component->point_dimension), m_point_get, &dimension_w_uiable, &dimension_h_uiable);
      if (dimension_w_uiable > dimension_w_self)
        dimension_w_self = dimension_w_uiable;
      dimension_h_components += dimension_h_uiable;
    }
    if (dimension_h_components > dimension_h_self)
      dimension_h_self = dimension_h_components;
    d_call(&(drawable_attributes_list->point_dimension), m_point_set, (dimension_w_self + dimension_w_scroll + (uiable_attributes_list->border_w * 2.0)),
        dimension_h_self + (uiable_attributes_list->border_h * 2.0));
    drawable_attributes_list->angle = drawable_attributes_self->angle;
    drawable_attributes_list->zoom = drawable_attributes_self->zoom;
    drawable_attributes_list->flip = drawable_attributes_self->flip;
    if ((d_call(contextual_menu_attributes->list, m_drawable_normalize_scale, camera_attributes->scene_reference_w, camera_attributes->scene_reference_h,
            camera_attributes->scene_offset_x, camera_attributes->scene_offset_y, camera_attributes->scene_center_x, camera_attributes->scene_center_y,
            camera_attributes->screen_w, camera_attributes->screen_h, camera_attributes->scene_zoom))) {
      while (((intptr_t)d_call(contextual_menu_attributes->list, m_drawable_draw, environment)) == d_drawable_return_continue);
    }
  }
  d_cast_return(result);
}
d_define_method(contextual_menu, delete)(struct s_object *self, struct s_contextual_menu_attributes *attributes) {
  if (attributes->list)
    d_delete(attributes->list);
  if (attributes->selected_element) {
    d_delete(attributes->selected_element);
    attributes->selected_element = NULL;
  }
  return NULL;
}
d_define_class(contextual_menu) {d_hook_method(contextual_menu, e_flag_public, set),
  d_hook_method(contextual_menu, e_flag_public, get_selected_uiable),
  d_hook_method_override(contextual_menu, e_flag_public, eventable, event),
  d_hook_method_override(contextual_menu, e_flag_public, drawable, draw),
  d_hook_delete(contextual_menu),
  d_hook_method_tail};
