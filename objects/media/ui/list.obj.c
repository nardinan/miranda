/*
 * miranda
 * Copyright (C) 2016 Andrea Nardinocchi (andrea@nardinan.it)
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
#include "list.obj.h"
struct s_list_attributes *p_list_alloc(struct s_object *self) {
	struct s_list_attributes *result = d_prepare(self, list);
	f_mutex_new(self);	/* inherit */
	f_memory_new(self);	/* inherit */
	f_uiable_new(self);	/* inherit */
	return result;
}

struct s_object *f_list_new(struct s_object *self, struct s_object *scroll) {
	return f_list_new_selected(self, scroll, d_list_default_R, d_list_default_G, d_list_default_B, d_list_default_A);
}

struct s_object *f_list_new_selected(struct s_object *self, struct s_object *scroll, unsigned int red, unsigned int green, unsigned int blue,
		unsigned int alpha) {
	struct s_list_attributes *attributes = p_list_alloc(self);
	memset(&(attributes->uiables), 0, sizeof(struct s_list));
	attributes->scroll = d_retain(scroll);
	attributes->selected_mask_R = red;
	attributes->selected_mask_G = green;
	attributes->selected_mask_B = blue;
	attributes->selected_mask_A = alpha;
	attributes->last_blend = e_drawable_blend_undefined;
	attributes->last_mask_R = 255.0;
	attributes->last_mask_G = 255.0;
	attributes->last_mask_B = 255.0;
	attributes->last_mask_A = 255.0;
	return self;
}

d_define_method(list, add_uiable)(struct s_object *self, struct s_object *uiable) {
	d_using(list);
	d_call(uiable, m_drawable_set_maskRGB, (unsigned int)list_attributes->last_mask_R, (unsigned int)list_attributes->last_mask_G,
			(unsigned int)list_attributes->last_mask_B);
	d_call(uiable, m_drawable_set_maskA, (unsigned int)list_attributes->last_mask_A);
	d_call(uiable, m_drawable_set_blend, list_attributes->last_blend);
	d_call(uiable, m_drawable_set_flags, e_drawable_kind_contour);
	f_list_append(&(list_attributes->uiables), d_retain(uiable), e_list_insert_tail);
	++(list_attributes->uiable_entries);
	d_call(list_attributes->scroll, m_scroll_set_range, 0, d_math_max((list_attributes->uiable_entries-1), 0));
	return self;
}

d_define_method(list, del_uiable)(struct s_object *self, struct s_object *uiable) {
	d_using(list);
	f_list_delete(&(list_attributes->uiables), (struct s_list_node *)uiable);
	d_delete(uiable);
	return uiable;
}

d_define_method(list, set_uiable)(struct s_object *self, struct s_object *uiable) {
	d_using(list);
	struct s_object *current_entry = uiable;
	int index = 0;
	while (((struct s_list_node *)current_entry)->back) {
		current_entry = (struct s_object *)((struct s_list_node *)current_entry)->back;
		++index;
	}
	list_attributes->selected = index;
	return self;
}

d_define_method(list, get_uiable)(struct s_object *self, unsigned int *index) {
	d_using(list);
	struct s_object *current_entry, *result = NULL;
	int current_index = 0;
	if (list_attributes->selected != d_list_selected_NULL)
		d_foreach(&(list_attributes->uiables), current_entry, struct s_object) {
			if (list_attributes->selected == current_index) {
				*index = current_index;
				result = current_entry;
				break;
			}
			++current_index;
		}
	return result;
}

d_define_method_override(list, mode)(struct s_object *self, enum e_uiable_modes mode) {
	d_using(list);
	struct s_object *current_entry;
	struct s_object *result = d_call_owner(self, uiable, m_uiable_mode, mode);
	d_call(list_attributes->scroll, m_uiable_mode, mode);
	d_foreach(&(list_attributes->uiables), current_entry, struct s_object)
		d_call(current_entry, m_uiable_mode, mode);
	return result;
}

d_define_method_override(list, event)(struct s_object *self, struct s_object *environment, SDL_Event *current_event) {
	d_using(list);
	struct s_uiable_attributes *uiable_attributes_entry;
	struct s_object *current_entry;
	struct s_object *result = d_call(list_attributes->scroll, m_eventable_event, environment, current_event);
	int index = 0;
	d_foreach(&(list_attributes->uiables), current_entry, struct s_object)
		d_call(current_entry, m_eventable_event, environment, current_event);
	d_foreach(&(list_attributes->uiables), current_entry, struct s_object) {
		uiable_attributes_entry = d_cast(current_entry, uiable);
		if (uiable_attributes_entry->selected_mode == e_uiable_mode_selected) {
			if (list_attributes->selected != index) {
				list_attributes->selected = index;
				d_call(self, m_emitter_raise, v_uiable_signals[e_uiable_signal_changed]);
			}
			break;
		}
		++index;
	}
	return result;
}

d_define_method_override(list, draw)(struct s_object *self, struct s_object *environment) {
	d_using(list);
	struct s_drawable_attributes *drawable_attributes_self = d_cast(self, drawable),
				     *drawable_attributes_scroll = d_cast(list_attributes->scroll, drawable),
				     *drawable_attributes_entry;
	struct s_environment_attributes *environment_attributes = d_cast(environment, environment);
	struct s_object *current_entry;
	double position_x, position_y, position_x_self, position_y_self, normalized_position_x_self, normalized_position_y_self, normalized_dimension_w_self,
	       normalized_dimension_h_self, dimension_w_self, dimension_h_self, center_x_self, center_y_self, normalized_dimension_w_scroll,
	       normalized_dimension_h_scroll, position_x_entry, position_y_entry, dimension_w_entry, dimension_h_entry, normalized_dimension_w_entry,
	       normalized_dimension_h_entry, center_x, center_y, normalized_center_x_self, normalized_center_y_self, new_position_y;
	int index = 0, starting_uiable, result = (intptr_t)d_call_owner(self, uiable, m_drawable_draw, environment); /* recall the father's draw method */
	drawable_attributes_scroll->angle = drawable_attributes_self->angle;
	d_call(&(drawable_attributes_self->point_destination), m_point_get, &position_x_self, &position_y_self);
	d_call(&(drawable_attributes_self->point_normalized_destination), m_point_get, &normalized_position_x_self, &normalized_position_y_self);
	d_call(&(drawable_attributes_self->point_dimension), m_point_get, &dimension_w_self, &dimension_h_self);
	d_call(&(drawable_attributes_self->point_normalized_dimension), m_point_get, &normalized_dimension_w_self, &normalized_dimension_h_self);
	d_call(&(drawable_attributes_self->point_center), m_point_get, &center_x_self, &center_y_self);
	d_call(&(drawable_attributes_self->point_normalized_center), m_point_get, &normalized_center_x_self, &normalized_center_y_self);
	d_call(list_attributes->scroll, m_drawable_set_dimension_h, dimension_h_self);
	drawable_attributes_scroll->angle = drawable_attributes_self->angle;
	drawable_attributes_scroll->zoom = drawable_attributes_self->zoom;
	drawable_attributes_scroll->flip = drawable_attributes_self->flip;
	if ((d_call(list_attributes->scroll, m_drawable_normalize_scale, environment_attributes->reference_w,
					environment_attributes->reference_h,
					environment_attributes->camera_origin_x,
					environment_attributes->camera_origin_y,
					environment_attributes->camera_focus_x,
					environment_attributes->camera_focus_y,
					environment_attributes->current_w,
					environment_attributes->current_h,
					environment_attributes->zoom))) {
		d_call(&(drawable_attributes_scroll->point_normalized_dimension), m_point_get, &normalized_dimension_w_scroll, &normalized_dimension_h_scroll);
		position_x = (normalized_position_x_self + normalized_dimension_w_self) - normalized_dimension_w_scroll;
		position_y = normalized_position_y_self;
		center_x = (normalized_position_x_self + normalized_center_x_self) - position_x;
		center_y = (normalized_position_y_self + normalized_center_y_self) - position_y;
		d_call(&(drawable_attributes_scroll->point_normalized_destination), m_point_set_x, position_x);
		d_call(&(drawable_attributes_scroll->point_normalized_destination), m_point_set_y, position_y);
		d_call(&(drawable_attributes_scroll->point_normalized_center), m_point_set_x, center_x);
		d_call(&(drawable_attributes_scroll->point_normalized_center), m_point_set_y, center_y);
		d_call(&(drawable_attributes_scroll->square_collision_box), m_square_set_top_left_x, position_x, d_false);
		d_call(&(drawable_attributes_scroll->square_collision_box), m_square_set_top_left_y, position_y, d_false);
		d_call(&(drawable_attributes_scroll->square_collision_box), m_square_set_bottom_right_x, (position_x + normalized_dimension_w_scroll));
		d_call(&(drawable_attributes_scroll->square_collision_box), m_square_set_bottom_right_y, (position_y + normalized_dimension_h_scroll));
		d_call(&(drawable_attributes_scroll->square_collision_box), m_square_set_center, center_x, center_y);
		while (((int)d_call(list_attributes->scroll, m_drawable_draw, environment)) == d_drawable_return_continue);
	}
	if ((starting_uiable = (intptr_t)d_call(list_attributes->scroll, m_scroll_get_position, NULL)) >= 0) {
		new_position_y = position_y_self;
		d_foreach(&(list_attributes->uiables), current_entry, struct s_object) {
			if (index >= starting_uiable) {
				drawable_attributes_entry = d_cast(current_entry, drawable);
				center_x = center_x_self;
				center_y = (position_y_self + center_y_self) - new_position_y;
				d_call(current_entry, m_drawable_set_position, position_x_self, new_position_y);
				d_call(current_entry, m_drawable_set_center, center_x, center_y);
				drawable_attributes_entry->angle = drawable_attributes_self->angle;
				drawable_attributes_entry->zoom = drawable_attributes_self->zoom;
				drawable_attributes_entry->flip = drawable_attributes_self->flip;
				d_call(&(drawable_attributes_entry->point_dimension), m_point_get, &dimension_w_entry, &dimension_h_entry);
				if ((d_call(current_entry, m_drawable_normalize_scale, environment_attributes->reference_w,
								environment_attributes->reference_h,
								environment_attributes->camera_origin_x,
								environment_attributes->camera_origin_y,
								environment_attributes->camera_focus_x,
								environment_attributes->camera_focus_y,
								environment_attributes->current_w,
								environment_attributes->current_h,
								environment_attributes->zoom))) {
					d_call(&(drawable_attributes_entry->point_normalized_destination), m_point_get, &position_x_entry, &position_y_entry);
					d_call(&(drawable_attributes_entry->point_normalized_dimension), m_point_get, &normalized_dimension_w_entry,
							&normalized_dimension_h_entry);
					if ((position_y_entry + normalized_dimension_h_entry) < (normalized_position_y_self + normalized_dimension_h_self)) {
						if (list_attributes->selected == index) {
							d_call(current_entry, m_drawable_set_maskRGB, (unsigned int)list_attributes->selected_mask_R,
									(unsigned int)list_attributes->selected_mask_G,
									(unsigned int)list_attributes->selected_mask_B);
							d_call(current_entry, m_drawable_set_maskA, (unsigned int)list_attributes->selected_mask_A);
						} else {
							d_call(current_entry, m_drawable_set_maskRGB, (unsigned int)list_attributes->last_mask_R,
									(unsigned int)list_attributes->last_mask_G,
									(unsigned int)list_attributes->last_mask_B);
							d_call(current_entry, m_drawable_set_maskA, (unsigned int)list_attributes->last_mask_A);
						}
						d_call(current_entry, m_drawable_set_blend, list_attributes->last_blend);
						while (((int)d_call(current_entry, m_drawable_draw, environment)) == d_drawable_return_continue);
					} else
						break; /* not visible anymore */
					new_position_y += dimension_h_entry;
				}
			}
			++index;
		}
	}
	d_cast_return(result);
}

d_define_method_override(list, set_maskRGB)(struct s_object *self, unsigned int red, unsigned int green, unsigned int blue) {
	d_using(list);
	struct s_object *current_node;
	d_call_owner(self, uiable, m_drawable_set_maskRGB, red, green ,blue);
	d_call(list_attributes->scroll, m_drawable_set_maskRGB, red, green, blue);
	d_foreach(&(list_attributes->uiables), current_node, struct s_object)
		d_call(current_node, m_drawable_set_maskRGB, red, green, blue);
	list_attributes->last_mask_R = red;
	list_attributes->last_mask_G = green;
	list_attributes->last_mask_B = blue;
	return self;
}

d_define_method_override(list, set_maskA)(struct s_object *self, unsigned int alpha) {
	d_using(list);
	struct s_object *current_node;
	d_call_owner(self, uiable, m_drawable_set_maskA, alpha);
	d_call(list_attributes->scroll, m_drawable_set_maskA, alpha);
	d_foreach(&(list_attributes->uiables), current_node, struct s_object)
		d_call(current_node, m_drawable_set_maskA, alpha);
	list_attributes->last_mask_A = alpha;
	return self;
}

d_define_method_override(list, set_blend)(struct s_object *self, enum e_drawable_blends blend) {
	d_using(list);
	struct s_object *current_node;
	d_call_owner(self, uiable, m_drawable_set_blend, blend);
	d_call(list_attributes->scroll, m_drawable_set_blend, blend);
	d_foreach(&(list_attributes->uiables), current_node, struct s_object)
		d_call(current_node, m_drawable_set_blend, blend);
	list_attributes->last_blend = blend;
	return self;
}

d_define_method(list, delete)(struct s_object *self, struct s_list_attributes *attributes) {
	struct s_object *current_node;
	d_delete(attributes->scroll);
	while ((current_node = (struct s_object *)(attributes->uiables.head))) {
		f_list_delete(&(attributes->uiables), (struct s_list_node *)current_node);
		d_delete(current_node);
	}
	return NULL;
}

d_define_class(list) {
	d_hook_method(list, e_flag_public, add_uiable),
	d_hook_method(list, e_flag_public, del_uiable),
	d_hook_method(list, e_flag_public, set_uiable),
	d_hook_method(list, e_flag_public, get_uiable),
	d_hook_method_override(list, e_flag_public, uiable, mode),
	d_hook_method_override(list, e_flag_public, eventable, event),
	d_hook_method_override(list, e_flag_public, drawable, draw),
	d_hook_method_override(list, e_flag_public, drawable, set_maskRGB),
	d_hook_method_override(list, e_flag_public, drawable, set_maskA),
	d_hook_method_override(list, e_flag_public, drawable, set_blend),
	d_hook_delete(list),
	d_hook_method_tail
};
