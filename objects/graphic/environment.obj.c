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
	for (surface = 0; surface < e_environment_surface_NULL; ++surface)
		for (index = 0; index < d_environment_layers; ++index)
			memset(&(attributes->drawable[surface][index]), 0, sizeof(struct s_list));
	memset(&(attributes->eventable), 0, sizeof(struct s_list));
	if ((initialized_systems = SDL_WasInit(d_environment_default_systems)) != d_environment_default_systems)
		if (SDL_Init(d_environment_default_systems&(~initialized_systems)) < 0)
			initialized = d_false;
	if (initialized)
		if ((attributes->window = SDL_CreateWindow(d_environment_default_title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height,
						(flags|SDL_WINDOW_OPENGL))))
			attributes->renderer = SDL_CreateRenderer(attributes->window, -1, (SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC));
	attributes->fps = d_environment_default_fps;
	/* geometry */
	attributes->reference_w = d_environment_default_reference_w;
	attributes->reference_h = d_environment_default_reference_h;
	attributes->current_w = width;
	attributes->current_h = height;
	attributes->camera_origin_x = 0;
	attributes->camera_origin_y = 0;
	attributes->camera_focus_x = (width / 2.0);
	attributes->camera_focus_y = (height / 2.0);
	attributes->zoom = 1;
	attributes->scalable_geometry = d_true;
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

d_define_method(environment, set_camera)(struct s_object *self, double offset_x, double offset_y) {
	d_using(environment);
	environment_attributes->camera_origin_x = offset_x;
	environment_attributes->camera_origin_y = offset_y;
	return self;
}

d_define_method(environment, get_camera)(struct s_object *self, double *offset_x, double *offset_y) {
	d_using(environment);
	if (offset_x)
		*offset_x = environment_attributes->camera_origin_x;
	if (offset_y)
		*offset_y = environment_attributes->camera_origin_y;
	return self;
}

d_define_method(environment, set_focus)(struct s_object *self, double focus_x, double focus_y) {
	d_using(environment);
	environment_attributes->camera_focus_x = focus_x;
	environment_attributes->camera_focus_y = focus_y;
	return self;
}

d_define_method(environment, get_focus)(struct s_object *self, double *focus_x, double *focus_y) {
	d_using(environment);
	if (focus_x)
		*focus_x = environment_attributes->camera_focus_x;
	if (focus_y)
		*focus_y = environment_attributes->camera_focus_y;
	return self;
}

d_define_method(environment, set_zoom)(struct s_object *self, double zoom) {
	d_using(environment);
	environment_attributes->zoom = zoom;
	return self;
}

d_define_method(environment, get_zoom)(struct s_object *self, double *zoom) {
	d_using(environment);
	*zoom = environment_attributes->zoom;
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
			if (SDL_PollEvent(&local_event)) {
				d_foreach(&(environment_attributes->eventable), eventable_object, struct s_object)
					d_call(eventable_object, m_eventable_event, self, &local_event);
				if (local_event.type == SDL_QUIT)
					environment_attributes->continue_loop = d_false;
			}
			SDL_RenderClear(environment_attributes->renderer);
			if (environment_attributes->main_call(self)) {
				for (surface = 0; surface < e_environment_surface_NULL; ++surface)
					for (index = 0; index < d_environment_layers; ++index)
						d_foreach(&(environment_attributes->drawable[surface][index]), drawable_object, struct s_object) {
							flags = (int)d_call(drawable_object, m_drawable_get_flags, NULL);
							if ((flags&e_drawable_kind_hidden) != e_drawable_kind_hidden)
								if ((d_call(drawable_object, m_drawable_normalize_scale, environment_attributes->reference_w,
												environment_attributes->reference_h,
												environment_attributes->camera_origin_x,
												environment_attributes->camera_origin_y,
												environment_attributes->camera_focus_x,
												environment_attributes->camera_focus_y,
												environment_attributes->current_w,
												environment_attributes->current_h,
												environment_attributes->zoom)))
									while (((int)d_call(drawable_object, m_drawable_draw, self)) ==
											d_drawable_return_continue);
						}
				current_time = SDL_GetTicks();
				if ((waiting_time = required_time - (current_time - starting_time)) > 0)
					SDL_Delay(waiting_time);
				else if (waiting_time < (d_environment_tolerance * -1))
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
