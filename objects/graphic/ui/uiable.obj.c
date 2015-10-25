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
const char *v_uiable_components[e_uiable_component_NULL] = {
	"T",
	"B",
	"L",
	"R",
	"C",
	"CTL",
	"CTR",
	"CBL",
	"CBR",
	NULL
};
struct s_uiable_attributes *p_uiable_alloc(struct s_object *self) {
	struct s_uiable_attributes *result = d_prepare(self, uiable);
	f_memory_new(self);									/* inherit */
	f_mutex_new(self);									/* inherit */
	f_drawable_new(self, (e_drawable_kind_single|e_drawable_kind_force_visibility));	/* inherit */
	return result;
}

struct s_object *f_uiable_new(struct s_object *self) {
	struct s_uiable_attributes *attributes = p_uiable_alloc(self);
	attributes->last_blend = e_drawable_blend_undefined;
	attributes->last_mask_R = 255.0;
	attributes->last_mask_G = 255.0;
	attributes->last_mask_B = 255.0;
	attributes->last_mask_A = 255.0;
	return self;
}

d_define_method(uiable, set)(struct s_object *self, struct s_object *drawable, enum e_uiable_components component) {
	d_using(uiable);
	if (uiable_attributes->background[component])
		d_delete(uiable_attributes->background[component]);
	uiable_attributes->background[component] = d_retain(drawable);
	if (uiable_attributes->last_blend != e_drawable_blend_undefined)
		d_call(uiable_attributes->background[component], m_drawable_set_blend, uiable_attributes->last_blend);
	d_call(uiable_attributes->background[component], m_drawable_set_maskRGB, (unsigned int)uiable_attributes->last_mask_R,
				(unsigned int)uiable_attributes->last_mask_G, (unsigned int)uiable_attributes->last_mask_B);
	d_call(uiable_attributes->background[component], m_drawable_set_maskA, (unsigned int)uiable_attributes->last_mask_A);
	return self;
}

d_define_method(uiable, draw)(struct s_object *self, struct s_object *environment) {
	d_using(uiable);
	struct s_environment_attributes *environment_attributes = d_cast(environment, environment);
	struct s_drawable_attributes *drawable_attributes_self = d_cast(self, drawable),
				     *drawable_attributes_core;
	double local_x, local_y, local_w, local_h, center_x, center_y, component_w[e_uiable_component_NULL], component_h[e_uiable_component_NULL];
	int index;
	d_call(&(drawable_attributes_self->point_destination), m_point_get, &local_x, &local_y);
	d_call(&(drawable_attributes_self->point_dimension), m_point_get, &local_w, &local_h);
	d_call(&(drawable_attributes_self->point_center), m_point_get, &center_x, &center_y);
	for (index = 0; index < e_uiable_component_NULL; ++index)
		if (uiable_attributes->background[index]) {
			drawable_attributes_core = d_cast(uiable_attributes->background[index], drawable);
			d_call(&(drawable_attributes_core->point_dimension), m_point_get, &(component_w[index]), &(component_h[index]));
		}
	if (uiable_attributes->background[e_uiable_component_center]) {
		d_call(uiable_attributes->background[e_uiable_component_center], m_drawable_set_position,
				(local_x-1),
				(local_y-1));
		d_call(uiable_attributes->background[e_uiable_component_center], m_drawable_set_dimension,
				(local_w+2),
				(local_h+2));
		d_call(uiable_attributes->background[e_uiable_component_center], m_drawable_set_center,
				(center_x+1),
				(center_y+1));
	}
	if (uiable_attributes->background[e_uiable_component_corner_top_left]) {
		d_call(uiable_attributes->background[e_uiable_component_corner_top_left], m_drawable_set_position,
		      		(local_x-component_w[e_uiable_component_corner_top_left]),
				(local_y-component_h[e_uiable_component_corner_top_left]));
		d_call(uiable_attributes->background[e_uiable_component_corner_top_left], m_drawable_set_center,
				(center_x+component_w[e_uiable_component_corner_top_left]),
				(center_y+component_h[e_uiable_component_corner_top_left]));
	}
	if (uiable_attributes->background[e_uiable_component_corner_top_right]) {
		d_call(uiable_attributes->background[e_uiable_component_corner_top_right], m_drawable_set_position,
				(local_x+local_w),
				(local_y-component_h[e_uiable_component_corner_top_right]));
		d_call(uiable_attributes->background[e_uiable_component_corner_top_right], m_drawable_set_center,
				(center_x-local_w)-1,
				(center_y+component_h[e_uiable_component_corner_top_right]));
	}
	if (uiable_attributes->background[e_uiable_component_corner_bottom_left]) {
		d_call(uiable_attributes->background[e_uiable_component_corner_bottom_left], m_drawable_set_position,
				(local_x-component_w[e_uiable_component_corner_bottom_left]),
				(local_y+local_h));
		d_call(uiable_attributes->background[e_uiable_component_corner_bottom_left], m_drawable_set_center,
				(center_x+component_w[e_uiable_component_corner_bottom_left]),
				(center_y-local_h));
	}
	if (uiable_attributes->background[e_uiable_component_corner_bottom_right]) {
		d_call(uiable_attributes->background[e_uiable_component_corner_bottom_right], m_drawable_set_position,
				(local_x+local_w),
				(local_y+local_h));
		d_call(uiable_attributes->background[e_uiable_component_corner_bottom_right], m_drawable_set_center,
				(center_x-local_w)-1,
				(center_y-local_h)-1);
	}
	if (uiable_attributes->background[e_uiable_component_top]) {
		d_call(uiable_attributes->background[e_uiable_component_top], m_drawable_set_position,
				(local_x-1),
				(local_y-component_h[e_uiable_component_top]));
		d_call(uiable_attributes->background[e_uiable_component_top], m_drawable_set_dimension_w,
				(local_w+2));
		d_call(uiable_attributes->background[e_uiable_component_top], m_drawable_set_center,
				(center_x+1),
				(center_y+component_h[e_uiable_component_top]));
	}
	if (uiable_attributes->background[e_uiable_component_bottom]) {
		d_call(uiable_attributes->background[e_uiable_component_bottom], m_drawable_set_position,
				(local_x-1),
				(local_y+local_h));
		d_call(uiable_attributes->background[e_uiable_component_bottom], m_drawable_set_dimension_w,
				(local_w+2));
		d_call(uiable_attributes->background[e_uiable_component_bottom], m_drawable_set_center,
				(center_x+1),
				(center_y-local_h));
	}
	if (uiable_attributes->background[e_uiable_component_left]) {
		d_call(uiable_attributes->background[e_uiable_component_left], m_drawable_set_position,
				(local_x-component_w[e_uiable_component_left]),
				(local_y-1));
		d_call(uiable_attributes->background[e_uiable_component_left], m_drawable_set_dimension_h,
				(local_h+2));
		d_call(uiable_attributes->background[e_uiable_component_left], m_drawable_set_center,
				(center_x+component_w[e_uiable_component_left]),
				(center_y+1));
	}
	if (uiable_attributes->background[e_uiable_component_right]) {
		d_call(uiable_attributes->background[e_uiable_component_right], m_drawable_set_position,
				(local_x+local_w),
				(local_y-1));
		d_call(uiable_attributes->background[e_uiable_component_right], m_drawable_set_dimension_h,
				(local_h+2));
		d_call(uiable_attributes->background[e_uiable_component_right], m_drawable_set_center,
				(center_x-local_w)-1,
				(center_y+1));
	}
	for (index = 0; index < e_uiable_component_NULL; ++index)
		if (uiable_attributes->background[index]) {
			drawable_attributes_core = d_cast(uiable_attributes->background[index], drawable);
			drawable_attributes_core->angle = drawable_attributes_self->angle;
			drawable_attributes_core->zoom = drawable_attributes_self->zoom;
			/* do not inerith the flip (this object, the uiable, doesn't flip) */
			if ((d_call(uiable_attributes->background[index], m_drawable_normalize_scale, environment_attributes->reference_w,
						environment_attributes->reference_h,
						environment_attributes->camera_origin_x,
						environment_attributes->camera_origin_y,
						environment_attributes->camera_focus_x,
						environment_attributes->camera_focus_y,
						environment_attributes->current_w,
						environment_attributes->current_h,
						environment_attributes->zoom)))
			while (((int)d_call(uiable_attributes->background[index], m_drawable_draw, environment)) == d_drawable_return_continue);
		}
	d_cast_return(d_drawable_return_last);
}

d_define_method_override(uiable, set_maskRGB)(struct s_object *self, unsigned int red, unsigned int green, unsigned int blue) {
	d_using(uiable);
	int index;
	uiable_attributes->last_mask_R = red;
	uiable_attributes->last_mask_G = green;
	uiable_attributes->last_mask_B = blue;
	for (index = 0; index < e_uiable_component_NULL; ++index)
		if (uiable_attributes->background[index])
			d_call(uiable_attributes->background[index], m_drawable_set_maskRGB, (unsigned int)red, (unsigned int)green, (unsigned int)blue);
	return self;
}

d_define_method_override(uiable, set_maskA)(struct s_object *self, unsigned int alpha) {
	d_using(uiable);
	int index;
	uiable_attributes->last_mask_A = alpha;
	for (index = 0; index < e_uiable_component_NULL; ++index)
		if (uiable_attributes->background[index])
			d_call(uiable_attributes->background[index], m_drawable_set_maskA, (unsigned int)alpha);
	return self;
}

d_define_method_override(uiable, set_blend)(struct s_object *self, enum e_drawable_blends blend) {
	d_using(uiable);
	int index;
	uiable_attributes->last_blend = blend;
	for (index = 0; index < e_uiable_component_NULL; ++index)
		if (uiable_attributes->background[index])
			d_call(uiable_attributes->background[index], m_drawable_set_blend, blend);
	return self;
}

d_define_method(uiable, delete)(struct s_object *self, struct s_uiable_attributes *attributes) {
	int index;
	for (index = 0; index != e_uiable_component_NULL; ++index)
		if (attributes->background[index])
			d_delete(attributes->background[index]);
	return NULL;
}

d_define_class(uiable) {
	d_hook_method(uiable, e_flag_public, set),
	d_hook_method_override(uiable, e_flag_public, drawable, draw),
	d_hook_method_override(uiable, e_flag_public, drawable, set_maskRGB),
	d_hook_method_override(uiable, e_flag_public, drawable, set_maskA),
	d_hook_method_override(uiable, e_flag_public, drawable, set_blend),
	d_hook_delete(uiable),
	d_hook_method_tail
};
