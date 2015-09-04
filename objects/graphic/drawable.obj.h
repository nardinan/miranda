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
#ifndef miranda_graphic_drawable_h
#define miranda_graphic_drawable_h
#include <SDL2/SDL_render.h>
#include "../math/point.obj.h"
#define d_drawable_return_continue 	1
#define d_drawable_return_last 		0
typedef enum e_drawable_kinds {
	e_drawable_kind_single =	0x01,
	e_drawable_kind_multiple = 	0x02,
	e_drawable_kind_hidden = 	0x04
} e_drawable_kinds;
typedef enum e_drawable_flips {
	e_drawable_flip_horizontal = 	SDL_FLIP_HORIZONTAL,
	e_drawable_flip_vertical =	SDL_FLIP_VERTICAL,
	e_drawable_flip_none =		SDL_FLIP_NONE
} e_drawable_flips;
d_declare_class(drawable) {
	struct s_attributes head;
	struct s_object point_destination, point_normalized_destination, point_dimension, point_normalized_dimension, point_center, point_normalized_center;
	double angle, zoom;
	enum e_drawable_flips flip;
	int flags;
} d_declare_class_tail(drawable);
struct s_drawable_attributes *p_drawable_alloc(struct s_object *self);
extern struct s_object *f_drawable_new(struct s_object  *self, int flags);
d_declare_method(drawable, draw)(struct s_object *self, struct s_object *environment);
d_declare_method(drawable, normalize_scale)(struct s_object *self, double reference_w, double reference_h, double offset_x, double offset_y,
		double focus_x, double focus_y, double current_w, double current_h, double zoom);
d_declare_method(drawable, set_position)(struct s_object *self, double x, double y);
d_declare_method(drawable, set_center)(struct s_object *self, double x, double y);
d_declare_method(drawable, set_angle)(struct s_object *self, double angle);
d_declare_method(drawable, set_zoom)(struct s_object *self, double zoom);
d_declare_method(drawable, flip)(struct s_object *self, enum e_drawable_flips flip);
d_declare_method(drawable, get_flags)(struct s_object *self);
#endif
