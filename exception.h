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
#ifndef miranda_exception_h
#define miranda_exception_h
#include <stdio.h>
#include <string.h>
#include <setjmp.h>
#define d_exception_kind_size 64
#define d_exception_file_size 64
#define d_exception_function_size 64
#define d_exception_description_size 128
#define d_try\
    do{\
        jmp_buf _local_hook, *_last_hook=v_exception_hook;\
        int _local_code;\
        v_exception_hook=&_local_hook;\
        if((_local_code=setjmp(_local_hook))==0){
#define d_catch_kind(k, o)\
        }else if(_local_code==(k).identificator){\
            v_exception_hook=_last_hook;\
            (o)=&v_exception_raised;
#define d_catch(o)\
        }else{\
            v_exception_hook=_last_hook;\
            (o)=&v_exception_raised;
#define d_endtry\
        }\
        v_exception_hook=_last_hook;\
    }while(0)
#define d_throw(x, m)\
    do{\
        if(v_exception_hook){\
            p_exception_fill((x),(m),__FILE__,__FUNCTION__,__LINE__);\
            longjmp(*v_exception_hook,v_exception_raised.identificator);\
        }\
    }while(0)
#define d_raise\
    do{\
        if(v_exception_hook)\
        longjmp(*v_exception_hook,v_exception_raised.identificator);\
    }while(0)
#define d_exception_dump(s, o)\
    do{\
        if(((o)->level++)==0)\
            fprintf((s),"%s:%s() @ %d {%s} %s\n",(o)->file,(o)->function,(o)->line,(o)->kind,(o)->description);\
        fprintf((s),"\t%s:%s() @ %d\n",__FILE__,__FUNCTION__,__LINE__);\
    }while(0)
#define d_exception_verbose_dump(s, o, c...)\
    do{\
        if(((o)->level++)==0)\
            fprintf((s),"%s:%s() @ %d {%s} %s\n", (o)->file,(o)->function,(o)->line,(o)->kind,(o)->description);\
        fprintf((s),"\t%s:%s() @ %d [",__FILE__,__FUNCTION__,__LINE__);\
        fprintf((s),##c);\
        fprintf((s),"]\n");\
    }while(0)
#define d_exception_declare(k) extern const struct s_exception v_exception_##k
#define d_exception_define(k, i, m) const struct s_exception v_exception_##k ={m,"undefined","undefine","undefined",0,0,i}
typedef struct s_exception {
  char kind[d_exception_kind_size], file[d_exception_file_size], function[d_exception_function_size], description[d_exception_description_size];
  unsigned int line, level;
  int identificator;
} s_exception;
extern jmp_buf *v_exception_hook;
extern struct s_exception v_exception_raised;
extern void p_exception_fill(struct s_exception exception, const char *message, const char *file, const char *function, unsigned int line);
#endif

