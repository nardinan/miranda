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
#include "eventable.obj.h"
struct s_eventable_attributes *p_eventable_alloc(struct s_object *self) {
    struct s_eventable_attributes *result = d_prepare(self, eventable);
    /* abstract (no memory inheritance) */
    return result;
}

struct s_object *f_eventable_new(struct s_object *self) {
    struct s_eventable_attributes *attributes = p_eventable_alloc(self);
    attributes->enable = d_true;
    return self;
}

d_define_method(eventable, set_enable)(struct s_object *self, t_boolean enable) {
    d_using(eventable);
    eventable_attributes->enable = enable;
    return self;
}

d_define_method(eventable, event)(struct s_object *self, struct s_object *environment, SDL_Event *current_event) {
    d_war(e_log_level_ever, "'event' method has not been implemented yet");
    return self;
}

d_define_class(eventable) {
    d_hook_method(eventable, e_flag_public, set_enable),
        d_hook_method(eventable, e_flag_public, event),
        d_hook_method_tail
};
