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
#ifndef miranda_media_illuminable_bitmap_h
#define miranda_media_illuminable_bitmap_h
#include "bitmap.obj.h"
#include "lights.obj.h"
#include "shadows.obj.h"
#define d_illuminable_bitmap_default_contour_color 0, 255.0, 0, 255.0
typedef enum e_illuminable_bitmap_sides {
  e_illuminable_bitmap_side_top = 0,
  e_illuminable_bitmap_side_right,
  e_illuminable_bitmap_side_bottom,
  e_illuminable_bitmap_side_left,
  e_illuminable_bitmap_side_front,
  e_illuminable_bitmap_side_NULL
} e_illuminable_bitmap_sides;
d_declare_class(illuminable_bitmap) {
  struct s_attributes head;
  struct s_object *drawable_mask[e_illuminable_bitmap_side_NULL];
  struct s_object *lights;
  struct s_object *shadows;
  struct s_object *polygon_shadow_caster, *polygon_shadow_caster_normalized;
} d_declare_class_tail(illuminable_bitmap);
struct s_illuminable_bitmap_attributes *p_illuminable_bitmap_alloc(struct s_object *self, struct s_object *stream, struct s_object *environment);
extern struct s_object *f_illuminable_bitmap_new(struct s_object *self, struct s_object *stream, struct s_object *environment);
d_declare_method(illuminable_bitmap, set_light_mask)(struct s_object *self, struct s_object *drawable_mask, enum e_illuminable_bitmap_sides side);
d_declare_method(illuminable_bitmap, set_lights)(struct s_object *self, struct s_object *lights);
d_declare_method(illuminable_bitmap, set_shadows)(struct s_object *self, struct s_object *shadows);
d_declare_method(illuminable_bitmap, set_shadow_caster_points)(struct s_object *self, size_t size, ...);
d_declare_method(illuminable_bitmap, draw)(struct s_object *self, struct s_object *environment);
d_declare_method(illuminable_bitmap, draw_contour)(struct s_object *self, struct s_object *environment);
d_declare_method(illuminable_bitmap, normalize_scale)(struct s_object *self, double reference_w, double reference_h, double offset_x, double offset_y,
  double focus_x, double focus_y, double current_w, double current_h, double zoom);
d_declare_method(illuminable_bitmap, keep_scale)(struct s_object *self, double current_w, double current_h);
d_declare_method(illuminable_bitmap, delete)(struct s_object *self, struct s_illuminable_bitmap_attributes *attributes);
#endif
