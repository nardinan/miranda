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
    struct s_list_attributes *attributes = p_list_alloc(self);
    int index;
    attributes->scroll = d_retain(scroll);
    attributes->last_blend = e_drawable_blend_undefined;
    attributes->last_mask_R = 255.0;
    attributes->last_mask_G = 255.0;
    attributes->last_mask_B = 255.0;
    attributes->last_mask_A = 255.0;
    attributes->multi_selection = d_true;
    for (index = 0; index < d_list_max_selected; ++index)
        attributes->selection[index] = d_list_selected_NULL;
    return self;
}

d_define_method(list, add_uiable)(struct s_object *self, struct s_object *uiable) {
    d_using(list);
    d_call(uiable, m_drawable_set_maskRGB, (unsigned int)list_attributes->last_mask_R, (unsigned int)list_attributes->last_mask_G,
            (unsigned int)list_attributes->last_mask_B);
    d_call(uiable, m_drawable_set_maskA, (unsigned int)list_attributes->last_mask_A);
    d_call(uiable, m_drawable_set_blend, list_attributes->last_blend);
    f_list_append(&(list_attributes->uiables), d_retain(uiable), e_list_insert_tail);
    ++(list_attributes->uiable_entries);
    d_call(list_attributes->scroll, m_scroll_set_range, 0, d_math_max((list_attributes->uiable_entries-1), 0));
    return self;
}

d_define_method(list, del_uiable)(struct s_object *self, struct s_object *uiable) {
    d_using(list);
    struct s_object *current_entry = uiable;
    int index, pointer = 0;
    while (((struct s_list_node *)current_entry)->back) {
        current_entry = (struct s_object *)((struct s_list_node *)current_entry)->back;
        ++pointer;
    }
    for (index = 0; index < d_list_max_selected; ++index)
        if (list_attributes->selection[index] == pointer)
            break;
    for (; index < (d_list_max_selected-1); ++index)
        list_attributes->selection[index] = list_attributes->selection[index+1];
    list_attributes->selection[index] = d_list_selected_NULL;
    /* clean all the pointers to the next elements */
    for (index = 0; index < d_list_max_selected; ++index)
        if (list_attributes->selection[index] > pointer)
            --list_attributes->selection[index];
    f_list_delete(&(list_attributes->uiables), (struct s_list_node *)uiable);
    --(list_attributes->uiable_entries);
    d_delete(uiable);
    return uiable;
}

d_define_method(list, get_uiable)(struct s_object *self, unsigned int index) {
    d_using(list);
    struct s_object *current_entry, *result = NULL;
    int current_index = 0;
    d_foreach(&(list_attributes->uiables), current_entry, struct s_object) {
        if (current_index == index) {
            result = current_entry;
            break;
        }
        ++current_index;
    }
    return result;
}

d_define_method(list, set_selected_uiable)(struct s_object *self, struct s_object *uiable) {
    d_using(list);
    struct s_object *current_entry = uiable;
    int index, pointer = 0;
    t_boolean new_selection = d_false;
    while (((struct s_list_node *)current_entry)->back) {
        current_entry = (struct s_object *)((struct s_list_node *)current_entry)->back;
        ++pointer;
    }
    if ((list_attributes->selection[0] != pointer) && (new_selection = d_true))
        list_attributes->selection[0] = pointer;
    for (index = 1; index < d_list_max_selected; ++index)
        list_attributes->selection[index] = d_list_selected_NULL;
    d_cast_return(new_selection);
}

d_define_method(list, add_selected_uiable)(struct s_object *self, struct s_object *uiable) {
    d_using(list);
    struct s_object *current_entry = uiable;
    int index, pointer = 0;
    t_boolean new_selection = d_false;
    while (((struct s_list_node *)current_entry)->back) {
        current_entry = (struct s_object *)((struct s_list_node *)current_entry)->back;
        ++pointer;
    }
    for (index = 0; index < d_list_max_selected; ++index)
        if ((list_attributes->selection[index] == pointer) || ((list_attributes->selection[index] == d_list_selected_NULL) &&
                    (new_selection = d_true))) {
            list_attributes->selection[index] = pointer;
            break;
        }
    d_cast_return(new_selection);
}

d_define_method(list, get_selected_uiable)(struct s_object *self) {
    d_using(list);
    d_cast_return(list_attributes->selection);
}

d_define_method(list, set_selected)(struct s_object *self, unsigned int red, unsigned int green, unsigned int blue, unsigned int alpha) {
    d_using(list);
    list_attributes->selected_background_R = red;
    list_attributes->selected_background_G = green;
    list_attributes->selected_background_B = blue;
    list_attributes->selected_background_A = alpha;
    return self;
}

d_define_method(list, set_over)(struct s_object *self, unsigned int red, unsigned int green, unsigned int blue, unsigned int alpha) {
    d_using(list);
    list_attributes->over_background_R = red;
    list_attributes->over_background_G = green;
    list_attributes->over_background_B = blue;
    list_attributes->over_background_A = alpha;
    return self;
}

d_define_method(list, set_unselected)(struct s_object *self, unsigned int red, unsigned int green, unsigned int blue, unsigned int alpha) {
    d_using(list);
    list_attributes->unselected_background_R = red;
    list_attributes->unselected_background_G = green;
    list_attributes->unselected_background_B = blue;
    list_attributes->unselected_background_A = alpha;
    return self;

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
    struct s_uiable_attributes *uiable_attributes_self = d_cast(self, uiable),
                               *uiable_attributes_entry;
    struct s_drawable_attributes *drawable_attributes = d_cast(self, drawable);
    struct s_scroll_attributes *scroll_attributes = d_cast(list_attributes->scroll, scroll);
    struct s_object *current_entry;
    const unsigned char *keystate = SDL_GetKeyboardState(NULL);
    int mouse_x, mouse_y, pointer = 0, forwarded_entries = 0, starting_uiable = 0;
    t_boolean new_selection = d_false, modified = d_false;
    SDL_GetMouseState(&mouse_x, &mouse_y);
    if (((intptr_t)d_call(&(drawable_attributes->square_collision_box), m_square_inside_coordinates, (double)mouse_x, (double)mouse_y))) { 
        scroll_attributes->force_event = d_true; {
            d_call(list_attributes->scroll, m_eventable_event, environment, current_event);
        } scroll_attributes->force_event = d_false;
        if ((starting_uiable = (intptr_t)d_call(list_attributes->scroll, m_scroll_get_position, NULL)) >= 0)
            d_foreach(&(list_attributes->uiables), current_entry, struct s_object) {
                if (pointer >= starting_uiable) {
                    d_call(current_entry, m_eventable_event, environment, current_event);
                    if ((current_event->type == SDL_MOUSEBUTTONUP) && (current_event->button.button == SDL_BUTTON_LEFT)) {
                        uiable_attributes_entry = d_cast(current_entry, uiable);
                        if (uiable_attributes_entry->selected_mode == e_uiable_mode_selected) {
                            if (((keystate[SDL_SCANCODE_LSHIFT]) || (keystate[SDL_SCANCODE_RSHIFT])) && (list_attributes->multi_selection))
                                new_selection |= (intptr_t)d_call(self, m_list_add_selected_uiable, current_entry);
                            else
                                new_selection |= (intptr_t)d_call(self, m_list_set_selected_uiable, current_entry);
                        }
                    }
                    if ((++forwarded_entries) == list_attributes->visible_entries)
                        break;
                }
                ++pointer;
            }
    }
    if ((uiable_attributes_self->selected_mode == e_uiable_mode_selected) && (current_event->type == SDL_KEYDOWN))
        if (list_attributes->selection[0] != d_list_selected_NULL) {
            switch (current_event->key.keysym.sym) {
                case SDLK_UP:
                    if (list_attributes->selection[0] > 0) {
                        --(list_attributes->selection[0]);
                        new_selection = d_true;
                        modified = d_true;
                    }
                    break;
                case SDLK_DOWN:
                    if (list_attributes->selection[0] < (list_attributes->uiable_entries - 1)) {
                        ++(list_attributes->selection[0]);
                        new_selection = d_true;
                        modified = d_true;
                    }

            }
            if (modified) {
                if (list_attributes->selection[0] < starting_uiable)
                    d_call(list_attributes->scroll, m_scroll_set_position, list_attributes->selection[0]);
                else if (list_attributes->selection[0] > (starting_uiable + (list_attributes->visible_entries - 1)))
                    d_call(list_attributes->scroll, m_scroll_set_position, (list_attributes->selection[0] - (list_attributes->visible_entries - 1)));
            }
        }
    if (new_selection)
        d_call(self, m_emitter_raise, v_uiable_signals[e_uiable_signal_content_changed]);
    return self;
}

d_define_method_override(list, draw)(struct s_object *self, struct s_object *environment) {
    d_using(list);
    struct s_drawable_attributes *drawable_attributes_self = d_cast(self, drawable),
                                 *drawable_attributes_scroll = d_cast(list_attributes->scroll, drawable),
                                 *drawable_attributes_entry;
    struct s_uiable_attributes *uiable_attributes = d_cast(self, uiable);
    struct s_environment_attributes *environment_attributes = d_cast(environment, environment);
    struct s_object *current_entry;
    double position_x, position_y, position_x_self, position_y_self, normalized_position_x_self, normalized_position_y_self, normalized_dimension_w_self,
           normalized_dimension_h_self, dimension_w_self, dimension_h_self, center_x_self, center_y_self, dimension_w_scroll, dimension_h_scroll,
           normalized_dimension_w_scroll, normalized_dimension_h_scroll, position_x_entry, position_y_entry, dimension_w_entry, dimension_h_entry,
           normalized_dimension_w_entry, normalized_dimension_h_entry, center_x, center_y, normalized_center_x_self, normalized_center_y_self,
           new_position_y;
    int index, pointer = 0, mouse_x, mouse_y, starting_uiable, result = (intptr_t)d_call_owner(self, uiable, m_drawable_draw, environment); /* recall the father's draw method */
    t_boolean is_selected;
    drawable_attributes_scroll->angle = drawable_attributes_self->angle;
    list_attributes->visible_entries = 0;
    d_call(&(drawable_attributes_self->point_destination), m_point_get, &position_x_self, &position_y_self);
    d_call(&(drawable_attributes_self->point_normalized_destination), m_point_get, &normalized_position_x_self, &normalized_position_y_self);
    d_call(&(drawable_attributes_self->point_dimension), m_point_get, &dimension_w_self, &dimension_h_self);
    d_call(&(drawable_attributes_self->point_normalized_dimension), m_point_get, &normalized_dimension_w_self, &normalized_dimension_h_self);
    d_call(&(drawable_attributes_self->point_center), m_point_get, &center_x_self, &center_y_self);
    d_call(&(drawable_attributes_self->point_normalized_center), m_point_get, &normalized_center_x_self, &normalized_center_y_self);
    d_call(&(drawable_attributes_scroll->point_dimension), m_point_get, &dimension_w_scroll, &dimension_h_scroll);
    d_call(list_attributes->scroll, m_drawable_set_dimension_h, (dimension_h_self - (uiable_attributes->border_h * 2.0)));
    drawable_attributes_scroll->angle = drawable_attributes_self->angle;
    drawable_attributes_scroll->zoom = drawable_attributes_self->zoom;
    drawable_attributes_scroll->flip = drawable_attributes_self->flip;
    if ((starting_uiable = (intptr_t)d_call(list_attributes->scroll, m_scroll_get_position, NULL)) >= 0) {
        SDL_GetMouseState(&mouse_x, &mouse_y);
        new_position_y = position_y_self;
        d_foreach(&(list_attributes->uiables), current_entry, struct s_object) {
            if (pointer >= starting_uiable) {
                drawable_attributes_entry = d_cast(current_entry, drawable);
                center_x = (center_x_self - uiable_attributes->border_w);
                center_y = (position_y_self + center_y_self) - new_position_y;
                d_call(current_entry, m_drawable_set_position, (position_x_self + uiable_attributes->border_w), new_position_y);
                d_call(current_entry, m_drawable_set_dimension_w, (dimension_w_self - dimension_w_scroll - (uiable_attributes->border_w * 2.0)));
                d_call(current_entry, m_drawable_set_center, center_x, center_y);
                drawable_attributes_entry->angle = drawable_attributes_self->angle;
                drawable_attributes_entry->zoom = drawable_attributes_self->zoom;
                drawable_attributes_entry->flip = drawable_attributes_self->flip;
                d_call(&(drawable_attributes_entry->point_dimension), m_point_get, &dimension_w_entry, &dimension_h_entry);
                if ((d_call(current_entry, m_drawable_normalize_scale, environment_attributes->reference_w[environment_attributes->current_surface],
                                environment_attributes->reference_h[environment_attributes->current_surface],
                                environment_attributes->camera_origin_x[environment_attributes->current_surface],
                                environment_attributes->camera_origin_y[environment_attributes->current_surface],
                                environment_attributes->camera_focus_x[environment_attributes->current_surface],
                                environment_attributes->camera_focus_y[environment_attributes->current_surface],
                                environment_attributes->current_w,
                                environment_attributes->current_h,
                                environment_attributes->zoom[environment_attributes->current_surface]))) {
                    d_call(&(drawable_attributes_entry->point_normalized_destination), m_point_get, &position_x_entry, &position_y_entry);
                    d_call(&(drawable_attributes_entry->point_normalized_dimension), m_point_get, &normalized_dimension_w_entry,
                            &normalized_dimension_h_entry);
                    if ((position_y_entry + normalized_dimension_h_entry) < (normalized_position_y_self + normalized_dimension_h_self)) {
                        is_selected = d_false;
                        for (index = 0; index < d_list_max_selected; ++index)
                            if (list_attributes->selection[index] == pointer) {
                                is_selected = d_true;
                                break;
                            }
                        if (is_selected)
                            d_call(current_entry, m_uiable_set_background, (unsigned int)list_attributes->selected_background_R,
                                    (unsigned int)list_attributes->selected_background_G,
                                    (unsigned int)list_attributes->selected_background_B,
                                    (unsigned int)list_attributes->selected_background_A);
                        else if ((intptr_t)d_call(&(drawable_attributes_entry->square_collision_box), m_square_inside_coordinates,
                                    (double)mouse_x, (double)mouse_y))
                            d_call(current_entry, m_uiable_set_background, (unsigned int)list_attributes->over_background_R,
                                    (unsigned int)list_attributes->over_background_G,
                                    (unsigned int)list_attributes->over_background_B,
                                    (unsigned int)list_attributes->over_background_A);
                        else
                            d_call(current_entry, m_uiable_set_background, (unsigned int)list_attributes->unselected_background_R,
                                    (unsigned int)list_attributes->unselected_background_G,
                                    (unsigned int)list_attributes->unselected_background_B,
                                    (unsigned int)list_attributes->unselected_background_A);
                        d_call(current_entry, m_drawable_set_blend, list_attributes->last_blend);
                        while (((int)d_call(current_entry, m_drawable_draw, environment)) == d_drawable_return_continue);
                        ++(list_attributes->visible_entries);
                    } else
                        break; /* not visible anymore */
                    new_position_y += dimension_h_entry;
                }
            }
            ++pointer;
        }
    }
    if ((d_call(list_attributes->scroll, m_drawable_normalize_scale, environment_attributes->reference_w[environment_attributes->current_surface],
                    environment_attributes->reference_h[environment_attributes->current_surface],
                    environment_attributes->camera_origin_x[environment_attributes->current_surface],
                    environment_attributes->camera_origin_y[environment_attributes->current_surface],
                    environment_attributes->camera_focus_x[environment_attributes->current_surface],
                    environment_attributes->camera_focus_y[environment_attributes->current_surface],
                    environment_attributes->current_w,
                    environment_attributes->current_h,
                    environment_attributes->zoom[environment_attributes->current_surface]))) {
        d_call(&(drawable_attributes_scroll->point_normalized_dimension), m_point_get, &normalized_dimension_w_scroll, &normalized_dimension_h_scroll);
        position_x = (normalized_position_x_self + normalized_dimension_w_self) - normalized_dimension_w_scroll - uiable_attributes->border_w;
        position_y = normalized_position_y_self + uiable_attributes->border_h;
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
        d_hook_method(list, e_flag_public, get_uiable),
        d_hook_method(list, e_flag_public, set_selected_uiable),
        d_hook_method(list, e_flag_public, add_selected_uiable),
        d_hook_method(list, e_flag_public, get_selected_uiable),
        d_hook_method(list, e_flag_public, set_selected),
        d_hook_method(list, e_flag_public, set_over),
        d_hook_method(list, e_flag_public, set_unselected),
        d_hook_method_override(list, e_flag_public, uiable, mode),
        d_hook_method_override(list, e_flag_public, eventable, event),
        d_hook_method_override(list, e_flag_public, drawable, draw),
        d_hook_method_override(list, e_flag_public, drawable, set_maskRGB),
        d_hook_method_override(list, e_flag_public, drawable, set_maskA),
        d_hook_method_override(list, e_flag_public, drawable, set_blend),
        d_hook_delete(list),
        d_hook_method_tail
};
