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
#ifndef miranda_media_scroll_h
#define miranda_media_scroll_h
#include "uiable.obj.h"
#define d_scroll_default_minimum 0
#define d_scroll_default_maximum 1000
d_declare_class(scroll) {
    struct s_attributes head;
    struct s_object *image;
    int minimum, maximum, position, modifier;
    t_boolean is_selected, force_event;
} d_declare_class_tail(scroll);
struct s_scroll_attributes *p_scroll_alloc(struct s_object *self);
extern struct s_object *f_scroll_new(struct s_object *self, struct s_object *image);
d_declare_method(scroll, set_range)(struct s_object *self, int minimum, int maximum);
d_declare_method(scroll, set_modifier)(struct s_object *self, int modifier);
d_declare_method(scroll, set_position)(struct s_object *self, int position);
d_declare_method(scroll, get_position)(struct s_object *self);
d_declare_method(scroll, event)(struct s_object *self, struct s_object *environment, SDL_Event *current_event);
d_declare_method(scroll, draw)(struct s_object *self, struct s_object *environment);
d_declare_method(scroll, set_maskRGB)(struct s_object *self, unsigned int red, unsigned int green, unsigned int blue);
d_declare_method(scroll, set_maskA)(struct s_object *self, unsigned int alpha);
d_declare_method(scroll, set_blend)(struct s_object *self, enum e_drawable_blends blend);
d_declare_method(scroll, delete)(struct s_object *self, struct s_scroll_attributes *attributes);
#endif
