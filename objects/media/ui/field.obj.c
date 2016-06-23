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
    return result;
}

struct s_object *f_field_new(struct s_object *self, char *string_content, TTF_Font *font, struct s_object *environment) {
    return f_field_new_alignment(self, string_content, font, e_label_background_format_adaptable, e_label_alignment_left, e_label_alignment_center,
            environment);
}

struct s_object *f_field_new_alignment(struct s_object *self, char *string_content, TTF_Font *font, enum e_label_background_formats format,
        enum e_label_alignments alignment_x, enum e_label_alignments alignment_y, struct s_object *environment) {
    struct s_field_attributes *attributes = p_field_alloc(self, string_content, font, format, alignment_x, alignment_y, environment);
    attributes->pointer = f_string_strlen(string_content);
    attributes->last_cursor_R = d_field_default_R;
    attributes->last_cursor_G = d_field_default_G;
    attributes->last_cursor_B = d_field_default_B;
    attributes->last_cursor_A = d_field_default_A;
    return self;
}

d_define_method(field, set_cursor)(struct s_object *self, unsigned int red, unsigned int green, unsigned int blue, unsigned int alpha) {
    d_using(field);
    field_attributes->last_cursor_R = red;
    field_attributes->last_cursor_G = green;
    field_attributes->last_cursor_B = blue;
    field_attributes->last_cursor_A = alpha;
    return self;
}

d_define_method(field, set_size)(struct s_object *self, size_t max_size) {
    d_using(field);
    field_attributes->max_size = max_size;
    return self;
}

d_define_method_override(field, event)(struct s_object *self, struct s_object *environment, SDL_Event *current_event) {
    d_using(field);
    struct s_label_attributes *label_attributes = d_cast(self, label);
    struct s_uiable_attributes *uiable_attributes = d_cast(self, uiable);
    t_boolean update_required = d_false;
    size_t string_length, new_length, incoming_length;
    struct s_object *result = d_call_owner(self, uiable, m_eventable_event, environment, current_event);
    if (uiable_attributes->selected_mode == e_uiable_mode_selected) { /* and the SDL_TextInput event should be enabled */
        switch (current_event->type) {
            case SDL_TEXTINPUT:
                string_length = f_string_strlen(label_attributes->string_content);
                incoming_length = f_string_strlen(current_event->text.text);
                if (field_attributes->pointer > string_length)
                    field_attributes->pointer = string_length;
                if (incoming_length > 0)
                    if ((field_attributes->max_size == 0) || ((string_length + incoming_length) < field_attributes->max_size)) {
                        if ((label_attributes->size == 0) || ((string_length + incoming_length) >= (label_attributes->size-1))) {
                            new_length = string_length + d_field_bucket;
                            while (new_length < (string_length + incoming_length + 1))
                                new_length += d_field_bucket;
                            if ((label_attributes->string_content = (char *)d_realloc(label_attributes->string_content, new_length)))
                                label_attributes->size = new_length;
                            else
                                d_die(d_error_malloc);
                        }
                        if (field_attributes->pointer < string_length)
                            memmove((label_attributes->string_content + (field_attributes->pointer + incoming_length)),
                                    (label_attributes->string_content + field_attributes->pointer),
                                    (label_attributes->size - field_attributes->pointer - incoming_length));
                        memcpy((label_attributes->string_content + field_attributes->pointer), current_event->text.text,
                                incoming_length);
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
    return result;
}

d_define_method_override(field, draw)(struct s_object *self, struct s_object *environment) {
    d_using(field);
    struct s_label_attributes *label_attributes = d_cast(self, label);
    struct s_drawable_attributes *drawable_attributes = d_cast(self, drawable);
    struct s_uiable_attributes *uiable_attributes = d_cast(self, uiable);
    struct s_environment_attributes *environment_attributes = d_cast(environment, environment);
    char buffer[d_string_buffer_size], *string_subcontent;
    int result = (intptr_t)d_call_owner(self, label, m_drawable_draw, environment); /* recall the father's draw method */
    double position_x, position_y, dimension_w, dimension_h, new_dimension_w = 0, center_x, center_y, width_factor, top_x, top_y, bottom_x, bottom_y,
           radians = (drawable_attributes->angle * d_math_pi)/180.0, sin_radians, cos_radians;
    size_t string_length = f_string_strlen(label_attributes->string_content);
    SDL_Surface *unoptimized_surface;
    SDL_Color white = {
        255,
        255,
        255,
        255
    };
    if (uiable_attributes->selected_mode == e_uiable_mode_selected) {
        d_call(&(drawable_attributes->point_normalized_destination), m_point_get, &position_x, &position_y);
        d_call(&(drawable_attributes->point_normalized_dimension), m_point_get, &dimension_w, &dimension_h);
        d_call(&(drawable_attributes->point_normalized_center), m_point_get, &center_x, &center_y);
        width_factor = (dimension_w / label_attributes->last_width);
        if ((field_attributes->pointer > 0) && (field_attributes->pointer < string_length) && (string_length > 0)) {
            if ((string_subcontent = (char *) d_malloc(field_attributes->pointer + 1))) {
                strncpy(string_subcontent, label_attributes->string_content, field_attributes->pointer);
                /* we should be sure that the font exits */
                if ((unoptimized_surface = TTF_RenderText_Blended(label_attributes->last_font, string_subcontent, white))) {
                    new_dimension_w = (unoptimized_surface->w - label_attributes->last_source.x) * width_factor;
                    SDL_FreeSurface(unoptimized_surface);
                } else {
                    snprintf(buffer, d_string_buffer_size, "ungenerable texture for label \"%s\" exception",
                            label_attributes->string_content);
                    d_throw(v_exception_texture, buffer);
                }
                d_free(string_subcontent);
            } else
                d_die(d_error_malloc);
        } else if ((field_attributes->pointer == string_length) && (string_length > 0))
            new_dimension_w = label_attributes->last_destination.w;
        top_x = label_attributes->last_destination.x + new_dimension_w + d_field_cursor_offset;
        top_y = label_attributes->last_destination.y + uiable_attributes->border_h;
        bottom_x = top_x;
        bottom_y = top_y + label_attributes->last_destination.h - (uiable_attributes->border_h * 2.0);
        sin_radians = sin(radians);
        cos_radians = cos(radians);
        p_square_normalize_coordinate(NULL, top_x, top_y, (position_x + center_x), (position_y + center_y), sin_radians, cos_radians, &top_x, &top_y);
        p_square_normalize_coordinate(NULL, bottom_x, bottom_y, (position_x + center_x), (position_y + center_y), sin_radians, cos_radians, &bottom_x, 
                &bottom_y);
        if ((intptr_t)d_call(&(drawable_attributes->square_collision_box), m_square_inside_coordinates, top_x + ((bottom_x - top_x)/2.0),
                    top_y + ((bottom_y - top_y)/2.0))) {
            SDL_SetRenderDrawColor(environment_attributes->renderer, field_attributes->last_cursor_B, field_attributes->last_cursor_G,
                    field_attributes->last_cursor_R, field_attributes->last_cursor_A);
            SDL_RenderDrawLine(environment_attributes->renderer, top_x, top_y, bottom_x, bottom_y);
        }
    }
    d_cast_return(result);
}

d_define_method(field, delete)(struct s_object *self, struct s_field_attributes *attributes) {
    return NULL;
}

d_define_class(field) {
    d_hook_method(field, e_flag_public, set_cursor),
        d_hook_method(field, e_flag_public, set_size),
        d_hook_method_override(field, e_flag_public, eventable, event),
        d_hook_method_override(field, e_flag_public, drawable, draw),
        d_hook_delete(field),
        d_hook_method_tail
};
