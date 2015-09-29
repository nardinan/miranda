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
#ifndef miranda_graphic_background_h
#define miranda_graphic_background_h
#include "../environment.obj.h"
enum e_background_components {
	e_background_component_top,
	e_background_component_bottom,
	e_background_component_left,
	e_background_component_right,
	e_background_component_center,
	e_background_component_corner_top_left,
	e_background_component_corner_top_right,
	e_background_component_corner_bottom_left,
	e_background_component_corner_bottom_right,
	e_background_component_NULL
} e_background_components;
d_declare_class(background) {
	struct s_attributes head;
	struct s_object *background[e_background_component_NULL];
	enum e_drawable_blends last_blend;
	double last_mask_R, last_mask_G, last_mask_B, last_mask_A;
} d_declare_class_tail(background);
struct s_background_attributes *p_background_alloc(struct s_object *self);
extern struct s_object *f_background_new(struct s_object *self);
d_declare_method(background, set)(struct s_object *self, struct s_object *drawable, enum e_background_components component);
d_declare_method(background, draw)(struct s_object *self, struct s_object *environment);
d_declare_method(background, set_maskRGB)(struct s_object *self, unsigned int red, unsigned int green, unsigned int blue);
d_declare_method(background, set_maskA)(struct s_object *self, unsigned int alpha);
d_declare_method(background, set_blend)(struct s_object *self, enum e_drawable_blends blend);
d_declare_method(background, delete)(struct s_object *self, struct s_background_attributes *attributes);
#endif
