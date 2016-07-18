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
unsigned int v_uiable_id = 0;
const char *v_uiable_components[e_uiable_component_NULL] = {
    "CTL",
    "CTR",
    "CBL",
    "CBR",
    "T",
    "B",
    "L",
    "R",
    "C"
};
const char *v_uiable_modes[e_uiable_mode_NULL] = {
    "active",
    "selected",
    "idle"
};
const char *v_uiable_signals[e_uiable_signal_NULL] = {
    "selected",
    "unselected",
    "clicked_left",
    "clicked_right",
    "changed",
    "content_changed"
};
struct s_uiable_attributes *p_uiable_alloc(struct s_object *self) {
    struct s_uiable_attributes *result = d_prepare(self, uiable);
    /* abstract (no memory inheritance) */
    f_morphable_new(self);                                                              /* inherit */
    f_drawable_new(self, (e_drawable_kind_single|e_drawable_kind_force_visibility));    /* inherit */
    f_emitter_new(self);                                                                /* inherit */
    return result;
}

struct s_object *f_uiable_new(struct s_object *self) {
    struct s_uiable_attributes *attributes = p_uiable_alloc(self);
    int index;
    attributes->border_w = d_uiable_default_border;
    attributes->border_h = d_uiable_default_border;
    attributes->component_id = v_uiable_id++;
    for (index = 0; index != e_uiable_signal_NULL; ++index)
        d_call(self, m_emitter_record, v_uiable_signals[index]);
    return self;
}

d_define_method(uiable, set)(struct s_object *self, struct s_object *drawable, enum e_uiable_modes mode, enum e_uiable_components component) {
    d_using(uiable);
    struct s_drawable_attributes *drawable_attributes = d_cast(self, drawable);
    if (uiable_attributes->background[mode][component])
        d_delete(uiable_attributes->background[mode][component]);
    uiable_attributes->background[mode][component] = d_retain(drawable);
    if (drawable_attributes->last_blend != e_drawable_blend_undefined)
        d_call(uiable_attributes->background[mode][component], m_drawable_set_blend, drawable_attributes->last_blend);
    d_call(uiable_attributes->background[mode][component], m_drawable_set_maskRGB, (unsigned int)drawable_attributes->last_mask_R, 
            (unsigned int)drawable_attributes->last_mask_G, (unsigned int)drawable_attributes->last_mask_B);
    d_call(uiable_attributes->background[mode][component], m_drawable_set_maskA, (unsigned int)drawable_attributes->last_mask_A);
    return self;
}

d_define_method(uiable, set_background)(struct s_object *self, unsigned int red, unsigned int green, unsigned int blue, unsigned int alpha) {
    d_using(uiable);
    uiable_attributes->background_mask_R = red;
    uiable_attributes->background_mask_G = green;
    uiable_attributes->background_mask_B = blue;
    uiable_attributes->background_mask_A = alpha;
    return self;
}

d_define_method(uiable, mode)(struct s_object *self, enum e_uiable_modes mode) {
    d_using(uiable);
    if (uiable_attributes->selected_mode != mode) {
        if ((uiable_attributes->selected_mode = mode) == e_uiable_mode_idle) {
            uiable_attributes->is_selected = d_false;
            d_call(self, m_eventable_set_enable, d_false);
        } else
            d_call(self, m_eventable_set_enable, d_true);
        d_call(self, m_emitter_raise, v_uiable_signals[e_uiable_signal_changed]);
    }
    return self;
}

d_define_method_override(uiable, event)(struct s_object *self, struct s_object *environment, SDL_Event *current_event) {
    d_using(uiable);
    struct s_drawable_attributes *drawable_attributes = d_cast(self, drawable);
    struct s_object *result = d_call_owner(self, morphable, m_eventable_event, environment, current_event);
    int mouse_x, mouse_y;
    SDL_GetMouseState(&mouse_x, &mouse_y);
    if (((intptr_t)d_call(&(drawable_attributes->square_collision_box), m_square_inside_coordinates, (double)mouse_x, (double)mouse_y))) {
        if (!uiable_attributes->is_selected) {
            uiable_attributes->is_selected = d_true;
            d_call(self, m_emitter_raise, v_uiable_signals[e_uiable_signal_selected]);
        }
        if (current_event->type == SDL_MOUSEBUTTONDOWN) {
            if (current_event->button.button == SDL_BUTTON_LEFT) {
                d_call(self, m_uiable_mode, e_uiable_mode_selected);
                d_call(self, m_emitter_raise, v_uiable_signals[e_uiable_signal_clicked_left]);
            } else if (current_event->button.button == SDL_BUTTON_RIGHT)
                d_call(self, m_emitter_raise, v_uiable_signals[e_uiable_signal_clicked_right]);
        }
    } else {
        if (uiable_attributes->is_selected) {
            uiable_attributes->is_selected = d_false;
            d_call(self, m_emitter_raise, v_uiable_signals[e_uiable_signal_unselected]);
        }
        if (current_event->type == SDL_MOUSEBUTTONDOWN)
            if (current_event->button.button == SDL_BUTTON_LEFT)
                d_call(self, m_uiable_mode, e_uiable_mode_active);
    }
    return result;
}

d_define_method_override(uiable, draw)(struct s_object *self, struct s_object *environment) {
    d_using(uiable);
    struct s_environment_attributes *environment_attributes = d_cast(environment, environment);
    struct s_drawable_attributes *drawable_attributes_self = d_cast(self, drawable),
                                 *drawable_attributes_core;
    struct s_square_attributes *square_attributes = d_cast(&(drawable_attributes_self->square_collision_box), square);
    struct s_object *result = d_call(self, m_morphable_update, environment);
    int index, background_x[] = {
        square_attributes->normalized_top_left_x,
        square_attributes->normalized_top_right_x,
        square_attributes->normalized_bottom_right_x,
        square_attributes->normalized_bottom_left_x
    }, background_y[] = {
        square_attributes->normalized_top_left_y,
        square_attributes->normalized_top_right_y,
        square_attributes->normalized_bottom_right_y,
        square_attributes->normalized_bottom_left_y
    };
    double local_x, local_y, local_w, local_h, center_x, center_y, component_w[e_uiable_component_NULL], component_h[e_uiable_component_NULL];
    d_call(&(drawable_attributes_self->point_normalized_destination), m_point_get, &local_x, &local_y);
    d_call(&(drawable_attributes_self->point_normalized_dimension), m_point_get, &local_w, &local_h);
    d_call(&(drawable_attributes_self->point_normalized_center), m_point_get, &center_x, &center_y);
    for (index = 0; index < e_uiable_component_NULL; ++index)
        if (uiable_attributes->background[uiable_attributes->selected_mode][index]) {
            drawable_attributes_core = d_cast(uiable_attributes->background[uiable_attributes->selected_mode][index], drawable);
            d_call(&(drawable_attributes_core->point_normalized_dimension), m_point_get, &(component_w[index]), &(component_h[index]));
        } else {
            component_w[index] = 0;
            component_h[index] = 0;
        }
    if (uiable_attributes->background[uiable_attributes->selected_mode][e_uiable_component_center]) {
        d_call(uiable_attributes->background[uiable_attributes->selected_mode][e_uiable_component_center], m_drawable_set_position,
                (local_x+component_w[e_uiable_component_corner_top_left]-1),
                (local_y+component_h[e_uiable_component_corner_top_left]-1));
        d_call(uiable_attributes->background[uiable_attributes->selected_mode][e_uiable_component_center], m_drawable_set_dimension,
                (local_w-component_w[e_uiable_component_corner_top_left]-component_w[e_uiable_component_corner_top_right]+2),
                (local_h-component_h[e_uiable_component_corner_top_left]-component_h[e_uiable_component_corner_bottom_left]+2));
        d_call(uiable_attributes->background[uiable_attributes->selected_mode][e_uiable_component_center], m_drawable_set_center,
                (center_x-component_w[e_uiable_component_corner_top_left]+1),
                (center_y-component_h[e_uiable_component_corner_top_left]+1));
    }
    if (uiable_attributes->background[uiable_attributes->selected_mode][e_uiable_component_top]) {
        d_call(uiable_attributes->background[uiable_attributes->selected_mode][e_uiable_component_top], m_drawable_set_position,
                (local_x+component_w[e_uiable_component_corner_top_left]-1),
                local_y);
        d_call(uiable_attributes->background[uiable_attributes->selected_mode][e_uiable_component_top], m_drawable_set_dimension_w,
                (local_w-component_w[e_uiable_component_corner_top_left]-component_w[e_uiable_component_corner_top_right])+2);
        d_call(uiable_attributes->background[uiable_attributes->selected_mode][e_uiable_component_top], m_drawable_set_center,
                (center_x-component_w[e_uiable_component_corner_top_left]+1),
                center_y);
    }
    if (uiable_attributes->background[uiable_attributes->selected_mode][e_uiable_component_bottom]) {
        d_call(uiable_attributes->background[uiable_attributes->selected_mode][e_uiable_component_bottom], m_drawable_set_position,
                (local_x+component_w[e_uiable_component_corner_bottom_left]-1),
                (local_y+local_h-component_h[e_uiable_component_bottom]));
        d_call(uiable_attributes->background[uiable_attributes->selected_mode][e_uiable_component_bottom], m_drawable_set_dimension_w,
                (local_w-component_w[e_uiable_component_corner_bottom_left]-component_w[e_uiable_component_corner_bottom_right]+2));
        d_call(uiable_attributes->background[uiable_attributes->selected_mode][e_uiable_component_bottom], m_drawable_set_center,
                (center_x-component_w[e_uiable_component_corner_bottom_left]+1),
                (center_y-local_h+component_h[e_uiable_component_bottom]));
    }
    if (uiable_attributes->background[uiable_attributes->selected_mode][e_uiable_component_left]) {
        d_call(uiable_attributes->background[uiable_attributes->selected_mode][e_uiable_component_left], m_drawable_set_position,
                local_x,
                (local_y+component_h[e_uiable_component_corner_top_left]-1));
        d_call(uiable_attributes->background[uiable_attributes->selected_mode][e_uiable_component_left], m_drawable_set_dimension_h,
                (local_h-component_h[e_uiable_component_corner_top_left]-component_h[e_uiable_component_corner_bottom_left]+2));
        d_call(uiable_attributes->background[uiable_attributes->selected_mode][e_uiable_component_left], m_drawable_set_center,
                center_x,
                (center_y-component_h[e_uiable_component_corner_top_left]+1));
    }
    if (uiable_attributes->background[uiable_attributes->selected_mode][e_uiable_component_right]) {
        d_call(uiable_attributes->background[uiable_attributes->selected_mode][e_uiable_component_right], m_drawable_set_position,
                (local_x+local_w-component_w[e_uiable_component_right]),
                (local_y+component_h[e_uiable_component_corner_top_right]-1));
        d_call(uiable_attributes->background[uiable_attributes->selected_mode][e_uiable_component_right], m_drawable_set_dimension_h,
                (local_h-component_h[e_uiable_component_corner_top_right]-component_h[e_uiable_component_corner_bottom_right]+2));
        d_call(uiable_attributes->background[uiable_attributes->selected_mode][e_uiable_component_right], m_drawable_set_center,
                (center_x-local_w+component_w[e_uiable_component_right]),
                (center_y-component_h[e_uiable_component_corner_top_right]+1));
    }
    if (uiable_attributes->background[uiable_attributes->selected_mode][e_uiable_component_corner_top_left]) {
        d_call(uiable_attributes->background[uiable_attributes->selected_mode][e_uiable_component_corner_top_left], m_drawable_set_position,
                local_x,
                local_y);
        d_call(uiable_attributes->background[uiable_attributes->selected_mode][e_uiable_component_corner_top_left], m_drawable_set_center,
                center_x,
                center_y);
    }
    if (uiable_attributes->background[uiable_attributes->selected_mode][e_uiable_component_corner_top_right]) {
        d_call(uiable_attributes->background[uiable_attributes->selected_mode][e_uiable_component_corner_top_right], m_drawable_set_position,
                (local_x+local_w-component_w[e_uiable_component_corner_top_right]),
                local_y);
        d_call(uiable_attributes->background[uiable_attributes->selected_mode][e_uiable_component_corner_top_right], m_drawable_set_center,
                (center_x-local_w+component_w[e_uiable_component_corner_top_right]),
                center_y);
    }
    if (uiable_attributes->background[uiable_attributes->selected_mode][e_uiable_component_corner_bottom_left]) {
        d_call(uiable_attributes->background[uiable_attributes->selected_mode][e_uiable_component_corner_bottom_left], m_drawable_set_position,
                local_x,
                (local_y+local_h-component_h[e_uiable_component_corner_bottom_left]));
        d_call(uiable_attributes->background[uiable_attributes->selected_mode][e_uiable_component_corner_bottom_left], m_drawable_set_center,
                center_x,
                (center_y-local_h+component_h[e_uiable_component_corner_bottom_left]));
    }
    if (uiable_attributes->background[uiable_attributes->selected_mode][e_uiable_component_corner_bottom_right]) {
        d_call(uiable_attributes->background[uiable_attributes->selected_mode][e_uiable_component_corner_bottom_right], m_drawable_set_position,
                (local_x+local_w-component_w[e_uiable_component_corner_bottom_right]),
                (local_y+local_h-component_h[e_uiable_component_corner_bottom_right]));
        d_call(uiable_attributes->background[uiable_attributes->selected_mode][e_uiable_component_corner_bottom_right], m_drawable_set_center,
                (center_x-local_w+component_w[e_uiable_component_corner_bottom_right]),
                (center_y-local_h+component_h[e_uiable_component_corner_bottom_right]));
    }
    for (index = 0; index < e_uiable_component_NULL; ++index)
        if (uiable_attributes->background[uiable_attributes->selected_mode][index]) {
            drawable_attributes_core = d_cast(uiable_attributes->background[uiable_attributes->selected_mode][index], drawable);
            drawable_attributes_core->angle = drawable_attributes_self->angle;
            /* doesn't inerith the flip (this object, the uiable, doesn't flip) and the zoom is hardcoded to one */
            d_call(uiable_attributes->background[uiable_attributes->selected_mode][index], m_drawable_keep_scale,
                    environment_attributes->current_w, environment_attributes->current_h);
            while (((int)d_call(uiable_attributes->background[uiable_attributes->selected_mode][index], m_drawable_draw, environment)) ==
                    d_drawable_return_continue);
        }
    if (uiable_attributes->background_mask_A) /* only if visible */
        f_primitive_fill_polygon(environment_attributes->renderer, background_x, background_y, 4, uiable_attributes->background_mask_R,
                uiable_attributes->background_mask_G, uiable_attributes->background_mask_B, uiable_attributes->background_mask_A);
    if ((drawable_attributes_self->flags&e_drawable_kind_contour) == e_drawable_kind_contour)
        d_call(self, m_drawable_draw_contour, environment);
    return result;
}

d_define_method_override(uiable, set_maskRGB)(struct s_object *self, unsigned int red, unsigned int green, unsigned int blue) {
    d_using(uiable);
    struct s_drawable_attributes *drawable_attributes = d_cast(self, drawable);
    int mode, index;
    drawable_attributes->last_mask_R = red;
    drawable_attributes->last_mask_G = green;
    drawable_attributes->last_mask_B = blue;
    for (mode = 0; mode < e_uiable_mode_NULL; ++mode)
        for (index = 0; index < e_uiable_component_NULL; ++index)
            if (uiable_attributes->background[mode][index])
                d_call(uiable_attributes->background[mode][index], m_drawable_set_maskRGB, red, green, blue);
    return self;
}

d_define_method_override(uiable, set_maskA)(struct s_object *self, unsigned int alpha) {
    d_using(uiable);
    struct s_drawable_attributes *drawable_attributes = d_cast(self, drawable);
    int mode, index;
    drawable_attributes->last_mask_A = alpha;
    for (mode = 0; mode < e_uiable_mode_NULL; ++mode)
        for (index = 0; index < e_uiable_component_NULL; ++index)
            if (uiable_attributes->background[mode][index])
                d_call(uiable_attributes->background[mode][index], m_drawable_set_maskA, alpha);
    return self;
}

d_define_method_override(uiable, set_blend)(struct s_object *self, enum e_drawable_blends blend) {
    d_using(uiable);
    struct s_drawable_attributes *drawable_attributes = d_cast(self, drawable);
    int mode, index;
    drawable_attributes->last_blend = blend;
    for (mode = 0; mode < e_uiable_mode_NULL; ++mode)
        for (index = 0; index < e_uiable_component_NULL; ++index)
            if (uiable_attributes->background[mode][index])
                d_call(uiable_attributes->background[mode][index], m_drawable_set_blend, blend);
    return self;
}

d_define_method(uiable, delete)(struct s_object *self, struct s_uiable_attributes *attributes) {
    int mode, index;
    for (mode = 0; mode != e_uiable_mode_NULL; ++mode)
        for (index = 0; index != e_uiable_component_NULL; ++index)
            if (attributes->background[mode][index])
                d_delete(attributes->background[mode][index]);
    return NULL;
}

d_define_class(uiable) {
    d_hook_method(uiable, e_flag_public, set),
        d_hook_method(uiable, e_flag_public, set_background),
        d_hook_method(uiable, e_flag_public, mode),
        d_hook_method_override(uiable, e_flag_public, drawable, draw),
        d_hook_method_override(uiable, e_flag_public, eventable, event),
        d_hook_method_override(uiable, e_flag_public, drawable, set_maskRGB),
        d_hook_method_override(uiable, e_flag_public, drawable, set_maskA),
        d_hook_method_override(uiable, e_flag_public, drawable, set_blend),
        d_hook_delete(uiable),
        d_hook_method_tail
};
