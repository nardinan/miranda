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
#include "uiable.obj.h"
struct s_uiable_attributes *p_uiable_alloc(struct s_object *self, int flags) {
	struct s_uiable_attributes *result = d_prepare(self, uiable);
	f_memory_new(self);		/* inherit */
	f_mutex_new(self);		/* inherit */
	f_drawable_new(self, flags);	/* inherit */
	return result;
}

struct s_object *f_uiable_new(struct s_object *self, int flags) {
	struct s_uiable_attributes *attributes = p_uiable_alloc(self, flags);
	attributes->background = f_background_new(d_new(uiable));
	attributes->offset_x = 0.0;
	attributes->offset_y = 0.0;
	attributes->offset_w = 0.0;
	attributes->offset_y = 0.0;
	return self;
}

d_define_method(uiable, set_background)(struct s_object *self, struct s_object *drawable, enum e_background_components component) {
	d_using(uiable);
	return d_call(uiable_attributes->background, m_background_set, drawable, component);
}

d_define_method(uiable, set_offset)(struct s_object *self, double offset_x, double offset_y, double offset_w, double offset_h) {
	d_using(uiable);
	uiable_attributes->offset_x = offset_x;
	uiable_attributes->offset_y = offset_y;
	uiable_attributes->offset_w = offset_w;
	uiable_attributes->offset_h = offset_h;
	return self;
}

d_define_method(uiable, draw_background)(struct s_object *self, struct s_object *environment) {
	d_using(uiable);
	struct s_drawable_attributes *drawable_attributes = d_cast(self, drawable);
	struct s_environment_attributes *environment_attributes = d_cast(environment, environment);
	double local_x, local_y, local_w, local_h;
	d_call(&(drawable_attributes->point_destination), m_point_get, &local_x, &local_y);
	d_call(&(drawable_attributes->point_dimension), m_point_get, &local_w, &local_h);
	local_x += uiable_attributes->offset_x;
	local_y += uiable_attributes->offset_y;
	local_w -= uiable_attributes->offset_x + uiable_attributes->offset_w;
	local_h -= uiable_attributes->offset_y + uiable_attributes->offset_h;
	d_call(uiable_attributes->background, m_drawable_set_position, local_x, local_y);
	d_call(uiable_attributes->background, m_drawable_set_dimension, local_w, local_h);
	if ((d_call(uiable_attributes->background, m_drawable_normalize_scale, environment_attributes->reference_w,
						environment_attributes->reference_h,
						environment_attributes->camera_origin_x,
						environment_attributes->camera_origin_y,
						environment_attributes->camera_focus_x,
						environment_attributes->camera_focus_y,
						environment_attributes->current_w,
						environment_attributes->current_h,
						environment_attributes->zoom)))
		while (((int)d_call(uiable_attributes->background, m_drawable_draw, environment)) == d_drawable_return_continue);
	d_cast_return(d_drawable_return_last);
}

d_define_method(uiable, delete)(struct s_object *self, struct s_uiable_attributes *attributes) {
	if (attributes->background)
		d_delete(attributes->background);
	return NULL;
}

d_define_class(uiable) {
	d_hook_method(uiable, e_flag_public, set_background),
	d_hook_method(uiable, e_flag_public, set_offset),
	d_hook_method(uiable, e_flag_public, draw_background),
	d_hook_delete(uiable),
	d_hook_method_tail
};
