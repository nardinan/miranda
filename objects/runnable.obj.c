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
#include "runnable.obj.h"
int sem_init_miranda(semaphore_t *semaphore, unsigned int value) {
  int result = 0;
  if ((*semaphore = semget(IPC_PRIVATE, 1, (IPC_CREAT | 0600))) != -1)
    result = semctl(*semaphore, 0, SETVAL, value);
  return result;
}
int sem_destroy_miranda(semaphore_t *semaphore) {
  return semctl(*semaphore, 0, IPC_RMID);
}
int sem_wait_miranda(semaphore_t *semaphore) {
  struct sembuf semaphore_options;
  semaphore_options.sem_num = 0;
  semaphore_options.sem_op = -1;
  semaphore_options.sem_flg = 0;
  return semop(*semaphore, &semaphore_options, 1);
}
int sem_trywait_miranda(semaphore_t *semaphore) {
  struct sembuf semaphore_options;
  semaphore_options.sem_num = 0;
  semaphore_options.sem_op = -1;
  semaphore_options.sem_flg = IPC_NOWAIT;
  return semop(*semaphore, &semaphore_options, 1);
}
int sem_post_miranda(semaphore_t *semaphore) {
  struct sembuf semaphore_options;
  semaphore_options.sem_num = 0;
  semaphore_options.sem_op = 1;
  semaphore_options.sem_flg = IPC_NOWAIT;
  return semop(*semaphore, &semaphore_options, 1);
}
int sem_getvalue_miranda(semaphore_t *semaphore) {
  return semctl(*semaphore, 0, GETVAL, 0);
}
struct s_object *f_runnable_new(struct s_object *self) {
  struct s_runnable_attributes *attributes = d_prepare(self, runnable);
  sem_init_miranda(&(attributes->kill_required), 0);
  sem_init_miranda(&(attributes->running_slots), 1);
  return self;
}
d_define_method(runnable, job)(struct s_object *self) {
  d_war(e_log_level_ever, "'job' method has not been implemented yet");
  return self;
}
d_define_method(runnable, run_interface)(struct s_object *self) {
  d_using(runnable);
  if (sem_wait_miranda(&(runnable_attributes->running_slots)) != -1) {
    d_call(self, m_runnable_job, NULL);
    sem_post_miranda(&(runnable_attributes->running_slots));
  }
  return self;
}
d_define_method(runnable, run)(struct s_object *self) {
  d_using(runnable);
  struct s_object *result = NULL;
  int value;
  if ((value = sem_getvalue_miranda(&(runnable_attributes->running_slots))) != -1)
    if (value > 0) {
      if ((value = sem_getvalue_miranda(&(runnable_attributes->kill_required))) != -1)
        if (value > 0)
          while (sem_trywait_miranda(&(runnable_attributes->kill_required)) == 0);
      if (pthread_create(&(runnable_attributes->descriptor), NULL, (t_thread_routine)&p_runnable_run_interface, self) == 0)
        result = self;
    }
  return result;
}
d_define_method(runnable, kill)(struct s_object *self) {
  d_using(runnable);
  sem_post_miranda(&(runnable_attributes->kill_required));
  return self;
}
d_define_method(runnable, kill_required)(struct s_object *self) {
  d_using(runnable);
  struct s_object *result = NULL;
  int value;
  if ((value = sem_getvalue_miranda(&(runnable_attributes->kill_required))) != -1)
    if (value > 0)
      result = self;
  return result;
}
d_define_method(runnable, running)(struct s_object *self) {
  d_using(runnable);
  struct s_object *result = self;
  int value;
  if ((value = sem_getvalue_miranda(&(runnable_attributes->running_slots))) != -1)
    if (value > 0)
      result = NULL;
  return result;
}
d_define_method(runnable, join)(struct s_object *self) {
  d_using(runnable);
  struct s_object *result = NULL;
  int value;
  if ((value = sem_getvalue_miranda(&(runnable_attributes->running_slots))) != -1)
    if (value == 0) {
      pthread_join(runnable_attributes->descriptor, NULL);
      result = self;
    }
  return result;
}
d_define_method(runnable, delete)(struct s_object *self, struct s_runnable_attributes *attributes) {
  int value;
  if ((value = sem_getvalue_miranda(&(attributes->running_slots))) != -1)
    if (value == 0) {
      sem_post_miranda(&(attributes->kill_required));
      pthread_join(attributes->descriptor, NULL);
    }
  sem_destroy_miranda(&(attributes->kill_required));
  sem_destroy_miranda(&(attributes->running_slots));
  return NULL;
}
d_define_class(runnable) {d_hook_method(runnable, e_flag_private, job),
  d_hook_method(runnable, e_flag_private, run_interface),
  d_hook_method(runnable, e_flag_public, run),
  d_hook_method(runnable, e_flag_public, kill),
  d_hook_method(runnable, e_flag_public, kill_required),
  d_hook_method(runnable, e_flag_public, running),
  d_hook_method(runnable, e_flag_public, join),
  d_hook_delete(runnable),
  d_hook_method_tail};
