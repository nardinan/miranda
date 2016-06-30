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
#include "scroll.obj.h"
struct s_scroll_attributes *p_scroll_alloc(struct s_object *self) {
    struct s_scroll_attributes *result = d_prepare(self, scroll);
    f_mutex_new(self);	/* inherit */
    f_memory_new(self);	/* inherit */
    f_uiable_new(self);	/* inherit */
    return result;
}

struct s_object *f_scroll_new(struct s_object *self, struct s_object *image) {
    struct s_scroll_attributes *attributes = p_scroll_alloc(self);
    attributes->image = d_retain(image);
    attributes->position = d_scroll_default_minimum;
    attributes->modifier = 1;
    attributes->minimum = d_scroll_default_minimum;
    attributes->maximum = d_scroll_default_maximum;
    return self;
}

d_define_method(scroll, set_range)(struct s_object *self, int minimum, int maximum) {
    d_using(scroll);
    scroll_attributes->minimum = minimum;
    scroll_attributes->maximum = maximum;
    return self;
}

d_define_method(scroll, set_modifier)(struct s_object *self, int modifier) {
    d_using(scroll);
    scroll_attributes->modifier = modifier;
    return self;
}

d_define_method(scroll, set_position)(struct s_object *self, int position) {
    d_using(scroll);
    scroll_attributes->position = position;
    return self;
}

d_define_method(scroll, get_position)(struct s_object *self) {
    d_using(scroll);
    d_cast_return(scroll_attributes->position);
    return self;
}

d_define_method_override(scroll, event)(struct s_object *self, struct s_object *environment, SDL_Event *current_event) {
    d_using(scroll);
    struct s_uiable_attributes *uiable_attributes = d_cast(self, uiable);
    struct s_drawable_attributes *drawable_attributes = d_cast(self, drawable);
    struct s_object *result = d_call_owner(self, uiable, m_eventable_event, environment, current_event);
    double position_x, position_y, dimension_w, dimension_h;
    int mouse_x, mouse_y;
    if (uiable_attributes->selected_mode != e_uiable_mode_idle) {
        d_call(&(drawable_attributes->point_destination), m_point_get, &position_x, &position_y);
        d_call(&(drawable_attributes->point_dimension), m_point_get, &dimension_w, &dimension_h);
        SDL_GetMouseState(&mouse_x, &mouse_y);
        if (current_event->type == SDL_MOUSEWHEEL)
            if ((scroll_attributes->force_event) || (((intptr_t)d_call(&(drawable_attributes->square_collision_box), m_square_inside_coordinates, 
                                (double)mouse_x, (double)mouse_y)))) {
                scroll_attributes->position += (current_event->wheel.y * scroll_attributes->modifier);
                if (scroll_attributes->position < scroll_attributes->minimum)
                    scroll_attributes->position = scroll_attributes->minimum;
                else if (scroll_attributes->position > scroll_attributes->maximum)
                    scroll_attributes->position = scroll_attributes->maximum;
                d_call(self, m_emitter_raise, v_uiable_signals[e_uiable_signal_changed]);
            }
    }
    return result;
}

d_define_method_override(scroll, draw)(struct s_object *self, struct s_object *environment) {
    d_using(scroll);
    struct s_drawable_attributes *drawable_attributes_self = d_cast(self, drawable),
                                 *drawable_attributes_image;
    struct s_environment_attributes *environment_attributes = d_cast(environment, environment);
    double position_x_self, position_y_self, dimension_w_self, dimension_h_self, normalized_dimension_w_self, normalized_dimension_h_self, center_x_self,
           center_y_self, dimension_w_image, dimension_h_image, normalized_dimension_w_image, normalized_dimension_h_image, position_x, position_y,
           center_x, center_y, size_ratio;
    int result = (intptr_t)d_call_owner(self, uiable, m_drawable_draw, environment); /* recall the father's draw method */
    if (scroll_attributes->image) {
        drawable_attributes_image = d_cast(scroll_attributes->image, drawable);
        d_call(&(drawable_attributes_self->point_normalized_destination), m_point_get, &position_x_self, &position_y_self);
        d_call(&(drawable_attributes_self->point_dimension), m_point_get, &dimension_w_self, &dimension_h_self);
        d_call(&(drawable_attributes_self->point_normalized_dimension), m_point_get, &normalized_dimension_w_self, &normalized_dimension_h_self);
        d_call(&(drawable_attributes_self->point_normalized_center), m_point_get, &center_x_self, &center_y_self);
        d_call(&(drawable_attributes_image->point_dimension), m_point_get, &dimension_w_image, &dimension_h_image);
        d_call(&(drawable_attributes_image->point_normalized_dimension), m_point_get, &normalized_dimension_w_image, &normalized_dimension_h_image);
        size_ratio = (normalized_dimension_h_self/dimension_h_self);
        position_x = position_x_self - ((normalized_dimension_w_image - normalized_dimension_w_self)/2.0);
        position_y = position_y_self + (((double)(scroll_attributes->position - scroll_attributes->minimum)/
                    (double)(scroll_attributes->maximum - scroll_attributes->minimum)) *
                d_math_max((normalized_dimension_h_self - normalized_dimension_h_image), 0));
        center_x = (position_x_self + center_x_self) - position_x;
        center_y = (position_y_self + center_y_self) - position_y;
        d_call(scroll_attributes->image, m_drawable_set_position, position_x, position_y);
        d_call(scroll_attributes->image, m_drawable_set_center, center_x, center_y);
        drawable_attributes_image->angle = drawable_attributes_self->angle;
        drawable_attributes_image->flip = drawable_attributes_self->flip;
        if ((d_call(scroll_attributes->image, m_drawable_keep_scale,
                        environment_attributes->current_w,
                        environment_attributes->current_h))) {
            d_call(&(drawable_attributes_image->point_normalized_dimension), m_point_set_x, (double)(dimension_w_image * size_ratio));
            d_call(&(drawable_attributes_image->point_normalized_dimension), m_point_set_y, (double)(dimension_h_image * size_ratio));
            while (((int)d_call(scroll_attributes->image, m_drawable_draw, environment)) == d_drawable_return_continue);
        }
    }
    d_cast_return(result);
}

d_define_method_override(scroll, set_maskRGB)(struct s_object *self, unsigned int red, unsigned int green, unsigned int blue) {
    d_using(scroll);
    d_call_owner(self, uiable, m_drawable_set_maskRGB, red, green ,blue);
    if (scroll_attributes->image)
        d_call(scroll_attributes->image, m_drawable_set_maskRGB, red, green, blue);
    return self;
}

d_define_method_override(scroll, set_maskA)(struct s_object *self, unsigned int alpha) {
    d_using(scroll);
    d_call_owner(self, uiable, m_drawable_set_maskA, alpha);
    if (scroll_attributes->image)
        d_call(scroll_attributes->image, m_drawable_set_maskA, alpha);
    return self;
}

d_define_method_override(scroll, set_blend)(struct s_object *self, enum e_drawable_blends blend) {
    d_using(scroll);
    d_call_owner(self, uiable, m_drawable_set_blend, blend);
    if (scroll_attributes->image)
        d_call(scroll_attributes->image, m_drawable_set_blend, blend);
    return self;
}


d_define_method(scroll, delete)(struct s_object *self, struct s_scroll_attributes *attributes) {
    if (attributes->image)
        d_delete(attributes->image);
    return NULL;
}

d_define_class(scroll) {
    d_hook_method(scroll, e_flag_public, set_range),
        d_hook_method(scroll, e_flag_public, set_modifier),
        d_hook_method(scroll, e_flag_public, set_position),
        d_hook_method(scroll, e_flag_public, get_position),
        d_hook_method_override(scroll, e_flag_public, eventable, event),
        d_hook_method_override(scroll, e_flag_public, drawable, draw),
        d_hook_method_override(scroll, e_flag_public, drawable, set_maskRGB),
        d_hook_method_override(scroll, e_flag_public, drawable, set_maskA),
        d_hook_method_override(scroll, e_flag_public, drawable, set_blend),
        d_hook_delete(scroll),
        d_hook_method_tail
};
