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
#include "track.obj.h"
d_exception_define(chunk, 15, "ungenerable chunk exception");
struct s_track_attributes *p_track_alloc(struct s_object *self) {
    struct s_track_attributes *result = d_prepare(self, track);
    f_mutex_new(self);	/* inherit */
    f_memory_new(self);	/* inherit */
    return result;
}

struct s_object *f_track_new(struct s_object *self, struct s_object *stream) {
    return f_track_new_channel(self, stream, d_track_auto_channel);
}

struct s_object *f_track_new_channel(struct s_object *self, struct s_object *stream, int channel) {
    struct s_track_attributes *attributes = p_track_alloc(self);
    struct s_stream_attributes *stream_attributes = d_cast(stream, stream);
    char *memblock, buffer[d_string_buffer_size];
    struct stat file_stats;
    SDL_RWops *tracked_block;
    if (attributes->chunk)
        Mix_FreeChunk(attributes->chunk);
    fstat(stream_attributes->descriptor, &file_stats);
    if ((memblock = mmap(NULL, file_stats.st_size, PROT_READ, MAP_SHARED, stream_attributes->descriptor, 0)) != MAP_FAILED) {
        tracked_block = SDL_RWFromMem(memblock, file_stats.st_size);
        if ((attributes->chunk = Mix_LoadWAV_RW(tracked_block, d_true))) {
            if ((attributes->channel = channel) == d_track_auto_channel)
                attributes->auto_channel = d_true;
            attributes->next_channel = channel;
            attributes->volume = d_track_default_volume;
        } else {
            snprintf(buffer, d_string_buffer_size, "unable to retrieve informations for track %s exception",
                    d_string_cstring(stream_attributes->string_name));
            d_throw(v_exception_chunk, buffer);
        }
        munmap(memblock, file_stats.st_size);
    } else {
        snprintf(buffer, d_string_buffer_size, "wrong type for file %s exception", d_string_cstring(stream_attributes->string_name));
        d_throw(v_exception_wrong_type, buffer);
    }
    return self;
}

d_define_method(track, set_volume)(struct s_object *self, int volume) {
    d_using(track);
    track_attributes->volume = volume;
    Mix_VolumeChunk(track_attributes->chunk, volume);
    return self;
}

d_define_method(track, set_channel)(struct s_object *self, int channel) {
    d_using(track);
    if ((track_attributes->next_channel = channel) == d_track_auto_channel)
        track_attributes->auto_channel = d_true;
    else
        track_attributes->auto_channel = d_false;
    return self;
}

d_define_method(track, set_position)(struct s_object *self, short angle, unsigned char distance) {
    d_using(track);
    track_attributes->angle = angle;
    track_attributes->distance = distance;
    if ((track_attributes->channel != d_track_auto_channel) && (Mix_Playing(track_attributes->channel)))
        Mix_SetPosition(track_attributes->channel, angle, distance);
    return self;
}

d_define_method(track, set_loops)(struct s_object *self, int loops) {
    d_using(track);
    track_attributes->loops = loops;
    return self;
}

d_define_method(track, play)(struct s_object *self, t_boolean restart) {
    return d_call(self, m_track_play_fade_in, restart, 1);
}

d_define_method(track, play_fade_in)(struct s_object *self, t_boolean restart, int delay) {
    d_using(track);
    t_boolean start = d_false;
    int channel;
    if ((track_attributes->channel != d_track_auto_channel) && ((Mix_Playing(track_attributes->channel)) || (Mix_Paused(track_attributes->channel)))) {
        if (restart) {
            Mix_HaltChannel(track_attributes->channel);
            track_attributes->channel = d_track_auto_channel;
            start = d_true;
        } else if (Mix_Paused(track_attributes->channel))
            Mix_Resume(track_attributes->channel);
    } else
        start = d_true;
    if (start) {
        if (track_attributes->auto_channel)
            channel = d_track_auto_channel;
        else
            channel = track_attributes->next_channel;
        d_call(self, m_track_set_volume, track_attributes->volume);
        if ((track_attributes->channel = Mix_FadeInChannel(channel, track_attributes->chunk, track_attributes->loops, delay)) >= 0)
            d_call(self, m_track_set_position, track_attributes->angle, track_attributes->distance);
    }
    return self;
}

d_define_method(track, pause)(struct s_object *self) {
    d_using(track);
    if ((track_attributes->channel != d_track_auto_channel) && (Mix_Playing(track_attributes->channel)))
        Mix_Pause(track_attributes->channel);
    return self;
}

d_define_method(track, stop)(struct s_object *self) {
    return d_call(self, m_track_stop_fade_out, 1);
}

d_define_method(track, stop_fade_out)(struct s_object *self, int delay) {
    d_using(track);
    if ((track_attributes->channel != d_track_auto_channel) && ((Mix_Playing(track_attributes->channel)) || (Mix_Paused(track_attributes->channel))))
        if (!Mix_FadeOutChannel(track_attributes->channel, delay))
            Mix_HaltChannel(track_attributes->channel);
    return self;
}

d_define_method(track, is_playing)(struct s_object *self) {
    d_using(track);
    t_boolean playing = d_false;
    if ((track_attributes->channel != d_track_auto_channel) && ((Mix_Playing(track_attributes->channel)) || (Mix_Paused(track_attributes->channel))))
        playing = d_true;
    d_cast_return(playing);
}

d_define_method(track, delete)(struct s_object *self, struct s_track_attributes *attributes) {
    if (attributes->channel != d_track_auto_channel)
        Mix_HaltChannel(attributes->channel);
    if (attributes->chunk)
        Mix_FreeChunk(attributes->chunk);
    return NULL;
}

d_define_class(track) {
    d_hook_method(track, e_flag_public, set_volume),
        d_hook_method(track, e_flag_public, set_channel),
        d_hook_method(track, e_flag_public, set_position),
        d_hook_method(track, e_flag_public, set_loops),
        d_hook_method(track, e_flag_public, play),
        d_hook_method(track, e_flag_public, play_fade_in),
        d_hook_method(track, e_flag_public, pause),
        d_hook_method(track, e_flag_public, stop),
        d_hook_method(track, e_flag_public, stop_fade_out),
        d_hook_method(track, e_flag_public, is_playing),
        d_hook_delete(track),
        d_hook_method_tail
};
