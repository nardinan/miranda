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
#include "log.h"
enum e_log_level v_log_level = d_log_level_default;
const char v_log_level_description[][d_log_description_size] = {
	"log",
	"low",
	"medium",
	"high"
};
void p_log_write(FILE *stream, enum e_log_level level, const char *file, const char *function, unsigned int line, const char *format, ...) {
	va_list arguments;
	if (level >= v_log_level) {
		fprintf(stream, "%s - [%s::%s() (%d)] ", v_log_level_description[level], file, function, line);
		va_start(arguments, format);
		vfprintf(stream, format, arguments);
		va_end(arguments);
		fputc(d_log_new_line, stream);
		fflush(stream);
	}
}

