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
#include "background.obj.h"
struct s_background_attributes *p_background_alloc(struct s_object *self) {
	struct s_background_attributes *result = d_prepare(self, background);
	f_memory_new(self);									/* inherit */
	f_mutex_new(self);									/* inherit */
	f_drawable_new(self, (e_drawable_kind_single|e_drawable_kind_force_visibility));	/* inherit */
	return result;
}

struct s_object *f_background_new(struct s_object *self) {
	struct s_background_attributes *attributes = p_background_alloc(self);
	attributes->last_blend = e_drawable_blend_undefined;
	attributes->last_mask_R = 255.0;
	attributes->last_mask_G = 255.0;
	attributes->last_mask_B = 255.0;
	attributes->last_mask_A = 255.0;
	return self;
}

d_define_method(background, set)(struct s_object *self, struct s_object *drawable, enum e_background_components component) {
	d_using(background);
	if (background_attributes->background[component])
		d_delete(background_attributes->background[component]);
	background_attributes->background[component] = d_retain(drawable);
	if (background_attributes->last_blend != e_drawable_blend_undefined)
		d_call(background_attributes->background[component], m_drawable_set_blend, background_attributes->last_blend);
	d_call(background_attributes->background[component], m_drawable_set_maskRGB, (unsigned int)background_attributes->last_mask_R,
				(unsigned int)background_attributes->last_mask_G, (unsigned int)background_attributes->last_mask_B);
	d_call(background_attributes->background[component], m_drawable_set_maskA, (unsigned int)background_attributes->last_mask_A);
	return self;
}

d_define_method(background, draw)(struct s_object *self, struct s_object *environment) {
	d_using(background);
	struct s_environment_attributes *environment_attributes = d_cast(environment, environment);
	struct s_drawable_attributes *drawable_attributes_self = d_cast(self, drawable),
				     *drawable_attributes_core;
	double local_x, local_y, local_w, local_h, component_w[e_background_component_NULL], component_h[e_background_component_NULL];
	int index;
	d_call(&(drawable_attributes_self->point_destination), m_point_get, &local_x, &local_y);
	d_call(&(drawable_attributes_self->point_dimension), m_point_get, &local_w, &local_h);
	for (index = 0; index < e_background_component_NULL; ++index)
		if (background_attributes->background[index]) {
			drawable_attributes_core = d_cast(background_attributes->background[index], drawable);
			d_call(&(drawable_attributes_core->point_dimension), m_point_get, &(component_w[index]), &(component_h[index]));
		}
	if (background_attributes->background[e_background_component_center]) {
		d_call(background_attributes->background[e_background_component_center], m_drawable_set_position,
				(local_x-1),
				(local_y-1));
		d_call(background_attributes->background[e_background_component_center], m_drawable_set_dimension,
				(local_w+2),
				(local_h+2));
	}
	if (background_attributes->background[e_background_component_corner_top_left])
		d_call(background_attributes->background[e_background_component_corner_top_left], m_drawable_set_position,
		      		(local_x-component_w[e_background_component_corner_top_left]),
				(local_y-component_h[e_background_component_corner_top_left]));
	if (background_attributes->background[e_background_component_corner_top_right])
		d_call(background_attributes->background[e_background_component_corner_top_right], m_drawable_set_position,
				(local_x+local_w),
				(local_y-component_h[e_background_component_corner_top_right]));
	if (background_attributes->background[e_background_component_corner_bottom_left])
		d_call(background_attributes->background[e_background_component_corner_bottom_left], m_drawable_set_position,
				(local_x-component_w[e_background_component_corner_bottom_left]),
				(local_y+local_h));
	if (background_attributes->background[e_background_component_corner_bottom_right])
		d_call(background_attributes->background[e_background_component_corner_bottom_right], m_drawable_set_position,
				(local_x+local_w),
				(local_y+local_h));
	if (background_attributes->background[e_background_component_top]) {
		d_call(background_attributes->background[e_background_component_top], m_drawable_set_position,
				(local_x-1),
				(local_y-component_h[e_background_component_top]));
		d_call(background_attributes->background[e_background_component_top], m_drawable_set_dimension_w,
				(local_w+2));
	}
	if (background_attributes->background[e_background_component_bottom]) {
		d_call(background_attributes->background[e_background_component_bottom], m_drawable_set_position,
				(local_x-1),
				(local_y+local_h));
		d_call(background_attributes->background[e_background_component_bottom], m_drawable_set_dimension_w,
				(local_w+2));
	}
	if (background_attributes->background[e_background_component_left]) {
		d_call(background_attributes->background[e_background_component_left], m_drawable_set_position,
				(local_x-component_w[e_background_component_left]),
				(local_y-1));
		d_call(background_attributes->background[e_background_component_left], m_drawable_set_dimension_h,
				(local_h+2));
	}
	if (background_attributes->background[e_background_component_right]) {
		d_call(background_attributes->background[e_background_component_right], m_drawable_set_position,
				(local_x+local_w),
				(local_y-1));
		d_call(background_attributes->background[e_background_component_right], m_drawable_set_dimension_h,
				(local_h+2));
	}
	for (index = 0; index < e_background_component_NULL; ++index)
		if (background_attributes->background[index])
			if ((d_call(background_attributes->background[index], m_drawable_normalize_scale, environment_attributes->reference_w,
						environment_attributes->reference_h,
						environment_attributes->camera_origin_x,
						environment_attributes->camera_origin_y,
						environment_attributes->camera_focus_x,
						environment_attributes->camera_focus_y,
						environment_attributes->current_w,
						environment_attributes->current_h,
						environment_attributes->zoom)))
			while (((int)d_call(background_attributes->background[index], m_drawable_draw, environment)) == d_drawable_return_continue);
	d_cast_return(d_drawable_return_last);
}

d_define_method_override(background, set_maskRGB)(struct s_object *self, unsigned int red, unsigned int green, unsigned int blue) {
	d_using(background);
	int index;
	background_attributes->last_mask_R = red;
	background_attributes->last_mask_G = green;
	background_attributes->last_mask_B = blue;
	for (index = 0; index < e_background_component_NULL; ++index)
		if (background_attributes->background[index])
			d_call(background_attributes->background[index], m_drawable_set_maskRGB, (unsigned int)red, (unsigned int)green, (unsigned int)blue);
	return self;
}

d_define_method_override(background, set_maskA)(struct s_object *self, unsigned int alpha) {
	d_using(background);
	int index;
	background_attributes->last_mask_A = alpha;
	for (index = 0; index < e_background_component_NULL; ++index)
		if (background_attributes->background[index])
			d_call(background_attributes->background[index], m_drawable_set_maskA, (unsigned int)alpha);
	return self;
}

d_define_method_override(background, set_blend)(struct s_object *self, enum e_drawable_blends blend) {
	d_using(background);
	int index;
	background_attributes->last_blend = blend;
	for (index = 0; index < e_background_component_NULL; ++index)
		if (background_attributes->background[index])
			d_call(background_attributes->background[index], m_drawable_set_blend, blend);
	return self;
}

d_define_method(background, delete)(struct s_object *self, struct s_background_attributes *attributes) {
	int index;
	for (index = 0; index != e_background_component_NULL; ++index)
		if (attributes->background[index])
			d_delete(attributes->background[index]);
	return NULL;
}

d_define_class(background) {
	d_hook_method(background, e_flag_public, set),
	d_hook_method_override(background, e_flag_public, drawable, draw),
	d_hook_method_override(background, e_flag_public, drawable, set_maskRGB),
	d_hook_method_override(background, e_flag_public, drawable, set_maskA),
	d_hook_method_override(background, e_flag_public, drawable, set_blend),
	d_hook_delete(background),
	d_hook_method_tail
};
