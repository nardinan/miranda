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
#include "container.obj.h"
struct s_container_attributes *p_container_alloc(struct s_object *self) {
    struct s_container_attributes *result = d_prepare(self, container);
    f_mutex_new(self);	/* inherit */
    f_memory_new(self);	/* inherit */
    f_uiable_new(self);	/* inherit */
    return result;
}

struct s_object *f_container_new(struct s_object *self, double border_top, double border_bottom, double border_left, double border_right,
        t_boolean floatable) {
    struct s_container_attributes *attributes = p_container_alloc(self);
    attributes->border_top = border_top;
    attributes->border_bottom = border_bottom;
    attributes->border_left = border_left;
    attributes->border_right = border_right;
    attributes->floatable = floatable;
    return self;
}

d_define_method(container, add_uiable)(struct s_object *self, struct s_object *uiable, double position_x, double position_y) {
    d_using(container);
    struct s_container_uiable *current_container;
    if ((current_container = (struct s_container_uiable *) d_malloc(sizeof(struct s_container_uiable)))) {
        current_container->uiable = d_retain(uiable);
        current_container->position_x = position_x;
        current_container->position_y = position_y;
        f_list_append(&(container_attributes->entries), (struct s_list_node *)current_container, e_list_insert_tail);
    } else
        d_die(d_error_malloc);
    return self;
}

d_define_method(container, del_uiable)(struct s_object *self, struct s_object *uiable) {
    d_using(container);
    struct s_container_uiable *current_container;
    d_foreach(&(container_attributes->entries), current_container, struct s_container_uiable)
        if (current_container->uiable == uiable) {
            f_list_delete(&(container_attributes->entries), (struct s_list_node *)current_container);
            d_delete(current_container->uiable);
            d_free(current_container);
            break;
        }
    return self;
}

d_define_method_override(container, event)(struct s_object *self, struct s_object *environment, SDL_Event *current_event) {
    d_using(container);
    struct s_uiable_attributes *uiable_attributes_self = d_cast(self, uiable),
                               *uiable_attributes_current;
    struct s_container_uiable *current_container;
    struct s_object *result = d_call_owner(self, uiable, m_eventable_event, environment, current_event);
    struct s_exception *exception;
    d_try {
        d_foreach(&(container_attributes->entries), current_container, struct s_container_uiable)
            if ((uiable_attributes_current = d_cast(current_container->uiable, uiable)))
                d_call(current_container->uiable, m_eventable_event, environment, current_event);
        if (container_attributes->floatable)
            switch (current_event->type) {
                case SDL_MOUSEBUTTONUP:
                    if (current_event->button.button == SDL_BUTTON_LEFT)
                        container_attributes->grabbed = d_false;
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    if ((current_event->button.button == SDL_BUTTON_LEFT) &&
                            (uiable_attributes_self->selected_mode == e_uiable_mode_selected)) {
                        container_attributes->grabbed = d_true;
                        container_attributes->offset_x = NAN;
                        container_attributes->offset_y = NAN;
                    }
            }
    } d_catch(exception) {
        d_exception_dump(stderr, exception);
        d_raise;
    } d_endtry;
    return result;
}

d_define_method_override(container, draw)(struct s_object *self, struct s_object *environment) {
    d_using(container);
    struct s_drawable_attributes *drawable_attributes_self = d_cast(self, drawable),
                                 *drawable_attributes_entry;
    struct s_environment_attributes *environment_attributes = d_cast(environment, environment);
    struct s_container_uiable *current_container;
    double position_x_self, position_y_self, normalized_position_x_self, normalized_position_y_self, position_x_entry, position_y_entry,
           normalized_position_x_entry, normalized_position_y_entry, position_x, position_y, center_x_self, center_y_self, center_x_entry, center_y_entry,
           normalized_dimension_w_entry, normalized_dimension_h_entry, max_w = container_attributes->border_left + container_attributes->border_right,
           max_h = container_attributes->border_top + container_attributes->border_bottom, current_w, current_h;
    int mouse_x, mouse_y, result = (intptr_t)d_call_owner(self, uiable, m_drawable_draw, environment); /* recall the father's draw method */
    d_call(&(drawable_attributes_self->point_destination), m_point_get, &position_x_self, &position_y_self);
    d_call(&(drawable_attributes_self->point_normalized_destination), m_point_get, &normalized_position_x_self, &normalized_position_y_self);
    d_call(&(drawable_attributes_self->point_center), m_point_get, &center_x_self, &center_y_self);
    d_foreach(&(container_attributes->entries), current_container, struct s_container_uiable) {
        drawable_attributes_entry = d_cast(current_container->uiable, drawable);
        position_x_entry = position_x_self + current_container->position_x + container_attributes->border_left;
        position_y_entry = position_y_self + current_container->position_y + container_attributes->border_top;
        center_x_entry = (position_x_self + center_x_self) - position_x_entry;
        center_y_entry = (position_y_self + center_y_self) - position_y_entry;
        d_call(current_container->uiable, m_drawable_set_position, position_x_entry, position_y_entry);
        if ((drawable_attributes_entry->flags&e_drawable_kind_ui_no_attribute_angle) != e_drawable_kind_ui_no_attribute_angle)
            drawable_attributes_entry->angle = drawable_attributes_self->angle;
        if ((drawable_attributes_entry->flags&e_drawable_kind_ui_no_attribute_zoom) != e_drawable_kind_ui_no_attribute_zoom) {
            d_call(current_container->uiable, m_drawable_set_center, center_x_entry, center_y_entry);
            drawable_attributes_entry->zoom = drawable_attributes_self->zoom;
        }
        if ((drawable_attributes_entry->flags&e_drawable_kind_ui_no_attribute_flip) != e_drawable_kind_ui_no_attribute_flip)
            drawable_attributes_entry->flip = drawable_attributes_self->flip;
        if (((intptr_t)d_call(current_container->uiable, m_drawable_normalize_scale,
                        environment_attributes->reference_w[environment_attributes->current_surface],
                        environment_attributes->reference_h[environment_attributes->current_surface],
                        environment_attributes->camera_origin_x[environment_attributes->current_surface],
                        environment_attributes->camera_origin_y[environment_attributes->current_surface],
                        environment_attributes->camera_focus_x[environment_attributes->current_surface],
                        environment_attributes->camera_focus_y[environment_attributes->current_surface],
                        environment_attributes->current_w,
                        environment_attributes->current_h,
                        environment_attributes->zoom[environment_attributes->current_surface]))) {
            d_call(&(drawable_attributes_entry->point_normalized_destination), m_point_get, &normalized_position_x_entry,
                    &normalized_position_y_entry);
            d_call(&(drawable_attributes_entry->point_normalized_dimension), m_point_get, &normalized_dimension_w_entry,
                    &normalized_dimension_h_entry);
            if ((current_w = ((normalized_position_x_entry + normalized_dimension_w_entry) - normalized_position_x_self +
                            container_attributes->border_right)) > max_w)
                max_w = current_w;
            if ((current_h = ((normalized_position_y_entry + normalized_dimension_h_entry) - normalized_position_y_self +
                            container_attributes->border_bottom)) > max_h)
                max_h = current_h;
            while(((int)d_call(current_container->uiable, m_drawable_draw, environment)) == d_drawable_return_continue);
        }
    }
    d_call(self, m_drawable_set_dimension, max_w, max_h);
    if ((container_attributes->floatable) && (container_attributes->grabbed)) {
        SDL_GetMouseState(&mouse_x, &mouse_y);
        mouse_x = ((double)mouse_x * environment_attributes->reference_w[environment_attributes->current_surface])/environment_attributes->current_w;
        mouse_y = ((double)mouse_y * environment_attributes->reference_h[environment_attributes->current_surface])/environment_attributes->current_h;
        if ((container_attributes->offset_x != container_attributes->offset_x) && (container_attributes->offset_y != container_attributes->offset_y)) {
            container_attributes->offset_x = (mouse_x - position_x_self);
            container_attributes->offset_y = (mouse_y - position_y_self);
        } else {
            position_x = (double)mouse_x - container_attributes->offset_x;
            position_y = (double)mouse_y - container_attributes->offset_y;
            d_call(self, m_drawable_set_position, position_x, position_y);
        }
    }
    d_cast_return(result);
}


d_define_method(container, delete)(struct s_object *self, struct s_container_attributes *attributes) {
    struct s_container_uiable *current_container;
    while ((current_container = (struct s_container_uiable *)attributes->entries.head)) {
        f_list_delete(&(attributes->entries), (struct s_list_node *)current_container);
        d_delete(current_container->uiable);
        d_free(current_container);
    }
    return NULL;
}

d_define_class(container) {
    d_hook_method(container, e_flag_public, add_uiable),
        d_hook_method(container, e_flag_public, del_uiable),
        d_hook_method_override(container, e_flag_public, eventable, event),
        d_hook_method_override(container, e_flag_public, drawable, draw),
        d_hook_delete(container),
        d_hook_method_tail
};
