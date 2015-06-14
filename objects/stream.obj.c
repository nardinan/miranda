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
#include "stream.obj.h"
d_exception_define(malformed, 	5, "malformed action exception");
d_exception_define(unreachable, 6, "unreachable resource exception");
d_exception_define(unsupported, 7, "unsupported action exception");
d_exception_define(closed, 	8, "closed resource exception");
struct s_stream_attributes *p_stream_alloc(struct s_object *self) {
	struct s_stream_attributes *result = d_prepare(self, stream);
	f_memory_new(self); 	/* inherit */
	f_mutex_new(self);	/* inherit */
	return result;
}

int p_stream_open_lock(const char *name) {
	int result = -1, descriptor;
	if ((descriptor = open(name, O_CREAT|O_RDWR, 0666)) >= 0) {
		if (flock(descriptor, LOCK_EX|LOCK_NB) >= 0)
			result = descriptor;
		else
			close(descriptor);
	}
	return result;
}

void p_stream_close_unlock(int *descriptor) {
	flock(*descriptor, LOCK_UN);
	close(*descriptor);
	*descriptor = -1;
}

struct s_object *f_stream_new(struct s_object *self, struct s_object *string_name, int descriptor) {
	struct s_stream_attributes *attributes = p_stream_alloc(self);
	attributes->string_name = d_retain(string_name);
	attributes->descriptor = descriptor;
	attributes->parameters = fcntl(attributes->descriptor, F_GETFL);
	attributes->flags.supplied = d_true;
	attributes->flags.opened = d_true;
	return self;
}

struct s_object *f_stream_new_file(struct s_object *self, struct s_object *string_name, const char *action, int permission) {
	struct s_stream_attributes *attributes = p_stream_alloc(self);
	char buffer[d_string_buffer_size];
	attributes->string_name = d_retain(string_name);
	attributes->parameters = -1;
	switch(action[0]) {
		case 'r':
		case 'R':
			attributes->parameters = d_stream_flag_read;
			if ((action[1] == 'w') || (action[1] == 'W'))
				attributes->parameters = d_stream_flag_write_read;
			break;
		case 'w':
		case 'W':
			attributes->parameters = d_stream_flag_truncate;
			if ((action[1] == 'a') || (action[1] == 'A'))
				attributes->parameters = d_stream_flag_append;
	}
	if (attributes->parameters != -1) {
		if ((attributes->descriptor = open(d_string_cstring(attributes->string_name), attributes->parameters, permission)) > -1)
			attributes->flags.opened = d_true;
		else {
			snprintf(buffer, d_string_buffer_size, "unreachable file %s exception", d_string_cstring(attributes->string_name));
			d_throw(v_exception_unreachable, buffer);
		}
	} else
		d_throw(v_exception_malformed, "malformed action format");
	return self;
}

struct s_object *f_stream_new_temporary(struct s_object *self, struct s_object *string_name) {
	struct s_stream_attributes *attributes = p_stream_alloc(self);
	attributes->string_name = d_retain(string_name);
	attributes->parameters = d_stream_flag_write_read;
	attributes->flags.temporary = d_true;
	if ((attributes->descriptor = mkstemp("magrathea_XXXXXX.tmp")) >= 0)
		attributes->flags.opened = d_true;
	else
		d_throw(v_exception_unreachable, "unreachable temporary file exception");
	return self;
}

d_define_method(stream, write)(struct s_object *self, unsigned char *raw, size_t size, size_t *written) {
	d_using(stream);
	size_t written_local;
	if (stream_attributes->flags.opened) {
		if (((stream_attributes->parameters&O_RDWR) == O_RDWR) || ((stream_attributes->parameters&O_WRONLY) == O_WRONLY)) {
			written_local = write(stream_attributes->descriptor, raw, size);
			if (written)
				*written = written_local;
		} else
			d_throw(v_exception_unsupported, "write in a read-only stream exception");
	} else
		d_throw(v_exception_closed, "write in a closed stream exception");
	return self;
}

d_define_method(stream, write_string)(struct s_object *self, struct s_object *string, size_t *written) {
	size_t dimension;
	d_call(string, m_string_length, &dimension);
	return d_call(self, m_stream_write, (unsigned char *)d_string_cstring(string), dimension, written);
}

d_define_method(stream, write_stream)(struct s_object *self, struct s_object *stream, size_t *written) {
	unsigned char block[d_stream_block_size];
	size_t readed, written_local = 0;
	d_call(stream, m_stream_seek, 0, e_stream_seek_begin, NULL);
	while ((d_call(stream, m_stream_read, block, d_stream_block_size, &readed)) && (readed > 0)) {
		d_call(self, m_stream_write, block, readed, &written_local);
		if (*written)
			*written += written_local;
	}
	return self;
}

d_define_method(stream, read)(struct s_object *self, unsigned char *buffer, size_t size, size_t *readed) {
	d_using(stream);
	size_t readed_local;
	if (stream_attributes->flags.opened) {
		if (((stream_attributes->parameters&O_RDWR) == O_RDWR) || ((stream_attributes->parameters&O_RDONLY) == O_RDONLY)) {
			readed_local = read(stream_attributes->descriptor, buffer, size);
			if (readed)
				*readed = readed_local;
		} else
			d_throw(v_exception_unsupported, "read in a write-only stream exception");
	} else
		d_throw(v_exception_closed, "read in a closed stream exception");
	return self;
}

d_define_method(stream, read_string)(struct s_object *self, struct s_object *string_supplied, size_t size) {
	d_using(stream);
	struct s_string_attributes *string_attributes = NULL;
	char character, buffer[d_stream_block_size];
	size_t readed_local = 0;
	int tail = d_true;
	if (stream_attributes->flags.opened) {
		if (((stream_attributes->parameters&O_RDWR) == O_RDWR) || ((stream_attributes->parameters&O_RDONLY) == O_RDONLY)) {
			while ((readed_local < size) && (read(stream_attributes->descriptor, &character, 1) > 0)) {
				tail = d_false;
				if ((character != '\n') && (character != '\0'))
					buffer[readed_local++] = character;
				else
					break;
			}
			if (!tail) {
				buffer[readed_local] = '\0';
				if (!string_supplied)
					string_supplied = f_string_new_size(d_new(string), buffer, size);
				else {
					string_attributes = d_cast(string_supplied, string);
					strncpy(string_attributes->content, buffer, readed_local);
					string_attributes->content[readed_local] = '\0';
					string_attributes->length = readed_local;
				}
			} else
				string_supplied = NULL;
		} else
			d_throw(v_exception_unsupported, "read in a write-only stream exception");
	} else
		d_throw(v_exception_closed, "read in a closed stream exception");
	return string_supplied;
}

d_define_method(stream, size)(struct s_object *self, size_t *size) {
	d_using(stream);
	off_t offset, current_offset;
	if (stream_attributes->flags.opened) {
		if (((stream_attributes->parameters&O_RDWR) == O_RDWR) || ((stream_attributes->parameters&O_RDONLY) == O_RDONLY)) {
			d_call(self, m_stream_seek, 0, e_stream_seek_current, &current_offset);
			d_call(self, m_stream_seek, 0, e_stream_seek_begin, NULL);
			d_call(self, m_stream_seek, 0, e_stream_seek_end, &offset);
			if (size)
				*size = (size_t)offset;
			d_call(self, m_stream_seek, current_offset, e_stream_seek_begin);
		} else
			d_throw(v_exception_unsupported, "read in a write-only stream exception");
	} else
		d_throw(v_exception_closed, "read in a closed stream exception");
	return self;
}

d_define_method(stream, seek)(struct s_object *self, off_t offset, enum e_stream_seek whence, off_t *moved) {
	d_using(stream);
	int whence_local = SEEK_SET;
	off_t moved_local;
	if (stream_attributes->flags.opened) {
		switch (whence) {
			case e_stream_seek_begin:
				whence_local = SEEK_SET;
				break;
			case e_stream_seek_current:
				whence_local = SEEK_CUR;
				break;
			case e_stream_seek_end:
				whence_local = SEEK_END;
				break;
			default:
				d_throw(v_exception_malformed, "malformed whence format exception");
		}
		moved_local = lseek(stream_attributes->descriptor, offset, whence_local);
		if (moved)
			*moved = moved_local;
	} else
		d_throw(v_exception_closed, "read in a closed stream exception");
	return self;
}

d_define_method(stream, lock)(struct s_object *self, int lock) {
	d_using(stream);
	int flags;
	if (stream_attributes->flags.opened) {
		flags = fcntl(stream_attributes->descriptor, F_GETFL);
		if (lock)
			flags &= ~O_NONBLOCK;
		else
			flags |= O_NONBLOCK;
		if (stream_attributes->parameters != flags)
			if (fcntl(stream_attributes->descriptor, F_SETFL, flags) != -1)
				stream_attributes->parameters = flags;
	} else
		d_throw(v_exception_closed, "read in a closed stream exception");
	return self;
}

d_define_method(stream, delete)(struct s_object *self, struct s_stream_attributes *attributes) {
	if ((!attributes->flags.supplied) && (attributes->flags.opened))
		close(attributes->descriptor);
	if (attributes->string_name)
		d_delete(attributes->string_name);
	return NULL;
}

d_define_class(stream) {
	d_hook_method(stream, e_flag_public, write),
	d_hook_method(stream, e_flag_public, write_string),
	d_hook_method(stream, e_flag_public, write_stream),
	d_hook_method(stream, e_flag_public, read),
	d_hook_method(stream, e_flag_public, read_string),
	d_hook_method(stream, e_flag_public, size),
	d_hook_method(stream, e_flag_public, seek),
	d_hook_method(stream, e_flag_public, lock),
	d_hook_delete(stream),
	d_hook_method_tail
};

