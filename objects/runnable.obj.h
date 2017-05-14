/*
 * miranda
 * Copyright (C) 2017 Andrea Nardinocchi (andrea@nardinan.it)
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
#ifndef miranda_object_runnable_h
#define miranda_object_runnable_h
#include <pthread.h>
#include <semaphore.h>
#include "object.h"
typedef void *(*t_thread_routine)(void *);
d_declare_class(runnable) {
    struct s_attributes head;
    pthread_t descriptor;
    sem_t kill_required, running_slots;
} d_declare_class_tail(runnable);
extern struct s_object *f_runnable_new(struct s_object *self);
d_declare_method(runnable, job)(struct s_object *self);
d_declare_method(runnable, run_interface)(struct s_object *self);
d_declare_method(runnable, run)(struct s_object *self);
d_declare_method(runnable, kill)(struct s_object *self);
d_declare_method(runnable, kill_required)(struct s_object *self);
d_declare_method(runnable, running)(struct s_object *self);
d_declare_method(runnable, join)(struct s_object *self);
d_declare_method(runnable, delete)(struct s_object *self, struct s_runnable_attributes *attributes);
#endif

