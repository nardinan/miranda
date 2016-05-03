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
#include "checkbox.obj.h"
#include "../eventable.obj.h"
struct s_checkbox_attributes *p_checkbox_alloc(struct s_object *self, char *string_content, TTF_Font *font, struct s_object *environment) {
    struct s_checkbox_attributes *result = d_prepare(self, checkbox);
    f_label_new_alignment(self, string_content, font, d_checkbox_background, d_checkbox_alignment, d_checkbox_alignment, environment); /* inherit */
    return result;
}

struct s_object *f_checkbox_new(struct s_object *self, char *string_content, TTF_Font *font, struct s_object *environment) {
    struct s_checkbox_attributes *attributes = p_checkbox_alloc(self, string_content, font, environment);
    attributes = attributes;
    return self;
}

d_define_method(checkbox, set_drawable)(struct s_object *self, struct s_object *checked, struct s_object *unchecked) {
    d_using(checkbox);
    checkbox_attributes->checked = d_retain(checked);
    checkbox_attributes->unchecked = d_retain(unchecked);
    return self;
}

d_define_method(checkbox, set_checked)(struct s_object *self, t_boolean is_checked) {
    d_using(checkbox);
    checkbox_attributes->is_checked = is_checked;
    return self;
}

d_define_method(checkbox, get_checked)(struct s_object *self) {
    d_using(checkbox);
    d_cast_return(checkbox_attributes->is_checked);
}

d_define_method_override(checkbox, event)(struct s_object *self, struct s_object *environment, SDL_Event *current_event) {
    d_using(checkbox);
    struct s_uiable_attributes *uiable_attributes = d_cast(self, uiable);
    struct s_object *result = d_call_owner(self, uiable, m_eventable_event, environment, current_event);
    if (uiable_attributes->selected_mode != e_uiable_mode_idle)
        if ((current_event->type == SDL_MOUSEBUTTONUP) && (current_event->button.button == SDL_BUTTON_LEFT)) {
            if (uiable_attributes->selected_mode == e_uiable_mode_selected)
                checkbox_attributes->is_checked = !checkbox_attributes->is_checked;
            d_call(self, m_uiable_mode, e_uiable_mode_active);
        }
    return result;
}

d_define_method_override(checkbox, draw)(struct s_object *self, struct s_object *environment) {
    d_using(checkbox);
    struct s_uiable_attributes *uiable_attributes = d_cast(self, uiable);
    struct s_environment_attributes *environment_attributes = d_cast(environment, environment);
    struct s_drawable_attributes *drawable_attributes_self = d_cast(self, drawable),
                                 *drawable_attributes_selected;
    struct s_object *selected_component = NULL;
    double position_x, position_y, new_position_x, new_position_y, center_x, center_y, new_center_x, new_center_y, dimension_w_self, dimension_h_self,
           dimension_w_selected, dimension_h_selected, new_dimension_w, new_dimension_h;
    int result = (intptr_t)d_call_owner(self, label, m_drawable_draw, environment); /* recall the father's draw method */
    d_call(&(drawable_attributes_self->point_destination), m_point_get, &position_x, &position_y);
    d_call(&(drawable_attributes_self->point_dimension), m_point_get, &dimension_w_self, &dimension_h_self);
    d_call(&(drawable_attributes_self->point_center), m_point_get, &center_x, &center_y);
    if ((checkbox_attributes->is_checked) && (checkbox_attributes->checked))
        selected_component = checkbox_attributes->checked;
    else if ((!checkbox_attributes->is_checked) && (checkbox_attributes->unchecked))
        selected_component = checkbox_attributes->unchecked;
    if (selected_component) {
        drawable_attributes_selected = d_cast(selected_component, drawable);
        d_call(&(drawable_attributes_selected->point_dimension), m_point_get, &dimension_w_selected, &dimension_h_selected);
        new_dimension_h = dimension_h_self - (uiable_attributes->border_h * 2.0);
        new_dimension_w = (dimension_w_selected * new_dimension_h)/dimension_h_selected;
        new_position_x = position_x + dimension_w_self - new_dimension_w - uiable_attributes->border_w;
        new_position_y = position_y + ((dimension_h_self - new_dimension_h) / 2.0);
        new_center_x = (position_x + center_x) - new_position_x;
        new_center_y = (position_y + center_y) - new_position_y;
        d_call(selected_component, m_drawable_set_position, new_position_x, new_position_y);
        d_call(selected_component, m_drawable_set_center, new_center_x, new_center_y);
        d_call(selected_component, m_drawable_set_dimension, new_dimension_w, new_dimension_h);
        drawable_attributes_selected->angle = drawable_attributes_self->angle;
        drawable_attributes_selected->zoom = drawable_attributes_self->zoom;
        drawable_attributes_selected->flip = drawable_attributes_self->flip;
        if ((d_call(selected_component, m_drawable_normalize_scale, environment_attributes->reference_w[environment_attributes->current_surface],
                        environment_attributes->reference_h[environment_attributes->current_surface],
                        environment_attributes->camera_origin_x[environment_attributes->current_surface],
                        environment_attributes->camera_origin_y[environment_attributes->current_surface],
                        environment_attributes->camera_focus_x[environment_attributes->current_surface],
                        environment_attributes->camera_focus_y[environment_attributes->current_surface],
                        environment_attributes->current_w,
                        environment_attributes->current_h,
                        environment_attributes->zoom[environment_attributes->current_surface])))
            while(((int)d_call(selected_component, m_drawable_draw, environment)) == d_drawable_return_continue);
    }
    d_cast_return(result);
}

d_define_method(checkbox, delete)(struct s_object *self, struct s_checkbox_attributes *attributes) {
    if (attributes->checked)
        d_delete(attributes->checked);
    if (attributes->unchecked)
        d_delete(attributes->unchecked);
    return NULL;
}

d_define_class(checkbox) {
    d_hook_method(checkbox, e_flag_public, set_drawable),
        d_hook_method(checkbox, e_flag_public, set_checked),
        d_hook_method(checkbox, e_flag_public, get_checked),
        d_hook_method_override(checkbox, e_flag_public, eventable, event),
        d_hook_method_override(checkbox, e_flag_public, drawable, draw),
        d_hook_delete(checkbox),
        d_hook_method_tail
};
