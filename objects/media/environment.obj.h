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
#ifndef miranda_media_environment_h
#define miranda_media_environment_h
#include "primitives.h"
#include "drawable.obj.h"
#include "eventable.obj.h"
#define d_environment_default_title "miranda"
#define d_environment_default_systems (SDL_INIT_VIDEO|SDL_INIT_AUDIO)
#define d_environment_default_codecs (MIX_INIT_OGG)
#define d_environment_default_fps 30.0
#define d_environment_default_reference_w 1024
#define d_environment_default_reference_h 768
#define d_environment_default_monitor 0 /* primary */
#define d_environment_layers 16
#define d_environment_tolerance 60.0
#define d_environment_channels 2
#define d_environment_audio_chunk 4096
typedef int (* t_environment_call)(struct s_object *);
typedef enum e_environment_surfaces {
    e_environment_surface_primary = 0,
    e_environment_surface_ui,
    e_environment_surface_NULL
} e_environment_surfaces;
d_declare_class(environment) {
    struct s_attributes head;
    SDL_Window *window;
    SDL_Renderer *renderer;
    t_environment_call init_call, main_call, quit_call;
    enum e_environment_surfaces current_surface;
    double reference_w[e_environment_surface_NULL], reference_h[e_environment_surface_NULL], current_w, current_h,
           camera_origin_x[e_environment_surface_NULL], camera_origin_y[e_environment_surface_NULL], camera_focus_x[e_environment_surface_NULL],
           camera_focus_y[e_environment_surface_NULL], zoom[e_environment_surface_NULL], fps, mask_R, mask_G, mask_B, mask_A;
    struct s_list drawable[e_environment_surface_NULL][d_environment_layers], eventable;
    t_boolean continue_loop;
} d_declare_class_tail(environment);
struct s_environment_attributes *p_environment_alloc(struct s_object *self);
extern struct s_object *f_environment_new(struct s_object *self, int width, int height);
extern struct s_object *f_environment_new_fullscreen(struct s_object *self, int width, int height, t_boolean fullscreen);
extern struct s_object *f_environment_new_flags(struct s_object *self, int width, int height, int flags);
d_declare_method(environment, set_methods)(struct s_object *self, t_environment_call init_call, t_environment_call mail_call, t_environment_call quit_call);
d_declare_method(environment, set_title)(struct s_object *self, const char *title);
d_declare_method(environment, set_channels)(struct s_object *self, int channels);
d_declare_method(environment, set_maskRGB)(struct s_object *self, unsigned int red, unsigned int green, unsigned int blue);
d_declare_method(environment, set_maskA)(struct s_object *self, unsigned int alpha);
d_declare_method(environment, set_fullscreen)(struct s_object *self, t_boolean fullscreen);
d_declare_method(environment, set_size)(struct s_object *self, int width, int height);
d_declare_method(environment, get_size)(struct s_object *self, int *width, int *height);
d_declare_method(environment, set_camera)(struct s_object *self, double offset_x, double offset_y, enum e_environment_surfaces surface);
d_declare_method(environment, get_camera)(struct s_object *self, double *offset_x, double *offset_y, enum e_environment_surfaces surface);
d_declare_method(environment, set_reference)(struct s_object *self, double reference_w, double reference_h, enum e_environment_surfaces surface);
d_declare_method(environment, get_reference)(struct s_object *self, double *reference_w, double *reference_h, enum e_environment_surfaces surface);
d_declare_method(environment, set_focus)(struct s_object *self, double camera_focus_x, double camera_focus_y, enum e_environment_surfaces surface);
d_declare_method(environment, get_focus)(struct s_object *self, double *camera_focus_x, double *camera_focus_y, enum e_environment_surfaces surface);
d_declare_method(environment, set_zoom)(struct s_object *self, double zoom, enum e_environment_surfaces surface);
d_declare_method(environment, get_zoom)(struct s_object *self, double *zoom, enum e_environment_surfaces surface);
d_declare_method(environment, add_drawable)(struct s_object *self, struct s_object *drawable, int layer, enum e_environment_surfaces surface);
d_declare_method(environment, del_drawable)(struct s_object *self, struct s_object *drawable, int layer, enum e_environment_surfaces surface);
d_declare_method(environment, add_eventable)(struct s_object *self, struct s_object *eventable);
d_declare_method(environment, del_eventable)(struct s_object *self, struct s_object *eventable);
d_declare_method(environment, run_loop)(struct s_object *self);
d_declare_method(environment, delete)(struct s_object *self, struct s_environment_attributes *attributes);
#endif
