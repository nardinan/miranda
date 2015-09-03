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
#include "bitmap.obj.h"
d_exception_define(wrong_type,	12, "wrong file type exception");
d_exception_define(texture,	13, "ungenerable texture excepton");
struct s_bitmap_attributes *p_bitmap_alloc(struct s_object *self) {
	struct s_bitmap_attributes *result = d_prepare(self, bitmap);
	f_memory_new(self);				/* inherit */
	f_mutex_new(self);				/* inherit */
	f_drawable_new(self, e_drawable_kind_single);	/* inherit */
	return result;
}

struct s_object *f_bitmap_new(struct s_object *self, struct s_object *stream, struct s_object *environment) {
	struct s_bitmap_attributes *attributes = p_bitmap_alloc(self);
	struct s_drawable_attributes *drawable_attributes = d_cast(self, drawable);
	struct s_stream_attributes *stream_attributes = d_cast(stream, stream);
	struct s_environment_attributes *environment_attributes = d_cast(environment, environment);
	struct s_object *result = self;
	char *memblock, buffer[d_string_buffer_size];
	struct stat file_stats;
	int width, height;
	SDL_RWops *surfaced_block;
	SDL_Surface *unoptimized_surface;
	fstat(stream_attributes->descriptor, &file_stats);
	if ((memblock = mmap(NULL, file_stats.st_size, PROT_READ, MAP_SHARED, stream_attributes->descriptor, 0)) != MAP_FAILED) {
		surfaced_block = SDL_RWFromMem(memblock, file_stats.st_size);
		if ((unoptimized_surface = IMG_Load_RW(surfaced_block, d_true))) {
			attributes->image = SDL_CreateTextureFromSurface(environment_attributes->renderer, unoptimized_surface);
			if (SDL_QueryTexture(attributes->image, NULL, NULL, &width, &height) == 0) {
				d_call(&(drawable_attributes->point_dimension), m_point_set_x, (double)width);
				d_call(&(drawable_attributes->point_dimension), m_point_set_y, (double)height);
				d_call(&(drawable_attributes->point_center), m_point_set_x, (double)(width/2.0));
				d_call(&(drawable_attributes->point_center), m_point_set_y, (double)(width/2.0));
			} else {
				snprintf(buffer, d_string_buffer_size, "unable to retrieve informations for bitmap %s exception",
						d_string_cstring(stream_attributes->string_name));
				d_throw(v_exception_texture, buffer);
			}
		} else {
			snprintf(buffer, d_string_buffer_size, "ungenerable texture for bitmap %s exception",
					d_string_cstring(stream_attributes->string_name));
			d_throw(v_exception_texture, buffer);
		}
		SDL_FreeSurface(unoptimized_surface);
		munmap(memblock, file_stats.st_size);
	} else {
		snprintf(buffer, d_string_buffer_size, "wrong type for file %s exception", d_string_cstring(stream_attributes->string_name));
		d_throw(v_exception_wrong_type, buffer);
	}
	return result;
}

d_define_method_override(bitmap, draw)(struct s_object *self, struct s_object *environment) {
	d_using(bitmap);
	double position_x, position_y, dimension_w, dimension_h, center_x, center_y;
	struct s_drawable_attributes *drawable_attributes = d_cast(self, drawable);
	struct s_environment_attributes *environment_attributes = d_cast(environment, environment);
	SDL_Rect destination;
	SDL_Point center;
	d_call(&(drawable_attributes->point_normalized_destination), m_point_get, &position_x, &position_y);
	d_call(&(drawable_attributes->point_normalized_dimension), m_point_get, &dimension_w, &dimension_h);
	d_call(&(drawable_attributes->point_normalized_center), m_point_get, &center_x, &center_y);
	destination.x = position_x;
	destination.y = position_y;
	destination.w = dimension_w;
	destination.h = dimension_h;
	center.x = center_x;
	center.y = center_y;
	SDL_RenderCopyEx(environment_attributes->renderer, bitmap_attributes->image, NULL, &destination, drawable_attributes->angle, &center,
			drawable_attributes->flip);
	d_cast_return(d_drawable_return_last);
}

d_define_method(bitmap, delete)(struct s_object *self, struct s_bitmap_attributes *attributes) {
	SDL_DestroyTexture(attributes->image);
	return NULL;
}

d_define_class(bitmap) {
	d_hook_method_override(bitmap, e_flag_public, drawable, draw),
	d_hook_delete(bitmap),
	d_hook_method_tail
};
