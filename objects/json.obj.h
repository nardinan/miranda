/*
 * miranda
 * Copyright (C) 2015 Andrea Nardinocchi (andrea@nardinan.it)
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
#ifndef miranda_object_json_h
#define miranda_object_json_h
#include "stream.obj.h"
#define d_json_tabs(o,t) for(int tb=((t)+1);(--tb)>0;write((o),"\t",sizeof(char)))
#define d_json_name_size 32
#define d_json_value_size 20
#define d_json_value_maximum_decimals 6
#define d_json_file_default_permission 0744
#define d_json_decimal_null -1
#define d_json_decimal_characters "."
#define d_json_division_characters " ,.:;{}()[]<>*/\\"
#define d_json_string_character "\"'"
#define d_json_ignorable_characters " \n\r\t"
#define d_json_separator_characters ":="
d_exception_declare(malformed_key);
d_exception_declare(malformed_value);
typedef enum e_json_token_types {
	e_json_token_type_symbol,
	e_json_token_type_word,
	e_json_token_type_string,
	e_json_token_type_value
} e_json_token_types;
typedef struct s_json_token {
	d_list_node_head;
	enum e_json_token_types type;
	union {
		char symbol_entry, *string_entry;
		float value_entry;
	};
} s_json_token;
typedef enum e_json_node_actions {
	e_json_node_action_key,
	e_json_node_action_symbol,
	e_json_node_action_value,
	e_json_node_action_close,
	e_json_node_action_end,
	e_json_node_action_undefined
} e_json_node_actions;
typedef enum e_json_node_types {
	e_json_node_type_string,
	e_json_node_type_value,
	e_json_node_type_boolean,
	e_json_node_type_array,
	e_json_node_type_object,
	e_json_node_type_null,
	e_json_node_type_undefined
} e_json_node_types;
typedef struct s_json_node_value {
	d_list_node_head;
	enum e_json_node_types type;
	union {
		char *string_entry;
		float value_entry;
		t_boolean boolean_entry;
		struct s_list *array_entry, *object_entry;
	};
} s_json_node_value;
typedef struct s_json_node {
	d_list_node_head;
	char *key;
	struct s_json_node_value value;
} s_json_node;
d_declare_class(json) {
	struct s_attributes head;
	struct s_list *tokens;
	struct s_json_node_value *root;
} d_declare_class_tail(json);
struct s_json_attributes *p_json_alloc(struct s_object *self);
extern void p_json_tokenizer_free(struct s_json_token *token);
extern size_t p_json_tokenizer_string_append(struct s_json_token *local_token, char *source_head, char *source_tail);
extern void f_json_tokenizer(struct s_object *stream_file, struct s_list *tokens);
extern void p_json_analyzer_free(struct s_json_node_value *value);
extern char p_json_analyzer_value(struct s_list *tokens, struct s_json_node_value *provided_value, struct s_list *nodes);
extern void p_json_analyzer_key(struct s_list *tokens, struct s_list *nodes);
extern void f_json_analyzer(struct s_list *tokens, struct s_json_node_value *root);
extern void p_json_write_value(struct s_json_node_value *node, int level, int output);
extern struct s_object *f_json_new(struct s_object *self, struct s_object *string_name);
extern struct s_object *f_json_new_stream(struct s_object *self, struct s_object *stream_file);
d_declare_method(json, write)(struct s_object *self, struct s_object *stream_file);
d_declare_method(json, get_value)(struct s_object *self, va_list parameters);
d_declare_method(json, get_string)(struct s_object *self, char **string_supply, ...);
d_declare_method(json, get_float)(struct s_object *self, float *value_supply, ...);
d_declare_method(json, delete)(struct s_object *self, struct s_json_attributes *attributes);
#endif

