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
#ifndef miranda_media_button_h
#define miranda_media_button_h
#include "label.obj.h"
#define d_button_alignment e_label_alignment_center
#define d_button_background e_label_background_format_fixed
d_declare_class(button) {
	struct s_attributes head;
} d_declare_class_tail(button);
struct s_button_attributes *p_button_alloc(struct s_object *self, char *string_content, TTF_Font *font, struct s_object *environment);
extern struct s_object *f_button_new(struct s_object *self, char *string_content, TTF_Font *font, struct s_object *environment);
d_declare_method(button, event)(struct s_object *self, struct s_object *environment, SDL_Event *current_event);
d_declare_method(button, draw)(struct s_object *self, struct s_object *environment);
d_declare_method(button, delete)(struct s_object *self, struct s_button_attributes *attributes);
#endif
