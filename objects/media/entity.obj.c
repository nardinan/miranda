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
#include "entity.obj.h"
struct s_entity_attributes *p_entity_alloc(struct s_object *self) {
    struct s_entity_attributes *result = d_prepare(self, entity);
    /* abstract (no memory inheritance) */
    f_drawable_new(self, (e_drawable_kind_single|e_drawable_kind_force_visibility));    /* inherit */
    return result;
}

struct s_object *f_entity_new(struct s_object *self, const char *key) {
    struct s_entity_attributes *attributes = p_entity_alloc(self);
    attributes = attributes;
    return self;
}

d_define_method(entity, get_component)(struct s_object *self, char *label) {
    d_using(entity);
    struct s_entity_component *current_component = NULL;
    d_foreach(&(entity_attributes->components), current_component, struct s_entity_component)
        if (f_string_strcmp(current_component->label, label) == 0)
            break;
    d_cast_return(current_component);
}


d_define_method(entity, add_component)(struct s_object *self, char *label) {
    d_using(entity);
    struct s_entity_component *current_component = NULL;
    if (!(current_component = (struct s_entity_component *)d_call(self, m_entity_get_component, label))) {
        if ((current_component = (struct s_entity_component *)d_malloc(sizeof(struct s_entity_component)))) {
            strncpy(current_component->label, label, d_entity_label_size);
            f_list_append(&(entity_attributes->components), (struct s_list_node *)current_component, e_list_insert_head);
        } else
            d_die(d_error_malloc)
    }
    return self;
}

d_define_method(entity, add_element)(struct s_object *self, char *label, double offset_x, double offset_y, struct s_object *drawable) {
    struct s_entity_component *current_component = NULL;
    struct s_entity_element *current_element = NULL;
    if ((current_component = (struct s_entity_component *)d_call(self, m_entity_get_component, label))) {
        if ((current_element = (struct s_entity_element *)d_malloc(sizeof (struct s_entity_element)))) {
            current_element->offset_x = offset_x;
            current_element->offset_y = offset_y;
            current_element->drawable = d_retain(drawable);
            f_list_append(&(current_component->elements), (struct s_list_node *)current_element, e_list_insert_head);
        } else
            d_die(d_error_malloc);
    }
    return self;
}

d_define_method(entity, set_component)(struct s_object *self, char *label) {
    d_using(entity);
    struct s_entity_component *current_component = NULL;
    if ((current_component = (struct s_entity_component *)d_call(self, m_entity_get_component, label)))
        entity_attributes->current_component = current_component;
    return self;
}

d_define_method(entity, collision)(struct s_object *self, struct s_object *entity) {
    struct s_drawable_attributes *drawable_attributes_self = d_cast(self, drawable),
                                 *drawable_attributes_core = d_cast(entity, drawable);
    t_boolean collision = (intptr_t)d_call(&(drawable_attributes_self->square_collision_box), m_square_collision, 
            &(drawable_attributes_core->square_collision_box));
    d_cast_return(collision);
}

d_define_method_override(entity, draw)(struct s_object *self, struct s_object *environment) {
    d_using(entity);
    struct s_drawable_attributes *drawable_attributes_self = d_cast(self, drawable),
                                 *drawable_attributes_core;
    struct s_environment_attributes *environment_attributes = d_cast(environment, environment);
    double local_position_x, local_position_y, local_center_x, local_center_y, local_dimension_w, local_dimension_h, position_x, position_y, center_x, 
           center_y, dimension_w = 0.0, dimension_h = 0.0, final_dimension_w = 0.0, final_dimension_h = 0.0;
    struct s_entity_element *current_element;
    struct s_exception *exception;
    if (entity_attributes->current_component) {
        d_call(&(drawable_attributes_self->point_destination), m_point_get, &local_position_x, &local_position_y);
        d_try {
            d_foreach(&(entity_attributes->current_component->elements), current_element, struct s_entity_element)
                if ((drawable_attributes_core = d_cast(current_element->drawable, drawable))) {
                    d_call(current_element->drawable, m_drawable_get_dimension, &dimension_w, &dimension_h);
                    if ((local_dimension_w = (dimension_w + current_element->offset_x)) > final_dimension_w)
                        final_dimension_w = local_dimension_w;
                    if ((local_dimension_h = (dimension_h + current_element->offset_y)) > final_dimension_h)
                        final_dimension_h = local_dimension_h;
                }
            local_center_x = (final_dimension_w/2.0);
            local_center_y = (final_dimension_h/2.0);
            d_call(self, m_drawable_set_dimension, final_dimension_w, final_dimension_h);
            d_call(self, m_drawable_set_center, local_center_x, local_center_y);
            d_foreach(&(entity_attributes->current_component->elements), current_element, struct s_entity_element)
                if ((drawable_attributes_core = d_cast(current_element->drawable, drawable))) {
                    position_x = local_position_x + current_element->offset_x;
                    position_y = local_position_y + current_element->offset_y;
                    center_x = local_center_x - current_element->offset_x;
                    center_y = local_center_y - current_element->offset_y;
                    d_call(current_element->drawable, m_drawable_set_position, position_x, position_y);
                    d_call(current_element->drawable, m_drawable_set_center, center_x, center_y);
                    drawable_attributes_core->zoom = drawable_attributes_self->zoom;
                    drawable_attributes_core->angle = drawable_attributes_self->angle;
                    if ((d_call(current_element->drawable, m_drawable_normalize_scale, 
                                    environment_attributes->reference_w[environment_attributes->current_surface],
                                    environment_attributes->reference_h[environment_attributes->current_surface],
                                    environment_attributes->camera_origin_x[environment_attributes->current_surface],
                                    environment_attributes->camera_origin_y[environment_attributes->current_surface],
                                    environment_attributes->camera_focus_x[environment_attributes->current_surface],
                                    environment_attributes->camera_focus_y[environment_attributes->current_surface],
                                    environment_attributes->current_w,
                                    environment_attributes->current_h,
                                    environment_attributes->zoom[environment_attributes->current_surface])))
                        while (((int)d_call(current_element->drawable, m_drawable_draw, environment)) == d_drawable_return_continue);
                }
        } d_catch(exception) {
            d_exception_dump(stderr, exception);
            d_raise;
        } d_endtry;
    }
    if ((drawable_attributes_self->flags&e_drawable_kind_contour) == e_drawable_kind_contour)
        d_call(self, m_drawable_draw_contour, environment);
    d_cast_return(d_drawable_return_last);
}

d_define_method(entity, delete)(struct s_object *self, struct s_entity_attributes *attributes) {
    struct s_entity_component *current_component;
    struct s_entity_element *current_element;
    while ((current_component = (struct s_entity_component *)attributes->components.head)) {
        f_list_delete(&(attributes->components), (struct s_list_node *)current_component);
        while ((current_element = (struct s_entity_element *)current_component->elements.head)) {
            f_list_delete(&(current_component->elements), (struct s_list_node *)current_element);
            d_delete(current_element->drawable);
            d_free(current_element);
        }
        d_free(current_component);
    }
    return NULL;
}

d_define_class(entity) {
    d_hook_method(entity, e_flag_public, add_component),
        d_hook_method(entity, e_flag_private, get_component),
        d_hook_method(entity, e_flag_public, add_element),
        d_hook_method(entity, e_flag_public, set_component),
        d_hook_method_override(entity, e_flag_public, drawable, draw),
        d_hook_delete(entity),
        d_hook_method_tail
};
