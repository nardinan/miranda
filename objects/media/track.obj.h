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
#ifndef miranda_media_track_h
#define miranda_media_track_h
#include "bitmap.obj.h"
#define d_track_auto_channel -1
#define d_track_infinite_loop -1
#define d_track_default_volume 128
#define d_track_default_fade_delay 500
d_exception_declare(chunk);
d_declare_class(track) {
	struct s_attributes head;
	Mix_Chunk *chunk;
	int channel, next_channel, volume, loops;
	short angle;
	unsigned char distance;
	t_boolean auto_channel;
} d_declare_class_tail(track);
struct s_track_attributes *p_track_alloc(struct s_object *self);
extern struct s_object *f_track_new(struct s_object *self, struct s_object *stream);
extern struct s_object *f_track_new_channel(struct s_object *self, struct s_object *stream, int channel);
d_declare_method(track, set_volume)(struct s_object *self, int volume);
d_declare_method(track, set_channel)(struct s_object *self, int channel);
d_declare_method(track, set_position)(struct s_object *self, short angle, unsigned char distance);
d_declare_method(track, set_loops)(struct s_object *self, int loops);
d_declare_method(track, play)(struct s_object *self, t_boolean restart);
d_declare_method(track, play_fade_in)(struct s_object *self, t_boolean restart, int delay /* milliseconds */);
d_declare_method(track, pause)(struct s_object *self);
d_declare_method(track, stop)(struct s_object *self);
d_declare_method(track, stop_fade_out)(struct s_object *self, int delay /* milliseconds */);
d_declare_method(track, is_playing)(struct s_object *self);
d_declare_method(track, delete)(struct s_object *self, struct s_track_attributes *attributes);
#endif
