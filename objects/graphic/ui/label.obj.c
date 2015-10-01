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
#include "label.obj.h"
struct s_label_attributes *p_label_alloc(struct s_object *self) {
	struct s_label_attributes *result = d_prepare(self, label);
	f_memory_new(self);				/* inherit */
	f_mutex_new(self);				/* inherit */
	f_uiable_new(self);				/* inherit */
	return result;
}

struct s_object *f_label_new(struct s_object *self, struct s_object *string_content, TTF_Font *font, struct s_object *environment) {
	struct s_label_attributes *attributes = p_label_alloc(self);
	attributes->last_blend = e_drawable_blend_undefined;
	attributes->last_mask_R = 255.0;
	attributes->last_mask_G = 255.0;
	attributes->last_mask_B = 255.0;
	attributes->last_mask_A = 255.0;
	return p_label_set_content(self, string_content, font, environment);
}

d_define_method(label, set_content)(struct s_object *self, struct s_object *string_content, TTF_Font *font, struct s_object *environment) {
	d_using(label);
	struct s_drawable_attributes *drawable_attributes = d_cast(self, drawable);
	struct s_environment_attributes *environment_attributes = d_cast(environment, environment);
	char buffer[d_string_buffer_size];
	int width, height;
	SDL_Surface *unoptimized_surface;
	SDL_Color white = {
		255,
		255,
		255,
		255
	};
	if (label_attributes->string_content)
		d_delete(label_attributes->string_content);
	label_attributes->string_content = d_retain(string_content);
	if (label_attributes->image)
		SDL_DestroyTexture(label_attributes->image);
	if ((unoptimized_surface = TTF_RenderText_Blended(font, d_string_cstring(string_content), white))) {
		label_attributes->image = SDL_CreateTextureFromSurface(environment_attributes->renderer, unoptimized_surface);
		if (SDL_QueryTexture(label_attributes->image, NULL, NULL, &width, &height) == 0) {
			d_call(&(drawable_attributes->point_dimension), m_point_set_x, (double)width);
			d_call(&(drawable_attributes->point_dimension), m_point_set_y, (double)height);
			d_call(&(drawable_attributes->point_center), m_point_set_x, (double)(width/2.0));
			d_call(&(drawable_attributes->point_center), m_point_set_y, (double)(height/2.0));
			if (label_attributes->last_blend != e_drawable_blend_undefined)
				d_call(self, m_drawable_set_blend, label_attributes->last_blend);
			d_call(self, m_drawable_set_maskRGB, (unsigned int)label_attributes->last_mask_R,
				(unsigned int)label_attributes->last_mask_G, (unsigned int)label_attributes->last_mask_B);
			d_call(self, m_drawable_set_maskA, (unsigned int)label_attributes->last_mask_A);
		} else {
			snprintf(buffer, d_string_buffer_size, "unable to retrieve informations for label \"%s\" exception",
					d_string_cstring(string_content));
			d_throw(v_exception_texture, buffer);
		}
		SDL_FreeSurface(unoptimized_surface);
	} else {
		snprintf(buffer, d_string_buffer_size, "ungenerable texture for label \"%s\" exception", d_string_cstring(string_content));
		d_throw(v_exception_texture, buffer);
	}
	return self;
}

d_define_method_override(label, draw)(struct s_object *self, struct s_object *environment) {
	d_using(label);
	double position_x, position_y, dimension_w, dimension_h, center_x, center_y;
	struct s_drawable_attributes *drawable_attributes = d_cast(self, drawable);
	struct s_environment_attributes *environment_attributes = d_cast(environment, environment);
	SDL_Rect destination;
	SDL_Point center;
	d_call_owner(self, uiable, m_drawable_draw, environment); /* recall the father's draw method */
	d_call(&(drawable_attributes->point_normalized_destination), m_point_get, &position_x, &position_y);
	d_call(&(drawable_attributes->point_normalized_dimension), m_point_get, &dimension_w, &dimension_h);
	d_call(&(drawable_attributes->point_normalized_center), m_point_get, &center_x, &center_y);
	destination.x = position_x;
	destination.y = position_y;
	destination.w = dimension_w;
	destination.h = dimension_h;
	center.x = center_x;
	center.y = center_y;
	SDL_RenderCopyEx(environment_attributes->renderer, label_attributes->image, NULL, &destination, drawable_attributes->angle, &center,
			drawable_attributes->flip);
	d_cast_return(d_drawable_return_last);
}

d_define_method_override(label, set_maskRGB)(struct s_object *self, unsigned int red, unsigned int green, unsigned int blue) {
	d_using(label);
	label_attributes->last_mask_R = red;
	label_attributes->last_mask_G = green;
	label_attributes->last_mask_B = blue;
	SDL_SetTextureColorMod(label_attributes->image, red, green, blue);
	return self;
}

d_define_method_override(label, set_maskA)(struct s_object *self, unsigned int alpha) {
	d_using(label);
	label_attributes->last_mask_A = alpha;
	SDL_SetTextureAlphaMod(label_attributes->image, alpha);
	return self;
}

d_define_method_override(label, set_blend)(struct s_object *self, enum e_drawable_blends blend) {
	d_using(label);
	label_attributes->last_blend = blend;
	SDL_SetTextureBlendMode(label_attributes->image, blend);
	return self;
}

d_define_method(label, delete)(struct s_object *self, struct s_label_attributes *attributes) {
	SDL_DestroyTexture(attributes->image);
	d_delete(attributes->string_content);
	return NULL;
}

d_define_class(label) {
	d_hook_method(label, e_flag_public, set_content),
	d_hook_method_override(label, e_flag_public, drawable, draw),
	d_hook_method_override(label, e_flag_public, drawable, set_maskRGB),
	d_hook_method_override(label, e_flag_public, drawable, set_maskA),
	d_hook_method_override(label, e_flag_public, drawable, set_blend),
	d_hook_delete(label),
	d_hook_method_tail
};
