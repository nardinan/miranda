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
#ifndef miranda_graphic_animation_h
#define miranda_graphic_animation_h
#include <sys/time.h>
#include "environment.obj.h"
#define d_animation_infinite_loop -1
typedef enum e_animation_directions {
	e_animation_direction_forward,
	e_animation_direction_rewind,
	e_animation_direction_stop
} e_animation_directions;
typedef struct s_animation_frame { d_list_node_head;
	struct s_object *drawable;
	double offset_x, offset_y, zoom, time;
} s_animation_frame;
d_declare_class(animation) {
	struct s_attributes head;
	struct s_list frames;
	struct s_animation_frame *current_frame;
	struct timeval last_update;
	int cycles, remaining_cycles;
	enum e_animation_directions status;
	enum e_drawable_blends last_blend;
	double last_mask_R, last_mask_G, last_mask_B, last_mask_A, time_ratio;
} d_declare_class_tail(animation);
struct s_animation_attributes *p_animation_alloc(struct s_object *self);
extern struct s_object *f_animation_new(struct s_object *self, int cycles, double time_ratio);
d_declare_method(animation, append_frame)(struct s_object *self, struct s_object *drawable, double offset_x, double offset_y, double zoom, double time);
d_declare_method(animation, set_status)(struct s_object *self, enum e_animation_directions status);
d_declare_method(animation, get_status)(struct s_object *self);
d_declare_method(animation, draw)(struct s_object *self, struct s_object *environment);
d_declare_method(animation, set_maskRGB)(struct s_object *self, unsigned int red, unsigned int green, unsigned int blue);
d_declare_method(animation, set_maskA)(struct s_object *self, unsigned int alpha);
d_declare_method(animation, set_blend)(struct s_object *self, enum e_drawable_blends blend);
d_declare_method(animation, delete)(struct s_object *self, struct s_animation_attributes *attributes);
#endif
