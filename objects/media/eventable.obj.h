/*
 * miranda
 * Copyright (C) 2014-2020 Andrea Nardinocchi (andrea@nardinan.it)
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
#ifndef miranda_media_eventable_h
#define miranda_media_eventable_h
#include <SDL_events.h>
#include "../memory.obj.h"
#include "../mutex.obj.h"
typedef enum e_eventable_statuses {
  e_eventable_status_ignored,
  e_eventable_status_captured
} e_eventable_statuses;
d_declare_class(eventable) {
  struct s_attributes head;
  t_boolean enable;
} d_declare_class_tail(eventable);
struct s_eventable_attributes *p_eventable_alloc(struct s_object *self);
extern struct s_object *f_eventable_new(struct s_object *self);
d_declare_method(eventable, set_enable)(struct s_object *self, t_boolean enable);
d_declare_method(eventable, event)(struct s_object *self, struct s_object *environment, SDL_Event *current_event);
#endif
