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
#include "field.obj.h"
#include "../eventable.obj.h"
struct s_field_attributes *p_field_alloc(struct s_object *self, char *string_content, TTF_Font *font, enum e_label_background_formats format,
		enum e_label_alignments alignment_x, enum e_label_alignments alignment_y, struct s_object *environment) {
	struct s_field_attributes *result = d_prepare(self, field);
	f_label_new_alignment(self, string_content, font, format, alignment_x, alignment_y, environment);	/* inherit */
	f_eventable_new(self);											/* inherit */
	return result;
}

struct s_object *f_field_new(struct s_object *self, char *string_content, TTF_Font *font, struct s_object *environment) {
	return f_field_new_alignment(self, string_content, font, e_label_background_format_adaptable, e_label_alignment_left, e_label_alignment_top,
			environment);
}

struct s_object *f_field_new_alignment(struct s_object *self, char *string_content, TTF_Font *font, enum e_label_background_formats format,
		enum e_label_alignments alignment_x, enum e_label_alignments alignment_y, struct s_object *environment) {
	struct s_field_attributes *attributes = p_field_alloc(self, string_content, font, format, alignment_x, alignment_y, environment);
	attributes->pointer = f_string_strlen(string_content);
	return self;
}

d_define_method_override(field, event)(struct s_object *self, struct s_object *environment, SDL_Event *current_event) {
	d_using(field);
	d_using(label);
	d_using(uiable);
	t_boolean update_required = d_false;
	size_t string_length, new_length, incoming_length;
	if (uiable_attributes->selected_mode == e_uiable_mode_selected) { /* and the TextInput event should be enabled */
		switch (current_event->type) {
			case SDL_TEXTINPUT:
				string_length = f_string_strlen(label_attributes->string_content);
				incoming_length = f_string_strlen(current_event->text.text);
				if (incoming_length > 0) {
					if ((label_attributes->size == 0) || ((string_length + incoming_length) >= (label_attributes->size-1))) {
						new_length = string_length + d_media_field_bucket;
						while (new_length < (string_length + incoming_length + 1))
							new_length += d_media_field_bucket;
						if ((label_attributes->string_content = (char *)d_realloc(label_attributes->string_content, new_length)))
							label_attributes->size = new_length;
						else
							d_die(d_error_malloc);
					}
					if (field_attributes->pointer < string_length)
						memmove((label_attributes->string_content + (field_attributes->pointer + incoming_length)),
								(label_attributes->string_content + field_attributes->pointer),
								(label_attributes->size - field_attributes->pointer - incoming_length));
					memcpy((label_attributes->string_content + field_attributes->pointer), current_event->text.text, incoming_length);
					field_attributes->pointer += incoming_length;
					update_required = d_true;
				}
				break;
			case SDL_KEYDOWN:
				switch (current_event->key.keysym.sym) {
					case SDLK_BACKSPACE:
						if (field_attributes->pointer > 0) {
							memmove((label_attributes->string_content+(field_attributes->pointer-1)),
									(label_attributes->string_content+field_attributes->pointer),
									(label_attributes->size-field_attributes->pointer));
							--field_attributes->pointer;
							update_required = d_true;
						}
						break;
					case SDLK_RIGHT:
						if (field_attributes->pointer < f_string_strlen(label_attributes->string_content))
							++field_attributes->pointer;
						break;
					case SDLK_LEFT:
						if (field_attributes->pointer > 0)
							--field_attributes->pointer;
					default:
						break;
				}
			default:
				break;
		}
		if (update_required)
			d_call(self, m_label_update_texture, NULL, environment);
	}
	return self;
}

d_define_method(field, delete)(struct s_object *self, struct s_field_attributes *attributes) {
	return NULL;
}

d_define_class(field) {
	d_hook_method_override(field, e_flag_public, eventable, event),
	d_hook_delete(field),
	d_hook_method_tail
};
