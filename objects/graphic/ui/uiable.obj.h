/*
 * miranda
 * Copyright (C) 2015 Andrea Nardinocchi (andrea@nardinan.it)
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
#ifndef miranda_graphic_uiable_h
#define miranda_graphic_uiable_h
#include "background.obj.h"
d_declare_class(uiable) {
	struct s_attributes head;
	struct s_object *background;
	double offset_x, offset_y, offset_w, offset_h;
} d_declare_class_tail(uiable);
struct s_uiable_attributes *p_uiable_alloc(struct s_object *self, int flags);
extern struct s_object *f_uiable_new(struct s_object *self, int flags);
d_declare_method(uiable, set_background)(struct s_object *self, struct s_object *drawable, enum e_background_components component);
d_declare_method(uiable, set_offset)(struct s_object *self, double offset_x, double offset_y, double offset_w, double offset_h);
d_declare_method(uiable, draw_background)(struct s_object *self, struct s_object *environment);
d_declare_method(uiable, delete)(struct s_object *self, struct s_uiable_attributes *attributes);
#endif
