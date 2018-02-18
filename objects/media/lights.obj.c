/*
 * miranda
 * Copyright (C) 2018 Andrea Nardinocchi (andrea@nardinan.it)
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
#include "lights.obj.h"
void p_lights_modulator_flickering(struct s_lights_emitter *emitter) {
    clock_t current_clock = clock();
    double default_factor = 0.1, new_intensity = ((sin(((double)current_clock) * default_factor) + 1.0) / 2.0) * emitter->original_intensity;
    emitter->current_intensity = new_intensity;
}

struct s_lights_attributes *p_lights_alloc(struct s_object *self) {
    struct s_lights_attributes *result = d_prepare(self, lights);
    f_mutex_new(self);				                /* inherit */
    f_memory_new(self);				                /* inherit */
    f_drawable_new(self, e_drawable_kind_single);	/* inherit */
    return result;
}

struct s_object *f_lights_new(struct s_object *self, unsigned char intensity, struct s_object *environment) {
    struct s_lights_attributes *attributes = p_lights_alloc(self);
    struct s_environment_attributes *environment_attributes = d_cast(environment, environment);
    size_t size = (environment_attributes->current_w * environment_attributes->current_h * 4 /* RGBA */);
    char buffer[d_string_buffer_size];
    attributes->intensity = intensity;
    if ((attributes->memblock = (unsigned char *)d_malloc(size))) {
        memset(attributes->memblock, intensity, size);
        if ((attributes->background = SDL_CreateTexture(environment_attributes->renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, 
                        environment_attributes->current_w, environment_attributes->current_h))) {
            attributes->current_w = environment_attributes->current_w;
            attributes->current_h = environment_attributes->current_h;
            attributes->memblock_size = size;
            SDL_UpdateTexture(attributes->background, NULL, attributes->memblock, (environment_attributes->current_w * 4 /* RGBA */));
            SDL_SetTextureBlendMode(attributes->background, SDL_BLENDMODE_MOD);
        } else {
            d_free(attributes->memblock);
            attributes->memblock = NULL;
            snprintf(buffer, d_string_buffer_size, "ungenerable texture from empty bitmap (size %.02f %.02f)", environment_attributes->current_w, 
                    environment_attributes->current_h);
            d_throw(v_exception_texture, buffer);
        }
    } else
        d_die(d_error_malloc);
    return self;
}

d_define_method(lights, add_light)(struct s_object *self, unsigned char intensity, unsigned char mask_R, unsigned char mask_G, unsigned char mask_B, 
        double radius, t_lights_intensity_modulator modulator, struct s_object *mask, struct s_object *reference, enum e_drawable_alignments alignment) {
    d_using(lights);
    struct s_lights_emitter *current_emitter;
    if ((current_emitter = (struct s_lights_emitter *)d_malloc(sizeof(struct s_lights_emitter)))) {
        current_emitter->current_intensity = current_emitter->original_intensity = intensity;
        current_emitter->current_mask_R = current_emitter->original_mask_R = mask_R;
        current_emitter->current_mask_G = current_emitter->original_mask_G = mask_G;
        current_emitter->current_mask_B = current_emitter->original_mask_B = mask_B;
        current_emitter->current_radius = current_emitter->original_radius = radius;
        current_emitter->modulator = modulator;
        current_emitter->alignment = alignment;
        current_emitter->reference = d_retain(reference);
        current_emitter->mask = d_retain(mask);
        d_call(current_emitter->mask, m_drawable_set_blend, (enum e_drawable_blends)e_drawable_blend_add);
        f_list_append(&(lights_attributes->emitters), (struct s_list_node *)current_emitter, e_list_insert_head);
    } else 
        d_die(d_error_malloc);
    return self;
}

d_define_method(lights, set_intensity)(struct s_object *self, unsigned char intensity) {
    d_using(lights);
    lights_attributes->intensity = intensity;
    if (lights_attributes->memblock)
        memset(lights_attributes->memblock, intensity, lights_attributes->memblock_size);
    return self;
}

d_define_method(lights, get_intensity)(struct s_object *self) {
    d_using(lights);
    d_cast_return(lights_attributes->intensity);
}

d_define_method_override(lights, draw)(struct s_object *self, struct s_object *environment) {
    d_using(lights);
    struct s_environment_attributes *environment_attributes = d_cast(environment, environment);
    struct s_lights_emitter *current_emitter;
    size_t size;
    double intensity_modificator;
    SDL_Rect source, destination;
    SDL_Point center;
    if ((environment_attributes->current_w != lights_attributes->current_w) || 
            (environment_attributes->current_h != lights_attributes->current_h)) {
        d_free(lights_attributes->memblock);
        size = (environment_attributes->current_w * environment_attributes->current_h * 4 /* RGBA */);
        if ((lights_attributes->memblock = (unsigned char *)d_malloc(size))) {
            memset(lights_attributes->memblock, lights_attributes->intensity, size);
            lights_attributes->current_w = environment_attributes->current_w;
            lights_attributes->current_h = environment_attributes->current_h;
            lights_attributes->memblock_size = size;
        } else
            d_die(d_error_malloc);
    }
    source.x = 0;
    source.y = 0;
    source.w = lights_attributes->current_w;
    source.h = lights_attributes->current_h;
    destination.x = 0;
    destination.y = 0;
    destination.w = lights_attributes->current_w;
    destination.h = lights_attributes->current_h;
    center.x = (lights_attributes->current_w / 2.0);
    center.y = (lights_attributes->current_h / 2.0);
    SDL_UpdateTexture(lights_attributes->background, NULL, lights_attributes->memblock, (environment_attributes->current_w * 4 /* RGBA */));
    SDL_SetTextureAlphaMod(lights_attributes->background, 255);
    d_miranda_lock(environment) {
        SDL_SetRenderTarget(environment_attributes->renderer, lights_attributes->background);
    } d_miranda_unlock(environment);
    d_foreach(&(lights_attributes->emitters), current_emitter, struct s_lights_emitter) {
       d_call(current_emitter->mask, m_drawable_copy_geometry, current_emitter->reference, current_emitter->alignment);
       if ((d_call(current_emitter->mask, m_drawable_normalize_scale, environment_attributes->reference_w[environment_attributes->current_surface],
                        environment_attributes->reference_h[environment_attributes->current_surface],
                        environment_attributes->camera_origin_x[environment_attributes->current_surface],
                        environment_attributes->camera_origin_y[environment_attributes->current_surface],
                        environment_attributes->camera_focus_x[environment_attributes->current_surface],
                        environment_attributes->camera_focus_y[environment_attributes->current_surface],
                        environment_attributes->current_w,
                        environment_attributes->current_h,
                        environment_attributes->zoom[environment_attributes->current_surface]))) {
           if (current_emitter->modulator)
               current_emitter->modulator(current_emitter);
           intensity_modificator = (current_emitter->current_intensity / 255.0);
           d_call(current_emitter->mask, m_drawable_set_zoom, current_emitter->current_radius);
           d_call(current_emitter->mask, m_drawable_set_maskRGB, (unsigned int)(current_emitter->current_mask_R * intensity_modificator),
                   (unsigned int)(current_emitter->current_mask_G * intensity_modificator), 
                   (unsigned int)(current_emitter->current_mask_B * intensity_modificator));
           while (d_call(current_emitter->mask, m_drawable_draw, environment) != d_drawable_return_last);
       }
    }
    d_miranda_lock(environment) {
        SDL_SetRenderTarget(environment_attributes->renderer, NULL);
        SDL_RenderCopyEx(environment_attributes->renderer, lights_attributes->background, &source, &destination, 0.0, &center, SDL_FLIP_NONE);
    } d_miranda_unlock(environment);
    d_cast_return(d_drawable_return_last);
}

d_define_method_override(lights, normalize_scale)(struct s_object *self, double reference_w, double reference_h, double offset_x, double offset_y,
        double focus_x, double focus_y, double current_w, double current_h, double zoom) {
    /* we don't need to perform any normalization on the current scale */
    return self;
}

d_define_method(lights, delete)(struct s_object *self, struct s_lights_attributes *attributes) {
    struct s_lights_emitter *current_emitter;
    while ((current_emitter = (struct s_lights_emitter *)attributes->emitters.head)) {
        d_delete(current_emitter->mask);
        d_delete(current_emitter->reference);
        f_list_delete(&(attributes->emitters), (struct s_list_node *)attributes->emitters.head);
    }
    if (attributes->memblock)
        d_free(attributes->memblock);
    SDL_DestroyTexture(attributes->background);
    return NULL;
}

d_define_class(lights) {
    d_hook_method(lights, e_flag_public, add_light),
    d_hook_method(lights, e_flag_public, set_intensity),
    d_hook_method(lights, e_flag_public, get_intensity),
    d_hook_method_override(lights, e_flag_public, drawable, draw),
    d_hook_method_override(lights, e_flag_public, drawable, normalize_scale),
    d_hook_delete(lights),
    d_hook_method_tail
};
