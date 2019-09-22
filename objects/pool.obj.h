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
#ifndef miranda_object_pool_h
#define miranda_object_pool_h
#include "memory.obj.h"
#include "mutex.obj.h"
d_declare_class(pool) {
  struct s_attributes head;
  struct s_list *pool;
} d_declare_class_tail(pool);
extern struct s_object *v_default_pool;
#define d_pool_init v_default_pool=f_pool_new(d_new(pool))
#define d_pool_destroy d_delete(v_default_pool)
struct s_pool_attributes *p_pool_alloc(struct s_object *self);
extern struct s_object *f_pool_new(struct s_object *self);
#define d_pool_begin(c)\
    do{\
        d_call(v_default_pool,m_pool_insert,p_object_malloc(__FILE__,__LINE__,(c),e_flag_placeholder));\
        do
#define d_p(p) d_call(v_default_pool,m_pool_insert,(p))
d_declare_method(pool, insert)(struct s_object *self, struct s_object *pointer);
#define d_pool_end\
        while(0);\
        d_call(v_default_pool,m_pool_clean,NULL);\
    }while(0)
d_declare_method(pool, clean)(struct s_object *self);
d_declare_method(pool, delete)(struct s_object *self, struct s_pool_attributes *attributes);
#endif

