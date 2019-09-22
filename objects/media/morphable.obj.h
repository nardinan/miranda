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
#ifndef miranda_media_morphable_h
#define miranda_media_morphable_h
#include "eventable.obj.h"
#include "drawable.obj.h"
#define d_morphable_z_factor 0.1
d_declare_class(morphable) {
  struct s_attributes head;
  double offset_x, offset_y, offset_z;
  t_boolean grabbed, freedom_x, freedom_y, freedom_z, visible;
} d_declare_class_tail(morphable);
struct s_morphable_attributes *p_morphable_alloc(struct s_object *self);
extern struct s_object *f_morphable_new(struct s_object *self);
d_declare_method(morphable, set_freedom_x)(struct s_object *self, t_boolean free);
d_declare_method(morphable, set_freedom_y)(struct s_object *self, t_boolean free);
d_declare_method(morphable, set_freedom_z)(struct s_object *self, t_boolean free);
d_declare_method(morphable, set_visibility)(struct s_object *self, t_boolean visiblity);
d_declare_method(morphable, update)(struct s_object *self, struct s_object *environment);
d_declare_method(morphable, event)(struct s_object *self, struct s_object *environment, SDL_Event *current_event);
#endif
