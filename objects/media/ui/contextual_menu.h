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
#ifndef miranda_media_contextual_menu_h
#define miranda_media_contextual_menu_h
#include "list.obj.h"
typedef enum e_contextual_menu_statuses {
  e_contextual_menu_status_visible,
  e_contextual_menu_status_hidden
} e_contextual_menu_statuses;
d_declare_class(contextual_menu) {
  struct s_attributes head;
  struct s_object *list;
  struct s_object *selected_element;
  e_contextual_menu_statuses status;
} d_declare_class_tail(contextual_menu);
struct s_contextual_menu_attributes *p_contextual_menu_alloc(struct s_object *self);
extern struct s_object *f_contextual_menu_new(struct s_object *self);
d_declare_method(contextual_menu, set)(struct s_object *self, struct s_object *list);
d_declare_method(contextual_menu, get_selected_uiable)(struct s_object *self);
d_declare_method(contextual_menu, event)(struct s_object *self, struct s_object *environment, SDL_Event *current_event);
d_declare_method(contextual_menu, draw)(struct s_object *self, struct s_object *environment);
d_declare_method(contextual_menu, delete)(struct s_object *self, struct s_contextual_menu_attributes *attributes);
#endif
