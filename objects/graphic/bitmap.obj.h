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
#ifndef miranda_graphic_bitmap_h
#define miranda_graphic_bitmap_h
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <SDL2/SDL_image.h>
#include "environment.obj.h"
#include "../io/stream.obj.h"
d_exception_declare(wrong_type);
d_exception_declare(texture);
d_declare_class(bitmap) {
	struct s_attributes head;
	SDL_Texture *image;
} d_declare_class_tail(bitmap);
struct s_bitmap_attributes *p_bitmap_alloc(struct s_object *self);
extern struct s_object *f_bitmap_new(struct s_object  *self, struct s_object *stream, struct s_object *environment);
d_declare_method(bitmap, draw)(struct s_object *self, struct s_object *environment);
d_declare_method(bitmap, delete)(struct s_object *self, struct s_bitmap_attributes *attributes);
#endif
