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
#include "morpholable.obj.h"
struct s_morpholable_attributes *p_morpholable_alloc(struct s_object *self) {
    struct s_morpholable_attributes *result = d_prepare(self, morpholable);
    /* abstract (no memory inheritance) */
    f_drawable_new(self, (e_drawable_kind_single|e_drawable_kind_force_visibility));    /* inherit */
    f_eventable_new(self);	                                                            /* inherit */
    return result;
}

struct s_object *f_morpholable_new(struct s_object *self) {
    struct s_morpholable_attributes *attributes = p_morpholable_alloc(self);
    attributes = attributes;
    return self;
}

d_define_method(morpholable, set_freedom_x)(struct s_object *self, t_boolean free) {
    d_using(morpholable);
    morpholable_attributes->freedom_x = free;
    return self;
}

d_define_method(morpholable, set_freedom_y)(struct s_object *self, t_boolean free) {
    d_using(morpholable);
    morpholable_attributes->freedom_y = free;
    return self;
}

d_define_method(morpholable, set_freedom_z)(struct s_object *self, t_boolean free) {
    d_using(morpholable);
    morpholable_attributes->freedom_z = free;
    return self;
}

d_define_method_override(morpholable, event)(struct s_object *self, struct s_object *environment, SDL_Event *current_event) {
    d_using(morpholable);
    struct s_drawable_attributes *drawable_attributes = d_cast(self, drawable);
    struct s_environment_attributes *environment_attributes = d_cast(environment, environment);
    struct s_exception *exception;
    int mouse_x, mouse_y;
    d_try {
        switch (current_event->type) {
            case SDL_MOUSEWHEEL:
                if (morpholable_attributes->grabbed)
                    morpholable_attributes->offset_z += (current_event->wheel.y * d_morpholable_z_factor);
                break;
            case SDL_MOUSEBUTTONUP:
                if (current_event->button.button == SDL_BUTTON_LEFT)
                    morpholable_attributes->grabbed = d_false;
                break;
            case SDL_MOUSEBUTTONDOWN:
                if (current_event->button.button == SDL_BUTTON_LEFT) {
                    SDL_GetMouseState(&mouse_x, &mouse_y);
                    mouse_x = ((double)mouse_x * environment_attributes->reference_w[environment_attributes->current_surface])/environment_attributes->current_w;
                    mouse_y = ((double)mouse_y * environment_attributes->reference_h[environment_attributes->current_surface])/environment_attributes->current_h;
                    if (((intptr_t)d_call(&(drawable_attributes->square_collision_box), m_square_inside_coordinates, (double)mouse_x, (double)mouse_y))) {
                        morpholable_attributes->grabbed = d_true;
                        morpholable_attributes->offset_x = NAN;
                        morpholable_attributes->offset_y = NAN;
                        morpholable_attributes->offset_z = NAN;
                    }
                }
        }
    } d_catch(exception) {
        d_exception_dump(stderr, exception);
        d_raise;
    } d_endtry;
    return self;
}

d_define_method_override(morpholable, draw)(struct s_object *self, struct s_object *environment) {
    d_using(morpholable);
    struct s_drawable_attributes *drawable_attributes = d_cast(self, drawable);
    struct s_environment_attributes *environment_attributes = d_cast(environment, environment);
    double position_x_self, position_y_self, position_x, position_y;
    int mouse_x, mouse_y;
    if (morpholable_attributes->grabbed) {
        d_call(&(drawable_attributes->point_destination), m_point_get, &position_x_self, &position_y_self);
        SDL_GetMouseState(&mouse_x, &mouse_y);
        mouse_x = ((double)mouse_x * environment_attributes->reference_w[environment_attributes->current_surface])/environment_attributes->current_w;
        mouse_y = ((double)mouse_y * environment_attributes->reference_h[environment_attributes->current_surface])/environment_attributes->current_h;
        if ((morpholable_attributes->offset_x != morpholable_attributes->offset_x) && (morpholable_attributes->offset_y != morpholable_attributes->offset_y) && 
                (morpholable_attributes->offset_z != morpholable_attributes->offset_z)) {
            morpholable_attributes->offset_x = (mouse_x - position_x_self);
            morpholable_attributes->offset_y = (mouse_y - position_y_self);
            morpholable_attributes->offset_z = drawable_attributes->zoom;
        } else {
            position_x = (double)mouse_x - morpholable_attributes->offset_x;
            position_y = (double)mouse_y - morpholable_attributes->offset_y;
            if (morpholable_attributes->freedom_x)
                d_call(self, m_drawable_set_position_x, position_x);
            if (morpholable_attributes->freedom_y)
                d_call(self, m_drawable_set_position_y, position_y);
            if (morpholable_attributes->freedom_z)
                d_call(self, m_drawable_set_zoom, morpholable_attributes->offset_z);
        }
    }
    d_cast_return(d_drawable_return_last);
}

d_define_method(morpholable, delete)(struct s_object *self, struct s_morpholable_attributes *attributes) {
    return NULL;
}

d_define_class(morpholable) {
    d_hook_method(morpholable, e_flag_public, set_freedom_x),
        d_hook_method(morpholable, e_flag_public, set_freedom_y),
        d_hook_method(morpholable, e_flag_public, set_freedom_z),
        d_hook_method_override(morpholable, e_flag_public, eventable, event),
        d_hook_method_override(morpholable, e_flag_public, drawable, draw),
        d_hook_delete(morpholable),
        d_hook_method_tail
};
