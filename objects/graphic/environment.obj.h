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
#ifndef miranda_graphic_environment_h
#define miranda_graphic_environment_h
#include <SDL2/SDL.h>
#include "drawable.obj.h"
#include "eventable.obj.h"
#define d_environment_default_title "miranda engine"
#define d_environment_default_systems (SDL_INIT_VIDEO|SDL_INIT_AUDIO)
#define d_environment_default_fps 60.0
#define d_environment_default_reference_w 1024
#define d_environment_default_reference_h 768
#define d_environment_layers 16
#define d_environment_tolerance 25
typedef int (* t_environment_call)(struct s_object *);
typedef enum e_environment_surfaces {
	e_environment_surface_under_light = 0,
	e_environment_surface_over_light,
	e_environment_surface_NULL
} e_environment_surfaces;
d_declare_class(environment) {
	struct s_attributes head;
	SDL_Window *window;
	SDL_Renderer *renderer;
	t_environment_call init_call, main_call, quit_call;
	double reference_w, reference_h, current_w, current_h, camera_origin_x, camera_origin_y, fps;
	struct s_list drawable[e_environment_surface_NULL][d_environment_layers], eventable;
	t_boolean continue_loop, scalable_geometry;
} d_declare_class_tail(environment);
struct s_environment_attributes *p_environment_alloc(struct s_object *self);
extern struct s_object *f_environment_new(struct s_object *self, int width, int height);
extern struct s_object *f_environment_new_fullscreen(struct s_object *self, int width, int height, t_boolean fullscreen);
extern struct s_object *f_environment_new_flags(struct s_object *self, int width, int height, int flags);
d_declare_method(environment, set_methods)(struct s_object *self, t_environment_call init_call, t_environment_call mail_call, t_environment_call quit_call);
d_declare_method(environment, set_title)(struct s_object *self, const char *title);
d_declare_method(environment, set_size)(struct s_object *self, int width, int height);
d_declare_method(environment, get_size)(struct s_object *self, int *width, int *height);
d_declare_method(environment, set_camera)(struct s_object *self, double offset_x, double offset_y);
d_declare_method(environment, get_camera)(struct s_object *self, double *offset_x, double *offset_y);
d_declare_method(environment, add_drawable)(struct s_object *self, struct s_object *drawable, int layer, enum e_environment_surfaces surface);
d_declare_method(environment, del_drawable)(struct s_object *self, struct s_object *drawable, int layer, enum e_environment_surfaces surface);
d_declare_method(environment, add_eventable)(struct s_object *self, struct s_object *eventable);
d_declare_method(environment, del_eventable)(struct s_object *self, struct s_object *eventable);
d_declare_method(environment, run_loop)(struct s_object *self);
d_declare_method(environment, delete)(struct s_object *self, struct s_environment_attributes *attributes);
#endif
