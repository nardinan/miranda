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
#include "runnable.obj.h"
struct s_object *f_runnable_new(struct s_object *self) {
    struct s_runnable_attributes *attributes = d_prepare(self, runnable);
    sem_init(&(attributes->kill_required), 0, 0);
    sem_init(&(attributes->running_slots), 0, 1);
    return self;
}

d_define_method(runnable, job)(struct s_object *self) {
    d_war(e_log_level_ever, "'job' method has not been implemented yet");
    return self;
}

d_define_method(runnable, run_interface)(struct s_object *self) {
    d_using(runnable);
    if (sem_wait(&(runnable_attributes->running_slots)) == 0) {
        d_call(self, m_runnable_job, NULL);
        sem_post(&(runnable_attributes->running_slots));
    }
    return self;
}

d_define_method(runnable, run)(struct s_object *self) {
    d_using(runnable);
    struct s_object *result = NULL;
    int value;
    if (sem_getvalue(&(runnable_attributes->running_slots), &value) == 0)
        if (value > 0) {
            if (sem_getvalue(&(runnable_attributes->kill_required), &value) == 0)
                if (value > 0)
                    while (sem_trywait(&(runnable_attributes->kill_required)) == 0);
            if (pthread_create(&(runnable_attributes->descriptor), NULL, (t_thread_routine)&p_runnable_run_interface, self) == 0)
                result = self;
        }
    return result;
}

d_define_method(runnable, kill)(struct s_object *self) {
    d_using(runnable);
    sem_post(&(runnable_attributes->kill_required));
    return self;
}

d_define_method(runnable, kill_required)(struct s_object *self) {
    d_using(runnable);
    struct s_object *result = NULL;
    int value;
    if (sem_getvalue(&(runnable_attributes->kill_required), &value) == 0)
        if (value > 0)
            result = self;
    return result;
}

d_define_method(runnable, running)(struct s_object *self) {
    d_using(runnable);
    struct s_object *result = self;
    int value;
    if (sem_getvalue(&(runnable_attributes->running_slots), &value) == 0)
        if (value > 0)
            result = NULL;
    return result;
}

d_define_method(runnable, join)(struct s_object *self) {
    d_using(runnable);
    struct s_object *result = NULL;
    int value;
    if (sem_getvalue(&(runnable_attributes->running_slots), &value) == 0)
        if (value == 0) {
            pthread_join(runnable_attributes->descriptor, NULL);
            result = self;
        }
    return result;
}

d_define_method(runnable, delete)(struct s_object *self, struct s_runnable_attributes *attributes) {
    int value;
    if (sem_getvalue(&(attributes->running_slots), &value) == 0)
        if (value == 0) {
            sem_post(&(attributes->kill_required));
            pthread_join(attributes->descriptor, NULL);
        }
    sem_destroy(&(attributes->kill_required));
    sem_destroy(&(attributes->running_slots));
    return NULL;
}

d_define_class(runnable) {
    d_hook_method(runnable, e_flag_private, job),
        d_hook_method(runnable, e_flag_private, run_interface),
        d_hook_method(runnable, e_flag_public, run),
        d_hook_method(runnable, e_flag_public, kill),
        d_hook_method(runnable, e_flag_public, kill_required),
        d_hook_method(runnable, e_flag_public, running),
        d_hook_method(runnable, e_flag_public, join),
        d_hook_delete(runnable),
        d_hook_method_tail
};
