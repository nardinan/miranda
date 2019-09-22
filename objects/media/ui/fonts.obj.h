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
#ifndef miranda_media_fonts_h
#define miranda_media_fonts_h
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include "../bitmap.obj.h"
#define d_fonts_collection 128
#define d_fonts_default 0
typedef struct s_fonts_pack {
  char *font_block;
  struct stat file_stats;
  TTF_Font *font;
} s_fonts_pack;
d_exception_declare(ttf);
d_declare_class(fonts) {
  struct s_attributes head;
  struct s_fonts_pack fonts[d_fonts_collection];
} d_declare_class_tail(fonts);
struct s_fonts_attributes *p_fonts_alloc(struct s_object *self);
extern struct s_object *f_fonts_new(struct s_object *self);
d_declare_method(fonts, add_font)(struct s_object *self, unsigned int id, struct s_object *stream, int size);
d_declare_method(fonts, get_font)(struct s_object *self, unsigned int id, int style);
d_declare_method(fonts, get_height)(struct s_object *self, unsigned int id);
d_declare_method(fonts, set_outline)(struct s_object *self, unsigned int id, int outline);
d_declare_method(fonts, delete)(struct s_object *self, struct s_fonts_attributes *attributes);
#endif
