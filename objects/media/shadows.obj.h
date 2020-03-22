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
#ifndef miranda_media_shadows_h
#define miranda_media_shadows_h
#include "illuminable_bitmap.obj.h"
#include "../array.obj.h"
#define d_shadows_default_contour_color 255, 0, 0, 255
#define d_shadows_default_maximum_intensity 0.5
#define d_shadows_maximum_vertices 64
d_declare_class(shadows) {
  struct s_attributes head;
  struct s_object *array_casters;
  double maximum_intensity;
} d_declare_class_tail(shadows);
struct s_shadows_attributes *p_shadows_alloc(struct s_object *self);
extern struct s_object *f_shadows_new(struct s_object *self);
d_declare_method(shadows, add_caster)(struct s_object *self, struct s_object *illuminable_bitmap);
d_declare_method(shadows, clear)(struct s_object *self);
d_declare_method(shadows, draw)(struct s_object *self, struct s_object *environment);
d_declare_method(shadows, draw_contour)(struct s_object *self, struct s_object *environment);
d_declare_method(shadows, is_visible)(struct s_object *self, double current_w, double current_h);
d_declare_method(shadows, delete)(struct s_object *self, struct s_shadows_attributes *attributes);
#endif
