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
#include "mutex.obj.h"
struct s_object *f_mutex_new(struct s_object *self) {
	struct s_mutex_attributes *attributes = d_prepare(self, mutex);
	pthread_mutex_init(&(attributes->mutex), NULL);
	return self;
}

d_define_method(mutex, trylock)(struct s_object *self) {
	d_using(mutex);
	return (pthread_mutex_trylock(&(mutex_attributes->mutex)) == 0)?self:NULL;
}

d_define_method(mutex, lock)(struct s_object *self) {
	d_using(mutex);
	pthread_mutex_lock(&(mutex_attributes->mutex));
	return self;
}

d_define_method(mutex, unlock)(struct s_object *self) {
	d_using(mutex);
	pthread_mutex_unlock(&(mutex_attributes->mutex));
	return self;
}

d_define_method(mutex, delete)(struct s_object *self, struct s_mutex_attributes *attributes) {
	pthread_mutex_destroy(&(attributes->mutex));
	return NULL;
}

d_define_class(mutex) {
	d_hook_method(mutex, e_flag_public, trylock),
		d_hook_method(mutex, e_flag_public, lock),
		d_hook_method(mutex, e_flag_public, unlock),
		d_hook_delete(mutex),
		d_hook_method_tail
};
