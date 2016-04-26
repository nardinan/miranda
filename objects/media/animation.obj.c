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
#include "animation.obj.h"
struct s_animation_attributes *p_animation_alloc(struct s_object *self) {
	struct s_animation_attributes *result = d_prepare(self, animation);
	f_mutex_new(self);									/* inherit */
	f_memory_new(self);									/* inherit */
	f_drawable_new(self, (e_drawable_kind_single|e_drawable_kind_force_visibility));	/* inherit */
	return result;
}

struct s_object *f_animation_new(struct s_object *self, int cycles, double time_ratio) {
	struct s_animation_attributes *attributes = p_animation_alloc(self);
	memset(&(attributes->frames), 0, sizeof(struct s_list));
	attributes->cycles = cycles;
	attributes->time_ratio = time_ratio;
	attributes->status = e_animation_direction_stop;
	attributes->last_blend = e_drawable_blend_undefined;
	attributes->last_mask_R = 255.0;
	attributes->last_mask_G = 255.0;
	attributes->last_mask_B = 255.0;
	attributes->last_mask_A = 255.0;
	return self;
}

d_define_method(animation, append_frame)(struct s_object *self, struct s_object *drawable, double offset_x, double offset_y, double zoom, double time) {
	d_using(animation);
	struct s_animation_frame *current_frame;
	if ((current_frame = (struct s_animation_frame *) d_malloc(sizeof(struct s_animation_frame)))) {
		current_frame->drawable = d_retain(drawable);
		if (animation_attributes->last_blend != e_drawable_blend_undefined)
			d_call(current_frame->drawable, m_drawable_set_blend, animation_attributes->last_blend);
		d_call(current_frame->drawable, m_drawable_set_maskRGB, (unsigned int)animation_attributes->last_mask_R,
				(unsigned int)animation_attributes->last_mask_G, (unsigned int)animation_attributes->last_mask_B);
		d_call(current_frame->drawable, m_drawable_set_maskA, (unsigned int)animation_attributes->last_mask_A);
		current_frame->offset_x = offset_x;
		current_frame->offset_y = offset_y;
		current_frame->zoom = zoom;
		current_frame->time = time;
		f_list_append(&(animation_attributes->frames), (struct s_list_node *)current_frame, e_list_insert_tail);
	} else
		d_die(d_error_malloc);
	return self;
}

d_define_method(animation, set_status)(struct s_object *self, enum e_animation_directions status) {
	d_using(animation);
	enum e_animation_directions previous_status = animation_attributes->status;
	switch ((animation_attributes->status = status)) {
		case e_animation_direction_forward:
			animation_attributes->remaining_cycles = (animation_attributes->cycles - 1);
			break;
		case e_animation_direction_rewind:
			animation_attributes->remaining_cycles = animation_attributes->cycles;
			if ((previous_status != e_animation_direction_stop) ||
					(animation_attributes->current_frame != (struct s_animation_frame *)animation_attributes->frames.head))
				--animation_attributes->remaining_cycles;
		default:
			break;
	}
	return self;
}

d_define_method(animation, get_status)(struct s_object *self) {
	d_using(animation);
	d_cast_return(animation_attributes->status);
}

d_define_method(animation, set_callback)(struct s_object *self, t_animation_reboot callback, struct s_object *raw_data) {
	d_using(animation);
	animation_attributes->callback = callback;
	animation_attributes->raw_data = raw_data;
	return self;
}

d_define_method_override(animation, draw)(struct s_object *self, struct s_object *environment) {
	d_using(animation);
	struct s_animation_frame *next_frame, *first_frame;
	struct s_drawable_attributes *drawable_attributes_self = d_cast(self, drawable),
				     *drawable_attributes_core;
	struct s_environment_attributes *environment_attributes = d_cast(environment, environment);
	struct timeval current, elapsed_update;
	double real_elapsed_update, local_position_x, local_position_y, position_x, position_y;
	gettimeofday(&current, NULL);
	d_call(&(drawable_attributes_self->point_destination), m_point_get, (double *)&local_position_x, (double *)&local_position_y);
	if (animation_attributes->current_frame) {
		if ((animation_attributes->status == e_animation_direction_forward) || (animation_attributes->status == e_animation_direction_rewind)) {
			timersub(&current, &(animation_attributes->last_update), &elapsed_update);
			real_elapsed_update = elapsed_update.tv_sec + ((double)(elapsed_update.tv_usec)/1000000.0);
			if (real_elapsed_update > (animation_attributes->current_frame->time * animation_attributes->time_ratio)) {
				switch (animation_attributes->status) {
					case e_animation_direction_forward:
						next_frame = (struct s_animation_frame *)((struct s_list_node *)animation_attributes->current_frame)->next;
						first_frame = (struct s_animation_frame *)animation_attributes->frames.head;
						break;
					case e_animation_direction_rewind:
						next_frame = (struct s_animation_frame *)((struct s_list_node *)animation_attributes->current_frame)->back;
						first_frame = (struct s_animation_frame *)animation_attributes->frames.tail;
					default:
						break;
				}
				if (!next_frame) {
					if (animation_attributes->remaining_cycles != 0) {
						if (animation_attributes->remaining_cycles > 0)
							--(animation_attributes->remaining_cycles);
						if (animation_attributes->current_frame != first_frame)
							/* we need at least two frame to call the animation callback
							 * (remember that this is only when the object is visible) */
							if (animation_attributes->callback)
								animation_attributes->callback(animation_attributes->raw_data);
						animation_attributes->current_frame = first_frame;
					} else
						animation_attributes->status = e_animation_direction_stop;
				} else
					animation_attributes->current_frame = next_frame;
				memcpy(&(animation_attributes->last_update), &current, sizeof(struct timeval));
			}
		}
	}
	if (!animation_attributes->current_frame) {
		switch (animation_attributes->status) {
			case e_animation_direction_forward:
			case e_animation_direction_stop:
				first_frame = (struct s_animation_frame *)animation_attributes->frames.head;
				break;
			case e_animation_direction_rewind:
				first_frame = (struct s_animation_frame *)animation_attributes->frames.tail;
			default:
				break;
		}
		animation_attributes->current_frame = first_frame;
		memcpy(&(animation_attributes->last_update), &current, sizeof(struct timeval));
	}
	if (animation_attributes->current_frame) {
		drawable_attributes_core = d_cast(animation_attributes->current_frame->drawable, drawable);
		position_x = local_position_x + animation_attributes->current_frame->offset_x;
		position_y = local_position_y + animation_attributes->current_frame->offset_y;
		d_call(&(drawable_attributes_core->point_destination), m_point_set_x, (double)position_x);
		d_call(&(drawable_attributes_core->point_destination), m_point_set_y, (double)position_y);
		drawable_attributes_core->zoom = (animation_attributes->current_frame->zoom * drawable_attributes_self->zoom);
		drawable_attributes_core->angle = drawable_attributes_self->angle;
		drawable_attributes_core->flip = drawable_attributes_self->flip;
		if ((d_call(animation_attributes->current_frame->drawable, m_drawable_normalize_scale, environment_attributes->reference_w[environment_attributes->current_surface],
						environment_attributes->reference_h[environment_attributes->current_surface],
						environment_attributes->camera_origin_x[environment_attributes->current_surface],
						environment_attributes->camera_origin_y[environment_attributes->current_surface],
						environment_attributes->camera_focus_x[environment_attributes->current_surface],
						environment_attributes->camera_focus_y[environment_attributes->current_surface],
						environment_attributes->current_w,
						environment_attributes->current_h,
						environment_attributes->zoom[environment_attributes->current_surface])))
			while (((int)d_call(animation_attributes->current_frame->drawable, m_drawable_draw, environment)) == d_drawable_return_continue);

	}
	d_cast_return(d_drawable_return_last);
}

d_define_method_override(animation, set_maskRGB)(struct s_object *self, unsigned int red, unsigned int green, unsigned int blue) {
	d_using(animation);
	struct s_animation_frame *current_frame;
	animation_attributes->last_mask_R = red;
	animation_attributes->last_mask_G = green;
	animation_attributes->last_mask_B = blue;
	d_foreach(&(animation_attributes->frames), current_frame, struct s_animation_frame)
		d_call(current_frame->drawable, m_drawable_set_maskRGB, (unsigned int)red, (unsigned int)green, (unsigned int)blue);
	return self;
}

d_define_method_override(animation, set_maskA)(struct s_object *self, unsigned int alpha) {
	d_using(animation);
	struct s_animation_frame *current_frame;
	animation_attributes->last_mask_A = alpha;
	d_foreach(&(animation_attributes->frames), current_frame, struct s_animation_frame)
		d_call(current_frame->drawable, m_drawable_set_maskA, (unsigned int)alpha);
	return self;
}

d_define_method_override(animation, set_blend)(struct s_object *self, enum e_drawable_blends blend) {
	d_using(animation);
	struct s_animation_frame *current_frame;
	animation_attributes->last_blend = blend;
	d_foreach(&(animation_attributes->frames), current_frame, struct s_animation_frame)
		d_call(current_frame->drawable, m_drawable_set_blend, blend);
	return self;
}

d_define_method_override(animation, get_scaled_position)(struct s_object *self, double *x, double *y) {
	d_using(animation);
	*x = NAN;
	*y = NAN;
	if (animation_attributes->current_frame)
		d_call(animation_attributes->current_frame->drawable, m_drawable_get_scaled_position, x, y);
	return self;
}

d_define_method_override(animation, get_scaled_center)(struct s_object *self, double *x, double *y) {
	d_using(animation);
	*x = NAN;
	*y = NAN;
	if (animation_attributes->current_frame)
		d_call(animation_attributes->current_frame->drawable, m_drawable_get_scaled_center, x, y);
	return self;
}

d_define_method_override(animation, get_dimension)(struct s_object *self, double *w, double *h) {
	d_using(animation);
	*w = NAN;
	*h = NAN;
	if (animation_attributes->current_frame)
		d_call(animation_attributes->current_frame->drawable, m_drawable_get_dimension, w, h);
	return self;
}

d_define_method_override(animation, get_scaled_dimension)(struct s_object *self, double *w, double *h) {
	d_using(animation);
	*w = NAN;
	*h = NAN;
	if (animation_attributes->current_frame)
		d_call(animation_attributes->current_frame->drawable, m_drawable_get_scaled_dimension, w, h);
	return self;
}

d_define_method(animation, delete)(struct s_object *self, struct s_animation_attributes *attributes) {
	struct s_animation_frame *current_frame;
	while ((current_frame = (struct s_animation_frame *)attributes->frames.head)) {
		d_delete(current_frame->drawable);
		f_list_delete(&(attributes->frames), (struct s_list_node *)current_frame);
		d_free(current_frame);
	}
	return NULL;
}

d_define_class(animation) {
	d_hook_method(animation, e_flag_public, append_frame),
	d_hook_method(animation, e_flag_public, set_status),
	d_hook_method(animation, e_flag_public, get_status),
	d_hook_method(animation, e_flag_public, set_callback),
	d_hook_method_override(animation, e_flag_public, drawable, draw),
	d_hook_method_override(animation, e_flag_public, drawable, set_maskRGB),
	d_hook_method_override(animation, e_flag_public, drawable, set_maskA),
	d_hook_method_override(animation, e_flag_public, drawable, set_blend),
	d_hook_method_override(animation, e_flag_public, drawable, get_scaled_position),
	d_hook_method_override(animation, e_flag_public, drawable, get_scaled_center),
	d_hook_method_override(animation, e_flag_public, drawable, get_dimension),
	d_hook_method_override(animation, e_flag_public, drawable, get_scaled_dimension),
	d_hook_delete(animation),
	d_hook_method_tail
};
