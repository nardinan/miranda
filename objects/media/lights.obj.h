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
#ifndef miranda_media_lights_h
#define miranda_media_lights_h
#define d_lights_default_contour_color_background 0, 0, 0, 255
#define d_lights_default_contour_color_body 100, 100, 100, 255
#define d_lights_default_contour_radius 8
#define d_lights_default_contour_width 6
#define d_lights_default_contour_height 6
#define d_lights_default_maximum_overexposion 0
#include "bitmap.obj.h"
struct s_lights_emitter;
typedef void (*t_lights_intensity_modulator)(struct s_lights_emitter *emitter);
typedef struct s_lights_emitter { d_list_node_head;
  struct s_object *mask, *reference;
  unsigned char original_intensity, current_intensity, original_mask_R, original_mask_G, original_mask_B, current_mask_R, current_mask_G, current_mask_B;
  double original_radius, current_radius, last_normalized_x, last_normalized_y, last_normalized_w, last_normalized_h, overexposion;
  enum e_drawable_alignments alignment;
  t_lights_intensity_modulator modulator; /* isn't mandatory */
  t_boolean project_shadows, overexposes;
} s_lights_emitter;
typedef struct s_lights_emitter_description {
  d_list_node_head;
  double position_x, position_y, distance, radius, intensity, mask_R, mask_G, mask_B;
  struct s_lights_emitter *reference;
} s_lights_emitter_description;
d_declare_class(lights) {
  struct s_attributes head;
  struct s_list emitters;
  double current_w, current_h;
  size_t memblock_size;
  unsigned char *memblock, intensity;
  SDL_Texture *background;
} d_declare_class_tail(lights);
extern void p_lights_modulator_flickering(struct s_lights_emitter *emitter);
extern void p_lights_modulator_pulsing_fast(struct s_lights_emitter *emitter);
extern void p_lights_modulator_pulsing_normal(struct s_lights_emitter *emitter);
extern void p_lights_modulator_pulsing_slow(struct s_lights_emitter *emitter);
struct s_lights_attributes *p_lights_alloc(struct s_object *self);
extern struct s_object *f_lights_new(struct s_object *self, unsigned char intensity, struct s_object *environment);
d_declare_method(lights, add_light)(struct s_object *self, unsigned char intensity, unsigned char mask_R, unsigned char mask_G, unsigned char mask_B,
    double radius, t_lights_intensity_modulator modulator, struct s_object *mask, struct s_object *reference, enum e_drawable_alignments alignment,
    t_boolean project_shadows);
d_declare_method(lights, del_light)(struct s_object *self, struct s_lights_emitter *emitter);
d_declare_method(lights, get_light)(struct s_object *self, struct s_object *reference);
d_declare_method(lights, clear)(struct s_object *self);
d_declare_method(lights, set_intensity)(struct s_object *self, unsigned char intensity);
d_declare_method(lights, get_intensity)(struct s_object *self);
d_declare_method(lights, get_affecting_lights)(struct s_object *self, struct s_object *drawable, struct s_list *container, struct s_object *environment,
    t_boolean shadows_generation);
d_declare_method(lights, get_localized_intensity)(struct s_object *self, double target_position_x, double target_position_y, struct s_object *environment, 
    double *intensity_detected, struct s_lights_emitter *emitter_to_ignore);
d_declare_method(lights, draw)(struct s_object *self, struct s_object *environment);
d_declare_method(lights, draw_contour)(struct s_object *self, struct s_object *environment);
d_declare_method(lights, is_visible)(struct s_object *self, double current_w, double current_h);
d_declare_method(lights, delete)(struct s_object *self, struct s_lights_attributes *attributes);
#endif
