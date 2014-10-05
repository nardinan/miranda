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
#ifndef miranda_object_mutex_h
#define miranda_object_mutex_h
#include <pthread.h>
#include "object.h"
d_declare_class(mutex) {
	struct s_attributes head;
	pthread_mutex_t mutex;
} d_declare_class_tail(mutex);
extern struct s_object *f_mutex_new(struct s_object *self);
d_declare_method(mutex, trylock)(struct s_object *self);
d_declare_method(mutex, lock)(struct s_object *self);
d_declare_method(mutex, unlock)(struct s_object *self);
d_declare_method(mutex, delete)(struct s_object *self, struct s_mutex_attributes *attributes);
#endif

