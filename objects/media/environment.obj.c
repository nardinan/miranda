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
#include "environment.obj.h"
struct s_environment_attributes *p_environment_alloc(struct s_object *self) {
    struct s_environment_attributes *result = d_prepare(self, environment);
    f_memory_new(self);	/* inherit */
    f_mutex_new(self);	/* inherit */
    return result;
}

struct s_object *f_environment_new(struct s_object *self, int width, int height) {
    return f_environment_new_flags(self, width, height, 0);
}

struct s_object *f_environment_new_fullscreen(struct s_object *self, int width, int height, t_boolean fullscreen) {
    return f_environment_new_flags(self, width, height, (fullscreen)?SDL_WINDOW_FULLSCREEN:0);
}

struct s_object *f_environment_new_flags(struct s_object *self, int width, int height, int flags) {
    struct s_environment_attributes *attributes = p_environment_alloc(self);
    int surface, index, initialized_systems;
    t_boolean initialized = d_true;
    SDL_DisplayMode current_display;
    for (surface = 0; surface < e_environment_surface_NULL; ++surface)
        for (index = 0; index < d_environment_layers; ++index)
            memset(&(attributes->drawable[surface][index]), 0, sizeof(struct s_list));
    memset(&(attributes->eventable), 0, sizeof(struct s_list));
    if ((initialized_systems = SDL_WasInit(d_environment_default_systems)) != d_environment_default_systems)
        if (SDL_Init(d_environment_default_systems&(d_environment_default_systems&(~initialized_systems))) < 0) {
            d_err(e_log_level_ever, "SDL graphical system returns an error during the initialization (flags 0x%08x)",
                    (d_environment_default_systems&(~initialized_systems)));
            initialized = d_false;
        }
    if (initialized) {
        SDL_StartTextInput();
        /* TTF initialization */
        if (TTF_WasInit() == 0)
            if (TTF_Init() < 0) {
                initialized = d_false;
                d_err(e_log_level_ever, "SDL font system returns an error during the initialization");
            }
        /* MIX initialization */
        initialized_systems = Mix_Init(d_environment_default_codecs);
        if (((initialized_systems&d_environment_default_codecs) != d_environment_default_codecs) ||
                (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, d_environment_channels, d_environment_audio_chunk) < 0)) {
            initialized = d_false;
            d_err(e_log_level_ever, "SDL audio systems returns an error during the initialization (%s)", Mix_GetError());
        }
        if ((!width) && (!height)) {
            if (SDL_GetCurrentDisplayMode(d_environment_default_monitor, &current_display) == 0) {
                d_log(e_log_level_medium, "SDL display uses resolution %d x %d", current_display.w, current_display.h);
                width = current_display.w;
                height = current_display.h;
            } else {
                d_err(e_log_level_ever, "SDL display isn't reachable or has not a resolution. I'll switch to the default one");
                width = d_environment_default_reference_w;
                height = d_environment_default_reference_h;
            }
        }
        if ((attributes->window = SDL_CreateWindow(d_environment_default_title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height,
                        (flags|SDL_WINDOW_OPENGL)))) {
            attributes->renderer = SDL_CreateRenderer(attributes->window, -1, (SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC));
            SDL_SetRenderDrawBlendMode(attributes->renderer, SDL_BLENDMODE_BLEND);
        }
    }
    attributes->fps = d_environment_default_fps;
    /* geometry */
    attributes->current_w = width;
    attributes->current_h = height;
    for (index = 0; index < e_environment_surface_NULL; ++index) {
        attributes->reference_w[index] = d_environment_default_reference_w;
        attributes->reference_h[index] = d_environment_default_reference_h;
        attributes->camera_focus_x[index] = (width / 2.0);
        attributes->camera_focus_y[index] = (height / 2.0);
        attributes->zoom[index] = 1;
    }
    attributes->continue_loop = d_true;
    return self;
}

d_define_method(environment, set_methods)(struct s_object *self, t_environment_call init_call, t_environment_call main_call, t_environment_call quit_call) {
    d_using(environment);
    environment_attributes->init_call = init_call;
    environment_attributes->main_call = main_call;
    environment_attributes->quit_call = quit_call;
    return self;
}

d_define_method(environment, set_title)(struct s_object *self, const char *title) {
    d_using(environment);
    SDL_SetWindowTitle(environment_attributes->window, title);
    return self;
}

d_define_method(environment, set_size)(struct s_object *self, int width, int height) {
    d_using(environment);
    environment_attributes->current_w = width;
    environment_attributes->current_h = height;
    SDL_SetWindowSize(environment_attributes->window, width, height);
    return self;
}

d_define_method(environment, get_size)(struct s_object *self, int *width, int *height) {
    d_using(environment);
    int local_width, local_height;
    SDL_GetWindowSize(environment_attributes->window, &local_width, &local_height);
    if (width)
        *width = local_width;
    if (height)
        *height = local_height;
    return self;
}

d_define_method(environment, set_camera)(struct s_object *self, double offset_x, double offset_y, enum e_environment_surfaces surface) {
    d_using(environment);
    environment_attributes->camera_origin_x[surface] = offset_x;
    environment_attributes->camera_origin_y[surface] = offset_y;
    return self;
}

d_define_method(environment, get_camera)(struct s_object *self, double *offset_x, double *offset_y, enum e_environment_surfaces surface) {
    d_using(environment);
    if (offset_x)
        *offset_x = environment_attributes->camera_origin_x[surface];
    if (offset_y)
        *offset_y = environment_attributes->camera_origin_y[surface];
    return self;
}

d_define_method(environment, set_reference)(struct s_object *self, double reference_w, double reference_h, enum e_environment_surfaces surface) {
    d_using(environment);
    environment_attributes->reference_w[surface] = reference_w;
    environment_attributes->reference_h[surface] = reference_h;
    return self;
}

d_define_method(environment, get_reference)(struct s_object *self, double *reference_w, double *reference_h, enum e_environment_surfaces surface) {
    d_using(environment);
    if (reference_w)
        *reference_w = environment_attributes->reference_w[surface];
    if (reference_h)
        *reference_h = environment_attributes->reference_h[surface];
    return self;
}

d_define_method(environment, set_focus)(struct s_object *self, double focus_x, double focus_y, enum e_environment_surfaces surface) {
    d_using(environment);
    environment_attributes->camera_focus_x[surface] = focus_x;
    environment_attributes->camera_focus_y[surface] = focus_y;
    return self;
}

d_define_method(environment, get_focus)(struct s_object *self, double *focus_x, double *focus_y, enum e_environment_surfaces surface) {
    d_using(environment);
    if (focus_x)
        *focus_x = environment_attributes->camera_focus_x[surface];
    if (focus_y)
        *focus_y = environment_attributes->camera_focus_y[surface];
    return self;
}

d_define_method(environment, set_zoom)(struct s_object *self, double zoom, enum e_environment_surfaces surface) {
    d_using(environment);
    environment_attributes->zoom[surface] = zoom;
    return self;
}

d_define_method(environment, get_zoom)(struct s_object *self, double *zoom, enum e_environment_surfaces surface) {
    d_using(environment);
    *zoom = environment_attributes->zoom[surface];
    return self;
}

d_define_method(environment, add_drawable)(struct s_object *self, struct s_object *drawable, int layer, enum e_environment_surfaces surface) {
    d_using(environment);
    f_list_append(&(environment_attributes->drawable[surface][layer]), (struct s_list_node *)drawable, e_list_insert_head);
    return drawable;
}

d_define_method(environment, del_drawable)(struct s_object *self, struct s_object *drawable, int layer, enum e_environment_surfaces surface) {
    d_using(environment);
    f_list_delete(&(environment_attributes->drawable[surface][layer]), (struct s_list_node *)drawable);
    return drawable;
}

d_define_method(environment, add_eventable)(struct s_object *self, struct s_object *eventable) {
    d_using(environment);
    f_list_append(&(environment_attributes->eventable), (struct s_list_node *)eventable, e_list_insert_head);
    return eventable;
}

d_define_method(environment, del_eventable)(struct s_object *self, struct s_object *eventable) {
    d_using(environment);
    f_list_delete(&(environment_attributes->eventable), (struct s_list_node *)eventable);
    return eventable;
}

d_define_method(environment, run_loop)(struct s_object *self) {
    d_using(environment);
    int starting_time = SDL_GetTicks(), current_time, waiting_time, required_time = (int)(1000.0f/environment_attributes->fps), surface, index, flags;
    struct s_object *drawable_object;
    struct s_object *eventable_object;
    struct s_exception *exception;
    SDL_Event local_event;
    environment_attributes->init_call(self);
    while (environment_attributes->continue_loop) {
        d_try {
            while (SDL_PollEvent(&local_event)) {
                d_foreach(&(environment_attributes->eventable), eventable_object, struct s_object)
                    d_call(eventable_object, m_eventable_event, self, &local_event);
                if (local_event.type == SDL_QUIT)
                    environment_attributes->continue_loop = d_false;
            }
            SDL_RenderClear(environment_attributes->renderer);
            if (environment_attributes->main_call(self)) {
                for (surface = 0; surface < e_environment_surface_NULL; ++surface) {
                    environment_attributes->current_surface = surface;
                    for (index = 0; index < d_environment_layers; ++index)
                        d_foreach(&(environment_attributes->drawable[surface][index]), drawable_object, struct s_object) {
                            flags = (int)d_call(drawable_object, m_drawable_get_flags, NULL);
                            if ((flags&e_drawable_kind_hidden) != e_drawable_kind_hidden)
                                if ((d_call(drawable_object, m_drawable_normalize_scale,
                                                environment_attributes->reference_w[surface],
                                                environment_attributes->reference_h[surface],
                                                environment_attributes->camera_origin_x[surface],
                                                environment_attributes->camera_origin_y[surface],
                                                environment_attributes->camera_focus_x[surface],
                                                environment_attributes->camera_focus_y[surface],
                                                environment_attributes->current_w,
                                                environment_attributes->current_h,
                                                environment_attributes->zoom[surface])))
                                    while (((int)d_call(drawable_object, m_drawable_draw, self)) ==
                                            d_drawable_return_continue);
                        }
                }
                current_time = SDL_GetTicks();
                if ((waiting_time = required_time - (current_time - starting_time)) > 0)
                    SDL_Delay(waiting_time);
                else if ((waiting_time * -1) > d_environment_tolerance)
                    d_war(e_log_level_medium, "loop time has a delay of %d mS", (waiting_time * -1));
                starting_time = current_time;
                /* align the FPS time delay and then refresh the image */
                SDL_RenderPresent(environment_attributes->renderer);
            } else
                environment_attributes->continue_loop = d_false;
        } d_catch(exception) {
            d_exception_dump(stderr, exception);
            d_raise;
            environment_attributes->continue_loop = d_false;
        } d_endtry;
    }
    environment_attributes->quit_call(self);
    return self;
}

d_define_method(environment, delete)(struct s_object *self, struct s_environment_attributes *attributes) {
    int surface, index;
    for (surface = 0; surface < e_environment_surface_NULL; ++surface)
        for (index = 0; index < d_environment_layers; ++index)
            while (attributes->drawable[surface][index].head)
                f_list_delete(&(attributes->drawable[surface][index]), attributes->drawable[surface][index].head);
    SDL_DestroyRenderer(attributes->renderer);
    SDL_DestroyWindow(attributes->window);
    Mix_CloseAudio();
    TTF_Quit();
    Mix_Quit();
    SDL_Quit();
    return NULL;
}

d_define_class(environment) {
    d_hook_method(environment, e_flag_public, set_methods),
        d_hook_method(environment, e_flag_public, set_title),
        d_hook_method(environment, e_flag_public, set_size),
        d_hook_method(environment, e_flag_public, get_size),
        d_hook_method(environment, e_flag_public, set_camera),
        d_hook_method(environment, e_flag_public, get_camera),
        d_hook_method(environment, e_flag_public, set_reference),
        d_hook_method(environment, e_flag_public, get_reference),
        d_hook_method(environment, e_flag_public, set_focus),
        d_hook_method(environment, e_flag_public, get_focus),
        d_hook_method(environment, e_flag_public, set_zoom),
        d_hook_method(environment, e_flag_public, get_zoom),
        d_hook_method(environment, e_flag_public, add_drawable),
        d_hook_method(environment, e_flag_public, del_drawable),
        d_hook_method(environment, e_flag_public, add_eventable),
        d_hook_method(environment, e_flag_public, del_eventable),
        d_hook_method(environment, e_flag_public, run_loop),
        d_hook_delete(environment),
        d_hook_method_tail
};
