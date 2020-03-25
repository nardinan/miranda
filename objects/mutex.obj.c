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
#include "mutex.obj.h"
#include <pthread.h>
struct s_object *f_mutex_new(struct s_object *self) {
  struct s_mutex_attributes *attributes = d_prepare(self, mutex);
  pthread_mutex_init(&(attributes->mutex), NULL);
  return self;
}
struct s_object *f_mutex_per_thread_new(struct s_object *self) {
  struct s_mutex_attributes *attributes = d_prepare(self, mutex);
  pthread_mutexattr_t mutex_attributes;
  if (pthread_mutexattr_init(&mutex_attributes) == 0) {
    if (pthread_mutexattr_settype(&mutex_attributes, PTHREAD_MUTEX_RECURSIVE_NP) == 0)
      pthread_mutex_init(&(attributes->mutex), &mutex_attributes);
    pthread_mutexattr_destroy(&mutex_attributes);
  }
  return self;
}
d_define_method(mutex, trylock)(struct s_object *self) {
  d_using(mutex);
  return (pthread_mutex_trylock(&(mutex_attributes->mutex)) == 0) ? self : NULL;
}
d_define_method(mutex, lock)(struct s_object *self, const char *file, const char *function, unsigned int line) {
  d_using(mutex);
  int result;
  if ((result = pthread_mutex_lock(&(mutex_attributes->mutex))) != 0)
    p_log_write(stderr, e_log_level_ever, "\x1B[31merr\x1B[0m", file, function, line,
        "failure attempting to lock the mutex (operation returned the following code: %d)", result);
  return self;
}
d_define_method(mutex, unlock)(struct s_object *self, const char *file, const char *function, unsigned int line) {
  d_using(mutex);
  int result;
  if ((result = pthread_mutex_unlock(&(mutex_attributes->mutex))) != 0)
    p_log_write(stderr, e_log_level_ever, "\x1B[31merr\x1B[0m", file, function, line,
        "failure attempting to unlock the mutex (operation returned the following code: %d)", result);
  return self;
}
d_define_method(mutex, delete)(struct s_object *self, struct s_mutex_attributes *attributes) {
  pthread_mutex_destroy(&(attributes->mutex));
  return NULL;
}
d_define_class(mutex) {d_hook_method(mutex, e_flag_public, trylock),
  d_hook_method(mutex, e_flag_public, lock),
  d_hook_method(mutex, e_flag_public, unlock),
  d_hook_delete(mutex),
  d_hook_method_tail};
