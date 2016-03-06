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
#include "drawable.obj.h"
struct s_drawable_attributes *p_drawable_alloc(struct s_object *self) {
	struct s_drawable_attributes *result = d_prepare(self, drawable);
	/* abstract (no memory inheritance) */
	return result;
}

struct s_object *f_drawable_new(struct s_object *self, int flags) {
	struct s_drawable_attributes *attributes = p_drawable_alloc(self);
	f_point_new(d_use(&(attributes->point_destination), point), 0, 0);
	f_point_new(d_use(&(attributes->point_normalized_destination), point), 0, 0);
	f_point_new(d_use(&(attributes->point_dimension), point), 0, 0);
	f_point_new(d_use(&(attributes->point_normalized_dimension), point), 0, 0);
	f_point_new(d_use(&(attributes->point_center), point), 0, 0);
	f_point_new(d_use(&(attributes->point_normalized_center), point), 0, 0);
	f_square_new(d_use(&(attributes->square_collision_box), square), 0, 0, 0, 0);
	attributes->zoom = 1;
	attributes->angle = 0;
	attributes->flip = e_drawable_flip_none;
	attributes->flags = flags;
	return self;
}

d_define_method(drawable, draw)(struct s_object *self, struct s_object *environment) {
	d_war(e_log_level_ever, "'draw' method has not been implemented yet");
	return self;
}

d_define_method(drawable, draw_contour)(struct s_object *self, struct s_object *environment) {
	d_using(drawable);
	struct s_square_attributes *square_attributes = d_cast(&(drawable_attributes->square_collision_box), square);
	struct s_environment_attributes *environment_attributes = d_cast(environment, environment);
	d_call(&(drawable_attributes->square_collision_box), m_square_normalize, NULL);
	SDL_SetRenderDrawColor(environment_attributes->renderer, d_drawable_default_contour_color);
	SDL_RenderDrawLine(environment_attributes->renderer, square_attributes->normalized_top_left_x, square_attributes->normalized_top_left_y,
			square_attributes->normalized_top_right_x, square_attributes->normalized_top_right_y);
	SDL_RenderDrawLine(environment_attributes->renderer, square_attributes->normalized_top_right_x, square_attributes->normalized_top_right_y,
			square_attributes->normalize_bottom_right_x, square_attributes->normalize_bottom_right_y);
	SDL_RenderDrawLine(environment_attributes->renderer, square_attributes->normalize_bottom_right_x, square_attributes->normalize_bottom_right_y,
			square_attributes->normalized_bottom_left_x, square_attributes->normalized_bottom_left_y);
	SDL_RenderDrawLine(environment_attributes->renderer, square_attributes->normalized_bottom_left_x, square_attributes->normalized_bottom_left_y,
			square_attributes->normalized_top_left_x, square_attributes->normalized_top_left_y);
	return self;
}

d_define_method(drawable, set_maskRGB)(struct s_object *self, unsigned int red, unsigned int green, unsigned int blue) {
	d_war(e_log_level_ever, "'set_maskRGB' method has not been implemented yet");
	return self;
}

d_define_method(drawable, set_maskA)(struct s_object *self, unsigned int alpha) {
	d_war(e_log_level_ever, "'set_maskA' method has not been implemented yet");
	return self;
}

d_define_method(drawable, set_blend)(struct s_object *self, enum e_drawable_blends blend) {
	d_war(e_log_level_ever, "'set_blend' method has not been implemented yet");
	return self;
}

d_define_method(drawable, normalize_scale)(struct s_object *self, double reference_w, double reference_h, double offset_x, double offset_y,
		double focus_x, double focus_y, double current_w, double current_h, double zoom) {
	d_using(drawable);
	double this_x, this_y, this_w, this_h, this_center_x, this_center_y, new_x, new_y, new_w, new_h, new_center_x, new_center_y, this_zoom;
	struct s_object *result = self;
	d_call(&(drawable_attributes->point_destination), m_point_get, &this_x, &this_y);
	d_call(&(drawable_attributes->point_dimension), m_point_get, &this_w, &this_h);
	d_call(&(drawable_attributes->point_center), m_point_get, &this_center_x, &this_center_y);
	new_x = (this_x + this_center_x) - (this_center_x * drawable_attributes->zoom);
	new_y = (this_y + this_center_y) - (this_center_y * drawable_attributes->zoom);
	new_x = (new_x + focus_x) - (focus_x * zoom);
	new_y = (new_y + focus_y) - (focus_y * zoom);
	this_zoom = drawable_attributes->zoom * zoom;
	new_w = this_w * this_zoom;
	new_h = this_h * this_zoom;
	new_center_x = this_center_x * this_zoom;
	new_center_y = this_center_y * this_zoom;
	new_x = (new_x * current_w)/reference_w - offset_x;
	new_y = (new_y * current_h)/reference_h - offset_y;
	new_w = (new_w * current_w)/reference_w;
	new_h = (new_h * current_h)/reference_h;
	new_center_x = (new_center_x * current_w)/reference_w;
	new_center_y = (new_center_y * current_h)/reference_h;
	d_call(&(drawable_attributes->point_normalized_destination), m_point_set_x, new_x);
	d_call(&(drawable_attributes->point_normalized_destination), m_point_set_y, new_y);
	d_call(&(drawable_attributes->point_normalized_dimension), m_point_set_x, new_w);
	d_call(&(drawable_attributes->point_normalized_dimension), m_point_set_y, new_h);
	d_call(&(drawable_attributes->point_normalized_center), m_point_set_x, new_center_x);
	d_call(&(drawable_attributes->point_normalized_center), m_point_set_y, new_center_y);
	d_call(&(drawable_attributes->square_collision_box), m_square_set_top_left_x, new_x, d_false);
	d_call(&(drawable_attributes->square_collision_box), m_square_set_top_left_y, new_y, d_false);
	d_call(&(drawable_attributes->square_collision_box), m_square_set_bottom_right_x, (new_x+new_w));
	d_call(&(drawable_attributes->square_collision_box), m_square_set_bottom_right_y, (new_y+new_h));
	d_call(&(drawable_attributes->square_collision_box), m_square_set_angle, drawable_attributes->angle);
	d_call(&(drawable_attributes->square_collision_box), m_square_set_center, new_center_x, new_center_y);
	/* is the object still visible ? */
	if ((drawable_attributes->flags&e_drawable_kind_force_visibility) != e_drawable_kind_force_visibility)
		if ((new_x > current_w) || (new_y > current_h) || (new_x < -new_w) || (new_y < -new_h))
			result = NULL;
	return result;
}

d_define_method(drawable, keep_scale)(struct s_object *self, double current_w, double current_h) {
	d_using(drawable);
	double this_x, this_y, this_w, this_h, this_center_x, this_center_y;
	struct s_object *result = self;
	d_call(&(drawable_attributes->point_destination), m_point_get, &this_x, &this_y);
	d_call(&(drawable_attributes->point_dimension), m_point_get, &this_w, &this_h);
	d_call(&(drawable_attributes->point_center), m_point_get, &this_center_x, &this_center_y);
	d_call(&(drawable_attributes->point_normalized_destination), m_point_set_point, &(drawable_attributes->point_destination));
	d_call(&(drawable_attributes->point_normalized_dimension), m_point_set_point, &(drawable_attributes->point_dimension));
	d_call(&(drawable_attributes->point_normalized_center), m_point_set_point, &(drawable_attributes->point_center));
	d_call(&(drawable_attributes->square_collision_box), m_square_set_top_left_x, this_x, d_false);
	d_call(&(drawable_attributes->square_collision_box), m_square_set_top_left_y, this_y, d_false);
	d_call(&(drawable_attributes->square_collision_box), m_square_set_bottom_right_x, (this_x+this_w));
	d_call(&(drawable_attributes->square_collision_box), m_square_set_bottom_right_y, (this_x+this_w));
	d_call(&(drawable_attributes->square_collision_box), m_square_set_angle, drawable_attributes->angle);
	d_call(&(drawable_attributes->square_collision_box), m_square_set_center, this_center_x, this_center_y);
	/* is the object still visible ? */
	if ((drawable_attributes->flags&e_drawable_kind_force_visibility) != e_drawable_kind_force_visibility)
		if ((this_x > current_w) || (this_y > current_h) || (this_x < -this_w) || (this_y < -this_h))
			result = NULL;
	return result;
}

d_define_method(drawable, set_position)(struct s_object *self, double x, double y) {
	d_using(drawable);
	d_call(&(drawable_attributes->point_destination), m_point_set_x, x);
	d_call(&(drawable_attributes->point_destination), m_point_set_y, y);
	return self;
}

d_define_method(drawable, get_position)(struct s_object *self, double *x, double *y) {
	d_using(drawable);
	d_call(&(drawable_attributes->point_destination), m_point_get, x, y);
	return self;
}

d_define_method(drawable, get_scaled_position)(struct s_object *self, double *x, double *y) {
	d_using(drawable);
	d_call(&(drawable_attributes->point_normalized_destination), m_point_get, x, y);
	return self;
}

d_define_method(drawable, get_scaled_center)(struct s_object *self, double *x, double *y) {
	d_using(drawable);
	d_call(&(drawable_attributes->point_normalized_center), m_point_get, x, y);
	return self;
}

d_define_method(drawable, set_dimension)(struct s_object *self, double w, double h) {
	d_using(drawable);
	d_call(&(drawable_attributes->point_dimension), m_point_set_x, w);
	d_call(&(drawable_attributes->point_dimension), m_point_set_y, h);
	return self;
}

d_define_method(drawable, set_dimension_w)(struct s_object *self, double w) {
	d_using(drawable);
	d_call(&(drawable_attributes->point_dimension), m_point_set_x, w);
	return self;
}

d_define_method(drawable, set_dimension_h)(struct s_object *self, double h) {
	d_using(drawable);
	d_call(&(drawable_attributes->point_dimension), m_point_set_y, h);
	return self;
}

d_define_method(drawable, get_dimension)(struct s_object *self, double *w, double *h) {
	d_using(drawable);
	d_call(&(drawable_attributes->point_dimension), m_point_get, w, h);
	return self;
}

d_define_method(drawable, get_scaled_dimension)(struct s_object *self, double *w, double *h) {
	d_using(drawable);
	d_call(&(drawable_attributes->point_normalized_dimension), m_point_get, w, h);
	return self;
}

d_define_method(drawable, set_center)(struct s_object *self, double x, double y) {
	d_using(drawable);
	d_call(&(drawable_attributes->point_center), m_point_set_x, x);
	d_call(&(drawable_attributes->point_center), m_point_set_y, y);
	return self;
}

d_define_method(drawable, set_angle)(struct s_object *self, double angle) {
	d_using(drawable);
	drawable_attributes->angle = fmod(angle, 360.0);
	return self;
}

d_define_method(drawable, set_zoom)(struct s_object *self, double zoom) {
	d_using(drawable);
	drawable_attributes->zoom = zoom;
	return self;
}

d_define_method(drawable, flip)(struct s_object *self, enum e_drawable_flips flip) {
	d_using(drawable);
	drawable_attributes->flip = flip;
	return self;
}

d_define_method(drawable, set_flags)(struct s_object *self, int flags) {
	d_using(drawable);
	drawable_attributes->flags = flags;
	return self;
}

d_define_method(drawable, get_flags)(struct s_object *self) {
	d_using(drawable);
	d_cast_return(drawable_attributes->flags);
}

d_define_method(drawable, delete)(struct s_object *self, struct s_drawable_attributes *attributes) {
	d_delete(&(attributes->point_destination));
	d_delete(&(attributes->point_normalized_destination));
	d_delete(&(attributes->point_dimension));
	d_delete(&(attributes->point_normalized_dimension));
	d_delete(&(attributes->point_center));
	d_delete(&(attributes->point_normalized_center));
	d_delete(&(attributes->square_collision_box));
	return NULL;
}

d_define_class(drawable) {
	d_hook_method(drawable, e_flag_public, draw),
	d_hook_method(drawable, e_flag_public, draw_contour),
	d_hook_method(drawable, e_flag_public, set_maskRGB),
	d_hook_method(drawable, e_flag_public, set_maskA),
	d_hook_method(drawable, e_flag_public, set_blend),
	d_hook_method(drawable, e_flag_public, normalize_scale),
	d_hook_method(drawable, e_flag_public, keep_scale),
	d_hook_method(drawable, e_flag_public, set_position),
	d_hook_method(drawable, e_flag_public, get_position),
	d_hook_method(drawable, e_flag_public, get_scaled_position),
	d_hook_method(drawable, e_flag_public, get_scaled_center),
	d_hook_method(drawable, e_flag_public, set_dimension),
	d_hook_method(drawable, e_flag_public, set_dimension_w),
	d_hook_method(drawable, e_flag_public, set_dimension_h),
	d_hook_method(drawable, e_flag_public, get_dimension),
	d_hook_method(drawable, e_flag_public, get_scaled_dimension),
	d_hook_method(drawable, e_flag_public, set_center),
	d_hook_method(drawable, e_flag_public, set_angle),
	d_hook_method(drawable, e_flag_public, set_zoom),
	d_hook_method(drawable, e_flag_public, flip),
	d_hook_method(drawable, e_flag_public, set_flags),
	d_hook_method(drawable, e_flag_public, get_flags),
	d_hook_delete(drawable),
	d_hook_method_tail
};
