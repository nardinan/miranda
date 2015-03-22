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
#include "console.h"
const char *v_console_styles[] = {
	"\x1B[0m",
	"\x1B[1m",
	"\x1B[3m",
	"\x1B[4m",
	"\x1B[5m",
	"\x1B[7m",
	"\x1B[31m",
	"\x1B[32m",
	"\x1B[33m",
	"\x1B[34m",
	"\x1b[37m",
	"\x1B[37m",
	"\033[1K",
	"\x1B[2J"
};
int f_console_parameter(const char *symbol, char **tokens, size_t elements, int is_flag) {
	int result = d_console_parameter_null, index;
	for (index = 0; index < elements; index++)
		if (f_string_strcmp(tokens[index], symbol) == 0) {
			if (is_flag)
				result = index;
			else if ((index+1) < elements)
				result = (index+1);
			break;
		}
	return result;
}

int f_console_init(struct s_console **console, struct s_console_command *commands, int descriptor) {
	int result = d_false;
	if ((*console = (struct s_console *) d_malloc(sizeof(struct s_console)))) {
		memset((*console), 0, sizeof(struct s_console));
		(*console)->commands = commands;
		(*console)->descriptor = descriptor;
		if (tcgetattr(descriptor, &((*console)->old_configuration)) == 0) {
			memcpy(&((*console)->current_configuration), &((*console)->old_configuration), sizeof(struct termios));
			(*console)->current_configuration.c_lflag &= ~(ECHO|ICANON);
			(*console)->current_configuration.c_cc[VMIN] = 1;
			(*console)->current_configuration.c_cc[VTIME] = 0;
			if (tcsetattr(descriptor, TCSAFLUSH, &((*console)->current_configuration)) == 0)
				result = d_true;
		}
	} else
		d_die(d_error_malloc);
	return result;
}

int f_console_destroy(struct s_console **console) {
	int result = d_false;
	if (*console) {
		if (tcsetattr((*console)->descriptor, TCSAFLUSH, &((*console)->old_configuration)) == 0)
			result = d_true;
		d_free(*console);
	}
	return result;
}

void f_console_refresh(struct s_console *console, struct s_console_input *input, int output) {
	char buffer[d_string_buffer_size] = {'\0'}, before[d_string_buffer_size] = {'\0'}, after[d_string_buffer_size] = {'\0'};
	if (input->data_length > 0) {
		if (input->data_length > input->data_pointer) {
			strncpy(before, input->input, input->data_pointer);
			strncpy(after, input->input+(input->data_pointer+1), (input->data_length+1)-(input->data_pointer+1));
			snprintf(buffer, d_string_buffer_size, "%s%s%c%s%s", before, v_console_styles[e_console_style_reverse],
					input->input[input->data_pointer], v_console_styles[e_console_style_reset], after);
		} else
			strcpy(buffer, input->input);
	}
	f_console_write(console, buffer, output);
}

void f_console_write(struct s_console *console, const char *buffer, int output) {
	if (output != d_console_descriptor_null) {
		write(output, v_console_styles[e_console_style_clean_line], f_string_strlen(v_console_styles[e_console_style_clean_line]));
		write(output, console->prefix, f_string_strlen(console->prefix));
		if (buffer)
			write(output, buffer, f_string_strlen(buffer));
		fsync(output);
	}
}

void p_console_append_history(struct s_console *console, const char *buffer) {
	int index;
	if (console->history_last >= d_console_history_size) {
		for (index = (console->history_last-1); index > 0; index--)
			strcpy(console->history[index-1], console->history[index]);
		console->history_last--;
	}
	strncpy(console->history[console->history_last++], buffer, d_string_buffer_size);
	console->history_pointer = console->history_last;
}

void p_console_write_history(struct s_console *console, struct s_console_input *input, int output) {
	int change = d_false;
	if (output != d_console_descriptor_null)
		switch (input->special[2]) {
			case 66:
				change = d_true;
				if (console->history_pointer <= console->history_last)
					console->history_pointer++;
				break;
			case 65:
				change = d_true;
				if (console->history_pointer > 0)
					console->history_pointer--;
				break;
			case 68:
				if (input->data_pointer > 0)
					input->data_pointer--;
				break;
			case 67:
				if (input->data_pointer < input->data_length)
					input->data_pointer++;

		}
	if ((console->history_pointer <= console->history_last) && (change)) {
		memset(input->input, 0, d_string_buffer_size);
		strcpy(input->input, console->history[console->history_pointer]);
		input->data_length = input->data_pointer = f_string_strlen(console->history[console->history_pointer]);
	}
}

void p_console_write_suggestion(struct s_console *console, struct s_console_input *input, int output) {
	char buffer[d_console_output_size] = {0}, backup[d_string_buffer_size], common_substring[d_console_command_size] = {0};
	int index, match, last_match = 0, current_match;
	if ((output != d_console_descriptor_null) && (console->commands)) {
		for (index = 0, match = 0; console->commands[index].initialized; index++)
			if (console->commands[index].level <= console->level)
				if (f_string_strncmp(input->input, console->commands[index].command, input->data_length) == 0) {
					if (match == 0)
						strcpy(common_substring, console->commands[index].command);
					else {
						current_match = 0;
						while (console->commands[index].command[current_match] != '\0') {
							if (console->commands[index].command[current_match] != common_substring[current_match]) {
								common_substring[current_match] = '\0';
								break;
							}
							current_match++;
						}
					}
					last_match = index;
					snprintf(backup, d_string_buffer_size, "%s%s", ((match%d_console_suggestion_columns) == 0)?"\n":"\t\t",
							console->commands[index].command);
					strncat(buffer, backup, (d_console_output_size-f_string_strlen(buffer))-1);
					match++;
				}
		if (match > 0) {
			strcpy(input->input, common_substring);
			input->data_length = input->data_pointer = f_string_strlen(common_substring);
			if (match == 1) {
				snprintf(buffer, d_console_output_size, "\n%s%sCOMMAND: %s%s\n%s%sDESCRIPTION: %s%s",
						v_console_styles[e_console_style_bold], v_console_styles[e_console_style_yellow],
						v_console_styles[e_console_style_reset], console->commands[last_match].command,
						v_console_styles[e_console_style_bold], v_console_styles[e_console_style_yellow],
						v_console_styles[e_console_style_reset], console->commands[last_match].description);
				if ((console->commands[last_match].parameters) && (console->commands[last_match].parameters[0].initialized)) {
					snprintf(backup, d_string_buffer_size, "\n%s%sPARAMETERS:%s", v_console_styles[e_console_style_bold],
							v_console_styles[e_console_style_yellow], v_console_styles[e_console_style_reset]);
					strncat(buffer, backup, (d_console_output_size-f_string_strlen(buffer))-1);
					for (index = 0; console->commands[last_match].parameters[index].initialized; index++) {
						snprintf(backup, d_string_buffer_size, "\n\t[%s%s%s]%s %s", v_console_styles[e_console_style_bold],
								console->commands[last_match].parameters[index].parameter,
								v_console_styles[e_console_style_reset],
								(console->commands[last_match].parameters[index].optional)?"":"[*]",
								console->commands[last_match].parameters[index].description);
						strncat(buffer, backup, (d_console_output_size-f_string_strlen(buffer))-1);
					}
				}
			}
			write(output, buffer, f_string_strlen(buffer));
			write(output, "\n", sizeof(char));
		} else
			write(output, "\a", sizeof(char));
	}
}

int f_console_read(struct s_console *console, struct s_console_input *input, int output, time_t sec, time_t usec) {
	struct timeval timeout = {sec, usec};
	char incoming_character;
	fd_set descriptor_set;
	if (input->ready)
		memset(input, 0, sizeof(struct s_console_input));
	/* refresh command line */
	f_console_refresh(console, input, output);
	FD_ZERO(&descriptor_set);
	FD_SET(console->descriptor, &descriptor_set);
	if (select(console->descriptor+1, &descriptor_set, NULL, NULL, &timeout) > 0)
		if (((read(console->descriptor, &incoming_character, sizeof(char))) > 0) && (console->input_enabled)) {
			if (input->special_pointer > 0) {
				input->special[input->special_pointer++] = incoming_character;
				if (input->special_pointer >= d_console_special_size) {
					p_console_write_history(console, input, output);
					input->special_pointer = 0; /* reset special pointer */
				}
			} else {
				switch(incoming_character) {
					case '\t':
						p_console_write_suggestion(console, input, output);
						break;
					case 127:
						if (input->data_pointer > 0) {
							if (input->data_length > input->data_pointer)
								memmove((input->input+input->data_pointer-1), (input->input+input->data_pointer),
										(d_string_buffer_size-input->data_pointer));
							else
								memset((input->input+input->data_pointer-1), '\0',
										(d_string_buffer_size-input->data_pointer));
							--(input->data_pointer);
							--(input->data_length);
						}
						break;
					case 27:
						if (input->special_pointer < (d_string_buffer_size-1)) /* leave some space for string tail character */
							input->special[input->special_pointer++] = incoming_character;
						break;
					case 10:
						if (input->data_length > 0) {
							input->ready = d_true;
							if (output != d_console_descriptor_null)
								write(output, (void *)&incoming_character, sizeof(char)); /* new line */
							p_console_append_history(console, input->input);
						}
						break;
					default:
						if (input->data_pointer < d_string_buffer_size) {
							if (input->data_length > input->data_pointer)
								memmove((input->input+input->data_pointer+1), (input->input+input->data_pointer),
										(input->data_length-input->data_pointer));
							input->input[input->data_pointer] = incoming_character;
							++(input->data_pointer);
							++(input->data_length);
						}
				}
			}
		}
	return input->ready;
}

int p_console_execute_verify(struct s_console_command *command, char **tokens, size_t elements, int output) {
	int result = d_true, confirmed, index, argument;
	char buffer[d_string_buffer_size];
	for (index = 0; (command->parameters[index].initialized) && (result); index++)
		if (!command->parameters[index].optional) {
			for (argument = 0, confirmed = d_false; (argument < elements) && (confirmed == d_false); argument++)
				if (f_string_strcmp(tokens[argument], command->parameters[index].parameter) == 0)
					if ((command->parameters[index].is_flag) || (((argument+1) < elements) && (f_string_strlen(tokens[argument+1]) > 0)))
						confirmed = d_true;
			if (!confirmed) {
				result = d_false;
				if (output != d_console_descriptor_null) {
					snprintf(buffer, d_string_buffer_size, "can't find required parameter '%s%s%s'; aborted\n",
							v_console_styles[e_console_style_bold], command->parameters[index].parameter,
							v_console_styles[e_console_style_reset]);
					write(output, buffer, f_string_strlen(buffer));
					fsync(output);
				}
			}
		}
	return result;
}

int p_console_execute(struct s_console *console, char *input, int output) {
	char **tokens, *pointer, *next, buffer[d_string_buffer_size];
	int index = 0, arguments = 1, match = 0;
	size_t length;
	pointer = input;
	while ((next = strchr(pointer, ' '))) {
		if ((next-pointer) > 0)
			arguments++;
		pointer = next+1;
	}
	if ((tokens = (char **) d_malloc(arguments*(sizeof(char *))))) {
		pointer = input;
		while ((next = strchr(pointer, ' '))) {
			if ((length = (next-pointer)) > 0) {
				*next = '\0';
				if ((tokens[index] = (char *) d_malloc(length+1))) {
					memcpy(tokens[index], pointer, length);
					f_string_trim(tokens[index]);
					index++;
				} else
					d_die(d_error_malloc);
			}
			pointer = next+1;
		}
		if ((length = f_string_strlen(pointer)) > 0) {
			if ((tokens[index] = (char *) d_malloc(length+1))) {
				memcpy(tokens[index], pointer, length);
				f_string_trim(tokens[index]);
			} else
				d_die(d_error_malloc);
		}
		if (console->commands)
			for (index = 0; console->commands[index].initialized; index++)
				if (f_string_strcmp(tokens[0], console->commands[index].command) == 0) {
					if (p_console_execute_verify(&(console->commands[index]), tokens, arguments, output))
						if (console->commands[index].call)
							console->commands[index].call(console, &(console->commands[index]), tokens, arguments,
									output);
					match++;
				}
		if ((match == 0) && (output != d_console_descriptor_null)) {
			snprintf(buffer, d_string_buffer_size, "ehm .. what does '%s%s%s' mean?\n", v_console_styles[e_console_style_bold],
					tokens[0], v_console_styles[e_console_style_reset]);
			write(output, buffer, f_string_strlen(buffer));
			fsync(output);
		}
		for (index = 0; index < arguments; index++)
			d_free(tokens[index]);
		d_free(tokens);
	}
	return match;
}

int f_console_execute(struct s_console *console, struct s_console_input *input, int output) {
	int result = 0;
	if ((f_string_strlen(input->input) > 0) && (input->ready))
		result = p_console_execute(console, input->input, output);
	return result;
}

