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
#ifndef miranda_media_uiable_h
#define miranda_media_uiable_h
#include "../environment.obj.h"
#include "../morpholable.obj.h"
#include "../../emitter.obj.h"
#define d_uiable_default_border 2
enum e_uiable_components {
    e_uiable_component_corner_top_left = 0,
    e_uiable_component_corner_top_right,
    e_uiable_component_corner_bottom_left,
    e_uiable_component_corner_bottom_right,
    e_uiable_component_top,
    e_uiable_component_bottom,
    e_uiable_component_left,
    e_uiable_component_right,
    e_uiable_component_center,
    e_uiable_component_NULL
} e_uiable_components;
enum e_uiable_modes {
    e_uiable_mode_active = 0,
    e_uiable_mode_selected,
    e_uiable_mode_idle,
    e_uiable_mode_NULL
} e_uiable_modes;
enum e_uiable_signals {
    e_uiable_signal_selected = 0,
    e_uiable_signal_unselected,
    e_uiable_signal_clicked_left,
    e_uiable_signal_clicked_right,
    e_uiable_signal_changed,
    e_uiable_signal_content_changed,
    e_uiable_signal_NULL
} e_uiable_signals;
extern unsigned int v_uiable_id;
extern const char *v_uiable_components[e_uiable_component_NULL];
extern const char *v_uiable_modes[e_uiable_mode_NULL];
extern const char *v_uiable_signals[e_uiable_signal_NULL];
d_declare_class(uiable) {
    struct s_attributes head;
    struct s_object *background[e_uiable_mode_NULL][e_uiable_component_NULL];
    unsigned int component_id;
    enum e_uiable_modes selected_mode;
    enum e_drawable_blends last_blend;
    t_boolean is_selected;
    double border_h, border_w, last_mask_R, last_mask_G, last_mask_B, last_mask_A, background_mask_R, background_mask_G, background_mask_B,
           background_mask_A;
} d_declare_class_tail(uiable);
struct s_uiable_attributes *p_uiable_alloc(struct s_object *self);
extern struct s_object *f_uiable_new(struct s_object *self);
d_declare_method(uiable, set)(struct s_object *self, struct s_object *drawable, enum e_uiable_modes mode, enum e_uiable_components component);
d_declare_method(uiable, set_background)(struct s_object *self, unsigned int red, unsigned int green, unsigned int blue, unsigned int alpha);
d_declare_method(uiable, mode)(struct s_object *self, enum e_uiable_modes mode);
d_declare_method(uiable, event)(struct s_object *self, struct s_object *environment, SDL_Event *current_event);
d_declare_method(uiable, draw)(struct s_object *self, struct s_object *environment);
d_declare_method(uiable, set_maskRGB)(struct s_object *self, unsigned int red, unsigned int green, unsigned int blue);
d_declare_method(uiable, set_maskA)(struct s_object *self, unsigned int alpha);
d_declare_method(uiable, set_blend)(struct s_object *self, enum e_drawable_blends blend);
d_declare_method(uiable, delete)(struct s_object *self, struct s_uiable_attributes *attributes);
#endif
