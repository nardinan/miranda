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
#ifndef miranda_string_h
#define miranda_string_h
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "types.h"
#include "logs.h"
#define d_string_argument_size 24
#define d_string_buffer_size 256
#define d_space_character(a) (((a)==' ')||((a)=='\t'))
#define d_final_character(a) (((a)=='\0')||((a)=='\n')||((a)=='\r'))
typedef char *(* t_string_formatter)(char *, size_t *, size_t, char *, va_list);
typedef enum e_string_key_kinds {
	e_string_key_kind_pointer = 0,		/* double ptr */
	e_string_key_kind_string,		/* single ptr */
	e_string_key_kind_float,		/* single ptr */
	e_string_key_kind_int			/* single ptr */
} e_string_key_kinds;
typedef struct s_string_key_format {
	enum e_string_key_kinds kind;
	const char *key;
	union {
		void *destination_ptr;
		void **destination_double_ptr;
	} pointer;
	size_t destination_size;
	int fall_after, assigned;
} s_string_key_format;
extern size_t f_string_strlen(const char *string);
extern int f_string_strncmp(const char *left, const char *right, size_t size);
extern int f_string_strcmp(const char *left, const char *right);
extern char *f_string_append(char **string, char *postfix, size_t *space);
extern char *f_string_trim(char *string);
extern int f_string_key(char *string, struct s_string_key_format *format, size_t size, char divisor);
extern char *f_string_format(char *buffer, size_t *computed_size, size_t size, char *symbols, t_string_formatter functions[], char *format, ...);
extern char *f_string_format_args(char *buffer, size_t *computed_size, size_t size, char *symbols, t_string_formatter functions[], char *format,
		va_list parameters);
extern char *p_string_format_skip(char *buffer, char *symbols);
#endif
