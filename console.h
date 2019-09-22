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
#ifndef miranda_console_h
#define miranda_console_h
#include <termios.h>
#include <unistd.h>
#include <limits.h>
#ifdef __APPLE__
#include <sys/syslimits.h>
#endif
#include "string.local.h"
#include "memory.h"
#define d_console_parameter_size 8
#define d_console_command_size 32
#define d_console_command_parameters 10
#define d_console_output_size PATH_MAX
#define d_console_history_size 256
#define d_console_special_size 3
#define d_console_descriptor_null (-1)
#define d_console_parameter_null (-1)
#define d_console_suggestion_columns 10
struct s_console;
struct s_console_command;
typedef enum e_console_style {
  e_console_style_reset = 0, /* styles */
  e_console_style_bold,
  e_console_style_italic,
  e_console_style_underline,
  e_console_style_blink,
  e_console_style_reverse, /* colors */
  e_console_style_red,
  e_console_style_green,
  e_console_style_yellow,
  e_console_style_blue,
  e_console_style_gray,
  e_console_style_white, /* utils */
  e_console_style_clean_line,
  e_console_style_clean_screen
} e_console_style;
typedef enum e_console_level {
  e_console_level_guest = 0,
  e_console_level_user,
  e_console_level_admin
} e_console_level;
extern const char *v_console_styles[];
typedef int (*t_console_recall)(struct s_console *, struct s_console_command *, char **, size_t, int);
typedef struct s_console_parameter {
  char parameter[d_console_parameter_size], description[d_string_buffer_size];
  t_boolean is_flag, optional, initialized;
} s_console_parameter;
typedef struct s_console_command {
  char command[d_console_command_size], description[d_string_buffer_size];
  struct s_console_parameter *parameters;
  t_console_recall call;
  enum e_console_level level;
  t_boolean initialized;
} s_console_command;
typedef struct s_console_input {
  char input[d_string_buffer_size], special[d_console_parameter_size];
  int data_pointer, special_pointer;
  t_boolean ready;
  size_t data_length;
} s_console_input;
typedef struct s_console {
  enum e_console_level level;
  struct s_console_command *commands;
  struct termios old_configuration, current_configuration;
  char history[d_console_history_size][d_string_buffer_size], prefix[d_string_buffer_size];
  int history_last, history_pointer, descriptor, input_enabled;
} s_console;
extern int f_console_parameter(const char *symbol, char **tokens, size_t elements, int is_flag);
extern int f_console_init(struct s_console **console, struct s_console_command *commands, int descriptor);
extern int f_console_destroy(struct s_console **console);
extern void f_console_refresh(struct s_console *console, struct s_console_input *input, int output);
extern void f_console_write(struct s_console *console, const char *buffer, int output);
extern void p_console_append_history(struct s_console *console, const char *buffer);
extern void p_console_write_history(struct s_console *console, struct s_console_input *input, int output);
extern void p_console_write_suggestion(struct s_console *console, struct s_console_input *input, int output);
extern int f_console_read(struct s_console *console, struct s_console_input *input, int output, time_t sec, time_t usec);
extern int p_console_execute_verify(struct s_console_command *command, char **tokens, size_t elements, int output);
extern int p_console_execute(struct s_console *console, char *input, int output);
extern int f_console_execute(struct s_console *console, struct s_console_input *input, int output);
#endif
