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
#include "lisp.obj.h"
const char *v_lisp_object_types[] = {
    "cons",
    "value",
    "string",
    "symbol",
    "primitive",
    "lambda"
};

struct s_lisp_attributes *p_lisp_alloc(struct s_object *self) {
    struct s_lisp_attributes *result = d_prepare(self, lisp);
    f_memory_new(self);	/* inherit */
    f_mutex_new(self);	/* inherit */
    return result;
}

struct s_lisp_object *p_lisp_object(struct s_object *self, enum e_lisp_object_types type, ...) {
    struct s_lisp_attributes *lisp_attributes = d_cast(self, lisp);
    struct s_lisp_object *result;
    char *string_buffer;
    size_t string_length;
    va_list parameters;
    va_start(parameters, type);
    if ((result = (struct s_lisp_object *)d_malloc(sizeof(struct s_lisp_object)))) {
        result->type = type;
        switch (type) {
            case e_lisp_object_type_value:
                result->value_double = (double)va_arg(parameters, double);
                break;
            case e_lisp_object_type_string:
                if ((string_buffer = (char *)va_arg(parameters, void *)))
                    if ((string_length = f_string_strlen(string_buffer)) > 0) {
                        if ((result->value_string = (char *) d_malloc(string_length + 1)))
                            strcpy(result->value_string, string_buffer);
                        else
                            d_die(d_error_malloc);
                    }
                break;
            case e_lisp_object_type_symbol:
                strncpy(result->value_symbol, (char *)va_arg(parameters, void *), d_lisp_symbol_size);
                break;
            case e_lisp_object_type_cons:
                result->cons.car = (struct s_lisp_object *)va_arg(parameters, void *);
                result->cons.cdr = (struct s_lisp_object *)va_arg(parameters, void *);
                break;
            case e_lisp_object_type_primitive:
                result->primitive = (t_lisp_primitive)va_arg(parameters, void *);
                break;
            case e_lisp_object_type_lambda:
                result->lambda.args = (struct s_lisp_object *)va_arg(parameters, void *);
                result->lambda.call = (struct s_lisp_object *)va_arg(parameters, void *);
                result->lambda.environment = (struct s_lisp_object *)va_arg(parameters, void *);
                break;
        }
        f_list_append(&(lisp_attributes->collector), (struct s_list_node *)result, e_list_insert_head);
    } else
        d_die(d_error_malloc);
    va_end(parameters);
    return result;
}

struct s_lisp_object *p_lisp_primitive_sum(struct s_object *self, struct s_lisp_object *args) {
    double value = 0.0;
    struct s_lisp_object *pointer = args, *entry;
    struct s_lisp_attributes *lisp_attributes = d_cast(self, lisp);
    while (pointer)
        if ((entry = d_lisp_car(pointer))) {
            if (entry->type == e_lisp_object_type_value) {
                value += entry->value_double;
                pointer = d_lisp_cdr(pointer);
            } else if (entry == lisp_attributes->base_symbols[e_lisp_object_symbol_nil]) {
                pointer = d_lisp_cdr(pointer);
            } else {
                d_err(e_log_level_low, "(source %s:%d) '%s' object founded while 'value' object was expected", d_string_cstring(lisp_attributes->string_name),
                        ((lisp_attributes->current_token)?lisp_attributes->current_token->line_number:0), d_lisp_object_type(entry));
                break;
            }
        } else
            break;
    return p_lisp_object(self, e_lisp_object_type_value, value);
}

struct s_lisp_object *p_lisp_primitive_subtract(struct s_object *self, struct s_lisp_object *args) {
    double value = 0.0;
    struct s_lisp_object *pointer = args, *entry;
    struct s_lisp_attributes *lisp_attributes = d_cast(self, lisp);
    if ((entry = d_lisp_car(pointer))) {
        if (entry->type == e_lisp_object_type_value) {
            value = entry->value_double;
            pointer = d_lisp_cdr(pointer);
            while (pointer)
                if ((entry = d_lisp_car(pointer))) {
                    if (entry->type == e_lisp_object_type_value) {
                        value -= entry->value_double;
                        pointer = d_lisp_cdr(pointer);
                    } else if (entry == lisp_attributes->base_symbols[e_lisp_object_symbol_nil]) {
                        pointer = d_lisp_cdr(pointer);
                    } else {
                        d_err(e_log_level_low, "(source %s:%d) '%s' object founded while 'value' object was expected", 
                                d_string_cstring(lisp_attributes->string_name), ((lisp_attributes->current_token)?lisp_attributes->current_token->line_number:0), 
                                d_lisp_object_type(entry));
                        break;
                    }
                } else
                    break;
        } else
            d_err(e_log_level_low, "(source %s:%d) '%s' object founded while 'value' object was expected", d_string_cstring(lisp_attributes->string_name), 
                    ((lisp_attributes->current_token)?lisp_attributes->current_token->line_number:0), d_lisp_object_type(entry));
    }
    return p_lisp_object(self, e_lisp_object_type_value, value);
}

struct s_lisp_object *p_lisp_primitive_multiply(struct s_object *self, struct s_lisp_object *args) {
    double value = 1.0;
    struct s_lisp_object *pointer = args, *entry;
    struct s_lisp_attributes *lisp_attributes = d_cast(self, lisp);
    while (pointer)
        if ((entry = d_lisp_car(pointer))) {
            if (entry->type == e_lisp_object_type_value) {
                value *= entry->value_double;
                pointer = d_lisp_cdr(pointer);
            } else if (entry == lisp_attributes->base_symbols[e_lisp_object_symbol_nil]) {
                value = 0.0;
                pointer = d_lisp_cdr(pointer);
            } else {
                d_err(e_log_level_low, "(source %s:%d) '%s' object founded while 'value' object was expected", d_string_cstring(lisp_attributes->string_name),
                        ((lisp_attributes->current_token)?lisp_attributes->current_token->line_number:0), d_lisp_object_type(entry));
                break;
            }
        } else
            break;
    return p_lisp_object(self, e_lisp_object_type_value, value);
}

struct s_lisp_object *p_lisp_primitive_divide(struct s_object *self, struct s_lisp_object *args) {
    double value = 0.0;
    struct s_lisp_object *pointer = args, *entry;
    struct s_lisp_attributes *lisp_attributes = d_cast(self, lisp);
    if ((entry = d_lisp_car(pointer))) {
        if (entry->type == e_lisp_object_type_value) {
            value = entry->value_double;
            pointer = d_lisp_cdr(pointer);
            while (pointer)
                if ((entry = d_lisp_car(pointer))) {
                    if (entry->type == e_lisp_object_type_value) {
                        value /= entry->value_double;
                        pointer = d_lisp_cdr(pointer);
                    } else if (entry == lisp_attributes->base_symbols[e_lisp_object_symbol_nil]) {
                        pointer = d_lisp_cdr(pointer);
                    } else {
                        d_err(e_log_level_low, "(source %s:%d) '%s' object founded while 'value' object was expected", 
                                d_string_cstring(lisp_attributes->string_name), ((lisp_attributes->current_token)?lisp_attributes->current_token->line_number:0), 
                                d_lisp_object_type(entry));
                        break;
                    }
                } else
                    break;
        } else
            d_err(e_log_level_low, "(source %s:%d) '%s' object founded while 'value' object was expected", d_string_cstring(lisp_attributes->string_name), 
                    ((lisp_attributes->current_token)?lisp_attributes->current_token->line_number:0), d_lisp_object_type(entry));
    }
    return p_lisp_object(self, e_lisp_object_type_value, value);
}

struct s_lisp_object *p_lisp_primitive_compare_gr(struct s_object *self, struct s_lisp_object *args) {
    struct s_lisp_attributes *lisp_attributes = d_cast(self, lisp);
    struct s_lisp_object *result = lisp_attributes->base_symbols[e_lisp_object_symbol_nil];
    double left_value = d_lisp_value(d_lisp_car(args)), right_value = d_lisp_value(d_lisp_cadr(args));
    if (left_value > right_value)
        result = lisp_attributes->base_symbols[e_lisp_object_symbol_true];
    return result;
}

struct s_lisp_object *p_lisp_primitive_compare_ge(struct s_object *self, struct s_lisp_object *args) {
    struct s_lisp_attributes *lisp_attributes = d_cast(self, lisp);
    struct s_lisp_object *result = lisp_attributes->base_symbols[e_lisp_object_symbol_nil];
    double left_value = d_lisp_value(d_lisp_car(args)), right_value = d_lisp_value(d_lisp_cadr(args));
    if (left_value >= right_value)
        result = lisp_attributes->base_symbols[e_lisp_object_symbol_true];
    return result;
}

struct s_lisp_object *p_lisp_primitive_compare_lt(struct s_object *self, struct s_lisp_object *args) {
    struct s_lisp_attributes *lisp_attributes = d_cast(self, lisp);
    struct s_lisp_object *result = lisp_attributes->base_symbols[e_lisp_object_symbol_nil];
    double left_value = d_lisp_value(d_lisp_car(args)), right_value = d_lisp_value(d_lisp_cadr(args));
    if (left_value < right_value)
        result = lisp_attributes->base_symbols[e_lisp_object_symbol_true];
    return result;
}

struct s_lisp_object *p_lisp_primitive_compare_le(struct s_object *self, struct s_lisp_object *args) {
    struct s_lisp_attributes *lisp_attributes = d_cast(self, lisp);
    struct s_lisp_object *result = lisp_attributes->base_symbols[e_lisp_object_symbol_nil];
    double left_value = d_lisp_value(d_lisp_car(args)), right_value = d_lisp_value(d_lisp_cadr(args));
    if (left_value <= right_value)
        result = lisp_attributes->base_symbols[e_lisp_object_symbol_true];
    return result;
}

struct s_lisp_object *p_lisp_primitive_compare_eq(struct s_object *self, struct s_lisp_object *args) {
    struct s_lisp_attributes *lisp_attributes = d_cast(self, lisp);
    struct s_lisp_object *result = lisp_attributes->base_symbols[e_lisp_object_symbol_nil];
    double left_value = d_lisp_value(d_lisp_car(args)), right_value = d_lisp_value(d_lisp_cadr(args));
    if (left_value == right_value)
        result = lisp_attributes->base_symbols[e_lisp_object_symbol_true];
    return result;
}

struct s_lisp_object *p_lisp_primitive_compare_not(struct s_object *self, struct s_lisp_object *args) {
    struct s_lisp_attributes *lisp_attributes = d_cast(self, lisp);
    struct s_lisp_object *result = lisp_attributes->base_symbols[e_lisp_object_symbol_nil];
    if ((!d_lisp_car(args)) || (d_lisp_car(args) == lisp_attributes->base_symbols[e_lisp_object_symbol_nil]))
        result = lisp_attributes->base_symbols[e_lisp_object_symbol_true];
    return result;
}

struct s_lisp_object *p_lisp_primitive_cons(struct s_object *self, struct s_lisp_object *args) {
    struct s_lisp_object *car = d_lisp_car(args), *cdr = d_lisp_cadr(args);
    return p_lisp_object(self, e_lisp_object_type_cons, car, cdr);
}

struct s_lisp_object *p_lisp_primitive_car(struct s_object *self, struct s_lisp_object *args) {
    return d_lisp_car(d_lisp_car(args));
}

struct s_lisp_object *p_lisp_primitive_cdr(struct s_object *self, struct s_lisp_object *args) {
    return d_lisp_cdr(d_lisp_car(args));
}

struct s_lisp_object *p_lisp_primitive_list(struct s_object *self, struct s_lisp_object *args) {
    struct s_lisp_attributes *lisp_attributes = d_cast(self, lisp);
    struct s_lisp_object *result = lisp_attributes->base_symbols[e_lisp_object_symbol_nil];
    if ((d_lisp_car(args)) && (d_lisp_car(args) != lisp_attributes->base_symbols[e_lisp_object_symbol_nil]))
        result = p_lisp_object(self, e_lisp_object_type_cons, d_lisp_car(args), p_lisp_primitive_list(self, d_lisp_cdr(args)));
    return result;
}

struct s_lisp_object *p_lisp_primitive_print(struct s_object *self, struct s_lisp_object *args) {
    struct s_lisp_attributes *lisp_attributes = d_cast(self, lisp);
    struct s_lisp_object *pointer = args, *car;
    while (pointer) {
        if ((car = d_lisp_car(pointer)) && (car != lisp_attributes->base_symbols[e_lisp_object_symbol_nil])) {
            d_call(self, m_lisp_write, d_lisp_car(pointer), lisp_attributes->output);
            write(lisp_attributes->output, " ", sizeof(char));
        }
        pointer = d_lisp_cdr(pointer);
    }
    return lisp_attributes->base_symbols[e_lisp_object_symbol_nil];
}

struct s_lisp_object *p_lisp_primitive_length(struct s_object *self, struct s_lisp_object *args) {
    char *string_content = d_lisp_string(d_lisp_car(args));
    return p_lisp_object(self, e_lisp_object_type_value, (double)f_string_strlen(string_content));
}

struct s_lisp_object *p_lisp_primitive_compare(struct s_object *self, struct s_lisp_object *args) {
    struct s_lisp_attributes *lisp_attributes = d_cast(self, lisp);
    char *string_content_left = d_lisp_string(d_lisp_car(args)), *string_content_right = d_lisp_string(d_lisp_cadr(args));
    if (f_string_strcmp(string_content_left, string_content_right) == 0)
        return lisp_attributes->base_symbols[e_lisp_object_symbol_true];
    return lisp_attributes->base_symbols[e_lisp_object_symbol_nil];
}

struct s_object *f_lisp_new(struct s_object *self, struct s_object *stream_file, int output) {
    struct s_lisp_attributes *attributes = p_lisp_alloc(self);
    struct s_stream_attributes *stream_attributes = d_cast(stream_file, stream);
    f_json_tokenizer(stream_file, &(attributes->tokens));
    attributes->string_name = d_retain(stream_attributes->string_name);
    attributes->base_symbols[e_lisp_object_symbol_true] = d_call(self, m_lisp_import_symbol, "t");
    attributes->base_symbols[e_lisp_object_symbol_nil] = d_call(self, m_lisp_import_symbol, "nil");
    attributes->base_symbols[e_lisp_object_symbol_quote] = d_call(self, m_lisp_import_symbol, "quote");
    attributes->base_symbols[e_lisp_object_symbol_if] = d_call(self, m_lisp_import_symbol, "if");
    attributes->base_symbols[e_lisp_object_symbol_lambda] = d_call(self, m_lisp_import_symbol, "lambda");
    attributes->base_symbols[e_lisp_object_symbol_set] = d_call(self, m_lisp_import_symbol, "set");
    attributes->base_symbols[e_lisp_object_symbol_define] = d_call(self, m_lisp_import_symbol, "define");
    attributes->base_symbols[e_lisp_object_symbol_begin] = d_call(self, m_lisp_import_symbol, "begin");
    d_call(self, m_lisp_extend_environment, attributes->base_symbols[e_lisp_object_symbol_nil]->value_symbol, 
            attributes->base_symbols[e_lisp_object_symbol_nil]);
    d_call(self, m_lisp_extend_environment, attributes->base_symbols[e_lisp_object_symbol_true]->value_symbol, 
            attributes->base_symbols[e_lisp_object_symbol_true]);
    d_call(self, m_lisp_extend_environment, "+", p_lisp_object(self, e_lisp_object_type_primitive, p_lisp_primitive_sum));
    d_call(self, m_lisp_extend_environment, "-", p_lisp_object(self, e_lisp_object_type_primitive, p_lisp_primitive_subtract));
    d_call(self, m_lisp_extend_environment, "*", p_lisp_object(self, e_lisp_object_type_primitive, p_lisp_primitive_multiply));
    d_call(self, m_lisp_extend_environment, "/", p_lisp_object(self, e_lisp_object_type_primitive, p_lisp_primitive_divide));
    d_call(self, m_lisp_extend_environment, "=", p_lisp_object(self, e_lisp_object_type_primitive, p_lisp_primitive_compare_eq));
    d_call(self, m_lisp_extend_environment, ">", p_lisp_object(self, e_lisp_object_type_primitive, p_lisp_primitive_compare_gr));
    d_call(self, m_lisp_extend_environment, "<", p_lisp_object(self, e_lisp_object_type_primitive, p_lisp_primitive_compare_lt));
    d_call(self, m_lisp_extend_environment, ">=", p_lisp_object(self, e_lisp_object_type_primitive, p_lisp_primitive_compare_ge));
    d_call(self, m_lisp_extend_environment, "<=", p_lisp_object(self, e_lisp_object_type_primitive, p_lisp_primitive_compare_le));
    d_call(self, m_lisp_extend_environment, "not", p_lisp_object(self, e_lisp_object_type_primitive, p_lisp_primitive_compare_not));
    d_call(self, m_lisp_extend_environment, "cons", p_lisp_object(self, e_lisp_object_type_primitive, p_lisp_primitive_cons));
    d_call(self, m_lisp_extend_environment, "car", p_lisp_object(self, e_lisp_object_type_primitive, p_lisp_primitive_car));
    d_call(self, m_lisp_extend_environment, "cdr", p_lisp_object(self, e_lisp_object_type_primitive, p_lisp_primitive_cdr));
    d_call(self, m_lisp_extend_environment, "list", p_lisp_object(self, e_lisp_object_type_primitive, p_lisp_primitive_list));
    d_call(self, m_lisp_extend_environment, "print", p_lisp_object(self, e_lisp_object_type_primitive, p_lisp_primitive_print));
    d_call(self, m_lisp_extend_environment, "length", p_lisp_object(self, e_lisp_object_type_primitive, p_lisp_primitive_length));
    d_call(self, m_lisp_extend_environment, "compare", p_lisp_object(self, e_lisp_object_type_primitive, p_lisp_primitive_compare));
    attributes->output = output;
    return self;
}

d_define_method(lisp, import_symbol)(struct s_object *self, const char *symbol) {
    d_using(lisp);
    struct s_lisp_object *pointer = lisp_attributes->symbols, *current_symbol = NULL, *founded_symbol = NULL;
    while (pointer) {
        if ((current_symbol = d_lisp_car(pointer)))
            if (f_string_strcmp(symbol, d_lisp_symbol(current_symbol)) == 0) {
                founded_symbol = current_symbol;
                break;
            }
        pointer = d_lisp_cdr(pointer);
    }
    if (!founded_symbol) {
        founded_symbol = p_lisp_object(self, e_lisp_object_type_symbol, symbol);
        lisp_attributes->symbols = p_lisp_object(self, e_lisp_object_type_cons, founded_symbol, lisp_attributes->symbols);
    }
    d_cast_return(founded_symbol);
}

d_define_method(lisp, retrieve_symbol)(struct s_object *self, struct s_lisp_object *symbol, struct s_lisp_object *environment) {
    struct s_lisp_object *result = NULL;
    if (environment) {
        if (d_lisp_car(d_lisp_car(environment)) == symbol)
            result = d_lisp_car(environment);
        else
            result = d_call(self, m_lisp_retrieve_symbol, symbol, d_lisp_cdr(environment));
    }
    d_cast_return(result);
}

d_define_method(lisp, extend_closure)(struct s_object *self, struct s_lisp_object *symbol, struct s_lisp_object *values, 
        struct s_lisp_object *environment) {
    struct s_lisp_object *result = environment;
    if (symbol)
        result = d_call(self, m_lisp_extend_closure, d_lisp_cdr(symbol), d_lisp_cdr(values), p_lisp_object(self, e_lisp_object_type_cons, 
                    p_lisp_object(self, e_lisp_object_type_cons, d_lisp_car(symbol), d_lisp_car(values)), environment));
    d_cast_return(result);
}

d_define_method(lisp, extend_environment)(struct s_object *self, const char *symbol, struct s_lisp_object *primitive) {
    d_using(lisp);
    struct s_lisp_object *current_symbol = d_call(self, m_lisp_import_symbol, symbol), *current_implementation;
    current_implementation = p_lisp_object(self, e_lisp_object_type_cons, current_symbol, primitive);
    lisp_attributes->environment = p_lisp_object(self, e_lisp_object_type_cons, current_implementation, lisp_attributes->environment);
    d_cast_return(primitive);
}

d_define_method(lisp, recursive_mark_environment)(struct s_object *self, struct s_lisp_object *current_object, int mark) {
    if (current_object) {
        current_object->mark = mark;
        switch (current_object->type) {
            case e_lisp_object_type_cons:
                d_call(self, m_lisp_recursive_mark_environment, d_lisp_car(current_object), mark);
                d_call(self, m_lisp_recursive_mark_environment, d_lisp_cdr(current_object), mark);
                break;
            case e_lisp_object_type_lambda:
                d_call(self, m_lisp_recursive_mark_environment, current_object->lambda.call, mark);
                d_call(self, m_lisp_recursive_mark_environment, current_object->lambda.args, mark);
                d_call(self, m_lisp_recursive_mark_environment, current_object->lambda.environment, mark);
            default:
                break;
        }
    }
    return self;
}

d_define_method(lisp, mark_environment)(struct s_object *self) {
    d_using(lisp);
    int index;
    d_call(self, m_lisp_recursive_mark_environment, lisp_attributes->environment, d_lisp_mark_environment);
    d_call(self, m_lisp_recursive_mark_environment, lisp_attributes->symbols, d_lisp_mark_symbols);
    for (index = 0; index < e_lisp_object_symbol_NULL; ++index)
        lisp_attributes->base_symbols[index]->mark = d_lisp_mark_language;
    return self;
}

d_define_method(lisp, next_token)(struct s_object *self) {
    d_using(lisp);
    int line_comment = -1;
    if (lisp_attributes->current_token)
        while ((lisp_attributes->current_token = (struct s_json_token *)(((struct s_list_node *)lisp_attributes->current_token)->next))) {
            if (line_comment != lisp_attributes->current_token->line_number) {
                if ((lisp_attributes->current_token->type == e_json_token_type_symbol) && (lisp_attributes->current_token->symbol_entry == ';'))
                    line_comment = lisp_attributes->current_token->line_number;
                else
                    break;
            }
        }
    return self;
}

d_define_method(lisp, read_object)(struct s_object *self, struct s_lisp_object *current_object) {
    d_using(lisp);
    struct s_json_token *current_token = lisp_attributes->current_token;
    struct s_lisp_object *result = NULL;
    char symbol_buffer[2] = {0, 0};
    if ((current_token->type == e_json_token_type_symbol) && (current_token->symbol_entry == '('))
        result = d_call(self, m_lisp_read_components, NULL);
    else if ((current_token->type == e_json_token_type_symbol) && (current_token->symbol_entry == '\'')) {
        /* move the cursor foward */
        d_call(self, m_lisp_next_token, NULL);
        result = p_lisp_object(self, e_lisp_object_type_cons, lisp_attributes->base_symbols[e_lisp_object_symbol_quote],
                p_lisp_object(self, e_lisp_object_type_cons, d_call(self, m_lisp_read_object, NULL), NULL));
    } else if (current_token->type == e_json_token_type_value)
        result = p_lisp_object(self, e_lisp_object_type_value, current_token->value_entry);
    else if (current_token->type == e_json_token_type_string)
        result = p_lisp_object(self, e_lisp_object_type_string, current_token->string_entry);
    else if (current_token->type == e_json_token_type_word)
        result = d_call(self, m_lisp_import_symbol, current_token->string_entry);
    else {
        symbol_buffer[0] = current_token->symbol_entry;
        result = d_call(self, m_lisp_import_symbol, symbol_buffer);
    }
    d_cast_return(result);
}

d_define_method(lisp, read_components)(struct s_object *self, struct s_lisp_object *current_object) {
    d_using(lisp);
    struct s_json_token *current_token;
    struct s_lisp_object *result = NULL, *current_car;
    /* move the cursor forward */
    d_call(self, m_lisp_next_token, NULL);
    if ((current_token = lisp_attributes->current_token))
        if ((current_token->type != e_json_token_type_symbol) || (current_token->symbol_entry != ')')) {
            current_car = d_call(self, m_lisp_read_object, NULL);
            result = p_lisp_object(self, e_lisp_object_type_cons, current_car, d_call(self, m_lisp_read_components, NULL));
        }
    d_cast_return(result);
}

d_define_method(lisp, recursive_evaluation)(struct s_object *self, struct s_lisp_object *current_object, struct s_lisp_object *environment) {
    struct s_lisp_object *result = NULL;
    if (current_object)
        result = p_lisp_object(self, e_lisp_object_type_cons, d_call(self, m_lisp_evaluate, d_lisp_car(current_object), environment),
                d_call(self, m_lisp_recursive_evaluation, d_lisp_cdr(current_object), environment));
    d_cast_return(result);
}

d_define_method(lisp, evaluate)(struct s_object *self, struct s_lisp_object *current_object, struct s_lisp_object *environment) {
    d_using(lisp);
    struct s_lisp_object *result = NULL, *evaluated_object, *symbol_object, *procedure_object, *arguments_object;
    char *symbol_string;
    if (current_object)
        switch (current_object->type) {
            case e_lisp_object_type_value:
            case e_lisp_object_type_string:
                result = current_object;
                break;
            case e_lisp_object_type_symbol:
                if ((result = d_call(self, m_lisp_retrieve_symbol, current_object, environment)))
                    result = d_lisp_cdr(result);
                else
                    d_err(e_log_level_low, "(source %s:%d) unbounded symbol '%s'", d_string_cstring(lisp_attributes->string_name), 
                            ((lisp_attributes->current_token)?lisp_attributes->current_token->line_number:0), current_object->value_symbol);
                break;
            case e_lisp_object_type_cons:
                if (d_lisp_car(current_object) == lisp_attributes->base_symbols[e_lisp_object_symbol_if]) {
                    if ((evaluated_object = d_call(self, m_lisp_evaluate, d_lisp_cadr(current_object), environment)) && 
                            (evaluated_object != lisp_attributes->base_symbols[e_lisp_object_symbol_nil]))
                        result = d_call(self, m_lisp_evaluate, d_lisp_caddr(current_object), environment);
                    else
                        result = d_call(self, m_lisp_evaluate, d_lisp_caddr(d_lisp_cdr(current_object)), environment);
                } else if (d_lisp_car(current_object) == lisp_attributes->base_symbols[e_lisp_object_symbol_lambda])
                    result = p_lisp_object(self, e_lisp_object_type_lambda, d_lisp_cadr(current_object), d_lisp_cdr(d_lisp_cdr(current_object)), environment);
                else if (d_lisp_car(current_object) == lisp_attributes->base_symbols[e_lisp_object_symbol_quote])
                    result = d_lisp_cadr(current_object);
                else if (d_lisp_car(current_object) == lisp_attributes->base_symbols[e_lisp_object_symbol_define]) {
                    if ((symbol_object = d_lisp_cadr(current_object)) && (symbol_object->type == e_lisp_object_type_symbol)) {
                        symbol_string = symbol_object->value_symbol;
                        struct s_lisp_object *evaluate = d_call(self, m_lisp_evaluate, d_lisp_caddr(current_object), environment);
                        result = d_call(self, m_lisp_extend_environment, symbol_string, evaluate);
                    } else
                        d_err(e_log_level_low, "(source %s:%d) malformed symbol, unreadable token", d_string_cstring(lisp_attributes->string_name),
                                ((lisp_attributes->current_token)?lisp_attributes->current_token->line_number:0));
                } else if (d_lisp_car(current_object) == lisp_attributes->base_symbols[e_lisp_object_symbol_begin]) {
                    if ((current_object = d_lisp_cdr(current_object))) {
                        while (d_lisp_cdr(current_object)) {
                            d_call(self, m_lisp_evaluate, d_lisp_car(current_object), environment);
                            current_object = d_lisp_cdr(current_object);
                        }
                        result = d_call(self, m_lisp_evaluate, d_lisp_car(current_object), environment);
                    }
                } else {
                    /* is not a base symbol */
                    if ((procedure_object = d_call(self, m_lisp_evaluate, d_lisp_car(current_object), environment))) {
                        arguments_object = d_call(self, m_lisp_recursive_evaluation, d_lisp_cdr(current_object), environment);
                        if (procedure_object->type == e_lisp_object_type_primitive)
                            result = procedure_object->primitive(self, arguments_object);
                        else if (procedure_object->type == e_lisp_object_type_lambda) {
                            environment = d_call(self, m_lisp_extend_closure, procedure_object->lambda.args, arguments_object, environment);
                            current_object = p_lisp_object(self, e_lisp_object_type_cons, lisp_attributes->base_symbols[e_lisp_object_symbol_begin],
                                    procedure_object->lambda.call);
                            result = d_call(self, m_lisp_evaluate, current_object, environment);
                        }
                    } else
                        d_err(e_log_level_low, "(source %s:%d) malformed symbol, unreadable token", d_string_cstring(lisp_attributes->string_name),
                                ((lisp_attributes->current_token)?lisp_attributes->current_token->line_number:0));
                }
                break;
            case e_lisp_object_type_primitive:
            case e_lisp_object_type_lambda:
                result = current_object;
        }
    d_cast_return(result);
}

d_define_method(lisp, write)(struct s_object *self, struct s_lisp_object *current_object, int output) {
    struct s_lisp_object *next_object;
    if (current_object)
        switch (current_object->type) {
            case e_lisp_object_type_value:
                dprintf(output, "%.02f", current_object->value_double);
                break;
            case e_lisp_object_type_string:
                dprintf(output, "'%s'", current_object->value_string);
                break;
            case e_lisp_object_type_cons:
                write(output, "(", sizeof(char));
                while (d_true) {
                    p_lisp_write(self, d_lisp_car(current_object), output);
                    next_object = d_lisp_cdr(current_object);
                    if (next_object) {
                        if (next_object->type != e_lisp_object_type_cons) {
                            write(output, " . ", (3 * sizeof(char)));
                            p_lisp_write(self, next_object, output);
                            write(output, ")", sizeof(char));
                            break;
                        }
                    } else {
                        write(output, ")", sizeof(char));
                        break;
                    }
                    write(output, " ", sizeof(char));
                    current_object = next_object;
                }
                break;
            case e_lisp_object_type_symbol:
                dprintf(output, "%s", current_object->value_symbol);
                break;
            case e_lisp_object_type_primitive:
                dprintf(output, "#<PRIMOP>");
                break;
            case e_lisp_object_type_lambda:
                dprintf(output, "[#<LAMBDA> ");
                p_lisp_write(self, current_object->lambda.call, output);
                p_lisp_write(self, current_object->lambda.args, output);
                write(output, "]", sizeof(char));
                break;
            default:
                break;
        }
    return self;
}

d_define_method(lisp, sweep_collector)(struct s_object *self, unsigned char excluded_marks) {
    d_using(lisp);
    struct s_lisp_object *current_entry = (struct s_lisp_object *)(lisp_attributes->collector.head), *next_entry;
    while (current_entry) {
        next_entry = (struct s_lisp_object *)(((struct s_list_node *)current_entry)->next);
        if ((current_entry->mark & excluded_marks) == 0) {
            f_list_delete(&(lisp_attributes->collector), (struct s_list_node *)current_entry);
            if ((current_entry->type == e_lisp_object_type_string) && (current_entry->value_string))
                d_free(current_entry->value_string);
            d_free(current_entry);
        }
        current_entry = next_entry;
    }
    return self;
}

d_define_method(lisp, run)(struct s_object *self) {
    d_using(lisp);
    int line_comment = -1;
    struct s_lisp_object *current_object, *current_evaluation;
    lisp_attributes->current_token = (struct s_json_token *)(lisp_attributes->tokens.head);
    while (lisp_attributes->current_token) {
        if (line_comment != lisp_attributes->current_token->line_number) {
            if ((lisp_attributes->current_token->type == e_json_token_type_symbol) && 
                    (lisp_attributes->current_token->symbol_entry == ';'))
                line_comment = lisp_attributes->current_token->line_number;
            else {
                line_comment = -1;
                break;
            }
        }
        lisp_attributes->current_token = (struct s_json_token *)(((struct s_list_node *)lisp_attributes->current_token)->next);
    }
    while (lisp_attributes->current_token) {
        current_object = d_call(self, m_lisp_read_object, NULL);
        current_evaluation = d_call(self, m_lisp_evaluate, current_object, lisp_attributes->environment);
        d_call(self, m_lisp_mark_environment, NULL);
        d_call(self, m_lisp_sweep_collector, d_lisp_mark_internal);
        d_call(self, m_lisp_next_token, NULL);
    }
    return self;
}

d_define_method(lisp, delete)(struct s_object *self, struct s_lisp_attributes *attributes) {
    struct s_json_token *local_token;
    d_call(self, m_lisp_sweep_collector, d_lisp_mark_none);
    while (attributes->tokens.head)
        if ((local_token = (struct s_json_token *)f_list_delete(&(attributes->tokens), attributes->tokens.head))) {
            p_json_tokenizer_free(local_token);
            d_free(local_token);
        }
    if (attributes->string_name)
        d_delete(attributes->string_name);
    return NULL;
}

d_define_class(lisp) {
    d_hook_method(lisp, e_flag_private, import_symbol),
        d_hook_method(lisp, e_flag_private, retrieve_symbol),
        d_hook_method(lisp, e_flag_public, extend_closure),
        d_hook_method(lisp, e_flag_public, extend_environment),
        d_hook_method(lisp, e_flag_private, recursive_mark_environment),
        d_hook_method(lisp, e_flag_private, mark_environment),
        d_hook_method(lisp, e_flag_private, next_token),
        d_hook_method(lisp, e_flag_private, read_object),
        d_hook_method(lisp, e_flag_private, read_components),
        d_hook_method(lisp, e_flag_private, recursive_evaluation),
        d_hook_method(lisp, e_flag_private, evaluate),
        d_hook_method(lisp, e_flag_public, write),
        d_hook_method(lisp, e_flag_private, sweep_collector),
        d_hook_method(lisp, e_flag_public, run),
        d_hook_delete(lisp),
        d_hook_method_tail
};
