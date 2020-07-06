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
#ifndef miranda_media_mirror_h
#define miranda_media_mirror_h
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include "drawable.obj.h"
#include "../io/stream.obj.h"
#include "../io/json.obj.h"
d_declare_class(mirror) {
  struct s_attributes head;
  struct s_object point_source, point_normalized_source;
  SDL_Texture *image;
} d_declare_class_tail(mirror);
struct s_mirror_attributes *p_mirror_alloc(struct s_object *self);
extern struct s_object *f_mirror_new(struct s_object *self, double source_x, double source_y, double source_w, double source_h);
d_declare_method(mirror, set_source)(struct s_object *self, double x, double y);
d_declare_method(mirror, set_source_x)(struct s_object *self, double x);
d_declare_method(mirror, set_source_y)(struct s_object *self, double y);
d_declare_method(mirror, update_texture)(struct s_object *self, struct s_object *environment);
d_declare_method(mirror, draw)(struct s_object *self, struct s_object *environment);
d_declare_method(mirror, set_maskRGB)(struct s_object *self, unsigned int red, unsigned int green, unsigned int blue);
d_declare_method(mirror, set_maskA)(struct s_object *self, unsigned int alpha);
d_declare_method(mirror, set_blend)(struct s_object *self, enum e_drawable_blends blend);
d_declare_method(mirror, delete)(struct s_object *self, struct s_mirror_attributes *attributes);
#endif
