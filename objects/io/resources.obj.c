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
#include "resources.obj.h"
struct s_resources_attributes *p_resources_alloc(struct s_object *self) {
  struct s_resources_attributes *result = d_prepare(self, resources);
  f_memory_new(self);  /* inherit */
  f_mutex_new(self);  /* inherit */
  return result;
}
t_hash_value p_resources_calculate(char *key) {
  t_hash_value result = 5381;
  char *pointer = key;
  while (*pointer) {
    result = ((result << 5) + result) + *pointer;
    pointer++;
  }
  return result;
}
void p_resources_scan_free(struct s_list *open_streams, struct s_resources_node *node) {
  if (node->stream_file) {
    f_list_delete(open_streams, (struct s_list_node *) node);
    d_delete(node->stream_file);
    node->stream_file = NULL;
  }
}
struct s_resources_node *f_resources_scan(struct s_list *open_streams, const char *directory, const char *extensions, struct s_hash_table *nodes) {
  DIR *descriptor;
  struct dirent *directory_entry;
  char local_path[PATH_MAX], *current_key, *current_extension, *pointer_begin, *pointer_end;
  struct s_resources_node *current_node = NULL;
  struct s_hash_bucket old_value;
  size_t length;
  if ((descriptor = opendir(directory))) {
    while ((directory_entry = readdir(descriptor)))
      if (directory_entry->d_name[0] != '.') {
        snprintf(local_path, PATH_MAX, "%s%c%s", directory, d_resources_folder_separator, directory_entry->d_name);
        f_resources_scan(open_streams, local_path, extensions, nodes);
      }
    closedir(descriptor);
  } else if ((current_extension = strrchr(directory, '.')) && (strstr(extensions, current_extension))) {
    if ((current_node = (struct s_resources_node *) d_malloc(sizeof(struct s_resources_node)))) {
      strcpy(current_node->path, directory);
      if (!(pointer_begin = strrchr(directory, d_resources_folder_separator)))
        pointer_begin = (char *) directory;
      if (!(pointer_end = strrchr(directory, '.')))
        pointer_end = (char *) directory + f_string_strlen(directory);
      while (*pointer_begin == d_resources_folder_separator)
        pointer_begin++;
      if (pointer_begin < pointer_end) {
        length = (pointer_end - pointer_begin);
        strncpy(current_node->key, pointer_begin, (d_resources_key_size < length) ? d_resources_key_size : length);
        if (nodes) {
          if ((current_key = (char *) d_malloc(length + 1))) {
            strncpy(current_key, pointer_begin, length);
            if ((f_hash_insert(nodes, current_key, current_node, d_true, &old_value)))
              if (old_value.kind == e_hash_kind_fill) {
                p_resources_scan_free(open_streams, (struct s_resources_node *) old_value.value);
                d_free(old_value.value);
                d_free(current_key);
              }
          } else
            d_die(d_error_malloc);
        }
      }
    } else
      d_die(d_error_malloc);
  }
  return current_node;
}
struct s_object *f_resources_new(struct s_object *self, struct s_object *string_path, const char *extensions) {
  return f_resources_new_template(self, string_path, NULL, extensions);
}
struct s_object *
f_resources_new_template(struct s_object *self, struct s_object *string_directory_path, struct s_object *string_template_path, const char *extensions) {
  struct s_resources_attributes *attributes = p_resources_alloc(self);
  strncpy(attributes->extensions, extensions, PATH_MAX);
  strncpy(attributes->path, d_string_cstring(string_directory_path), PATH_MAX);
  f_hash_init(&(attributes->nodes), (t_hash_compare *) &f_string_strcmp, (t_hash_calculate *) &p_resources_calculate);
  memset(&(attributes->open_streams), 0, sizeof(struct s_list));
  f_resources_scan(&(attributes->open_streams), d_string_cstring(string_directory_path), extensions, attributes->nodes);
  if (string_template_path)
    attributes->default_template = f_resources_scan(&(attributes->open_streams), d_string_cstring(string_template_path), extensions, NULL);
  return self;
}
d_define_method(resources, reload)(struct s_object *self) {
  d_using(resources);
  struct s_resources_node *node;
  struct s_hash_bucket *current_item, old_content;
  t_hash_value elements = (resources_attributes->nodes->mask + 1), index;
  for (index = 0; index < elements; ++index) {
    current_item = &(resources_attributes->nodes->table[index]);
    if (current_item->kind == e_hash_kind_fill) {
      f_hash_delete(resources_attributes->nodes, current_item->key, &old_content);
      if ((node = (struct s_resources_node *) old_content.value)) {
        p_resources_scan_free(&(resources_attributes->open_streams), node);
        d_free(node);
      }
      d_free(old_content.key);
    }
  }
  f_resources_scan(&(resources_attributes->open_streams), resources_attributes->path, resources_attributes->extensions, resources_attributes->nodes);
  return self;
}
d_define_method(resources, get)(struct s_object *self, const char *key) {
  d_using(resources);
  d_cast_return(f_hash_get(resources_attributes->nodes, (void *) key));
}
d_define_method(resources, open_stream)(struct s_object *self, struct s_resources_node *current_node, enum e_resources_types type) {
  d_using(resources);
  struct s_object *stream_file = NULL;
  struct s_object *string_path = NULL;
  struct stat informations;
  struct s_resources_node *last_node;
  string_path = f_string_new_constant(d_new(string), current_node->path);
  switch (type) {
    case e_resources_type_common:
      stat(current_node->path, &informations);
      if ((current_node->stream_file) && (current_node->last_timestamp != informations.st_mtime)) {
        d_delete(current_node->stream_file);
        current_node->stream_file = NULL;
        f_list_delete(&(resources_attributes->open_streams), (struct s_list_node *) current_node);
      }
      if (!current_node->stream_file) {
        current_node->last_timestamp = informations.st_mtime;
        current_node->stream_file = f_stream_new_file(d_new(stream), string_path, "r", d_resources_file_default_permission);
        if (resources_attributes->open_streams.fill > d_resources_stream_size)
          if ((last_node = (struct s_resources_node *) resources_attributes->open_streams.head)) {
            if (last_node->stream_file) {
              d_delete(last_node->stream_file);
              last_node->stream_file = NULL;
            }
            f_list_delete(&(resources_attributes->open_streams), (struct s_list_node *) last_node);
          }
        f_list_append(&(resources_attributes->open_streams), (struct s_list_node *) current_node, e_list_insert_tail);
      } else
        d_call(current_node->stream_file, m_stream_seek, 0, e_stream_seek_begin, NULL);
      stream_file = current_node->stream_file;
      break;
    case e_resources_type_read:
      stream_file = f_stream_new_file(d_new(stream), string_path, "r", d_resources_file_default_permission);
      break;
    case e_resources_type_write:
      stream_file = f_stream_new_file(d_new(stream), string_path, "w", d_resources_file_default_permission);
      break;
  }
  d_delete(string_path);
  return stream_file;
}
d_define_method(resources, get_stream)(struct s_object *self, const char *key, enum e_resources_types type) {
  d_using(resources);
  struct s_resources_node *current_node;
  struct s_object *result = NULL;
  if ((current_node = (struct s_resources_node *) d_call(self, m_resources_get, key)) || (current_node = resources_attributes->default_template))
    result = d_call(self, m_resources_open_stream, current_node, type);
  return result;
}
d_define_method(resources, get_stream_strict)(struct s_object *self, const char *key, enum e_resources_types type) {
  struct s_resources_node *current_node;
  struct s_object *result = NULL;
  if ((current_node = (struct s_resources_node *) d_call(self, m_resources_get, key)))
    result = d_call(self, m_resources_open_stream, current_node, type);
  return result;
}
d_define_method(resources, del_stream)(struct s_object *self, const char *key, t_boolean destroy) {
  d_using(resources);
  struct s_resources_node *current_node;
  struct s_hash_bucket old_content;
  f_hash_delete(resources_attributes->nodes, (void *) key, &old_content);
  if (old_content.kind == e_hash_kind_fill) {
    if ((current_node = (struct s_resources_node *) old_content.value)) {
      p_resources_scan_free(&(resources_attributes->open_streams), current_node);
      if (destroy)
        remove(current_node->path);
      d_free(current_node);
    }
    d_free(old_content.key);
  }
  return self;
}
d_define_method(resources, delete)(struct s_object *self, struct s_resources_attributes *attributes) {
  struct s_resources_node *node;
  struct s_hash_bucket *current_item, old_content;
  t_hash_value elements = (attributes->nodes->mask + 1), index;
  for (index = 0; index < elements; ++index) {
    current_item = &(attributes->nodes->table[index]);
    if (current_item->kind == e_hash_kind_fill) {
      f_hash_delete(attributes->nodes, current_item->key, &old_content);
      if ((node = (struct s_resources_node *) old_content.value)) {
        p_resources_scan_free(&(attributes->open_streams), node);
        d_free(node);
      }
      d_free(old_content.key);
    }
  }
  f_hash_destroy(&(attributes->nodes));
  if (attributes->default_template) {
    p_resources_scan_free(&(attributes->open_streams), attributes->default_template);
    d_free(attributes->default_template);
  }
  return NULL;
}
d_define_class(resources) {d_hook_method(resources, e_flag_public, reload),
                           d_hook_method(resources, e_flag_private, get),
                           d_hook_method(resources, e_flag_private, open_stream),
                           d_hook_method(resources, e_flag_public, get_stream),
                           d_hook_method(resources, e_flag_public, get_stream_strict),
                           d_hook_method(resources, e_flag_public, del_stream),
                           d_hook_delete(resources),
                           d_hook_method_tail};
