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
#include "animation.obj.h"
#include "camera.obj.h"
struct s_animation_attributes *p_animation_alloc(struct s_object *self) {
  struct s_animation_attributes *result = d_prepare(self, animation);
  f_mutex_new(self);                                                                    /* inherit */
  f_memory_new(self);                                                                   /* inherit */
  f_drawable_new(self, (e_drawable_kind_single | e_drawable_kind_force_visibility));    /* inherit */
  return result;
}
struct s_object *f_animation_new(struct s_object *self, int cycles, double time_ratio) {
  struct s_animation_attributes *attributes = p_animation_alloc(self);
  memset(&(attributes->frames), 0, sizeof(struct s_list));
  attributes->cycles = cycles;
  attributes->time_ratio = time_ratio;
  attributes->status = e_animation_direction_stop;
  return self;
}
d_define_method(animation, append_frame)(struct s_object *self, struct s_object *drawable, double offset_x, double offset_y, double zoom, double time) {
  return d_call(self, m_animation_append_key_frame, drawable, offset_x, offset_y, zoom, time, d_false);
}
d_define_method(animation, append_key_frame)(struct s_object *self, struct s_object *drawable, double offset_x, double offset_y, double zoom, double time,
  t_boolean key) {
  d_using(animation);
  struct s_drawable_attributes *drawable_attributes = d_cast(self, drawable);
  struct s_animation_frame *current_frame;
  if ((current_frame = (struct s_animation_frame *)d_malloc(sizeof(struct s_animation_frame)))) {
    if ((current_frame->drawable = d_retain(drawable))) {
      if (drawable_attributes->last_blend != e_drawable_blend_undefined)
        d_call(current_frame->drawable, m_drawable_set_blend, drawable_attributes->last_blend);
      d_call(current_frame->drawable, m_drawable_set_maskRGB, (unsigned int)drawable_attributes->last_mask_R, (unsigned int)drawable_attributes->last_mask_G,
        (unsigned int)drawable_attributes->last_mask_B);
      d_call(current_frame->drawable, m_drawable_set_maskA, (unsigned int)drawable_attributes->last_mask_A);
    }
    current_frame->offset_x = offset_x;
    current_frame->offset_y = offset_y;
    current_frame->zoom = zoom;
    current_frame->time = time;
    current_frame->key = key;
    f_list_append(&(animation_attributes->frames), (struct s_list_node *)current_frame, e_list_insert_tail);
  } else
    d_die(d_error_malloc);
  return self;
}
d_define_method(animation, set_status)(struct s_object *self, enum e_animation_directions status) {
  d_using(animation);
  enum e_animation_directions previous_status = animation_attributes->status;
  switch ((animation_attributes->status = status)) {
    case e_animation_direction_forward:
      animation_attributes->remaining_cycles = (animation_attributes->cycles - 1);
      break;
    case e_animation_direction_rewind:
      animation_attributes->remaining_cycles = animation_attributes->cycles;
      if ((previous_status != e_animation_direction_stop) ||
          (animation_attributes->current_frame != (struct s_animation_frame *)animation_attributes->frames.head))
        --animation_attributes->remaining_cycles;
      break;
    case e_animation_direction_stop:
      animation_attributes->current_frame = NULL;
    default:
      break;
  }
  return self;
}
d_define_method(animation, get_status)(struct s_object *self) {
  d_using(animation);
  d_cast_return(animation_attributes->status);
}
d_define_method(animation, get_master_frame)(struct s_object *self, t_boolean key_frame) {
  d_using(animation);
  struct s_animation_frame *master_frame = NULL, *current_frame;
  if (key_frame)
    d_foreach(&(animation_attributes->frames), current_frame, struct s_animation_frame)
      if (current_frame->key) {
        master_frame = current_frame;
        break;
      }
  if (!key_frame)
    switch (animation_attributes->status) {
      case e_animation_direction_rewind:
        master_frame = (struct s_animation_frame *)animation_attributes->frames.tail;
        break;
      default:
        master_frame = (struct s_animation_frame *)animation_attributes->frames.head;
    }
  d_cast_return(master_frame);
}
d_define_method(animation, set_callback)(struct s_object *self, t_animation_reboot callback, struct s_object *raw_data) {
  d_using(animation);
  animation_attributes->callback = callback;
  animation_attributes->raw_data = raw_data;
  return self;
}
d_define_method(animation, update_frame)(struct s_object *self) {
  d_using(animation);
  struct s_animation_frame *next_frame = NULL;
  struct s_drawable_attributes *drawable_attributes_self = d_cast(self, drawable), *drawable_attributes_core;
  struct timeval current, elapsed_update;
  double real_elapsed_update, local_position_x, local_position_y, local_center_x, local_center_y, position_x, position_y, center_x, center_y, dimension_w = 0.0,
    dimension_h = 0.0;
  gettimeofday(&current, NULL);
  d_call(&(drawable_attributes_self->point_destination), m_point_get, &local_position_x, &local_position_y);
  d_call(&(drawable_attributes_self->point_center), m_point_get, &local_center_x, &local_center_y);
  if (animation_attributes->current_frame) {
    if ((animation_attributes->status == e_animation_direction_forward) || (animation_attributes->status == e_animation_direction_rewind)) {
      timersub(&current, &(animation_attributes->last_update), &elapsed_update);
      real_elapsed_update = elapsed_update.tv_sec + ((double)(elapsed_update.tv_usec) / 1000000.0);
      if (real_elapsed_update > (animation_attributes->current_frame->time * animation_attributes->time_ratio)) {
        switch (animation_attributes->status) {
          case e_animation_direction_forward:
            next_frame = (struct s_animation_frame *)((struct s_list_node *)animation_attributes->current_frame)->next;
            break;
          case e_animation_direction_rewind:
            next_frame = (struct s_animation_frame *)((struct s_list_node *)animation_attributes->current_frame)->back;
          default:
            break;
        }
        if (!next_frame) {
          if (animation_attributes->remaining_cycles != 0) {
            if (animation_attributes->remaining_cycles > 0)
              --(animation_attributes->remaining_cycles);
            if (animation_attributes->callback)
              animation_attributes->callback(animation_attributes->raw_data);
            animation_attributes->current_frame = (struct s_animation_frame *)d_call(self, m_animation_get_master_frame, d_true);
          } else
            animation_attributes->status = e_animation_direction_stop;
        } else
          animation_attributes->current_frame = next_frame;
        memcpy(&(animation_attributes->last_update), &current, sizeof(struct timeval));
      }
    }
  }
  if (!animation_attributes->current_frame) {
    animation_attributes->current_frame = (struct s_animation_frame *)d_call(self, m_animation_get_master_frame, d_false);
    memcpy(&(animation_attributes->last_update), &current, sizeof(struct timeval));
  }
  if ((animation_attributes->current_frame) && (animation_attributes->current_frame->drawable)) {
    /* inherith the base geometry from the current drawable element */
    drawable_attributes_core = d_cast(animation_attributes->current_frame->drawable, drawable);
    d_call(&(drawable_attributes_core->point_dimension), m_point_get, &dimension_w, &dimension_h);
    center_x = local_center_x - animation_attributes->current_frame->offset_x;
    center_y = local_center_y - animation_attributes->current_frame->offset_y;
    position_x = local_position_x + animation_attributes->current_frame->offset_x;
    position_y = local_position_y + animation_attributes->current_frame->offset_y;
    d_call(animation_attributes->current_frame->drawable, m_drawable_set_position, position_x, position_y);
    d_call(animation_attributes->current_frame->drawable, m_drawable_set_center, center_x, center_y);
    drawable_attributes_core->zoom = (animation_attributes->current_frame->zoom * drawable_attributes_self->zoom);
    drawable_attributes_core->angle = drawable_attributes_self->angle;
    drawable_attributes_core->flip = drawable_attributes_self->flip;
  }
  d_call(self, m_drawable_set_dimension, dimension_w, dimension_h);
  return self;
}
d_define_method_override(animation, draw)(struct s_object *self, struct s_object *environment) {
  d_using(animation);
  struct s_drawable_attributes *drawable_attributes = d_cast(self, drawable);
  struct s_environment_attributes *environment_attributes = d_cast(environment, environment);
  struct s_camera_attributes *camera_attributes = d_cast(environment_attributes->current_camera, camera);
  if ((animation_attributes->current_frame) && (animation_attributes->current_frame->drawable))
    if (d_call(animation_attributes->current_frame->drawable, m_drawable_is_visible, camera_attributes->screen_w, camera_attributes->screen_h))
      while (((intptr_t)d_call(animation_attributes->current_frame->drawable, m_drawable_draw, environment)) == d_drawable_return_continue);
  if ((drawable_attributes->flags & e_drawable_kind_contour) == e_drawable_kind_contour)
    d_call(self, m_drawable_draw_contour, environment);
  d_cast_return(d_drawable_return_last);
}
d_define_method_override(animation, set_maskRGB)(struct s_object *self, unsigned int red, unsigned int green, unsigned int blue) {
  d_using(animation);
  struct s_drawable_attributes *drawable_attributes = d_cast(self, drawable);
  struct s_animation_frame *current_frame;
  drawable_attributes->last_mask_R = red;
  drawable_attributes->last_mask_G = green;
  drawable_attributes->last_mask_B = blue;
  d_foreach(&(animation_attributes->frames), current_frame, struct s_animation_frame)
    d_call(current_frame->drawable, m_drawable_set_maskRGB, (unsigned int)red, (unsigned int)green, (unsigned int)blue);
  return self;
}
d_define_method_override(animation, set_maskA)(struct s_object *self, unsigned int alpha) {
  d_using(animation);
  struct s_drawable_attributes *drawable_attributes = d_cast(self, drawable);
  struct s_animation_frame *current_frame;
  drawable_attributes->last_mask_A = alpha;
  d_foreach(&(animation_attributes->frames), current_frame, struct s_animation_frame)
    d_call(current_frame->drawable, m_drawable_set_maskA, (unsigned int)alpha);
  return self;
}
d_define_method_override(animation, set_blend)(struct s_object *self, enum e_drawable_blends blend) {
  d_using(animation);
  struct s_drawable_attributes *drawable_attributes = d_cast(self, drawable);
  struct s_animation_frame *current_frame;
  drawable_attributes->last_blend = blend;
  d_foreach(&(animation_attributes->frames), current_frame, struct s_animation_frame)
    d_call(current_frame->drawable, m_drawable_set_blend, blend);
  return self;
}
d_define_method_override(animation, normalize_scale)(struct s_object *self, double reference_w, double reference_h, double offset_x, double offset_y,
  double focus_x, double focus_y, double current_w, double current_h, double zoom) {
  d_using(animation);
  struct s_object *result = NULL;
  d_call(self, m_animation_update_frame, NULL);
  if ((animation_attributes->current_frame) && (animation_attributes->current_frame->drawable))
    result = d_call(animation_attributes->current_frame->drawable, m_drawable_normalize_scale, reference_w, reference_h, offset_x, offset_y, focus_x, focus_y,
      current_w, current_h, zoom);
  return result;
}
d_define_method_override(animation, keep_scale)(struct s_object *self, double current_w, double current_h) {
  d_using(animation);
  struct s_object *result = NULL;
  d_call(self, m_animation_update_frame, NULL);
  if ((animation_attributes->current_frame) && (animation_attributes->current_frame->drawable))
    result = d_call(animation_attributes->current_frame->drawable, m_drawable_keep_scale, current_w, current_h);
  return result;
}
d_define_method_override(animation, get_scaled_position)(struct s_object *self, double *x, double *y) {
  d_using(animation);
  struct s_animation_frame *current_frame = animation_attributes->current_frame;
  *x = NAN;
  *y = NAN;
  if (!current_frame)
    current_frame = (struct s_animation_frame *)animation_attributes->frames.head;
  if (current_frame)
    d_call(current_frame->drawable, m_drawable_get_scaled_position, x, y);
  return self;
}
d_define_method_override(animation, get_scaled_center)(struct s_object *self, double *x, double *y) {
  d_using(animation);
  struct s_animation_frame *current_frame = animation_attributes->current_frame;
  *x = NAN;
  *y = NAN;
  if (!current_frame)
    current_frame = (struct s_animation_frame *)animation_attributes->frames.head;
  if (current_frame)
    d_call(current_frame->drawable, m_drawable_get_scaled_center, x, y);
  return self;
}
d_define_method_override(animation, get_dimension)(struct s_object *self, double *w, double *h) {
  d_using(animation);
  struct s_animation_frame *current_frame = animation_attributes->current_frame;
  *w = NAN;
  *h = NAN;
  if (!current_frame)
    current_frame = (struct s_animation_frame *)animation_attributes->frames.head;
  if (current_frame)
    d_call(current_frame->drawable, m_drawable_get_dimension, w, h);
  return self;
}
d_define_method_override(animation, get_scaled_dimension)(struct s_object *self, double *w, double *h) {
  d_using(animation);
  struct s_animation_frame *current_frame = animation_attributes->current_frame;
  *w = NAN;
  *h = NAN;
  if (!current_frame)
    current_frame = (struct s_animation_frame *)animation_attributes->frames.head;
  if (current_frame)
    d_call(current_frame->drawable, m_drawable_get_scaled_dimension, w, h);
  return self;
}
d_define_method(animation, delete)(struct s_object *self, struct s_animation_attributes *attributes) {
  struct s_animation_frame *current_frame;
  while ((current_frame = (struct s_animation_frame *)attributes->frames.head)) {
    if (current_frame->drawable)
      d_delete(current_frame->drawable);
    f_list_delete(&(attributes->frames), (struct s_list_node *)current_frame);
    d_free(current_frame);
  }
  return NULL;
}
d_define_class(animation) {d_hook_method(animation, e_flag_public, append_frame),
                           d_hook_method(animation, e_flag_public, append_key_frame),
                           d_hook_method(animation, e_flag_public, set_status),
                           d_hook_method(animation, e_flag_public, get_status),
                           d_hook_method(animation, e_flag_public, get_master_frame),
                           d_hook_method(animation, e_flag_public, set_callback),
                           d_hook_method(animation, e_flag_private, update_frame),
                           d_hook_method_override(animation, e_flag_public, drawable, draw),
                           d_hook_method_override(animation, e_flag_public, drawable, set_maskRGB),
                           d_hook_method_override(animation, e_flag_public, drawable, set_maskA),
                           d_hook_method_override(animation, e_flag_public, drawable, set_blend),
                           d_hook_method_override(animation, e_flag_public, drawable, normalize_scale),
                           d_hook_method_override(animation, e_flag_public, drawable, keep_scale),
                           d_hook_method_override(animation, e_flag_public, drawable, get_scaled_position),
                           d_hook_method_override(animation, e_flag_public, drawable, get_scaled_center),
                           d_hook_method_override(animation, e_flag_public, drawable, get_dimension),
                           d_hook_method_override(animation, e_flag_public, drawable, get_scaled_dimension),
                           d_hook_delete(animation),
                           d_hook_method_tail};
