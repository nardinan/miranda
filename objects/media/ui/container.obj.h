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
#ifndef miranda_media_container_h
#define miranda_media_container_h
#include "label.obj.h"
typedef struct s_container_uiable { d_list_node_head;
	struct s_object *uiable;
	double position_x, position_y;
	t_boolean remove;
} s_container_uiable;
d_declare_class(container) {
	struct s_attributes head;
	struct s_list entries;
	double border_top, border_bottom, border_left, border_right, offset_x, offset_y;
	t_boolean floatable, grabbed;
} d_declare_class_tail(container);
struct s_container_attributes *p_container_alloc(struct s_object *self);
extern struct s_object *f_container_new(struct s_object *self, double border_top, double border_bottom, double border_left, double border_right,
		t_boolean floatable);
d_declare_method(container, add_uiable)(struct s_object *self, struct s_object *uiable, double position_x, double position_y);
d_declare_method(container, del_uiable)(struct s_object *self, struct s_object *uiable);
d_declare_method(container, event)(struct s_object *self, struct s_object *environment, SDL_Event *current_event);
d_declare_method(container, draw)(struct s_object *self, struct s_object *environment);
d_declare_method(container, delete)(struct s_object *self, struct s_container_attributes *attributes);
#endif
