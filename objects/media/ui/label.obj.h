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
#ifndef miranda_media_label_h
#define miranda_media_label_h
#include "fonts.obj.h"
#include "uiable.obj.h"
typedef enum e_label_background_formats {
  e_label_background_format_adaptable = 0,
  e_label_background_format_fixed
} e_label_background_formats;
typedef enum e_label_alignments {
  e_label_alignment_left = 0,
  e_label_alignment_top,
  e_label_alignment_center,
  e_label_alignment_right,
  e_label_alignment_bottom
} e_label_alignments;
d_declare_class(label) {
  struct s_attributes head;
  SDL_Texture *image;
  TTF_Font *last_font;
  SDL_Rect last_source, last_destination;
  char *string_content;
  size_t size;
  enum e_label_background_formats format;
  enum e_label_alignments alignment_x, alignment_y;
  enum e_drawable_blends last_blend;
  double last_mask_R, last_mask_G, last_mask_B, last_mask_A, last_width, last_height, string_width, string_height;
} d_declare_class_tail(label);
struct s_label_attributes *p_label_alloc(struct s_object *self);
extern struct s_object *f_label_new(struct s_object *self, char *string_content, TTF_Font *font, struct s_object *environment);
extern struct s_object *f_label_new_alignment(struct s_object *self, char *string_content, TTF_Font *font, enum e_label_background_formats format,
  enum e_label_alignments alignment_x,enum e_label_alignments alignment_y, struct s_object *environment);
d_declare_method(label, set_content_string)(struct s_object *self, struct s_object *string_content, TTF_Font *font, struct s_object *environment);
d_declare_method(label, set_content_char)(struct s_object *self, char *string_content, TTF_Font *font, struct s_object *environment);
d_declare_method(label, get_content_char)(struct s_object *self);
d_declare_method(label, set_alignment)(struct s_object *self, enum e_label_alignments alignment_x, enum e_label_alignments alignment_y);
d_declare_method(label, update_texture)(struct s_object *self, TTF_Font *font, struct s_object *environment);
d_declare_method(label, set_dimension)(struct s_object *self, double w, double h);
d_declare_method(label, set_dimension_w)(struct s_object *self, double w);
d_declare_method(label, set_dimension_h)(struct s_object *self, double h);
d_declare_method(label, draw)(struct s_object *self, struct s_object *environment);
d_declare_method(label, set_maskRGB)(struct s_object *self, unsigned int red, unsigned int green, unsigned int blue);
d_declare_method(label, set_maskA)(struct s_object *self, unsigned int alpha);
d_declare_method(label, set_blend)(struct s_object *self, enum e_drawable_blends blend);
d_declare_method(label, delete)(struct s_object *self, struct s_label_attributes *attributes);
#endif
