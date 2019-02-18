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
#ifndef miranda_media_factory_h
#define miranda_media_factory_h
#include "../io/resources.obj.h"
#include "../io/json.obj.h"
#include "../io/lisp.obj.h"
#include "illuminable_bitmap.obj.h"
#include "particle.obj.h"
#include "animation.obj.h"
#include "transition.obj.h"
#include "track.obj.h"
#define d_media_factory_animation_frame_time 100.0
#define d_media_factory_max_channels 127
#define d_media_factory_min_channels 1
typedef enum e_media_factory_media_types {
  e_media_factory_media_type_bitmap = 0,
  e_media_factory_media_type_animation,
  e_media_factory_media_type_transition,
  e_media_factory_media_type_particle,
  e_media_factory_media_type_NULL
} e_media_factory_media_types;
d_declare_class(media_factory) {
  struct s_attributes head;
  struct s_object *environment;
  struct s_object *resources_png, *resources_json, *resources_ogg, *resources_lisp;
  int current_channel;
} d_declare_class_tail(media_factory);
struct s_media_factory_attributes *p_media_factory_alloc(struct s_object *self);
extern struct s_object *f_media_factory_new(struct s_object *self, struct s_object *resources_png, struct s_object *resources_json,
  struct s_object *resources_ogg, struct s_object *resources_lisp, struct s_object *environment);
d_declare_method(media_factory, get_bitmap)(struct s_object *self, const char *label);
d_declare_method(media_factory, get_illuminable_bitmap)(struct s_object *self, const char *label_core, const char *label_left, const char *label_right,
  const char *label_top, const char *label_bottom, const char *label_front);
d_declare_method(media_factory, get_animation)(struct s_object *self, const char *label);
d_declare_method(media_factory, get_transition)(struct s_object *self, const char *label);
d_declare_method(media_factory, get_particle_structure)(struct s_object *self, struct s_object *json, struct s_particle_configuration_core *configuration,
  const char *prefix);
d_declare_method(media_factory, get_particle)(struct s_object *self, const char *label);
d_declare_method(media_factory, get_media)(struct s_object *self, const char *label, enum e_media_factory_media_types *selected_type);
d_declare_method(media_factory, get_track)(struct s_object *self, const char *label);
d_declare_method(media_factory, delete)(struct s_object *self, struct s_media_factory_attributes *attributes);
#endif
