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
  f_memory_new(self);   /* inherit */
  f_mutex_new(self);    /* inherit */
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
    f_list_delete(open_streams, (struct s_list_node *)node);
    d_delete(node->stream_file);
    node->stream_file = NULL;
  }
}
struct s_resources_node *f_resources_scan(struct s_list *open_streams, const char *directory, const char *extensions, struct s_hash_table *nodes) {
  DIR *descriptor;
  struct dirent *directory_entry;
  char local_path[d_resources_path_size], *current_key, *current_extension, *pointer_begin, *pointer_end;
  struct s_resources_node *current_node = NULL;
  struct s_hash_bucket old_value;
  size_t length;
  if ((descriptor = opendir(directory))) {
    if (nodes)
      while ((directory_entry = readdir(descriptor)))
        if (directory_entry->d_name[0] != '.') {
          snprintf(local_path, d_resources_path_size, "%s%c%s", directory, d_resources_folder_separator, directory_entry->d_name);
          f_resources_scan(open_streams, local_path, extensions, nodes);
        }
    closedir(descriptor);
  } else if ((current_extension = strrchr(directory, '.')) && (strstr(extensions, current_extension))) {
    if ((current_node = (struct s_resources_node *)d_malloc(sizeof(struct s_resources_node)))) {
      strcpy(current_node->path, directory);
      if (!(pointer_begin = strrchr(directory, d_resources_folder_separator)))
        pointer_begin = (char *)directory;
      if (!(pointer_end = strrchr(directory, '.')))
        pointer_end = (char *)directory + f_string_strlen(directory);
      while (*pointer_begin == d_resources_folder_separator)
        pointer_begin++;
      if (pointer_begin < pointer_end) {
        length = (pointer_end - pointer_begin);
        strncpy(current_node->key, pointer_begin, (d_resources_key_size < length) ? d_resources_key_size : length);
        if (nodes) {
          if ((current_key = (char *)d_malloc(length + 1))) {
            strncpy(current_key, pointer_begin, length);
            if ((f_hash_insert(nodes, current_key, current_node, d_true, &old_value)))
              if (old_value.kind == e_hash_kind_fill) {
                p_resources_scan_free(open_streams, (struct s_resources_node *)old_value.value);
                d_free(old_value.value);
                d_free(current_key);
              }
          } else
            d_die(d_error_malloc);
        }
      } else
        d_err(e_log_level_low, "inconsistency in the separation of the path '%s' into components (source, file, extension). "
                               "The node will not be considered a resource", directory);
    } else
      d_die(d_error_malloc);
  }
  return current_node;
}
struct s_object *f_resources_new(struct s_object *self, struct s_object *string_path, const char *extensions) {
  return f_resources_new_template(self, string_path, NULL, extensions);
}
struct s_object *f_resources_new_template(struct s_object *self, struct s_object *string_directory_path,
  struct s_object *string_template_path, const char *extensions) {
  struct s_resources_attributes *attributes = p_resources_alloc(self);
  strncpy(attributes->extensions, extensions, d_resources_extensions_size);
  strncpy(attributes->path, d_string_cstring(string_directory_path), d_resources_path_size);
  f_hash_init(&(attributes->nodes), (t_hash_compare *)&f_string_strcmp, (t_hash_calculate *)&p_resources_calculate);
  memset(&(attributes->open_streams), 0, sizeof(struct s_list));
  f_resources_scan(&(attributes->open_streams), d_string_cstring(string_directory_path), extensions, attributes->nodes);
  if (string_template_path)
    attributes->default_template = f_resources_scan(&(attributes->open_streams), d_string_cstring(string_template_path), extensions, NULL);
  return self;
}
struct s_object *f_resources_new_inflate(struct s_object *self, struct s_object *datafile_stream) {
  struct s_resources_attributes *attributes = p_resources_alloc(self);
  struct s_resources_header header;
  struct s_resources_block_header block_header;
  struct s_object *string_path;
  unsigned int index;
  size_t read_bytes, residual_size, block_size;
  struct s_hash_bucket old_value;
  struct s_resources_node *current_node;
  unsigned char *block_data;
  char buffer[d_string_buffer_size], raw_path[d_resources_path_size], *current_key;
  attributes->destroy_content = d_true;
  f_hash_init(&(attributes->nodes), (t_hash_compare *)&f_string_strcmp, (t_hash_calculate *)&p_resources_calculate);
  d_call(datafile_stream, m_stream_size, &residual_size);
  if (residual_size > sizeof(struct s_resources_header)) {
    d_call(datafile_stream, m_stream_read, &header, sizeof(struct s_resources_header), &read_bytes);
    residual_size -= sizeof(struct s_resources_header);
    if (f_endian_check() == d_little_endian)
      f_endian_swap(&(header.entries), sizeof(uint32_t));
    if ((header.magic_sequence_byte1 == d_resources_magic_sequence_byte1) &&
      (header.magic_sequence_byte2 == d_resources_magic_sequence_byte2) &&
      (header.magic_sequence_byte3 == d_resources_magic_sequence_byte3) &&
      (header.magic_sequence_byte4 == d_resources_magic_sequence_byte4)) {
      strncpy(attributes->extensions, header.extensions, d_resources_extensions_size);
      strncpy(attributes->path, header.path, d_resources_path_size);
      for (index = 0; index < header.entries; ++index) {
        d_call(datafile_stream, m_stream_read, &block_header, sizeof(struct s_resources_block_header), &read_bytes);
        if (f_endian_check() == d_little_endian)
          f_endian_swap(&(block_header.size), sizeof(uint32_t));
        if (residual_size >= block_header.size) {
          block_size = block_header.size;
          if ((block_data = (unsigned char *)d_malloc(block_size))) {
            d_call(datafile_stream, m_stream_read, block_data, block_size, NULL);
            snprintf(raw_path, d_resources_path_size, "/tmp/%s.XXXXXX", block_header.key);
            string_path = f_string_new(d_new(string), mktemp(raw_path));
            if ((current_node = (struct s_resources_node *)d_malloc(sizeof(struct s_resources_node)))) {
              strncpy(current_node->path, d_string_cstring(string_path), d_resources_path_size);
              strncpy(current_node->key, block_header.key, d_resources_key_size);
              current_node->stream_file = f_stream_new_file(d_new(stream), string_path, "w", 0777);
              d_call(current_node->stream_file, m_stream_write, block_data, block_size, NULL);
              if ((current_key = (char *)d_malloc(f_string_strlen(current_node->key) + 1))) {
                strncpy(current_key, current_node->key, f_string_strlen(current_node->key));
                if ((f_hash_insert(attributes->nodes, current_key, current_node, d_true, &old_value)))
                  if (old_value.kind == e_hash_kind_fill) {
                    p_resources_scan_free(&(attributes->open_streams), (struct s_resources_node *)old_value.value);
                    d_free(old_value.value);
                    d_free(current_key);
                  }
              } else
                d_die(d_error_malloc);
              f_list_append(&(attributes->open_streams), (struct s_list_node *)current_node, e_list_insert_tail);
            } else
              d_die(d_error_malloc);
            d_delete(string_path);
            d_free(block_data);
          } else
            d_die(d_error_malloc);
          residual_size -= block_header.size;
        } else {
          snprintf(buffer, d_string_buffer_size, "the size of the datafile seems to be shorted than required (%zu residual bytes, VS expected %du)",
                   residual_size, block_header.size);
          d_throw(v_exception_malformed, buffer);
        }
      }
    } else
      d_throw(v_exception_malformed, "the magic sequence used to identify a datafile does not correspond with the sequence here defined");
  } else {
    snprintf(buffer, d_string_buffer_size, "the size of the datafile seems to be shorted than required (%zu residual bytes, VS expected %zu)", residual_size,
             sizeof(struct s_resources_header));
    d_throw(v_exception_malformed, buffer);
  }
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
      if ((node = (struct s_resources_node *)old_content.value)) {
        p_resources_scan_free(&(resources_attributes->open_streams), node);
        d_free(node);
      }
      d_free(old_content.key);
    }
  }
  f_resources_scan(&(resources_attributes->open_streams), resources_attributes->path, resources_attributes->extensions, resources_attributes->nodes);
  return self;
}
d_define_method(resources, deflate)(struct s_object *self, struct s_object *string_name) {
  d_using(resources);
  struct s_resources_node *node;
  struct s_hash_bucket *current_item;
  struct s_object *output_stream;
  struct s_object *stream_file;
  struct s_exception *exception;
  struct s_resources_header header;
  struct s_resources_block_header block_header;
  t_hash_value elements = (resources_attributes->nodes->mask + 1);
  unsigned int index;
  size_t written_bytes, size_file;
  d_try
      {
        output_stream = f_stream_new_file(d_new(stream), string_name, "w", 0777);
        header.magic_sequence_byte1 = d_resources_magic_sequence_byte1;
        header.magic_sequence_byte2 = d_resources_magic_sequence_byte2;
        header.magic_sequence_byte3 = d_resources_magic_sequence_byte3;
        header.magic_sequence_byte4 = d_resources_magic_sequence_byte4;
        strncpy(header.extensions, resources_attributes->extensions, d_resources_extensions_size);
        strncpy(header.path, resources_attributes->path, d_resources_path_size);
        header.entries = elements + ((resources_attributes->default_template)?1:0);
        if (f_endian_check() == d_little_endian)
          f_endian_swap(&(header.entries), sizeof(uint32_t));
        d_call(output_stream, m_stream_write, (unsigned char *)&header, sizeof(struct s_resources_header), &written_bytes);
        for (index = 0; index < elements; ++index) {
          current_item = &(resources_attributes->nodes->table[index]);
          if (current_item->kind == e_hash_kind_fill) {
            if ((node = (struct s_resources_node *)current_item->value)) {
              if ((stream_file = d_call(self, m_resources_open_stream, node, e_resources_type_common))) {
                memset(&block_header, 0, sizeof(struct s_resources_block_header));
                strncpy(block_header.key, (char *)current_item->key, d_resources_key_size);
                d_call(stream_file, m_stream_size, (size_t *)&size_file);
                block_header.size = size_file;
                block_header.template = d_false;
                if (f_endian_check() == d_little_endian)
                  f_endian_swap(&(block_header.size), sizeof(uint32_t));
                d_call(output_stream, m_stream_write, (unsigned char *)&block_header, sizeof(struct s_resources_block_header), &written_bytes);
                d_call(output_stream, m_stream_write_stream, stream_file, &written_bytes);
                d_log(e_log_level_low, "file '%s' has been included in the compressed datafile (%zu bytes written)",
                      block_header.key, written_bytes);
              }
            }
          }
        }
        if (resources_attributes->default_template) {
          memset(&block_header, 0, sizeof(struct s_resources_block_header));
          strncpy(block_header.key, resources_attributes->default_template->key, d_resources_key_size);
          d_call(resources_attributes->default_template->stream_file, m_stream_size, &size_file);
          block_header.size = size_file;
          block_header.template = d_true;
          if (f_endian_check() == d_little_endian)
            f_endian_swap(&(block_header.size), sizeof(uint32_t));
          d_call(output_stream, m_stream_write, (unsigned char *)&block_header, sizeof(struct s_resources_block_header), &written_bytes);
          d_call(output_stream, m_stream_write_stream, resources_attributes->default_template->stream_file, &written_bytes);
          d_log(e_log_level_low, "file '%s', considered as a template, has been included in the compressed datafiles (%zu bytes written)",
                resources_attributes->default_template->key, written_bytes);
        }
        d_delete(output_stream);
      }
    d_catch(exception)
      {
        d_exception_dump(stdout, exception);
        d_raise;
      }
  d_endtry;
  return self;
}
d_define_method(resources, get)(struct s_object *self, const char *key) {
  d_using(resources);
  d_cast_return(f_hash_get(resources_attributes->nodes, (void *)key));
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
        f_list_delete(&(resources_attributes->open_streams), (struct s_list_node *)current_node);
      }
      if (!current_node->stream_file) {
        current_node->last_timestamp = informations.st_mtime;
        current_node->stream_file = f_stream_new_file(d_new(stream), string_path, "r", d_resources_file_default_permission);
        if (resources_attributes->open_streams.fill > d_resources_stream_size)
          if ((last_node = (struct s_resources_node *)resources_attributes->open_streams.head)) {
            if (last_node->stream_file) {
              d_delete(last_node->stream_file);
              last_node->stream_file = NULL;
            }
            f_list_delete(&(resources_attributes->open_streams), (struct s_list_node *)last_node);
          }
        f_list_append(&(resources_attributes->open_streams), (struct s_list_node *)current_node, e_list_insert_tail);
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
  if ((current_node = (struct s_resources_node *)d_call(self, m_resources_get, key)) ||
    (current_node = resources_attributes->default_template))
    result = d_call(self, m_resources_open_stream, current_node, type);
  return result;
}
d_define_method(resources, get_stream_strict)(struct s_object *self, const char *key, enum e_resources_types type) {
  struct s_resources_node *current_node;
  struct s_object *result = NULL;
  if ((current_node = (struct s_resources_node *)d_call(self, m_resources_get, key)))
    result = d_call(self, m_resources_open_stream, current_node, type);
  return result;
}
d_define_method(resources, del_stream)(struct s_object *self, const char *key, t_boolean destroy) {
  d_using(resources);
  struct s_resources_node *current_node;
  struct s_hash_bucket old_content;
  f_hash_delete(resources_attributes->nodes, (void *)key, &old_content);
  if (old_content.kind == e_hash_kind_fill) {
    if ((current_node = (struct s_resources_node *)old_content.value)) {
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
      if ((node = (struct s_resources_node *)old_content.value)) {
        if (attributes->destroy_content)
          if (unlink(node->path) == -1) {
            d_err(e_log_level_high, "seems impossible to delete the file %s uncompressed (errno %s)",
              node->path, strerror(errno));
          }
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
                           d_hook_method(resources, e_flag_public, deflate),
                           d_hook_method(resources, e_flag_private, get),
                           d_hook_method(resources, e_flag_private, open_stream),
                           d_hook_method(resources, e_flag_public, get_stream),
                           d_hook_method(resources, e_flag_public, get_stream_strict),
                           d_hook_method(resources, e_flag_public, del_stream),
                           d_hook_delete(resources),
                           d_hook_method_tail};
