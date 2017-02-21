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
#ifndef miranda_object_resources_h
#define miranda_object_resources_h
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#ifdef __APPLE__
#include <sys/syslimits.h>
#endif
#include "stream.obj.h"
#define d_resources_key_size 64
#define d_resources_file_default_permission 0766
#define d_resources_folder_separator '/'
typedef enum e_resources_types {
    e_resources_type_common,
    e_resources_type_read,
    e_resources_type_write
} e_resources_types;
typedef struct s_resources_node {
    char key[d_resources_key_size], path[PATH_MAX];
    struct s_object *stream_file; /* read mode */
    time_t last_timestamp;
} s_resources_node;
d_declare_class(resources) {
    struct s_attributes head;
    char extensions[PATH_MAX], path[PATH_MAX];
    struct s_hash_table *nodes;
    struct s_resources_node *default_template;
} d_declare_class_tail(resources);
struct s_resources_attributes *p_resources_alloc(struct s_object *self);
extern t_hash_value p_resources_calculate(char *key);
extern void p_resources_scan_free(struct s_resources_node *node);
extern struct s_resources_node *f_resources_scan(const char *directory, const char *extensions, struct s_hash_table *nodes);
extern struct s_object *f_resources_new(struct s_object *self, struct s_object *string_path, const char *extensions);
extern struct s_object *f_resources_new_template(struct s_object *self, struct s_object *string_directory_path, struct s_object *string_template_path,
        const char *extensions);
d_declare_method(resources, reload)(struct s_object *self);
d_declare_method(resources, get)(struct s_object *self, const char *key);
d_declare_method(resources, open_stream)(struct s_object *self, struct s_resources_node *current_node, enum e_resources_types type);
d_declare_method(resources, get_stream)(struct s_object *self, const char *key, enum e_resources_types type);
d_declare_method(resources, get_stream_strict)(struct s_object *self, const char *key, enum e_resources_types type);
d_declare_method(resources, del_stream)(struct s_object *self, const char *key, t_boolean destroy);
d_declare_method(resources, delete)(struct s_object *self, struct s_resources_attributes *attributes);
#endif

