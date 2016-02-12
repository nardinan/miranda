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
#ifndef miranda_media_field_h
#define miranda_media_field_h
#include "label.obj.h"
#define d_field_bucket 32
#define d_field_default_line_size 1
#define d_field_default_R 0
#define d_field_default_G 0
#define d_field_default_B 0
#define d_field_default_A 255.0
d_declare_class(field) {
	struct s_attributes head;
	size_t pointer, max_size;
	double last_cursor_R, last_cursor_G, last_cursor_B, last_cursor_A;
} d_declare_class_tail(field);
struct s_field_attributes *p_field_alloc(struct s_object *self, char *string_content, TTF_Font *font, enum e_label_background_formats format,
		enum e_label_alignments alignment_x, enum e_label_alignments alignment_y, struct s_object *environment);
extern struct s_object *f_field_new(struct s_object *self, char *string_content, TTF_Font *font, struct s_object *environment);
extern struct s_object *f_field_new_alignment(struct s_object *self, char *string_content, TTF_Font *font, enum e_label_background_formats format,
		enum e_label_alignments alignment_x, enum e_label_alignments alignment_y, struct s_object *environment);
d_declare_method(field, set_cursor)(struct s_object *self, double cursor_R, double cursor_G, double cursor_B, double cursor_A);
d_declare_method(field, set_size)(struct s_object *self, size_t max_size);
d_declare_method(field, event)(struct s_object *self, struct s_object *environment, SDL_Event *current_event);
d_declare_method(field, draw)(struct s_object *self, struct s_object *environment);
d_declare_method(field, delete)(struct s_object *self, struct s_field_attributes *attributes);
#endif
