/*
 * miranda
 * Copyright (C) 2019 Andrea Nardinocchi (andrea@nardinan.it)
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
#include "shadows.obj.h"
struct s_shadows_attributes *p_shadows_alloc(struct s_object *self) {
  struct s_shadows_attributes *result = d_prepare(self, shadows);
  f_mutex_new(self);                             /* inherit */
  f_memory_new(self);                            /* inherit */
  f_drawable_new(self, e_drawable_kind_single);  /* inherit */
  return result;
}
struct s_object *f_shadows_new(struct s_object *self) {
  struct s_shadows_attributes *attributes = p_shadows_alloc(self);
  if (!(attributes->array_shadow_casters = f_array_new(d_new(array), d_array_bucket)))
    d_die(d_error_malloc);
  return self;
}
d_define_method(shadows, set_lights)(struct s_object *self, struct s_object *lights) {
  d_using(shadows);
  if (shadows_attributes->lights)
    d_delete(shadows_attributes->lights);
  shadows_attributes->lights = d_retain(lights);
  return self;
}
d_define_method(shadows, add_shadow_caster)(struct s_object *self, struct s_object *polygon) {
  d_using(shadows);
  return d_call(shadows_attributes->array_shadow_casters, m_array_push, polygon);
}
d_define_method(shadows, clear)(struct s_object *self) {
  d_using(shadows);
  return d_call(shadows_attributes->array_shadow_casters, m_array_clear, NULL);
}
d_define_method_override(shadows, draw)(struct s_object *self, struct s_object *environment) {
  d_cast_return(d_drawable_return_last);
}
d_define_method_override(shadows, draw_contour)(struct s_object *self, struct s_object *environment) {
  return self;
}
d_define_method_override(shadows, is_visible)(struct s_object *self, double current_w, double current_h) {
  /* is visible, every time */
  return self;
}
d_define_method(shadows, delete)(struct s_object *self, struct s_shadows_attributes *attributes) {
  return NULL;
}
d_define_class(shadows) {d_hook_method(shadows, e_flag_public, set_lights),
                        d_hook_method(shadows, e_flag_public, add_shadow_caster),
                        d_hook_method(shadows, e_flag_public, clear),
                        d_hook_method_override(shadows, e_flag_public, drawable, draw),
                        d_hook_method_override(shadows, e_flag_public, drawable, draw_contour),
                        d_hook_method_override(shadows, e_flag_public, drawable, is_visible),
                        d_hook_delete(shadows),
                        d_hook_method_tail};