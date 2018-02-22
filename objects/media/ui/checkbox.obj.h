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
#ifndef miranda_media_checkbox_h
#define miranda_media_checkbox_h
#include "label.obj.h"
#define d_checkbox_alignment e_label_alignment_left
#define d_checkbox_background e_label_background_format_adaptable
d_declare_class(checkbox) {
  struct s_attributes head;
  struct s_object *checked, *unchecked;
  t_boolean is_checked;
} d_declare_class_tail(checkbox);
struct s_checkbox_attributes *p_checkbox_alloc(struct s_object *self, char *string_content, TTF_Font *font, struct s_object *environment);
extern struct s_object *f_checkbox_new(struct s_object *self, char *string_content, TTF_Font *font, struct s_object *environment);
d_declare_method(checkbox, set_drawable)(struct s_object *self, struct s_object *checked, struct s_object *unchecked);
d_declare_method(checkbox, set_checked)(struct s_object *self, t_boolean is_checked);
d_declare_method(checkbox, get_checked)(struct s_object *self);
d_declare_method(checkbox, event)(struct s_object *self, struct s_object *environment, SDL_Event *current_event);
d_declare_method(checkbox, draw)(struct s_object *self, struct s_object *environment);
d_declare_method(checkbox, delete)(struct s_object *self, struct s_checkbox_attributes *attributes);
#endif
