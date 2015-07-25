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
#include "json.obj.h"
d_exception_define(malformed_key,	9,  "malformed JSON key exception");
d_exception_define(malformed_value,	10, "malformed JSON value exception");
d_exception_define(wrong_type,		11, "wrong type required exception");
const char *v_json_node_types[] = {
	"string",
	"integer/double",
	"boolean",
	"array",
	"object",
	"null",
	"undefined"
};
struct s_json_attributes *p_json_alloc(struct s_object *self) {
	struct s_json_attributes *result = d_prepare(self, json);
	f_memory_new(self);	/* inherit */
	f_mutex_new(self);	/* inherit */
	return result;
}

void p_json_tokenizer_free(struct s_json_token *token) {
	if ((token->type == e_json_token_type_string) || (token->type == e_json_token_type_word))
		if (token->string_entry)
			d_free(token->string_entry);
}

size_t p_json_tokenizer_string_append(struct s_json_token *local_token, char *source_head, char *source_tail) {
	size_t old_content_length, add_content_length, new_content_length;
	old_content_length = f_string_strlen(local_token->string_entry);
	add_content_length = (source_tail - source_head);
	new_content_length = (old_content_length + add_content_length);
	if ((local_token->string_entry = d_realloc(local_token->string_entry, (new_content_length + 1))))
		strncpy((local_token->string_entry + old_content_length), source_head, add_content_length);
	else
		d_die(d_error_malloc);
	return new_content_length;
}

void f_json_tokenizer(struct s_object *stream_file, struct s_list *tokens) {
	struct s_json_token *local_token = NULL;
	struct s_object *current_string, *string_readed = NULL;
	size_t length;
	char *string_pointer, *head_pointer = NULL, string_definition_character = '\0', last_character = '\0';
	int index, decimal_digits = d_json_decimal_null;
	t_boolean submit_token, keep_index;
	while ((current_string = d_call(stream_file, m_stream_read_string, string_readed, d_stream_block_size))) {
		string_readed = current_string;
		d_call(string_readed, m_string_length, &length);
		if ((length > 0) && ((string_pointer = d_string_cstring(string_readed)))) {
			index = 0;
			while (index < length) {
				submit_token = d_false;
				keep_index = d_false;
				if (local_token) {
					switch (local_token->type) {
						case e_json_token_type_string:
							if ((string_pointer[index] == string_definition_character) && (last_character != '\\')) {
								if (head_pointer)
									p_json_tokenizer_string_append(local_token, head_pointer, &(string_pointer[index]));
								submit_token = d_true;
							} else if (!head_pointer)
								head_pointer = &(string_pointer[index]);
							break;
						case e_json_token_type_word:
							if (strchr(d_json_division_characters, string_pointer[index])) {
								if (head_pointer)
									p_json_tokenizer_string_append(local_token, head_pointer, &(string_pointer[index]));
								submit_token = d_true;
								keep_index = d_true;
							} else if (!head_pointer)
								head_pointer = &(string_pointer[index]);
							break;
						case e_json_token_type_value:
							if (strchr(d_json_decimal_characters, string_pointer[index]))
								decimal_digits = 0;
							else if (isdigit(string_pointer[index])) {
								local_token->value_entry = (local_token->value_entry * 10) + (string_pointer[index] - '0');
								if (decimal_digits != d_json_decimal_null)
									decimal_digits++;
							} else {
								for (; decimal_digits > 0; --decimal_digits)
									local_token->value_entry /= 10.0f;
								decimal_digits = d_json_decimal_null;
								submit_token = d_true;
								keep_index = d_true;
							}
						default:
							break;
					}
				} else if (!strchr(d_json_ignorable_characters, string_pointer[index])) {
					if ((local_token = (struct s_json_token *) d_malloc(sizeof(struct s_json_token)))) {
						if (isalpha(string_pointer[index])) {
							/* it has to be a word */
							local_token->type = e_json_token_type_word;
							head_pointer = &(string_pointer[index]);
						} else if (isdigit(string_pointer[index])) {
							/* it has to be a number */
							local_token->type = e_json_token_type_value;
							local_token->value_entry = (string_pointer[index] - '0');
						} else if (strchr(d_json_string_character, string_pointer[index])) {
							/* it has to be a string */
							local_token->type = e_json_token_type_string;
							string_definition_character = string_pointer[index];
						} else {
							/* it has to be a symbol */
							local_token->type = e_json_token_type_symbol;
							local_token->symbol_entry = string_pointer[index];
							submit_token = d_true;
						}
					} else
						d_die(d_error_malloc);
				}
				if ((submit_token) && (local_token)) {
					f_list_append(tokens, (struct s_list_node *)local_token, e_list_insert_tail);
					local_token = NULL;
					head_pointer = NULL;
				}
				last_character = string_pointer[index];
				if (keep_index == d_false)
					index++;
			}
			if ((local_token) && (head_pointer)) {
				p_json_tokenizer_string_append(local_token, head_pointer, &(string_pointer[index]));
				head_pointer = NULL;
			}
		}
	}
	if (local_token) {
		if (head_pointer)
			p_json_tokenizer_string_append(local_token, head_pointer, &(string_pointer[index]));
		f_list_append(tokens, (struct s_list_node *)local_token, e_list_insert_tail);
	}
	if (string_readed)
		d_delete(string_readed);
}

void p_json_analyzer_free(struct s_json_node_value *value) {
	struct s_json_node_value *local_value;
	struct s_json_node *local_node;
	switch (value->type) {
		case e_json_node_type_array:
			while (value->array_entry->head)
				if ((local_value = (struct s_json_node_value *)f_list_delete(value->array_entry, value->array_entry->head))) {
					p_json_analyzer_free(local_value);
					d_free(local_value);
				}
			f_list_destroy(&(value->array_entry));
			break;
		case e_json_node_type_object:
			while (value->object_entry->head)
				if ((local_node = (struct s_json_node *)f_list_delete(value->object_entry, value->object_entry->head))) {
					p_json_analyzer_free(&(local_node->value));
					if ((local_node->allocated) && (local_node->key))
						d_free(local_node->key);
					d_free(local_node);
				}
			f_list_destroy(&(value->object_entry));
			break;
		case e_json_node_type_string:
			if ((value->allocated) && (value->string_entry)) {
				d_free(value->string_entry);
				value->string_entry = NULL;
			}
		default:
			break;
	}
}

char p_json_analyzer_value(struct s_list *tokens, struct s_json_node_value *provided_value, struct s_list *nodes) {
	struct s_json_token *local_token;
	struct s_json_node_value *local_value = provided_value;
	enum e_json_node_actions current_action = e_json_node_action_value;
	char last_character = '\0';
	while ((local_token = (struct s_json_token *)tokens->current)) {
		if (!local_value)
			if (!(local_value = (struct s_json_node_value *) d_malloc(sizeof(struct s_json_node_value))))
				d_die(d_error_malloc);
		switch (current_action) {
			case e_json_node_action_value:
				switch (local_token->type) {
					case e_json_token_type_symbol:
						/* a symbol means that is either a object or an array */
						switch (local_token->symbol_entry) {
							case '{':
								local_value->type = e_json_node_type_object;
								f_list_init(&(local_value->object_entry));
								tokens->current = tokens->current->next;
								p_json_analyzer_key(tokens, local_value->object_entry);
								break;
							case '[':
								local_value->type = e_json_node_type_array;
								f_list_init(&(local_value->array_entry));
								tokens->current = tokens->current->next;
								p_json_analyzer_value(tokens, NULL, local_value->array_entry);
								break;
							default:
								local_value->type = e_json_node_type_undefined;
						}
						break;
					case e_json_token_type_word:
						/* a word means that is either a boolean or a null */
						if (f_string_strcmp(local_token->string_entry, "false") == 0) {
							local_value->type = e_json_node_type_boolean;
							local_value->boolean_entry = d_false;
						} else if (f_string_strcmp(local_token->string_entry, "true") == 0) {
							local_value->type = e_json_node_type_boolean;
							local_value->boolean_entry = d_true;
						} else if (f_string_strcmp(local_token->string_entry, "null") == 0)
							local_value->type = e_json_node_type_null;
						else
							local_value->type = e_json_node_type_undefined;
						break;
					case e_json_token_type_value:
						local_value->type = e_json_node_type_value;
						local_value->value_entry = local_token->value_entry;
						break;
					case e_json_token_type_string:
						local_value->type = e_json_node_type_string;
						local_value->string_entry = local_token->string_entry;
						break;
					default:
						local_value->type = e_json_node_type_undefined;
				}
				if (local_value->type != e_json_node_type_undefined)
					current_action = e_json_node_action_close;
				else
					d_throw(v_exception_malformed_value, "unexpected undefined token as value");
				break;
			case e_json_node_action_close:
				if (local_token->type == e_json_token_type_symbol) {
					switch (local_token->symbol_entry) {
						case '}':
						case ']':
							last_character = local_token->symbol_entry;
							current_action = e_json_node_action_end;
							break;
						case ',':
							if (!nodes) {
								last_character = local_token->symbol_entry;
								current_action = e_json_node_action_end;
							} else
								current_action = e_json_node_action_value;
							break;
						default:
							current_action = e_json_node_action_undefined;
					}
				} else
					current_action = e_json_node_action_undefined;
				if (current_action != e_json_node_action_undefined) {
					if (nodes)
						f_list_append(nodes, (struct s_list_node *)local_value, e_list_insert_tail);
					local_value = NULL;
				} else
					d_throw(v_exception_malformed_value, "unexpected undefined token as closing character");
			default:
				break;
		}
		if (current_action == e_json_node_action_end)
			break;
		tokens->current = tokens->current->next;
	}
	return last_character;
}

void p_json_analyzer_key(struct s_list *tokens, struct s_list *nodes) {
	struct s_json_node *local_node = NULL;
	struct s_json_token *local_token;
	struct s_exception *exception;
	enum e_json_node_actions current_action = e_json_node_action_key;
	char buffer[d_string_buffer_size], last_character;
	while ((local_token = (struct s_json_token *)tokens->current)) {
		if (!local_node)
			if (!(local_node = (struct s_json_node *) d_malloc(sizeof(struct s_json_node))))
				d_die(d_error_malloc);
		switch (current_action) {
			case e_json_node_action_key:
				if ((local_token->type == e_json_token_type_string) || (local_token->type == e_json_token_type_word)) {
					local_node->key = local_token->string_entry;
					current_action = e_json_node_action_symbol;
				} else
					d_throw(v_exception_malformed_key, "unexpected non-string token as key");
				break;
			case e_json_node_action_symbol:
				if ((local_token->type == e_json_token_type_symbol) && (strchr(d_json_separator_characters, local_token->symbol_entry)))
					current_action = e_json_node_action_value;
				else {
					snprintf(buffer, d_string_buffer_size, "unexpected non-symbol token after key %s", local_node->key);
					d_throw(v_exception_malformed_key, buffer);
				}
				break;
			case e_json_node_action_value:
				d_try {
					if ((last_character = p_json_analyzer_value(tokens, &(local_node->value), NULL)) == '}')
						current_action = e_json_node_action_end;
					else
						current_action = e_json_node_action_key;
					f_list_append(nodes, (struct s_list_node *)local_node, e_list_insert_tail);
					local_node = NULL;
				} d_catch(exception) {
					d_exception_dump(stderr, exception);
					d_raise;
				} d_endtry;
			default:
				break;
		}
		if (current_action == e_json_node_action_end)
			break;
		tokens->current = tokens->current->next;
	}
}

void f_json_analyzer(struct s_list *tokens, struct s_json_node_value *root) {
	tokens->current = tokens->head;
	p_json_analyzer_value(tokens, root, NULL);
}

void p_json_write_value(struct s_json_node_value *node, int level, int output) {
	struct s_json_node_value *local_value;
	struct s_json_node *local_node;
	char buffer[d_json_value_size], *digital_position;
	int index;
	t_boolean first_entry;
	switch (node->type) {
		case e_json_node_type_array:
			write(output, "[", sizeof(char));
			first_entry = d_false;
			d_foreach(node->array_entry, local_value, struct s_json_node_value)
				if (local_value->type != e_json_node_type_null) {
					/* we have to remove every NULL value in arrays */
					if (!first_entry)
						write(output, ",", sizeof(char));
					first_entry = d_false;
					p_json_write_value(local_value, (level + 1), output);
				}
			write(output, "]", sizeof(char));
			break;
		case e_json_node_type_object:
			write(output, "{", sizeof(char));
			d_foreach(node->object_entry, local_node, struct s_json_node) {
				write(output, "\n", sizeof(char));
				for (index = ((level+1)+1); (--index) > 0; write(output, "\t", sizeof(char)));
				dprintf(output, "\"%s\":", local_node->key);
				p_json_write_value(&(local_node->value), (level + 1), output);
				if (local_node->head.next)
					write(output, ",", sizeof(char));
			}
			write(output, "\n", sizeof(char));
			for (index = (level+1); (--index) > 0; write(output, "\t", sizeof(char)));
			write(output, "}", sizeof(char));
			break;
		case e_json_node_type_string:
			dprintf(output, "\"%s\"", node->string_entry);
			break;
		case e_json_node_type_value:
			snprintf(buffer, d_json_value_size, "%f", node->value_entry);
			if ((digital_position = strchr(buffer, '.')))
				dprintf(output, "%.*g", (int)((digital_position-buffer)+d_json_value_maximum_decimals), node->value_entry);
			break;
		case e_json_node_type_null:
			write(output, "null", f_string_strlen("null"));
			break;
		case e_json_node_type_boolean:
			dprintf(output, "%s", (node->boolean_entry)?"true":"false");
		default:
			break;
	}
	fsync(output);
}

struct s_object *f_json_new(struct s_object *self, struct s_object *string_name) {
	struct s_object *stream_file = f_stream_new_file(d_new(stream), string_name, "r", d_json_file_default_permission);
	f_json_new_stream(self, stream_file);
	d_delete(stream_file);
	return self;
}

struct s_object *f_json_new_stream(struct s_object *self, struct s_object *stream_file) {
	struct s_json_attributes *attributes = p_json_alloc(self);
	struct s_exception *exception;
	f_list_init(&(attributes->tokens));
	if ((attributes->root = (struct s_json_node_value *) d_malloc(sizeof(struct s_json_node_value)))) {
		f_json_tokenizer(stream_file, attributes->tokens);
		d_try {
			f_json_analyzer(attributes->tokens, attributes->root);
		} d_catch(exception) {
			d_exception_dump(stderr, exception);
			d_raise;
		} d_endtry;
	} else
		d_die(d_error_malloc);
	return self;
}

d_define_method(json, write)(struct s_object *self, struct s_object *stream_file) {
	d_using(json);
	int descriptor;
	d_call(stream_file, m_stream_get_descriptor, &descriptor);
	p_json_write_value(json_attributes->root, 0, descriptor);
	return self;
}

d_define_method(json, get_value)(struct s_object *self, const char *format, va_list parameters) {
	d_using(json);
	struct s_json_node_value *value = json_attributes->root, *local_value;
	struct s_json_node *local_node;
	int argument_long;
	char *argument_string, *pointer = (char *)format;
	t_boolean founded = d_false;
	while ((*pointer) && (value)) {
		switch (value->type) {
			case e_json_node_type_array:
				local_value = (struct s_json_node_value *)value->array_entry->head;
				for (argument_long = va_arg(parameters, long); argument_long > 0; --argument_long)
					if (local_value)
						local_value = (struct s_json_node_value *)local_value->head.next;
				value = local_value;
				break;
			case e_json_node_type_object:
				argument_string = va_arg(parameters, char *);
				d_foreach(value->object_entry, local_node, struct s_json_node)
					if (f_string_strcmp(local_node->key, argument_string) == 0)
						break;
				if (local_node)
					value = &(local_node->value);
				else
					value = NULL;
				break;
			default:
				founded = d_true;
		}
		if (founded)
			break;
		pointer++;
	}
	return (struct s_object *)value;
}

d_define_method(json, get_string)(struct s_object *self, char **string_supply, const char *format, ...) {
	struct s_json_node_value *value = NULL;
	va_list parameters;
	char buffer[d_string_buffer_size];
	va_start(parameters, format);
	if ((value = (struct s_json_node_value *)d_call(self, m_json_get_value, format, parameters))) {
		if (value->type == e_json_node_type_string)
			*string_supply = value->string_entry;
		else {
			snprintf(buffer, d_string_buffer_size, "string has been required but %s has been returned", v_json_node_types[value->type]);
			d_throw(v_exception_wrong_type, buffer);
		}
	}
	va_end(parameters);
	return (struct s_object *)value;
}

d_define_method(json, get_float)(struct s_object *self, float *value_supply, const char *format, ...) {
	struct s_json_node_value *value = NULL;
	va_list parameters;
	char buffer[d_string_buffer_size];
	va_start(parameters, format);
	if ((value = (struct s_json_node_value *)d_call(self, m_json_get_value, format, parameters))) {
		if (value->type == e_json_node_type_value)
			*value_supply = value->value_entry;
		else {
			snprintf(buffer, d_string_buffer_size, "int/double has been required but %s has been returned", v_json_node_types[value->type]);
			d_throw(v_exception_wrong_type, buffer);
		}
	}
	va_end(parameters);
	return (struct s_object *)value;
}

d_define_method(json, get_boolean)(struct s_object *self, t_boolean *boolean_supply, const char *format, ...) {
	struct s_json_node_value *value = NULL;
	va_list parameters;
	char buffer[d_string_buffer_size];
	va_start(parameters, format);
	if ((value = (struct s_json_node_value *)d_call(self, m_json_get_value, format, parameters))) {
		if (value->type == e_json_node_type_boolean)
			*boolean_supply = value->boolean_entry;
		else {
			snprintf(buffer, d_string_buffer_size, "boolean has been required but %s has been returned", v_json_node_types[value->type]);
			d_throw(v_exception_wrong_type, buffer);
		}
	}
	va_end(parameters);
	return (struct s_object *)value;
}

d_define_method(json, set_value)(struct s_object *self, const char *format, va_list parameters) {
	struct s_json_node_value *value = NULL, *local_value = NULL;
	if ((value = (struct s_json_node_value *)d_call(self, m_json_get_value, format, parameters))) {
		switch (value->type) {
			case e_json_node_type_array:
				if ((local_value = (struct s_json_node_value *) d_malloc(sizeof(struct s_json_node_value)))) {
					local_value->type = e_json_node_type_null;
					f_list_append(value->array_entry, (struct s_list_node *)local_value, e_list_insert_tail);
				} else
					d_die(d_error_malloc);
				value = local_value;
				break;
			case e_json_node_type_object:
				d_throw(v_exception_wrong_type, "a base-type isn't writable in an oject");
			default:
				break;
		}
		p_json_analyzer_free(value);
	}
	return (struct s_object *)value;
}

d_define_method(json, set_string)(struct s_object *self, char *string_supply, const char *format, ...) {
	struct s_json_node_value *value = NULL;
	struct s_exception *exception;
	va_list parameters;
	va_start(parameters, format);
	d_try {
		if ((value = (struct s_json_node_value *)d_call(self, m_json_set_value, format, parameters))) {
			value->type = e_json_node_type_string;
			value->allocated = d_true;
			if ((value->string_entry = (char *) d_malloc(f_string_strlen(string_supply) + 1)))
				strcpy(value->string_entry, string_supply);
			else
				d_die(d_error_malloc);
		}
	} d_catch(exception) {
		d_exception_dump(stderr, exception);
		d_raise;
	} d_endtry;
	va_end(parameters);
	return (struct s_object *)value;
}

d_define_method(json, set_float)(struct s_object *self, float value_supply, const char *format, ...) {
	struct s_json_node_value *value = NULL;
	struct s_exception *exception;
	va_list parameters;
	va_start(parameters, format);
	d_try {
		if ((value = (struct s_json_node_value *)d_call(self, m_json_set_value, format, parameters))) {
			value->type = e_json_node_type_value;
			value->value_entry = value_supply;
		}
	} d_catch(exception) {
		d_exception_dump(stderr, exception);
		d_raise;
	} d_endtry;
	va_end(parameters);
	return (struct s_object *)value;
}

d_define_method(json, set_boolean)(struct s_object *self, t_boolean boolean_supply, const char *format, ...) {
	struct s_json_node_value *value = NULL;
	struct s_exception *exception;
	va_list parameters;
	va_start(parameters, format);
	d_try {
		if ((value = (struct s_json_node_value *)d_call(self, m_json_set_value, format, parameters))) {
			value->type = e_json_node_type_boolean;
			value->boolean_entry = boolean_supply;
		}
	} d_catch(exception) {
		d_exception_dump(stderr, exception);
		d_raise;
	} d_endtry;
	va_end(parameters);
	return (struct s_object *)value;
}

d_define_method(json, set_array)(struct s_object *self, const char *format, ...) {
	struct s_json_node_value *value = NULL;
	struct s_exception *exception;
	va_list parameters;
	va_start(parameters, format);
	d_try {
		if ((value = (struct s_json_node_value *)d_call(self, m_json_set_value, format, parameters))) {
			value->type = e_json_node_type_array;
			f_list_init(&(value->array_entry));
		}
	} d_catch(exception) {
		d_exception_dump(stderr, exception);
		d_raise;
	} d_endtry;
	va_end(parameters);
	return (struct s_object *)value;
}

d_define_method(json, insert_value)(struct s_object *self, const char *key, const char *format, ...) {
	struct s_json_node_value *value = NULL, *local_value = NULL;
	struct s_json_node *local_node = NULL;
	va_list parameters;
	va_start(parameters, format);
	if ((value = (struct s_json_node_value *)d_call(self, m_json_get_value, format, parameters))) {
		p_json_write_value(value, 0, STDOUT_FILENO);
		if ((local_node = (struct s_json_node *) d_malloc(sizeof(struct s_json_node)))) {
			local_node->allocated = d_true;
			if ((local_node->key = (char *) d_malloc(f_string_strlen(key) + 1))) {
				strcpy(local_node->key, key);
				local_node->value.type = e_json_node_type_null;
				switch (value->type) {
					case e_json_node_type_array:
						if ((local_value = (struct s_json_node_value *) d_malloc(sizeof(struct s_json_node_value)))) {
							local_value->type = e_json_node_type_object;
							f_list_init(&(local_value->object_entry));
							f_list_append(local_value->object_entry, (struct s_list_node *)local_node, e_list_insert_tail);
						} else
							d_die(d_error_malloc);
						f_list_append(value->array_entry, (struct s_list_node *)local_value, e_list_insert_tail);
						break;
					case e_json_node_type_null:
						/* promotion of the NULL entry to a real object */
						value->type = e_json_node_type_object;
						f_list_init(&(local_value->object_entry));
					case e_json_node_type_object:
						f_list_append(value->object_entry, (struct s_list_node *)local_node, e_list_insert_tail);
						break;
					default:
						d_throw(v_exception_wrong_type, "an object isn't writable in a base-type");
				}
			} else
				d_die(d_error_malloc);
		} else
			d_die(d_error_malloc);
	}
	va_end(parameters);
	return (struct s_object *)local_node;
}

d_define_method(json, delete_value)(struct s_object *self, const char *format, ...) {
	struct s_json_node_value *value;
	va_list parameters;
	va_start(parameters, format);
	if ((value = (struct s_json_node_value *)d_call(self, m_json_get_value, format, parameters))) {
		p_json_analyzer_free(value);
		value->type = e_json_node_type_null;
	}
	va_end(parameters);
	return (struct s_object *)value;
}

d_define_method(json, delete)(struct s_object *self, struct s_json_attributes *attributes) {
	struct s_json_token *local_token;
	if (attributes->tokens) {
		while (attributes->tokens->head)
			if ((local_token = (struct s_json_token *)f_list_delete(attributes->tokens, attributes->tokens->head))) {
				p_json_tokenizer_free(local_token);
				d_free(local_token);
			}
		f_list_destroy(&(attributes->tokens));
	}
	if (attributes->root) {
		p_json_analyzer_free(attributes->root);
		d_free(attributes->root);
	}
	return NULL;
}

d_define_class(json) {
	d_hook_method(json, e_flag_public, write),
	d_hook_method(json, e_flag_private, get_value),
	d_hook_method(json, e_flag_public, get_string),
	d_hook_method(json, e_flag_public, get_float),
	d_hook_method(json, e_flag_public, get_boolean),
	d_hook_method(json, e_flag_private, set_value),
	d_hook_method(json, e_flag_public, set_string),
	d_hook_method(json, e_flag_public, set_float),
	d_hook_method(json, e_flag_public, set_boolean),
	d_hook_method(json, e_flag_public, set_array),
	d_hook_method(json, e_flag_public, insert_value),
	d_hook_method(json, e_flag_public, delete_value),
	d_hook_delete(json),
	d_hook_method_tail
};
