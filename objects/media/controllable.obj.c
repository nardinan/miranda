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
#include "controllable.obj.h"
struct s_controllable_attributes *p_controllable_alloc(struct s_object *self) {
    struct s_controllable_attributes *result = d_prepare(self, controllable);
    /* abstract (no memory inheritance) */
    f_eventable_new(self);  /* inherit */
    return result;
}

struct s_object *f_controllable_new(struct s_object *self) {
    struct s_controllable_attributes *attributes = p_controllable_alloc(self);
    attributes = attributes;
    return self;
}

d_define_method(controllable, add_configuration)(struct s_object *self, int key, t_controllable_action action_pressed, t_controllable_action action_released) {
    d_using(controllable);
    struct s_controllable_entry *current_entry;
    if (!(current_entry = (struct s_controllable_entry *)d_call(self, m_controllable_get_configuration, key))) {
        if ((current_entry = (struct s_controllable_entry *)d_malloc(sizeof(struct s_controllable_entry)))) {
            current_entry->key = key;
            f_list_append(&(controllable_attributes->configurations), (struct s_list_node *)current_entry, e_list_insert_head);
        } else
            d_die(d_error_malloc);
    }
    current_entry->enabled = d_true;
    current_entry->action_pressed = action_pressed;
    current_entry->action_released = action_released;
    return self;
}

d_define_method(controllable, get_configuration)(struct s_object *self, int key) {
    d_using(controllable);
    struct s_controllable_entry *current_entry = NULL;
    d_foreach(&(controllable_attributes->configurations), current_entry, struct s_controllable_entry)
        if (current_entry->key == key)
            break;
    d_cast_return(current_entry);
}

d_define_method(controllable, del_configuration)(struct s_object *self, int key) {
    struct s_controllable_entry *current_entry;
    if ((current_entry = d_call(self, m_controllable_get_configuration, key)))
        current_entry->enabled = d_false;
    return self;
}

d_define_method_override(controllable, event)(struct s_object *self, struct s_object *environment, SDL_Event *current_event) {
    d_using(controllable);
    struct s_controllable_entry *current_entry;
    struct s_exception *exception;
    if (controllable_attributes->enable) {
        d_try {
            d_foreach(&(controllable_attributes->configurations), current_entry, struct s_controllable_entry) {
                if (current_entry->enabled) {
                    switch (current_event->type) {
                        case SDL_KEYDOWN:
                            if (current_event->key.keysym.sym == current_entry->key)
                                current_entry->action_pressed(self, current_entry, d_true);
                            break;
                        case SDL_KEYUP:
                            if (current_event->key.keysym.sym == current_entry->key)
                                current_entry->action_released(self, current_entry, d_false);
                    }
                }
            }        
        } d_catch(exception) {
            d_exception_dump(stderr, exception);
            d_raise;
        } d_endtry;
    }
    return self;
}

d_define_method(controllable, delete)(struct s_object *self, struct s_controllable_attributes *attributes) {
    struct s_controllable_entry *current_entry;
    while ((current_entry = (struct s_controllable_entry *)attributes->configurations.head)) {
        f_list_delete(&(attributes->configurations), (struct s_list_node *)current_entry);
        d_free(current_entry);
    }
    return NULL;
}

d_define_class(controllable) {
    d_hook_method(controllable, e_flag_public, add_configuration),
        d_hook_method(controllable, e_flag_private, get_configuration),
        d_hook_method(controllable, e_flag_public, del_configuration),
        d_hook_method_override(controllable, e_flag_public, eventable, event),
        d_hook_delete(controllable),
        d_hook_method_tail
};
