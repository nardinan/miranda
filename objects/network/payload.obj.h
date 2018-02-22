/*
 * miranda
 * Copyright (C) 2017 Andrea Nardinocchi (andrea@nardinan.it)
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
#ifndef miranda_object_payload_h
#define miranda_object_payload_h
#include "../object.h"
#include "../memory.obj.h"
#include "../mutex.obj.h"
#include "../../endian.local.h"
#define d_payload_chunk 128
#pragma pack(push, 1)
typedef struct s_payload_header {
  uint16_t signature_face;
  uint32_t payload_size;
  unsigned char payload_type, payload_version;
  uint16_t signature_dead;
} s_payload_header;
#pragma pack(pop)
d_declare_class(payload) {
  struct s_attributes head;
  unsigned char *buffer, *payload, type;
  size_t buffer_size, buffer_shift, payload_size;
} d_declare_class_tail(payload);
extern struct s_object *f_payload_new(struct s_object *self);
d_declare_method(payload, scan)(struct s_object *self);
d_declare_method(payload, push)(struct s_object *self, unsigned char *buffer, size_t size);
d_declare_method(payload, get)(struct s_object *self, size_t *payload_size, unsigned char *type);
d_declare_method(payload, delete)(struct s_object *self, struct s_payload_attributes *attributes);
#endif

