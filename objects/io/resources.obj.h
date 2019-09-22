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
#ifndef miranda_object_resources_h
#define miranda_object_resources_h
#include <sys/stat.h>
#include <sys/errno.h>
#include <unistd.h>
#include <dirent.h>
#ifdef __APPLE__
#include <sys/syslimits.h>
#endif
#include "stream.obj.h"
#include "../../endian.local.h"
#define d_resources_key_size 64
#define d_resources_extensions_size 128
#define d_resources_path_size 1024
#define d_resources_magic_sequence_byte1 0x7a
#define d_resources_magic_sequence_byte2 0x77
#define d_resources_magic_sequence_byte3 0x01
#define d_resources_magic_sequence_byte4 0xa0
#define d_resources_file_default_permission 0766
#define d_resources_folder_separator '/'
#define d_resources_stream_size 32
typedef enum e_resources_types {
  e_resources_type_common,
  e_resources_type_read,
  e_resources_type_write
} e_resources_types;
typedef struct s_resources_node {
  d_list_node_head;
  char key[d_resources_key_size], path[d_resources_path_size];
  struct s_object *stream_file; /* read mode */
  time_t last_timestamp;
} s_resources_node;
#pragma pack(push, 1)
typedef struct s_resources_header {
  unsigned char magic_sequence_byte1, magic_sequence_byte2, magic_sequence_byte3, magic_sequence_byte4;
  char extensions[d_resources_extensions_size], path[d_resources_path_size];
  uint32_t entries;
} s_resources_header;
typedef struct s_resources_block_header {
  char key[d_resources_key_size];
  unsigned char template;
  uint32_t size;
} s_resources_block_header;
#pragma pack(pop)
d_declare_class(resources) {
  struct s_attributes head;
  char extensions[d_resources_extensions_size], path[d_resources_path_size];
  struct s_hash_table *nodes;
  struct s_resources_node *default_template;
  struct s_list open_streams;
  t_boolean destroy_content;
} d_declare_class_tail(resources);
struct s_resources_attributes *p_resources_alloc(struct s_object *self);
extern t_hash_value p_resources_calculate(char *key);
extern void p_resources_scan_free(struct s_list *open_streams, struct s_resources_node *node);
extern struct s_resources_node *f_resources_scan(struct s_list *open_streams, const char *directory, const char *extensions, struct s_hash_table *nodes);
extern struct s_object *f_resources_new(struct s_object *self, struct s_object *string_path, const char *extensions);
extern struct s_object *f_resources_new_template(struct s_object *self, struct s_object *string_directory_path,
  struct s_object *string_template_path, const char *extensions);
extern struct s_object *f_resources_new_inflate(struct s_object *self, struct s_object *datafile_stream);
d_declare_method(resources, reload)(struct s_object *self);
d_declare_method(resources, deflate)(struct s_object *self, struct s_object *string_name);
d_declare_method(resources, get)(struct s_object *self, const char *key);
d_declare_method(resources, open_stream)(struct s_object *self, struct s_resources_node *current_node, enum e_resources_types type);
d_declare_method(resources, get_stream)(struct s_object *self, const char *key, enum e_resources_types type);
d_declare_method(resources, get_stream_strict)(struct s_object *self, const char *key, enum e_resources_types type);
d_declare_method(resources, del_stream)(struct s_object *self, const char *key, t_boolean destroy);
d_declare_method(resources, delete)(struct s_object *self, struct s_resources_attributes *attributes);
#endif

