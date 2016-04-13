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
#include "fonts.obj.h"
d_exception_define(ttf,	14, "ungenerable TTF exception");
struct s_fonts_attributes *p_fonts_alloc(struct s_object *self) {
	struct s_fonts_attributes *result = d_prepare(self, fonts);
	f_memory_new(self);	/* inherit */
	f_mutex_new(self);	/* inherit */
	return result;
}

struct s_object *f_fonts_new(struct s_object *self) {
	struct s_fonts_attributes *attributes = p_fonts_alloc(self);
	memset(attributes->fonts, 0, (sizeof(TTF_Font *)*d_fonts_collection));
	return self;
}

d_define_method(fonts, add_font)(struct s_object *self, unsigned int id, struct s_object *stream, int size) {
	d_using(fonts);
	struct s_stream_attributes *stream_attributes = d_cast(stream, stream);
	char buffer[d_string_buffer_size];
	SDL_RWops *font_block;
	if (fonts_attributes->fonts[id].font) {
		TTF_CloseFont(fonts_attributes->fonts[id].font);
		munmap(fonts_attributes->fonts[id].font_block, fonts_attributes->fonts[id].file_stats.st_size);
	}
	fstat(stream_attributes->descriptor, &(fonts_attributes->fonts[id].file_stats));
	if ((fonts_attributes->fonts[id].font_block = mmap(NULL, fonts_attributes->fonts[id].file_stats.st_size, PROT_READ, MAP_SHARED,
					stream_attributes->descriptor, 0)) != MAP_FAILED) {
		font_block = SDL_RWFromMem(fonts_attributes->fonts[id].font_block, fonts_attributes->fonts[id].file_stats.st_size);
		if (!(fonts_attributes->fonts[id].font = TTF_OpenFontRW(font_block, d_true, size))) {
			munmap(fonts_attributes->fonts[id].font_block, fonts_attributes->fonts[id].file_stats.st_size);
			snprintf(buffer, d_string_buffer_size, "ungenerable TTF for font %s exception",
					d_string_cstring(stream_attributes->string_name));
			d_throw(v_exception_ttf, buffer);
		}
	} else {
		snprintf(buffer, d_string_buffer_size, "wrong type for file %s exception", d_string_cstring(stream_attributes->string_name));
		d_throw(v_exception_wrong_type, buffer);
	}
	return self;
}

d_define_method(fonts, get_font)(struct s_object *self, unsigned int id, int style) {
	d_using(fonts);
	if (fonts_attributes->fonts[id].font)
		TTF_SetFontStyle(fonts_attributes->fonts[id].font, style);
	d_cast_return(fonts_attributes->fonts[id].font);
}

d_define_method(fonts, get_height)(struct s_object *self, unsigned int id) {
	int height = 0;
	TTF_Font *current_font;
	if ((current_font = d_call(self, m_fonts_get_font, id, TTF_STYLE_NORMAL)))
		height = TTF_FontHeight(current_font);
	d_cast_return(height);
}

d_define_method(fonts, delete)(struct s_object *self, struct s_fonts_attributes *attributes) {
	int index;
	for (index = 0; index < d_fonts_collection; ++index)
		if (attributes->fonts[index].font) {
			TTF_CloseFont(attributes->fonts[index].font);
			munmap(attributes->fonts[index].font_block, attributes->fonts[index].file_stats.st_size);
		}
	return NULL;
}

d_define_class(fonts) {
	d_hook_method(fonts, e_flag_public, add_font),
	d_hook_method(fonts, e_flag_public, get_font),
	d_hook_method(fonts, e_flag_public, get_height),
	d_hook_delete(fonts),
	d_hook_method_tail
};
