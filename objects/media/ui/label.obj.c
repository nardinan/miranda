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
    f_mutex_new(self);	/* inherit */
    f_memory_new(self);	/* inherit */
    f_uiable_new(self);	/* inherit */
    return result;
}

struct s_object *f_label_new(struct s_object *self, char *string_content, TTF_Font *font, struct s_object *environment) {
    return f_label_new_alignment(self, string_content, font, e_label_background_format_adaptable, e_label_alignment_left, e_label_alignment_top,
            environment);
}

struct s_object *f_label_new_alignment(struct s_object *self, char *string_content, TTF_Font *font, enum e_label_background_formats format,
        enum e_label_alignments alignment_x, enum e_label_alignments alignment_y, struct s_object *environment) {
    struct s_label_attributes *attributes = p_label_alloc(self);
    attributes->format = format;
    attributes->alignment_x = alignment_x;
    attributes->alignment_y = alignment_y;
    attributes->last_font = font;
    attributes->last_blend = e_drawable_blend_alpha;
    attributes->last_mask_R = 255.0;
    attributes->last_mask_G = 255.0;
    attributes->last_mask_B = 255.0;
    attributes->last_mask_A = 255.0;
    return p_label_set_content_char(self, string_content, font, environment);
}

d_define_method(label, set_content_string)(struct s_object *self, struct s_object *string_content, TTF_Font *font, struct s_object *environment) {
    struct s_string_attributes *string_attributes = d_cast(string_content, string);
    return p_label_set_content_char(self, string_attributes->content, font, environment);
}

d_define_method(label, set_content_char)(struct s_object *self, char *string_content, TTF_Font *font, struct s_object *environment) {
    d_using(label);
    size_t string_length = f_string_strlen(string_content);
    if (string_length > 0) {
        if (label_attributes->string_content) {
            /* realloc only if really needed */
            if (label_attributes->size < (string_length+1)) {
                if ((label_attributes->string_content = (char *)d_realloc(label_attributes->string_content, (string_length+1))))
                    label_attributes->size = (string_length+1);
                else
                    d_die(d_error_malloc);
            }
        } else if ((label_attributes->string_content = (char *)d_malloc(string_length+1)))
            label_attributes->size = (string_length+1);
        else
            d_die(d_error_malloc);
        strncpy(label_attributes->string_content, string_content, string_length);
        label_attributes->string_content[string_length] = '\0';
    } else if (label_attributes->string_content)
        memset(label_attributes->string_content, '\0', label_attributes->size);
    p_label_update_texture(self, font, environment);
    return self;
}

d_define_method(label, get_content_char)(struct s_object *self) {
    d_using(label);
    d_cast_return(label_attributes->string_content);
}

d_define_method(label, update_texture)(struct s_object *self, TTF_Font *font, struct s_object *environment) {
    d_using(label);
    char buffer[d_string_buffer_size];
    int width, height;
    struct s_drawable_attributes *drawable_attributes = d_cast(self, drawable);
    struct s_uiable_attributes *uiable_attributes = d_cast(self, uiable);
    struct s_environment_attributes *environment_attributes = d_cast(environment, environment);
    TTF_Font *current_font;
    SDL_Surface *unoptimized_surface;
    SDL_Color white = {
        255,
        255,
        255,
        255
    };
    if (label_attributes->image) {
        SDL_DestroyTexture(label_attributes->image);
        label_attributes->image = NULL;
    }
    if ((current_font = font) || (current_font = label_attributes->last_font)) {
        label_attributes->last_font = current_font;
        if (f_string_strlen(label_attributes->string_content) > 0) {
            if ((unoptimized_surface = TTF_RenderText_Blended(current_font, label_attributes->string_content, white))) {
                label_attributes->image = SDL_CreateTextureFromSurface(environment_attributes->renderer, unoptimized_surface);
                if (SDL_QueryTexture(label_attributes->image, NULL, NULL, &width, &height) == 0) {
                    label_attributes->string_width = width;
                    label_attributes->string_height = height;
                    if (label_attributes->format == e_label_background_format_adaptable) {
                        label_attributes->last_width = width + (uiable_attributes->border_w * 2.0);
                        label_attributes->last_height = height + (uiable_attributes->border_h * 2.0);
                    }
                    d_call(&(drawable_attributes->point_dimension), m_point_set_x, (double)label_attributes->last_width);
                    d_call(&(drawable_attributes->point_dimension), m_point_set_y, (double)label_attributes->last_height);
                    if (label_attributes->last_blend != e_drawable_blend_undefined)
                        d_call(self, m_drawable_set_blend, label_attributes->last_blend);
                    d_call(self, m_drawable_set_maskRGB, (unsigned int)label_attributes->last_mask_R, (unsigned int)label_attributes->last_mask_G, 
                            (unsigned int)label_attributes->last_mask_B);
                    d_call(self, m_drawable_set_maskA, (unsigned int)label_attributes->last_mask_A);
                } else {
                    snprintf(buffer, d_string_buffer_size, "unable to retrieve informations for label \"%s\" exception",
                            label_attributes->string_content);
                    d_throw(v_exception_texture, buffer);
                }
                SDL_FreeSurface(unoptimized_surface);
            } else {
                snprintf(buffer, d_string_buffer_size, "ungenerable texture for label \"%s\" exception",
                        label_attributes->string_content);
                d_throw(v_exception_texture, buffer);
            }
        }
    } else {
        snprintf(buffer, d_string_buffer_size, "ungenerable texture for label \"%s\" with missing font exception", label_attributes->string_content);
        d_throw(v_exception_texture, buffer);
    }
    d_call(self, m_emitter_raise, v_uiable_signals[e_uiable_signal_content_changed]);
    return self;
}

d_define_method_override(label, set_dimension)(struct s_object *self, double w, double h) {
    d_using(label);
    struct s_drawable_attributes *drawable_attributes = d_cast(self, drawable);
    struct s_object *result = d_call_owner(self, drawable, m_drawable_set_dimension, w, h);
    label_attributes->last_width = w;
    label_attributes->last_height = h;
    label_attributes->format = e_label_background_format_fixed;
    d_call(&(drawable_attributes->point_center), m_point_set_x, (double)(label_attributes->last_width/2.0));
    d_call(&(drawable_attributes->point_center), m_point_set_y, (double)(label_attributes->last_height/2.0));
    return result;
}

d_define_method_override(label, set_dimension_w)(struct s_object *self, double w) {
    d_using(label);
    struct s_drawable_attributes *drawable_attributes = d_cast(self, drawable);
    struct s_object *result = d_call_owner(self, drawable, m_drawable_set_dimension_w, w);
    label_attributes->last_width = w;
    label_attributes->format = e_label_background_format_fixed;
    d_call(&(drawable_attributes->point_center), m_point_set_x, (double)(label_attributes->last_width/2.0));
    return result;
}

d_define_method_override(label, set_dimension_h)(struct s_object *self, double h) {
    d_using(label);
    struct s_drawable_attributes *drawable_attributes = d_cast(self, drawable);
    struct s_object *result = d_call_owner(self, drawable, m_drawable_set_dimension_h, h);
    label_attributes->last_height = h;
    label_attributes->format = e_label_background_format_fixed;
    d_call(&(drawable_attributes->point_center), m_point_set_y, (double)(label_attributes->last_height/2.0));
    return result;
}

d_define_method_override(label, draw)(struct s_object *self, struct s_object *environment) {
    d_using(label);
    struct s_drawable_attributes *drawable_attributes = d_cast(self, drawable);
    struct s_uiable_attributes *uiable_attributes = d_cast(self, uiable);
    struct s_environment_attributes *environment_attributes = d_cast(environment, environment);
    double position_x, position_y, dimension_w, dimension_h, center_x, center_y, width_factor, height_factor;
    int result = (intptr_t)d_call_owner(self, uiable, m_drawable_draw, environment); /* recall the father's draw method */
    SDL_Rect source, destination;
    SDL_Point center;
    if (label_attributes->image) {
        d_call(&(drawable_attributes->point_normalized_destination), m_point_get, &position_x, &position_y);
        d_call(&(drawable_attributes->point_normalized_dimension), m_point_get, &dimension_w, &dimension_h);
        d_call(&(drawable_attributes->point_normalized_center), m_point_get, &center_x, &center_y);
        width_factor = (dimension_w/label_attributes->last_width);
        height_factor = (dimension_h/label_attributes->last_height);
        source.x = 0;
        source.y = 0;
        destination.x = (position_x + uiable_attributes->border_w);
        destination.y = (position_y + uiable_attributes->border_h);
        if (label_attributes->format == e_label_background_format_fixed) {
            source.w = d_math_min((label_attributes->last_width - (uiable_attributes->border_w * 2.0)), label_attributes->string_width);
            source.h = d_math_min((label_attributes->last_height - (uiable_attributes->border_h * 2.0)), label_attributes->string_height);
            switch (label_attributes->alignment_x) {
                case e_label_alignment_center:
                    if ((source.x = d_math_max(((label_attributes->string_width-label_attributes->last_width)/2.0), 0)) == 0)
                        destination.x = position_x + (((label_attributes->last_width - label_attributes->string_width)/2.0) *
                                width_factor);
                    break;
                case e_label_alignment_right:
                    if ((source.x = d_math_max((label_attributes->string_width-label_attributes->last_width), 0)) == 0)
                        destination.x = position_x + ((label_attributes->last_width - label_attributes->string_width) * width_factor);
                default:
                    break;
            }
            switch (label_attributes->alignment_y) {
                case e_label_alignment_center:
                    if ((source.y = d_math_max(((label_attributes->string_height-label_attributes->last_height)/2.0), 0)) == 0)
                        destination.y = position_y + (((label_attributes->last_height - label_attributes->string_height)/2.0) *
                                height_factor);
                    break;
                case e_label_alignment_bottom:
                    if ((source.y = d_math_max((label_attributes->string_height-label_attributes->last_height), 0)) == 0)
                        destination.y = position_y + ((label_attributes->last_height - label_attributes->string_height) *
                                height_factor);
                default:
                    break;
            }
        } else if (label_attributes->format == e_label_background_format_adaptable) {
            source.w = label_attributes->string_width;
            source.h = label_attributes->string_height;
        }
        destination.w = source.w * width_factor;
        destination.h = source.h * height_factor;
        center.x = (position_x + center_x) - destination.x;
        center.y = (position_y + center_y) - destination.y;
        label_attributes->last_source = source;
        label_attributes->last_destination = destination;
        SDL_RenderCopyEx(environment_attributes->renderer, label_attributes->image, &source, &destination, drawable_attributes->angle, &center,
                (SDL_RendererFlip)drawable_attributes->flip);
    }
    d_cast_return(result);
}

d_define_method_override(label, set_maskRGB)(struct s_object *self, unsigned int red, unsigned int green, unsigned int blue) {
    d_using(label);
    label_attributes->last_mask_R = red;
    label_attributes->last_mask_G = green;
    label_attributes->last_mask_B = blue;
    if (label_attributes->image)
        SDL_SetTextureColorMod(label_attributes->image, red, green, blue);
    return self;
}

d_define_method_override(label, set_maskA)(struct s_object *self, unsigned int alpha) {
    d_using(label);
    label_attributes->last_mask_A = alpha;
    if (label_attributes->image)
        SDL_SetTextureAlphaMod(label_attributes->image, alpha);
    return self;
}

d_define_method_override(label, set_blend)(struct s_object *self, enum e_drawable_blends blend) {
    d_using(label);
    label_attributes->last_blend = blend;
    if (label_attributes->image)
        SDL_SetTextureBlendMode(label_attributes->image, (SDL_BlendMode)blend);
    return self;
}

d_define_method(label, delete)(struct s_object *self, struct s_label_attributes *attributes) {
    if (attributes->image)
        SDL_DestroyTexture(attributes->image);
    if (attributes->string_content)
        d_free(attributes->string_content);
    return NULL;
}

d_define_class(label) {
    d_hook_method(label, e_flag_public, set_content_string),
        d_hook_method(label, e_flag_public, set_content_char),
        d_hook_method(label, e_flag_public, get_content_char),
        d_hook_method(label, e_flag_public, update_texture),
        d_hook_method_override(label, e_flag_public, drawable, set_dimension),
        d_hook_method_override(label, e_flag_public, drawable, set_dimension_w),
        d_hook_method_override(label, e_flag_public, drawable, set_dimension_h),
        d_hook_method_override(label, e_flag_public, drawable, draw),
        d_hook_method_override(label, e_flag_public, drawable, set_maskRGB),
        d_hook_method_override(label, e_flag_public, drawable, set_maskA),
        d_hook_method_override(label, e_flag_public, drawable, set_blend),
        d_hook_delete(label),
        d_hook_method_tail
};
