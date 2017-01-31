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
#ifndef miranda_media_drawable_h
#define miranda_media_drawable_h
#include <SDL2/SDL_render.h>
#include "../geometry/point.obj.h"
#include "../geometry/square.obj.h"
#include "environment.obj.h"
#define d_drawable_point_inside(x,y,xc,yc,wc,hc) (((x>=xc)&&(x<=(xc+wc)))&&((y>=yc)&&(y<=(yc+hc))))
#define d_drawable_return_continue 	1
#define d_drawable_return_last 		0
#define d_drawable_default_contour_color 255.0, 0, 0, 255.0
typedef enum e_drawable_kinds {
    e_drawable_kind_single 			        = 0x01,
    e_drawable_kind_multiple 		        = 0x02,
    e_drawable_kind_hidden 			        = 0x04,
    e_drawable_kind_force_visibility 	    = 0x08,
    e_drawable_kind_contour			        = 0x10,
    e_drawable_kind_ui_no_attribute_zoom 	= 0x20,
    e_drawable_kind_ui_no_attribute_angle	= 0x40,
    e_drawable_kind_ui_no_attribute_flip	= 0x80
} e_drawable_kinds;
typedef enum e_drawable_flips {
    e_drawable_flip_horizontal 	= SDL_FLIP_HORIZONTAL,
    e_drawable_flip_vertical 	= SDL_FLIP_VERTICAL,
    e_drawable_flip_both	 	= (SDL_FLIP_HORIZONTAL|SDL_FLIP_VERTICAL),
    e_drawable_flip_none 		= SDL_FLIP_NONE
} e_drawable_flips;
typedef enum e_drawable_blends {
    e_drawable_blend_alpha 		= SDL_BLENDMODE_BLEND,
    e_drawable_blend_add 		= SDL_BLENDMODE_ADD,
    e_drawable_blend_mod 		= SDL_BLENDMODE_MOD,
    e_drawable_blend_none 		= SDL_BLENDMODE_NONE,
    e_drawable_blend_undefined
} e_drawable_blends;
d_declare_class(drawable) {
    struct s_attributes head;
    struct s_object point_destination, point_normalized_destination, point_dimension, point_normalized_dimension, point_center, point_normalized_center;
    struct s_object square_collision_box;
    double angle, zoom;
    enum e_drawable_flips flip;
    int flags;
    enum e_drawable_blends last_blend;
    double last_mask_R, last_mask_G, last_mask_B, last_mask_A;
} d_declare_class_tail(drawable);
struct s_drawable_attributes *p_drawable_alloc(struct s_object *self);
extern struct s_object *f_drawable_new(struct s_object *self, int flags);
d_declare_method(drawable, draw)(struct s_object *self, struct s_object *environment); /* abstract */
d_declare_method(drawable, draw_contour)(struct s_object *self, struct s_object *environment);
d_declare_method(drawable, set_maskRGB)(struct s_object *self, unsigned int red, unsigned int green, unsigned int blue); /* abstract */
d_declare_method(drawable, set_maskA)(struct s_object *self, unsigned int alpha); /* abstract */
d_declare_method(drawable, set_blend)(struct s_object *self, enum e_drawable_blends blend); /* abstract */
d_declare_method(drawable, normalize_scale)(struct s_object *self, double reference_w, double reference_h, double offset_x, double offset_y,
        double focus_x, double focus_y, double current_w, double current_h, double zoom);
d_declare_method(drawable, keep_scale)(struct s_object *self, double current_w, double current_h);
d_declare_method(drawable, set_position)(struct s_object *self, double x, double y);
d_declare_method(drawable, set_position_x)(struct s_object *self, double x);
d_declare_method(drawable, set_position_y)(struct s_object *self, double y);
d_declare_method(drawable, get_position)(struct s_object *self, double *x, double *y);
d_declare_method(drawable, get_scaled_position)(struct s_object *self, double *x, double *y);
d_declare_method(drawable, get_scaled_center)(struct s_object *self, double *x, double *y);
d_declare_method(drawable, set_dimension)(struct s_object *self, double w, double h);
d_declare_method(drawable, set_dimension_w)(struct s_object *self, double w);
d_declare_method(drawable, set_dimension_h)(struct s_object *self, double h);
d_declare_method(drawable, get_dimension)(struct s_object *self, double *w, double *h);
d_declare_method(drawable, get_scaled_dimension)(struct s_object *self, double *w, double *h);
d_declare_method(drawable, get_principal_point)(struct s_object *self, double *x, double *y);
d_declare_method(drawable, get_scaled_principal_point)(struct s_object *self, double *x, double *y);
d_declare_method(drawable, set_center)(struct s_object *self, double x, double y);
d_declare_method(drawable, set_angle)(struct s_object *self, double angle);
d_declare_method(drawable, set_zoom)(struct s_object *self, double zoom);
d_declare_method(drawable, get_zoom)(struct s_object *self, double *zoom);
d_declare_method(drawable, flip)(struct s_object *self, enum e_drawable_flips flip);
d_declare_method(drawable, set_flags)(struct s_object *self, int flags);
d_declare_method(drawable, add_flags)(struct s_object *self, int flags);
d_declare_method(drawable, get_flags)(struct s_object *self);
#endif
