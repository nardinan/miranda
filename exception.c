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
#include "exception.h"
jmp_buf *v_exception_hook = NULL;
struct s_exception v_exception_raised;
void p_exception_fill(struct s_exception exception, const char *message, const char *file, const char *function, unsigned int line) {
	memcpy(&v_exception_raised, &exception, sizeof(struct s_exception));
	strncpy(v_exception_raised.description, message, (d_exception_description_size-1));
	strncpy(v_exception_raised.file, file, (d_exception_file_size-1));
	strncpy(v_exception_raised.function, function, (d_exception_function_size-1));
	v_exception_raised.line = line;
	v_exception_raised.level = 0;
}

