/*
 * miranda
 * Copyright (C) 2014-2020 Andrea Nardinocchi (andrea@nardinan.it)
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
#ifndef miranda_object_timer_h
#define miranda_object_timer_h
#include <sys/time.h>
#include "memory.obj.h"
#include "mutex.obj.h"
d_declare_class(timer) {
  struct s_attributes head;
  struct timeval start;
} d_declare_class_tail(timer);
struct s_timer_attributes *p_timer_alloc(struct s_object *self);
extern struct s_object *f_timer_new(struct s_object *self);
d_declare_method(timer, reset)(struct s_object *self);
d_declare_method(timer, elapsed_time_s)(struct s_object *self, double *elapsed_time);
d_declare_method(timer, elapsed_time_ms)(struct s_object *self, double *elapsed_time);
d_declare_method(timer, elapsed_time_us)(struct s_object *self, long long *elapsed_time);
#endif