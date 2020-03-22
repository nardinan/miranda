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
#include "button.obj.h"
struct s_button_attributes *p_button_alloc(struct s_object *self, char *string_content, TTF_Font *font, struct s_object *environment) {
  struct s_button_attributes *result = d_prepare(self, button);
  f_label_new_alignment(self, string_content, font, d_button_background, d_button_alignment, d_button_alignment, environment); /* inherit */
  return result;
}
struct s_object *f_button_new(struct s_object *self, char *string_content, TTF_Font *font, struct s_object *environment) {
  struct s_button_attributes *attributes = p_button_alloc(self, string_content, font, environment);
  attributes = attributes;
  return self;
}
d_define_method_override(button, event)(struct s_object *self, struct s_object *environment, SDL_Event *current_event) {
  t_boolean changed = d_false;
  if ((current_event->type == SDL_MOUSEBUTTONUP) && (current_event->button.button == SDL_BUTTON_LEFT)) {
    d_call(self, m_uiable_mode, e_uiable_mode_active);
    changed = d_true;
  }
  d_cast_return(((changed)?e_eventable_status_captured:e_eventable_status_ignored));
}
d_define_class(button) {d_hook_method_override(button, e_flag_public, eventable, event),
  d_hook_method_tail};
