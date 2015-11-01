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
#ifndef miranda_graphic_label_h
#define miranda_graphic_label_h
#include "fonts.obj.h"
#include "uiable.obj.h"
d_declare_class(label) {
	struct s_attributes head;
	SDL_Texture *image;
	TTF_Font *last_font;
	struct s_object *string_content;
	enum e_drawable_blends last_blend;
	double last_mask_R, last_mask_G, last_mask_B, last_mask_A;
} d_declare_class_tail(label);
struct s_label_attributes *p_label_alloc(struct s_object *self);
extern struct s_object *f_label_new(struct s_object *self, struct s_object *string_content, TTF_Font *font, struct s_object *environment);
d_declare_method(label, set_content)(struct s_object *self, struct s_object *string_content, TTF_Font *font, struct s_object *environment);
d_declare_method(label, draw)(struct s_object *self, struct s_object *environment);
d_declare_method(label, set_maskRGB)(struct s_object *self, unsigned int red, unsigned int green, unsigned int blue);
d_declare_method(label, set_maskA)(struct s_object *self, unsigned int alpha);
d_declare_method(label, set_blend)(struct s_object *self, enum e_drawable_blends blend);
d_declare_method(label, delete)(struct s_object *self, struct s_label_attributes *attributes);
#endif
