/*
 * miranda
 * Copyright (C) 2016 Andrea Nardinocchi (andrea@nardinan.it)
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
#ifndef miranda_object_lisp_h
#define miranda_object_lisp_h
#include <stdarg.h>
#include "json.obj.h"
#define d_lisp_symbol_size 64
#define d_lisp_symbols 1024
#define d_lisp_mark_none        0x00
#define d_lisp_mark_environment 0x01
#define d_lisp_mark_symbols     0x02
#define d_lisp_mark_language    0x04
#define d_lisp_mark_internal (d_lisp_mark_environment|d_lisp_mark_symbols|d_lisp_mark_language)
#define d_lisp_true_token(t) ((t)->value_symbol[0]=='t')
typedef enum e_lisp_object_symbols {
  e_lisp_object_symbol_true = 0,
  e_lisp_object_symbol_nil,
  e_lisp_object_symbol_quote,
  e_lisp_object_symbol_lambda,
  e_lisp_object_symbol_if,
  e_lisp_object_symbol_cond,
  e_lisp_object_symbol_define,
  e_lisp_object_symbol_set,
  e_lisp_object_symbol_begin,
  e_lisp_object_symbol_load,
  e_lisp_object_symbol_NULL
} e_lisp_object_symbols;
typedef enum e_lisp_object_types {
  e_lisp_object_type_cons = 0,
  e_lisp_object_type_value,
  e_lisp_object_type_string,
  e_lisp_object_type_symbol,
  e_lisp_object_type_primitive,
  e_lisp_object_type_lambda
} e_lisp_object_types;
typedef struct s_lisp_object *(*t_lisp_primitive)(struct s_object *, struct s_lisp_object *);
typedef struct s_lisp_object {
  d_list_node_head;
  enum e_lisp_object_types type;
  unsigned char mark;
  union {
    struct {
      struct s_lisp_object *car, *cdr;
    } cons;
    struct {
      struct s_lisp_object *args, *call, *environment;
    } lambda;
    double value_double;
    char *value_string, value_symbol[d_lisp_symbol_size];
    t_lisp_primitive primitive;
  };
} s_lisp_object;
d_declare_class(lisp) {
  struct s_attributes head;
  struct s_list tokens, collector;
  struct s_json_token *current_token;
  struct s_object *string_name;
  struct s_lisp_object *environment, *symbols, *base_symbols[e_lisp_object_symbol_NULL];
  int output;
} d_declare_class_tail(lisp);
extern const char *v_lisp_object_types[];
#define d_lisp_object_type(o) ((o)?v_lisp_object_types[(o)->type]:"nil")
struct s_lisp_attributes *p_lisp_alloc(struct s_object *self);
#define d_lisp_car(obj) ((obj)?obj->cons.car:NULL)
#define d_lisp_cdr(obj) ((obj)?obj->cons.cdr:NULL)
#define d_lisp_caar(obj) (d_lisp_car(d_lisp_car(obj)))
#define d_lisp_cdar(obj) (d_lisp_cdr(d_lisp_car(obj)))
#define d_lisp_cadr(obj) (d_lisp_car(d_lisp_cdr(obj)))
#define d_lisp_caddr(obj) (d_lisp_car(d_lisp_cdr(d_lisp_cdr(obj))))
#define d_lisp_value(obj) (((obj)&&(obj->type==e_lisp_object_type_value))?obj->value_double:0.0)
#define d_lisp_string(obj) (((obj)&&(obj->type==e_lisp_object_type_string))?obj->value_string:"")
#define d_lisp_symbol(obj) (((obj)&&(obj->type==e_lisp_object_type_symbol))?obj->value_symbol:"")
extern struct s_lisp_object *p_lisp_object(struct s_object *self, enum e_lisp_object_types type, ...);
extern struct s_lisp_object *p_lisp_primitive_sum(struct s_object *self, struct s_lisp_object *args);
extern struct s_lisp_object *p_lisp_primitive_subtract(struct s_object *self, struct s_lisp_object *args);
extern struct s_lisp_object *p_lisp_primitive_multiply(struct s_object *self, struct s_lisp_object *args);
extern struct s_lisp_object *p_lisp_primitive_divide(struct s_object *self, struct s_lisp_object *args);
extern struct s_lisp_object *p_lisp_primitive_and(struct s_object *self, struct s_lisp_object *args);
extern struct s_lisp_object *p_lisp_primitive_or(struct s_object *self, struct s_lisp_object *args);
extern struct s_lisp_object *p_lisp_primitive_compare_gr(struct s_object *self, struct s_lisp_object *args);
extern struct s_lisp_object *p_lisp_primitive_compare_ge(struct s_object *self, struct s_lisp_object *args);
extern struct s_lisp_object *p_lisp_primitive_compare_lt(struct s_object *self, struct s_lisp_object *args);
extern struct s_lisp_object *p_lisp_primitive_compare_le(struct s_object *self, struct s_lisp_object *args);
extern struct s_lisp_object *p_lisp_primitive_compare_eq(struct s_object *self, struct s_lisp_object *args);
extern struct s_lisp_object *p_lisp_primitive_compare_not(struct s_object *self, struct s_lisp_object *args);
extern struct s_lisp_object *p_lisp_primitive_cons(struct s_object *self, struct s_lisp_object *args);
extern struct s_lisp_object *p_lisp_primitive_car(struct s_object *self, struct s_lisp_object *args);
extern struct s_lisp_object *p_lisp_primitive_cdr(struct s_object *self, struct s_lisp_object *args);
extern struct s_lisp_object *p_lisp_primitive_list(struct s_object *self, struct s_lisp_object *args);
extern struct s_lisp_object *p_lisp_primitive_print(struct s_object *self, struct s_lisp_object *args);
extern struct s_lisp_object *p_lisp_primitive_length(struct s_object *self, struct s_lisp_object *args);
extern struct s_lisp_object *p_lisp_primitive_compare(struct s_object *self, struct s_lisp_object *args);
extern struct s_object *f_lisp_new(struct s_object *self, struct s_object *stream_file, int output);
d_declare_method(lisp, import_symbol)(struct s_object *self, const char *symbol);
d_declare_method(lisp, retrieve_symbol)(struct s_object *self, struct s_lisp_object *symbol, struct s_lisp_object *environment);
d_declare_method(lisp, extend_closure)(struct s_object *self, struct s_lisp_object *symbol, struct s_lisp_object *values, struct s_lisp_object *environment);
d_declare_method(lisp, extend_environment)(struct s_object *self, const char *symbol, struct s_lisp_object *primitive);
d_declare_method(lisp, recursive_mark_environment)(struct s_object *self, struct s_lisp_object *current_object, int mark);
d_declare_method(lisp, mark_environment)(struct s_object *self);
d_declare_method(lisp, next_token)(struct s_object *self);
d_declare_method(lisp, read_object)(struct s_object *self, struct s_lisp_object *current_object);
d_declare_method(lisp, read_components)(struct s_object *self, struct s_lisp_object *current_object);
d_declare_method(lisp, recursive_evaluation)(struct s_object *self, struct s_lisp_object *current_object, struct s_lisp_object *environment);
d_declare_method(lisp, evaluate)(struct s_object *self, struct s_lisp_object *current_object, struct s_lisp_object *environment);
d_declare_method(lisp, write)(struct s_object *self, struct s_lisp_object *current_object, int output);
d_declare_method(lisp, run)(struct s_object *self);
d_declare_method(lisp, sweep_collector)(struct s_object *self, unsigned char excluded_marks);
d_declare_method(lisp, delete)(struct s_object *self, struct s_lisp_attributes *attributes);
#endif

