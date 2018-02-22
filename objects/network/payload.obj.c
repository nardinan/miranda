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
#include "payload.obj.h"
struct s_payload_attributes *p_payload_alloc(struct s_object *self) {
  struct s_payload_attributes *result = d_prepare(self, payload);
  f_memory_new(self);     /* inherit */
  f_mutex_new(self);      /* inherit */
  return result;
}
struct s_object *f_payload_new(struct s_object *self) {
  struct s_payload_attributes *attributes = p_payload_alloc(self);
  attributes = attributes;
  return self;
}
d_define_method(payload, scan)(struct s_object *self) {
  d_using(payload);
  struct s_payload_header header;
  size_t unchecked_buffer, header_size = sizeof(struct s_payload_header);
  t_boolean misaligned = d_true, allocated = d_false;
  if ((payload_attributes->buffer) && (payload_attributes->buffer_size > 0)) {
    while ((unchecked_buffer = payload_attributes->buffer_size - payload_attributes->buffer_shift) >= header_size) {
      memcpy(&header, payload_attributes->buffer + payload_attributes->buffer_shift, header_size);
      if (f_endian_check() == d_little_endian) {
        f_endian_swap(&header.signature_face, sizeof(uint16_t));
        f_endian_swap(&header.signature_dead, sizeof(uint16_t));
      }
      if ((header.signature_face == (uint16_t) 0xface) && (header.signature_dead == (uint16_t) 0xdead)) {
        misaligned = d_false;
        break;
      } else {
        ++payload_attributes->buffer_shift;
      }
    }
    if (!misaligned) {
      if (f_endian_check() == d_little_endian)
        f_endian_swap(&header.payload_size, sizeof(uint32_t));
      if ((header.payload_size + header_size) <= unchecked_buffer) {
        if (payload_attributes->payload)
          d_free(payload_attributes->payload);
        if ((payload_attributes->payload = (unsigned char *) d_malloc(header.payload_size))) {
          memcpy(payload_attributes->payload, (payload_attributes->buffer + payload_attributes->buffer_shift + header_size), header.payload_size);
          payload_attributes->payload_size = header.payload_size;
          payload_attributes->type = header.payload_type;
        } else
          d_die(d_error_malloc);
        if (payload_attributes->buffer) {
          d_free(payload_attributes->buffer);
          payload_attributes->buffer = NULL;
        }
        payload_attributes->buffer_size = 0;
        payload_attributes->buffer_shift = 0;
        allocated = d_true;
      }
    }
  }
  return (allocated) ? self : NULL;
}
d_define_method(payload, push)(struct s_object *self, unsigned char *buffer, size_t size) {
  d_using(payload);
  unsigned char *new_buffer;
  size_t final_buffer = size, unchecked_buffer;
  struct s_object *result = NULL;
  if (size > 0) {
    if ((payload_attributes->buffer) && (payload_attributes->buffer_size > 0)) {
      if ((unchecked_buffer = payload_attributes->buffer_size - payload_attributes->buffer_shift) > 0) {
        final_buffer = (size + unchecked_buffer);
        if ((new_buffer = (unsigned char *) d_malloc(final_buffer))) {
          memcpy(new_buffer, (payload_attributes->buffer + payload_attributes->buffer_shift), unchecked_buffer);
          memcpy((new_buffer + unchecked_buffer), buffer, size);
        } else
          d_die(d_error_malloc);
      } else if ((new_buffer = (unsigned char *) d_malloc(final_buffer))) {
        memcpy(new_buffer, payload_attributes->buffer, size);
      } else
        d_die(d_error_malloc);
      d_free(payload_attributes->buffer);
      payload_attributes->buffer = new_buffer;
    } else if ((payload_attributes->buffer = (unsigned char *) d_malloc(final_buffer))) {
      memcpy(payload_attributes->buffer, buffer, size);
    }
    payload_attributes->buffer_shift = 0;
    payload_attributes->buffer_size = final_buffer;
    result = d_call(self, m_payload_scan, NULL);
  }
  return result;
}
d_define_method(payload, get)(struct s_object *self, size_t *payload_size, unsigned char *type) {
  d_using(payload);
  *payload_size = payload_attributes->payload_size;
  *type = payload_attributes->type;
  return (struct s_object *) payload_attributes->payload;
}
d_define_method(payload, delete)(struct s_object *self, struct s_payload_attributes *attributes) {
  if (attributes->buffer) {
    d_free(attributes->buffer);
    attributes->buffer = NULL;
  }
  if (attributes->payload) {
    d_free(attributes->payload);
    attributes->payload = NULL;
  }
  return NULL;
}
d_define_class(payload) {d_hook_method(payload, e_flag_private, scan),
                         d_hook_method(payload, e_flag_public, push),
                         d_hook_method(payload, e_flag_public, get),
                         d_hook_delete(payload),
                         d_hook_method_tail};
