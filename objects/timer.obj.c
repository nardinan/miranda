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
#include "timer.obj.h"
struct s_timer_attributes *p_timer_alloc(struct s_object *self) {
  struct s_timer_attributes *result = d_prepare(self, timer);
  f_memory_new(self);     /* inherit */
  f_mutex_new(self);      /* inherit */
  return result;
}
struct s_object *f_timer_new(struct s_object *self) {
  struct s_timer_attributes *attributes = p_timer_alloc(self);
  gettimeofday(&(attributes->start), NULL);
  return self;
}
d_define_method(timer, reset)(struct s_object *self) {
  d_using(timer);
  gettimeofday(&(timer_attributes->start), NULL);
  return self;
}
d_define_method(timer, elapsed_time_s)(struct s_object *self, double *elapsed_time) {
  d_using(timer);
  struct timeval now;
  gettimeofday(&now, NULL);
  *elapsed_time = (double)(now.tv_sec - timer_attributes->start.tv_sec) + ((now.tv_usec - timer_attributes->start.tv_usec) / 1000000.0);
  return self;
}
d_define_method(timer, elapsed_time_ms)(struct s_object *self, double *elapsed_time) {
  d_using(timer);
  struct timeval now;
  gettimeofday(&now, NULL);
  *elapsed_time = ((now.tv_sec - timer_attributes->start.tv_sec) * 1000.0) + ((now.tv_usec - timer_attributes->start.tv_usec) / 1000.0);
  return self;
}
d_define_method(timer, elapsed_time_us)(struct s_object *self, long long int *elapsed_time) {
  d_using(timer);
  struct timeval now;
  gettimeofday(&now, NULL);
  *elapsed_time = ((now.tv_sec - timer_attributes->start.tv_sec) * 1000000L) + (now.tv_usec - timer_attributes->start.tv_usec);
  return self;
}
d_define_class(timer) {d_hook_method(timer, e_flag_public, reset),
                       d_hook_method(timer, e_flag_public, elapsed_time_s),
                       d_hook_method(timer, e_flag_public, elapsed_time_ms),
                       d_hook_method(timer, e_flag_public, elapsed_time_us),
                       d_hook_method_tail};
