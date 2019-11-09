/*
 * miranda
 * Copyright (C) 2014-2020 Andrea Nardinocchi (andrea@nardinan.it)
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
#include "drawable.obj.h"
#include "camera.obj.h"
struct s_environment_attributes *p_environment_alloc(struct s_object *self) {
  struct s_environment_attributes *result = d_prepare(self, environment);
  f_memory_new(self);   /* inherit */
  f_mutex_new(self);    /* inherit */
  return result;
}
struct s_object *f_environment_new(struct s_object *self, int width, int height) {
  return f_environment_new_flags(self, width, height, 0);
}
struct s_object *f_environment_new_fullscreen(struct s_object *self, int width, int height, t_boolean fullscreen) {
  return f_environment_new_flags(self, width, height, (fullscreen) ? (SDL_WINDOW_FULLSCREEN_DESKTOP | SDL_WINDOW_INPUT_GRABBED) : 0);
}
struct s_object *f_environment_new_flags(struct s_object *self, int width, int height, int flags) {
  struct s_environment_attributes *attributes = p_environment_alloc(self);
  int surface, index, initialized_systems;
  t_boolean initialized = d_true;
  SDL_DisplayMode current_display;
  attributes->mask_A = 255.0;
  for (surface = 0; surface < e_environment_surface_NULL; ++surface)
    for (index = 0; index < d_environment_layers; ++index)
      memset(&(attributes->drawable[surface][index]), 0, sizeof(struct s_list));
  memset(&(attributes->eventable), 0, sizeof(struct s_list));
  if ((attributes->cameras = f_map_new(d_new(map)))) {
    if ((initialized_systems = SDL_WasInit(d_environment_default_systems)) != d_environment_default_systems)
      if (SDL_Init(d_environment_default_systems & (d_environment_default_systems & (~initialized_systems))) < 0) {
        d_err(e_log_level_ever, "SDL graphical system returns an error during the initialization (flags 0x%08x)",
              (d_environment_default_systems & (~initialized_systems)));
        initialized = d_false;
      }
    if (initialized) {
      SDL_StartTextInput();
      /* TTF initialization */
      if (TTF_WasInit() == 0)
        if (TTF_Init() < 0) {
          d_err(e_log_level_ever, "SDL font system returns an error during the initialization");
        }
      /* MIX initialization */
      initialized_systems = Mix_Init(d_environment_default_codecs);
      if (((initialized_systems & d_environment_default_codecs) != d_environment_default_codecs) ||
          (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, d_environment_channels, d_environment_audio_chunk) < 0)) {
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
      if ((attributes->window =
             SDL_CreateWindow(d_environment_default_title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, (flags | SDL_WINDOW_OPENGL)))) {
        attributes->renderer = SDL_CreateRenderer(attributes->window, -1, (SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC));
        SDL_SetRenderDrawBlendMode(attributes->renderer, SDL_BLENDMODE_BLEND);
        SDL_GetWindowSize(attributes->window, &width, &height);
      }
    }
    attributes->fps = d_environment_default_fps;
    /* geometry */
    attributes->current_w = width;
    attributes->current_h = height;
    attributes->continue_loop = d_true;
  } else
    d_die(d_error_malloc);
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
d_define_method(environment, set_channels)(struct s_object *self, int channels) {
  Mix_AllocateChannels(channels);
  return self;
}
d_define_method(environment, set_maskRGB)(struct s_object *self, unsigned int red, unsigned int green, unsigned int blue) {
  d_using(environment);
  environment_attributes->mask_R = red;
  environment_attributes->mask_G = green;
  environment_attributes->mask_B = blue;
  return self;
}
d_define_method(environment, set_maskA)(struct s_object *self, unsigned int alpha) {
  d_using(environment);
  environment_attributes->mask_A = alpha;
  return self;
}
d_define_method(environment, set_fullscreen)(struct s_object *self, t_boolean fullscreen) {
  d_using(environment);
  int width, height;
  SDL_SetWindowFullscreen(environment_attributes->window, ((fullscreen) ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0));
  SDL_GetWindowSize(environment_attributes->window, &width, &height);
  environment_attributes->current_w = width;
  environment_attributes->current_h = height;
  return self;
}
d_define_method(environment, set_size)(struct s_object *self, int width, int height) {
  d_using(environment);
  SDL_SetWindowSize(environment_attributes->window, width, height);
  SDL_GetWindowSize(environment_attributes->window, &width, &height);
  environment_attributes->current_w = width;
  environment_attributes->current_h = height;
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
d_define_method(environment, add_camera)(struct s_object *self, struct s_object *label, struct s_object *camera) {
  d_using(environment);
  d_call(environment_attributes->cameras, m_map_insert, label, camera);
  return self;
}
d_define_method(environment, get_camera)(struct s_object *self, struct s_object *label) {
  d_using(environment);
  return d_call(environment_attributes->cameras, m_map_find, label);
}
d_define_method(environment, get_mouse_normalized)(struct s_object *self, char *camera_label, int *mouse_x, int *mouse_y) {
  d_using(environment);
  struct s_object *string_camera_label = d_kstr(camera_label);
  struct s_object *camera = d_call(environment_attributes->cameras, m_map_find, string_camera_label);
  struct s_camera_attributes *camera_attributes;
  int current_mouse_x, current_mouse_y;
  SDL_GetMouseState(&current_mouse_x, &current_mouse_y);
  if (camera) {
    camera_attributes = d_cast(camera, camera);
    current_mouse_x = (current_mouse_x * camera_attributes->scene_reference_w) / camera_attributes->screen_w;
    current_mouse_y = (current_mouse_y * camera_attributes->scene_reference_h) / camera_attributes->screen_h;
    current_mouse_x = (current_mouse_x - camera_attributes->scene_offset_x);
    current_mouse_y = (current_mouse_y - camera_attributes->scene_offset_y);
    current_mouse_x = (current_mouse_x / camera_attributes->scene_zoom);
    current_mouse_y = (current_mouse_y / camera_attributes->scene_zoom);
  }
  *mouse_x = current_mouse_x;
  *mouse_y = current_mouse_y;
  d_free(string_camera_label);
  return self;
}
d_define_method(environment, add_drawable)(struct s_object *self, struct s_object *drawable, int layer, enum e_environment_surfaces surface) {
  d_using(environment);
  if (!d_list_node_in(&(environment_attributes->drawable[surface][layer]), (struct s_list_node *)drawable))
    f_list_append(&(environment_attributes->drawable[surface][layer]), (struct s_list_node *)(d_retain(drawable)), e_list_insert_tail);
  return drawable;
}
d_define_method(environment, del_drawable)(struct s_object *self, struct s_object *drawable, int layer, enum e_environment_surfaces surface) {
  d_using(environment);
  if (d_list_node_in(&(environment_attributes->drawable[surface][layer]), (struct s_list_node *)drawable)) {
    f_list_delete(&(environment_attributes->drawable[surface][layer]), (struct s_list_node *)drawable);
    d_delete(drawable);
  }
  return drawable;
}
d_define_method(environment, add_eventable)(struct s_object *self, struct s_object *eventable) {
  d_using(environment);
  f_list_append(&(environment_attributes->eventable), (struct s_list_node *)(d_retain(eventable)), e_list_insert_tail);
  return eventable;
}
d_define_method(environment, del_eventable)(struct s_object *self, struct s_object *eventable) {
  d_using(environment);
  if (d_list_node_in(&(environment_attributes->eventable), (struct s_list_node *)eventable)) {
    f_list_delete(&(environment_attributes->eventable), (struct s_list_node *)eventable);
    d_delete(eventable);
  }
  return eventable;
}
d_define_method(environment, run_loop)(struct s_object *self) {
  d_using(environment);
  int starting_time = SDL_GetTicks(), waiting_time, required_time = (int)(1000.0f / environment_attributes->fps), surface, index;
  struct s_eventable_attributes *eventable_attributes;
  struct s_camera_attributes *camera_attributes;
  struct s_object *drawable_object;
  struct s_object *eventable_object;
  struct s_object *string_object;
  struct s_object *camera_object;
  struct s_exception *exception;
  SDL_Event local_event;
  environment_attributes->init_call(self);
  t_boolean event_captured;
  while (environment_attributes->continue_loop) {
    d_try
        {
          while (SDL_PollEvent(&local_event)) {
            event_captured = d_false;
            d_reverse_foreach(&(environment_attributes->eventable), eventable_object, struct s_object) {
              if ((eventable_attributes = d_cast(eventable_object, eventable)))
                if ((eventable_attributes->enable) && ((!eventable_attributes->ignore_event_if_consumed) || (!event_captured)))
                  if ((intptr_t)d_call(eventable_object, m_eventable_event, self, &local_event) == e_eventable_status_captured)
                    event_captured = d_true;
            }
            for (surface = (e_environment_surface_NULL - 1); surface >= 0; --surface) {
              for (index = (d_environment_layers - 1); index >= 0; --index) {
                d_reverse_foreach(&(environment_attributes->drawable[surface][index]), drawable_object, struct s_object) {
                  if ((eventable_attributes = d_cast(drawable_object, eventable)))
                    if ((eventable_attributes->enable) && ((!eventable_attributes->ignore_event_if_consumed) || (!event_captured)))
                      if ((intptr_t)d_call(drawable_object, m_eventable_event, self, &local_event) == e_eventable_status_captured)
                        event_captured = d_true;
                }
              }
            }
            if ((local_event.type == SDL_QUIT) || ((local_event.type == SDL_KEYDOWN) && (local_event.key.keysym.sym == SDLK_ESCAPE)))
              environment_attributes->continue_loop = d_false;
          }
          d_miranda_lock(self) {
            SDL_SetRenderDrawColor(environment_attributes->renderer, environment_attributes->mask_R, environment_attributes->mask_G,
                                   environment_attributes->mask_B, environment_attributes->mask_A);
            SDL_RenderClear(environment_attributes->renderer);
          } d_miranda_unlock(self);
          if (environment_attributes->main_call(self)) {
            d_map_foreach(environment_attributes->cameras, camera_object, string_object) {
              environment_attributes->current_camera = camera_object;
              if ((camera_attributes = d_cast(camera_object, camera))) {
                environment_attributes->current_surface = camera_attributes->surface;
                /* normalization of all the objects scale, preparing them for the camera */
                for (index = 0; index < d_environment_layers; ++index) {
                  environment_attributes->current_layer = index;
                  d_foreach(&(environment_attributes->drawable[camera_attributes->surface][index]), drawable_object, struct s_object)
                    d_call(drawable_object, m_drawable_normalize_scale, camera_attributes->scene_reference_w, camera_attributes->scene_reference_h,
                           camera_attributes->scene_offset_x, camera_attributes->scene_offset_y, camera_attributes->scene_center_x,
                           camera_attributes->scene_center_y, camera_attributes->screen_w, camera_attributes->screen_h, camera_attributes->scene_zoom);
                }
                d_call(camera_object, m_camera_initialize_context, self);
                {
                  environment_attributes->current_surface = camera_attributes->surface;
                  for (index = 0; index < d_environment_layers; ++index) {
                    environment_attributes->current_layer = index;
                    d_foreach(&(environment_attributes->drawable[camera_attributes->surface][index]), drawable_object, struct s_object)
                      if (d_call(drawable_object, m_drawable_is_visible, camera_attributes->screen_w, camera_attributes->screen_h))
                        while (((intptr_t)d_call(drawable_object, m_drawable_draw, self)) == d_drawable_return_continue);
                  }
                }
                d_call(camera_object, m_camera_finalize_context, self);
              } else
                d_err(e_log_level_ever, "unrecognizable object stored into the camera stack (labeled as '%s') as type", d_string_cstring(string_object),
                      camera_object->type);
              environment_attributes->current_camera = NULL;
            }
            /* align the FPS time delay and then refresh the image */
            if ((waiting_time = required_time - (SDL_GetTicks() - starting_time)) > 0)
              SDL_Delay(waiting_time);
            else if (abs(waiting_time) > d_environment_tolerance)
              d_war(e_log_level_medium, "loop time has a delay of %d mS", abs(waiting_time));
            starting_time = SDL_GetTicks();
            d_miranda_lock(self) {
              SDL_RenderPresent(environment_attributes->renderer);
            } d_miranda_unlock(self);
          } else
            environment_attributes->continue_loop = d_false;
        }
      d_catch(exception)
        {
          d_exception_dump(stderr, exception);
          d_raise;
        }
    d_endtry;
  }
  environment_attributes->quit_call(self);
  return self;
}
d_define_method(environment, delete)(struct s_object *self, struct s_environment_attributes *attributes) {
  d_delete(attributes->cameras);
  SDL_DestroyRenderer(attributes->renderer);
  SDL_DestroyWindow(attributes->window);
  Mix_CloseAudio();
  TTF_Quit();
  Mix_Quit();
  SDL_Quit();
  return NULL;
}
d_define_class(environment) {d_hook_method(environment, e_flag_public, set_methods),
                             d_hook_method(environment, e_flag_public, set_title),
                             d_hook_method(environment, e_flag_public, set_channels),
                             d_hook_method(environment, e_flag_public, set_maskRGB),
                             d_hook_method(environment, e_flag_public, set_maskA),
                             d_hook_method(environment, e_flag_public, set_fullscreen),
                             d_hook_method(environment, e_flag_public, set_size),
                             d_hook_method(environment, e_flag_public, get_size),
                             d_hook_method(environment, e_flag_public, add_camera),
                             d_hook_method(environment, e_flag_public, get_camera),
                             d_hook_method(environment, e_flag_public, get_mouse_normalized),
                             d_hook_method(environment, e_flag_public, add_drawable),
                             d_hook_method(environment, e_flag_public, del_drawable),
                             d_hook_method(environment, e_flag_public, add_eventable),
                             d_hook_method(environment, e_flag_public, del_eventable),
                             d_hook_method(environment, e_flag_public, run_loop),
                             d_hook_delete(environment),
                             d_hook_method_tail};
