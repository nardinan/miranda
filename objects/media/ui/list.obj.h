/*
 * miranda
 * Copyright (C) 2016 Andrea Nardinocchi (andrea@nardinan.it)
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
#ifndef miranda_media_list_h
#define miranda_media_list_h
#include "label.obj.h"
#include "scroll.obj.h"
#define d_list_selected_NULL -1
#define d_list_max_selected 32
d_declare_class(list) {
    struct s_attributes head;
    struct s_list uiables;
    struct s_object *scroll;
    ssize_t selection[d_list_max_selected];
    size_t uiable_entries, visible_entries;
    t_boolean multi_selection;
    enum e_drawable_blends last_blend;
    double last_mask_R, last_mask_G, last_mask_B, last_mask_A, selected_background_R, selected_background_G, selected_background_B, selected_background_A,
           over_background_R, over_background_G, over_background_B, over_background_A, unselected_background_R, unselected_background_G,
           unselected_background_B, unselected_background_A;
} d_declare_class_tail(list);
struct s_list_attributes *p_list_alloc(struct s_object *self);
extern struct s_object *f_list_new(struct s_object *self, struct s_object *scroll);
d_declare_method(list, add_uiable)(struct s_object *self, struct s_object *uiable);
d_declare_method(list, del_uiable)(struct s_object *self, struct s_object *uiable);
d_declare_method(list, get_uiable)(struct s_object *self, unsigned int index);
d_declare_method(list, set_selected_uiable)(struct s_object *self, struct s_object *uiable);
d_declare_method(list, add_selected_uiable)(struct s_object *self, struct s_object *uiable);
d_declare_method(list, get_selected_uiable)(struct s_object *self);
d_declare_method(list, set_selected)(struct s_object *self, unsigned int red, unsigned int green, unsigned int blue, unsigned int alpha);
d_declare_method(list, set_over)(struct s_object *self, unsigned int red, unsigned int green, unsigned int blue, unsigned int alpha);
d_declare_method(list, set_unselected)(struct s_object *self, unsigned int red, unsigned int green, unsigned int blue, unsigned int alpha);
d_declare_method(list, mode)(struct s_object *self, enum e_uiable_modes mode);
d_declare_method(list, event)(struct s_object *self, struct s_object *environment, SDL_Event *current_event);
d_declare_method(list, draw)(struct s_object *self, struct s_object *environment);
d_declare_method(list, set_maskRGB)(struct s_object *self, unsigned int red, unsigned int green, unsigned int blue);
d_declare_method(list, set_maskA)(struct s_object *self, unsigned int alpha);
d_declare_method(list, set_blend)(struct s_object *self, enum e_drawable_blends blend);
d_declare_method(list, delete)(struct s_object *self, struct s_list_attributes *attributes);
#endif
