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
#include "drawable.obj.h"
struct s_drawable_attributes *p_drawable_alloc(struct s_object *self) {
  struct s_drawable_attributes *result = d_prepare(self, drawable);
  /* abstract (no memory inheritance) */
  return result;
}
struct s_object *f_drawable_new(struct s_object *self, int flags) {
  struct s_drawable_attributes *attributes = p_drawable_alloc(self);
  f_point_new(d_use(&(attributes->point_destination), point), 0, 0);
  f_point_new(d_use(&(attributes->point_normalized_destination), point), 0, 0);
  f_point_new(d_use(&(attributes->point_dimension), point), 0, 0);
  f_point_new(d_use(&(attributes->point_normalized_dimension), point), 0, 0);
  f_point_new(d_use(&(attributes->point_center), point), 0, 0);
  f_point_new(d_use(&(attributes->point_normalized_center), point), 0, 0);
  f_square_new(d_use(&(attributes->square_collision_box), square), 0, 0, 0, 0);
  attributes->zoom = 1.0;
  attributes->flip = e_drawable_flip_none;
  attributes->flags = flags;
  attributes->last_blend = e_drawable_blend_alpha;
  attributes->last_mask_R = 255.0;
  attributes->last_mask_G = 255.0;
  attributes->last_mask_B = 255.0;
  attributes->last_mask_A = 255.0;
  return self;
}
d_define_method(drawable, copy_geometry)(struct s_object *self, struct s_object *drawable, enum e_drawable_alignments alignment) {
  d_using(drawable);
  struct s_drawable_attributes *drawable_attributes_other = d_cast(drawable, drawable);
  double position_x, position_y, dimension_w, dimension_h, final_position_x, final_position_y;
  d_call(&(drawable_attributes_other->point_normalized_destination), m_point_get, &position_x, &position_y);
  d_call(&(drawable_attributes_other->point_normalized_dimension), m_point_get, &dimension_w, &dimension_h);
  final_position_x = position_x;
  final_position_y = position_y;
  switch (alignment) {
    case e_drawable_alignment_centered:
      final_position_x = (position_x + (dimension_w / 2.0));
      final_position_y = (position_y + (dimension_h / 2.0));
      break;
    case e_drawable_alignment_top_right:
      final_position_x = position_x + dimension_w;
      break;
    case e_drawable_alignment_bottom_left:
      final_position_y = position_y + dimension_h;
      break;
    case e_drawable_alignment_bottom_right:
      final_position_x = position_x + dimension_w;
      final_position_y = position_y + dimension_h;
    default:
      break;
  }
  d_call(&(drawable_attributes->point_destination), m_point_set, final_position_x, final_position_y);
  drawable_attributes->angle = drawable_attributes_other->angle;
  drawable_attributes->flip = drawable_attributes_other->flip;
  return self;
}
d_define_method(drawable, draw)(struct s_object *self, struct s_object *environment) {
  d_war(e_log_level_ever, "'draw' method has not been implemented yet");
  return self;
}
d_define_method(drawable, draw_contour)(struct s_object *self, struct s_object *environment) {
  d_using(drawable);
  struct s_square_attributes *square_attributes = d_cast(&(drawable_attributes->square_collision_box), square);
  struct s_environment_attributes *environment_attributes = d_cast(environment, environment);
  double center_x, center_y;
  d_call(&(drawable_attributes->square_collision_box), m_square_normalize, NULL);
  d_call(&(drawable_attributes->point_normalized_center), m_point_get, &center_x, &center_y);
  d_miranda_lock(environment) {
    SDL_SetRenderDrawColor(environment_attributes->renderer, d_drawable_default_contour_color);
    SDL_RenderDrawLine(environment_attributes->renderer, square_attributes->normalized_top_left_x, square_attributes->normalized_top_left_y,
        square_attributes->normalized_top_right_x, square_attributes->normalized_top_right_y);
    SDL_RenderDrawLine(environment_attributes->renderer, square_attributes->normalized_top_right_x, square_attributes->normalized_top_right_y,
        square_attributes->normalized_bottom_right_x, square_attributes->normalized_bottom_right_y);
    SDL_RenderDrawLine(environment_attributes->renderer, square_attributes->normalized_bottom_right_x, square_attributes->normalized_bottom_right_y,
        square_attributes->normalized_bottom_left_x, square_attributes->normalized_bottom_left_y);
    SDL_RenderDrawLine(environment_attributes->renderer, square_attributes->normalized_bottom_left_x, square_attributes->normalized_bottom_left_y,
        square_attributes->normalized_top_left_x, square_attributes->normalized_top_left_y);
    SDL_RenderDrawLine(environment_attributes->renderer, square_attributes->normalized_top_left_x, square_attributes->normalized_top_left_y,
        square_attributes->top_left_x + center_x, square_attributes->top_left_y + center_y);
  } d_miranda_unlock(environment);
  return self;
}
d_define_method(drawable, set_maskRGB)(struct s_object *self, unsigned int red, unsigned int green, unsigned int blue) {
  d_war(e_log_level_ever, "'set_maskRGB' method has not been implemented yet");
  return self;
}
d_define_method(drawable, set_maskA)(struct s_object *self, unsigned int alpha) {
  d_war(e_log_level_ever, "'set_maskA' method has not been implemented yet");
  return self;
}
d_define_method(drawable, set_blend)(struct s_object *self, enum e_drawable_blends blend) {
  d_war(e_log_level_ever, "'set_blend' method has not been implemented yet");
  return self;
}
d_define_method(drawable, normalize_scale)(struct s_object *self, double reference_w, double reference_h, double offset_x, double offset_y, double focus_x,
    double focus_y, double current_w, double current_h, double zoom) {
  d_using(drawable);
  struct s_square_attributes *square_attributes;
  double this_x, this_y, this_w, this_h, this_center_x, this_center_y, new_x, new_y, new_w, new_h, new_center_x, new_center_y;
  d_call(&(drawable_attributes->point_destination), m_point_get, &this_x, &this_y);
  d_call(&(drawable_attributes->point_dimension), m_point_get, &this_w, &this_h);
  d_call(&(drawable_attributes->point_center), m_point_get, &this_center_x, &this_center_y);
  if ((drawable_attributes->flags & e_drawable_kind_do_not_normalize_environment_zoom) != e_drawable_kind_do_not_normalize_environment_zoom) {
    /* global zoom */
    new_x = focus_x - ((focus_x - this_x) * zoom);
    new_y = focus_y - ((focus_y - this_y) * zoom);
    new_center_x = this_center_x * zoom;
    new_center_y = this_center_y * zoom;
    new_w = this_w * zoom;
    new_h = this_h * zoom;
  } else {
    new_x = this_x;
    new_y = this_y;
    new_center_x = this_center_x;
    new_center_y = this_center_y;
    new_w = this_w;
    new_h = this_h;
  }
  if ((drawable_attributes->flags & e_drawable_kind_do_not_normalize_local_zoom) != e_drawable_kind_do_not_normalize_local_zoom) {
    /* local zoom */
    new_x = (new_x + new_center_x) - (new_center_x * drawable_attributes->zoom);
    new_y = (new_y + new_center_y) - (new_center_y * drawable_attributes->zoom);
    new_center_x = new_center_x * drawable_attributes->zoom;
    new_center_y = new_center_y * drawable_attributes->zoom;
    new_w = new_w * drawable_attributes->zoom;
    new_h = new_h * drawable_attributes->zoom;
  }
  if ((drawable_attributes->flags & e_drawable_kind_do_not_normalize_reference_ratio) != e_drawable_kind_do_not_normalize_reference_ratio) {
    /* screen scale, in respect of the applied reference */
    new_center_x = (new_center_x * current_w) / reference_w;
    new_center_y = (new_center_y * current_h) / reference_h;
    new_x = (new_x * current_w) / reference_w;
    new_y = (new_y * current_h) / reference_h;
    new_w = (new_w * current_w) / reference_w;
    new_h = (new_h * current_h) / reference_h;
  }
  if ((drawable_attributes->flags & e_drawable_kind_do_not_normalize_camera) != e_drawable_kind_do_not_normalize_camera) {
    /* camera offset */
    new_x = new_x - offset_x;
    new_y = new_y - offset_y;
  }
  d_call(&(drawable_attributes->point_normalized_destination), m_point_set_x, new_x);
  d_call(&(drawable_attributes->point_normalized_destination), m_point_set_y, new_y);
  d_call(&(drawable_attributes->point_normalized_dimension), m_point_set_x, new_w);
  d_call(&(drawable_attributes->point_normalized_dimension), m_point_set_y, new_h);
  d_call(&(drawable_attributes->point_normalized_center), m_point_set_x, new_center_x);
  d_call(&(drawable_attributes->point_normalized_center), m_point_set_y, new_center_y);
  /* update the collision box */
  d_assert((square_attributes = d_cast(&(drawable_attributes->square_collision_box), square)));
  square_attributes->top_left_x = new_x;
  square_attributes->top_left_y = new_y;
  square_attributes->bottom_right_x = (new_x + new_w);
  square_attributes->bottom_right_y = (new_y + new_h);
  square_attributes->angle = drawable_attributes->angle;
  square_attributes->center_x = (new_center_x + new_x);
  square_attributes->center_y = (new_center_y + new_y);
  square_attributes->normalized = d_false;
  d_call(&(drawable_attributes->square_collision_box), m_square_normalize, NULL);
  return d_call(self, m_drawable_is_visible, current_w, current_h);
}
d_define_method(drawable, keep_scale)(struct s_object *self, double current_w, double current_h) {
  d_using(drawable);
  struct s_square_attributes *square_attributes;
  double this_x, this_y, this_w, this_h, this_center_x, this_center_y;
  d_call(&(drawable_attributes->point_destination), m_point_get, &this_x, &this_y);
  d_call(&(drawable_attributes->point_dimension), m_point_get, &this_w, &this_h);
  d_call(&(drawable_attributes->point_center), m_point_get, &this_center_x, &this_center_y);
  d_call(&(drawable_attributes->point_normalized_destination), m_point_set_point, &(drawable_attributes->point_destination));
  d_call(&(drawable_attributes->point_normalized_dimension), m_point_set_point, &(drawable_attributes->point_dimension));
  d_call(&(drawable_attributes->point_normalized_center), m_point_set_point, &(drawable_attributes->point_center));
  d_assert((square_attributes = d_cast(&(drawable_attributes->square_collision_box), square)));
  square_attributes->top_left_x = this_x;
  square_attributes->top_left_y = this_y;
  square_attributes->bottom_right_x = (this_x + this_w);
  square_attributes->bottom_right_y = (this_y + this_h);
  square_attributes->angle = drawable_attributes->angle;
  square_attributes->center_x = (this_center_x + this_x);
  square_attributes->center_y = (this_center_y + this_x);
  square_attributes->normalized = d_false;
  d_call(&(drawable_attributes->square_collision_box), m_square_normalize, NULL);
  return d_call(self, m_drawable_is_visible, current_w, current_h);
}
d_define_method(drawable, is_visible)(struct s_object *self, double current_w, double current_h) {
  d_using(drawable);
  struct s_object *result = self;
  double position_x, position_y, width, height, distance_object, diagonal_screen, diagonal_object;
  if ((drawable_attributes->flags & e_drawable_kind_hidden) != e_drawable_kind_hidden) {
    if ((drawable_attributes->flags & e_drawable_kind_force_visibility) != e_drawable_kind_force_visibility) {
      d_call(&(drawable_attributes->point_normalized_destination), m_point_get, &position_x, &position_y);
      d_call(&(drawable_attributes->point_normalized_dimension), m_point_get, &width, &height);
      distance_object = d_point_square_distance((position_x + (width/2.0)), (position_y + (height / 2.0)), (current_w / 2.0),
          (current_h / 2.0));
      diagonal_screen = (d_math_square(current_w) + d_math_square(current_h));
      diagonal_object = (d_math_square(width) + d_math_square(height));
      if (distance_object > ((diagonal_screen / 2.0) + (diagonal_object / 2.0)))
        result = NULL;
    }
  } else
    result = NULL;
  return result;
}
d_define_method(drawable, set_position)(struct s_object *self, double x, double y) {
  d_using(drawable);
  d_call(&(drawable_attributes->point_destination), m_point_set_x, x);
  d_call(&(drawable_attributes->point_destination), m_point_set_y, y);
  return self;
}
d_define_method(drawable, set_position_x)(struct s_object *self, double x) {
  d_using(drawable);
  d_call(&(drawable_attributes->point_destination), m_point_set_x, x);
  return self;
}
d_define_method(drawable, set_position_y)(struct s_object *self, double y) {
  d_using(drawable);
  d_call(&(drawable_attributes->point_destination), m_point_set_y, y);
  return self;
}
d_define_method(drawable, get_position)(struct s_object *self, double *x, double *y) {
  d_using(drawable);
  d_call(&(drawable_attributes->point_destination), m_point_get, x, y);
  return self;
}
d_define_method(drawable, get_scaled_position)(struct s_object *self, double *x, double *y) {
  d_using(drawable);
  d_call(&(drawable_attributes->point_normalized_destination), m_point_get, x, y);
  return self;
}
d_define_method(drawable, get_scaled_center)(struct s_object *self, double *x, double *y) {
  d_using(drawable);
  d_call(&(drawable_attributes->point_normalized_center), m_point_get, x, y);
  return self;
}
d_define_method(drawable, set_dimension)(struct s_object *self, double w, double h) {
  d_using(drawable);
  d_call(&(drawable_attributes->point_dimension), m_point_set_x, w);
  d_call(&(drawable_attributes->point_dimension), m_point_set_y, h);
  return self;
}
d_define_method(drawable, set_dimension_w)(struct s_object *self, double w) {
  d_using(drawable);
  d_call(&(drawable_attributes->point_dimension), m_point_set_x, w);
  return self;
}
d_define_method(drawable, set_dimension_h)(struct s_object *self, double h) {
  d_using(drawable);
  d_call(&(drawable_attributes->point_dimension), m_point_set_y, h);
  return self;
}
d_define_method(drawable, get_dimension)(struct s_object *self, double *w, double *h) {
  d_using(drawable);
  d_call(&(drawable_attributes->point_dimension), m_point_get, w, h);
  return self;
}
d_define_method(drawable, get_scaled_dimension)(struct s_object *self, double *w, double *h) {
  d_using(drawable);
  d_call(&(drawable_attributes->point_normalized_dimension), m_point_get, w, h);
  return self;
}
d_define_method(drawable, get_principal_point)(struct s_object *self, double *x, double *y) {
  d_using(drawable);
  double position_x, position_y, width, height;
  d_call(&(drawable_attributes->point_destination), m_point_get, &position_x, &position_y);
  d_call(&(drawable_attributes->point_dimension), m_point_get, &width, &height);
  *x = (position_x + (width / 2.0));
  *y = (position_y + (height / 2.0));
  return self;
}
d_define_method(drawable, get_scaled_principal_point)(struct s_object *self, double *x, double *y) {
  d_using(drawable);
  double position_x, position_y, width, height;
  d_call(&(drawable_attributes->point_normalized_destination), m_point_get, &position_x, &position_y);
  d_call(&(drawable_attributes->point_normalized_dimension), m_point_get, &width, &height);
  *x = (position_x + (width / 2.0));
  *y = (position_y + (height / 2.0));
  return self;
}
d_define_method(drawable, set_center)(struct s_object *self, double x, double y) {
  d_using(drawable);
  d_call(&(drawable_attributes->point_center), m_point_set_x, x);
  d_call(&(drawable_attributes->point_center), m_point_set_y, y);
  return self;
}
d_define_method(drawable, set_center_alignment)(struct s_object *self, enum e_drawable_alignments alignment) {
  d_using(drawable);
  double new_center_x = 0, new_center_y = 0, current_w, current_h;
  d_call(&(drawable_attributes->point_dimension), m_point_get, &current_w, &current_h);
  switch (alignment) {
    case e_drawable_alignment_centered:
      new_center_x = (current_w / 2.0);
      new_center_y = (current_h / 2.0);
      break;
    case e_drawable_alignment_bottom_right:
      new_center_y = current_h;
    case e_drawable_alignment_top_right:
      new_center_x = current_w;
      break;
    case e_drawable_alignment_bottom_left:
      new_center_y = current_h;
    default:
      break;
  }
  d_call(&(drawable_attributes->point_center), m_point_set_x, new_center_x);
  d_call(&(drawable_attributes->point_center), m_point_set_y, new_center_y);
  return self;
}
d_define_method(drawable, get_center)(struct s_object *self, double *x, double *y) {
  d_using(drawable);
  d_call(&(drawable_attributes->point_center), m_point_get, x, y);
  return self;
}
d_define_method(drawable, set_angle)(struct s_object *self, double angle) {
  d_using(drawable);
  drawable_attributes->angle = fmod(angle, 360.0);
  return self;
}
d_define_method(drawable, get_angle)(struct s_object *self, double *angle) {
  d_using(drawable);
  *angle = drawable_attributes->angle;
  return self;
}
d_define_method(drawable, set_zoom)(struct s_object *self, double zoom) {
  d_using(drawable);
  drawable_attributes->zoom = zoom;
  return self;
}
d_define_method(drawable, get_zoom)(struct s_object *self, double *zoom) {
  d_using(drawable);
  *zoom = drawable_attributes->zoom;
  return self;
}
d_define_method(drawable, flip)(struct s_object *self, enum e_drawable_flips flip) {
  d_using(drawable);
  drawable_attributes->flip = flip;
  return self;
}
d_define_method(drawable, set_flags)(struct s_object *self, int flags) {
  d_using(drawable);
  drawable_attributes->flags = flags;
  return self;
}
d_define_method(drawable, add_flags)(struct s_object *self, int flags) {
  d_using(drawable);
  drawable_attributes->flags |= flags;
  return self;
}
d_define_method(drawable, get_flags)(struct s_object *self) {
  d_using(drawable);
  d_cast_return(drawable_attributes->flags);
}
d_define_method(drawable, delete)(struct s_object *self, struct s_drawable_attributes *attributes) {
  d_delete(&(attributes->point_destination));
  d_delete(&(attributes->point_normalized_destination));
  d_delete(&(attributes->point_dimension));
  d_delete(&(attributes->point_normalized_dimension));
  d_delete(&(attributes->point_center));
  d_delete(&(attributes->point_normalized_center));
  d_delete(&(attributes->square_collision_box));
  return NULL;
}
d_define_class(drawable) {d_hook_method(drawable, e_flag_public, copy_geometry),
  d_hook_method(drawable, e_flag_public, draw),
  d_hook_method(drawable, e_flag_public, draw_contour),
  d_hook_method(drawable, e_flag_public, set_maskRGB),
  d_hook_method(drawable, e_flag_public, set_maskA),
  d_hook_method(drawable, e_flag_public, set_blend),
  d_hook_method(drawable, e_flag_public, normalize_scale),
  d_hook_method(drawable, e_flag_public, keep_scale),
  d_hook_method(drawable, e_flag_public, is_visible),
  d_hook_method(drawable, e_flag_public, set_position),
  d_hook_method(drawable, e_flag_public, set_position_x),
  d_hook_method(drawable, e_flag_public, set_position_y),
  d_hook_method(drawable, e_flag_public, get_position),
  d_hook_method(drawable, e_flag_public, get_scaled_position),
  d_hook_method(drawable, e_flag_public, get_scaled_center),
  d_hook_method(drawable, e_flag_public, set_dimension),
  d_hook_method(drawable, e_flag_public, set_dimension_w),
  d_hook_method(drawable, e_flag_public, set_dimension_h),
  d_hook_method(drawable, e_flag_public, get_dimension),
  d_hook_method(drawable, e_flag_public, get_scaled_dimension),
  d_hook_method(drawable, e_flag_public, get_principal_point),
  d_hook_method(drawable, e_flag_public, get_scaled_principal_point),
  d_hook_method(drawable, e_flag_public, set_center),
  d_hook_method(drawable, e_flag_public, set_center_alignment),
  d_hook_method(drawable, e_flag_public, get_center),
  d_hook_method(drawable, e_flag_public, set_angle),
  d_hook_method(drawable, e_flag_public, get_angle),
  d_hook_method(drawable, e_flag_public, set_zoom),
  d_hook_method(drawable, e_flag_public, get_zoom),
  d_hook_method(drawable, e_flag_public, flip),
  d_hook_method(drawable, e_flag_public, set_flags),
  d_hook_method(drawable, e_flag_public, add_flags),
  d_hook_method(drawable, e_flag_public, get_flags),
  d_hook_delete(drawable),
  d_hook_method_tail};
