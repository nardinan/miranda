/*
 * miranda
 * Copyright (C) 2014 Andrea Nardinocchi (andrea@nardinan.it)
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
#ifndef miranda_object_stream_h
#define miranda_object_stream_h
#include <sys/file.h>
#include <unistd.h>
#include "string.obj.h"
#define d_stream_block_size 5120
#define d_stream_flag_append (O_WRONLY|O_CREAT|O_APPEND)
#define d_stream_flag_truncate (O_WRONLY|O_CREAT|O_TRUNC)
#define d_stream_flag_read (O_RDONLY)
#define d_stream_flag_write_read (O_RDWR|O_CREAT)
d_exception_declare(malformed);
d_exception_declare(unreachable);
d_exception_declare(unsupported);
d_exception_declare(closed);
typedef enum e_stream_seek {
	e_stream_seek_begin,
	e_stream_seek_current,
	e_stream_seek_end
} e_stream_seek;
d_declare_class(stream) {
	struct s_attributes head;
	struct s_object *string_name;
	int descriptor, parameters;
	struct {
		unsigned int supplied:1, opened:1, temporary:1;
	} flags;
} d_declare_class_tail(stream);
struct s_stream_attributes *p_stream_alloc(struct s_object *self);
int p_stream_open_lock(const char *name);
void p_stream_close_unlock(int *descriptor);
extern struct s_object *f_stream_new(struct s_object *self, struct s_object *string_name, int descriptor);
extern struct s_object *f_stream_new_file(struct s_object *self, struct s_object *string_name, const char *action, int permission);
extern struct s_object *f_stream_new_temporary(struct s_object *self, struct s_object *string_name);
d_declare_method(stream, write)(struct s_object *self, unsigned char *raw, size_t size, size_t *written);
d_declare_method(stream, write_string)(struct s_object *self, struct s_object *string, size_t *written);
d_declare_method(stream, write_stream)(struct s_object *self, struct s_object *stream, size_t *written);
d_declare_method(stream, read)(struct s_object *self, unsigned char *raw, size_t size, size_t *readed);
d_declare_method(stream, read_string)(struct s_object *self, struct s_object *string_supplied, size_t size);
d_declare_method(stream, size)(struct s_object *self, size_t *size);
d_declare_method(stream, seek)(struct s_object *self, off_t offset, enum e_stream_seek whence, off_t *moved);
d_declare_method(stream, lock)(struct s_object *self, int lock);
d_declare_method(stream, delete)(struct s_object *self, struct s_stream_attributes *attributes);
d_declare_method(stream, hash)(struct s_object *self, t_hash_value *value);
d_declare_method(stream, compare)(struct s_object *self, struct s_object *other);
#endif

