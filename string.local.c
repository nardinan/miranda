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
#include "string.local.h"
#include "memory.h"
size_t f_string_strlen(const char *string) {
	size_t result = 0;
	if (string)
		result = strlen(string);
	return result;
}

int f_string_strncmp(const char *left, const char *right, size_t size) {
	int result;
	if ((left) && (right))
		result = strncmp(left, right, size);
	else
		result = left-right;
	return result;
}

int f_string_strcmp(const char *left, const char *right) {
	size_t right_size = f_string_strlen(left), left_size = f_string_strlen(right);
	int result;
	if (left_size == right_size)
		result = f_string_strncmp(left, right, left_size);
	else
		result = left_size-right_size;
	return result;
}

char *f_string_append(char **string, char *postfix, size_t *space) {
	size_t size_string = f_string_strlen(*string), size_element = f_string_strlen(postfix), total;
	if ((total = size_string+size_element+1) >= *space) {
		if ((*string = (char *) d_realloc(*string, total)))
			*space = total;
		else
			d_die(d_error_malloc);
	}
	memcpy((*string)+size_string, postfix, size_element+1);
	return *string;
}

int f_string_key(char *string, struct s_string_key_format *format, size_t size, char divisor) {
	char *key_pointer = string, *value_pointer, *last_pointer;
	int index, result = 0;
	while ((key_pointer) && (value_pointer = strchr(key_pointer, divisor))) {
		*value_pointer = '\0';
		value_pointer++;
		if (f_string_strlen(value_pointer) > 0) {
			if ((last_pointer = strchr(value_pointer, '\n'))) {
				*last_pointer = '\0';
				last_pointer++;
			}
			f_string_trim(key_pointer);
			f_string_trim(value_pointer);
			for (index = 0; index < size; ++index)
				if ((!format[index].assigned) && (f_string_strcmp(format[index].key, key_pointer) == 0)) {
					switch (format[index].kind) {
						case e_string_key_kind_string:
							strncpy((char *)format[index].pointer.destination_ptr, value_pointer, format[index].destination_size);
							break;
						case e_string_key_kind_float:
							*((float *)format[index].pointer.destination_ptr) = atof(value_pointer);
							break;
						case e_string_key_kind_int:
							*((int *)format[index].pointer.destination_ptr) = atoi(value_pointer);
							break;
						default:
							*(format[index].pointer.destination_double_ptr) = value_pointer;

					}
					format[index].assigned = d_true;
					result++;
				}
		}
		key_pointer = last_pointer;
	}
	return result;
}

char *f_string_trim(char *string) {
	size_t length = f_string_strlen(string);
	char *begin = string, *final = (string+length)-1;
	while ((d_space_character(*begin) || (d_space_character(*final)) || (d_final_character(*final))) && (final >= begin)) {
		if (d_space_character(*begin))
			begin++;
		if ((d_space_character(*final)) || (d_final_character(*final))) {
			*final = '\0';
			final--;
		}
	}
	if (begin > string)
		memmove(string, begin, f_string_strlen(begin)+1);
	return string;
}

char *f_string_format(char *buffer, size_t *computed_size, size_t size, char *symbols, t_string_formatter functions[], char *format, ...) {
	va_list parameters;
	va_start(parameters, format);
	f_string_format_args(buffer, computed_size, size, symbols, functions, format, parameters);
	va_end(parameters);
	return buffer;
}

char *f_string_format_args(char *buffer, size_t *computed_size, size_t size, char *symbols, t_string_formatter functions[], char *format, va_list parameters) {
	char *target = buffer, *pointer = format, *next, *last, *tail, argument[d_string_argument_size];
	size_t dimension, remaining = size-1, lower, written;
	*computed_size = 0;
	while ((next = strchr(pointer, '%'))) {
		if ((dimension = (next-pointer)) > 0) {
			*computed_size += dimension;
			if ((lower = (dimension>remaining)?remaining:dimension)) {
				remaining -= lower;
				memcpy(target, pointer, lower);
				target += lower;
			}
		}
		if ((pointer = (next+1)))
			if ((last = p_string_format_skip(pointer, symbols))) {
				memset(argument, '\0', d_string_argument_size);
				memcpy(argument, next, (last-next));
				if ((!symbols) || (!(tail = strchr(symbols, *(last-1))))) {
					switch(tolower(*(last-1))) {
						case 'd':
						case 'i':
						case 'u':
						case 'x':
						case 'o':
						case 'n':
						case 'c':
							written = snprintf(target, remaining+1, argument, va_arg(parameters, long));
							break;
						case 'f':
						case 'e':
						case 'g':
							written = snprintf(target, remaining+1, argument, va_arg(parameters, double));
							break;
						case 's':
						case 'p':
							written = snprintf(target, remaining+1, argument, va_arg(parameters, void *));
							break;
					}
					*computed_size += written;
					written = ((written>remaining)?remaining:written);
					remaining -= written;
					target += written;
				} else {
					next = functions[(tail-symbols)](target, &written, remaining, argument, parameters);
					*computed_size += written;
					remaining -= (next-target);
					target = next;
				}
				pointer = last;
			}
	}
	if ((dimension = strlen(pointer)) > 0) {
		*computed_size += dimension;
		if ((lower = (dimension>remaining)?remaining:dimension)) {
			remaining -= lower;
			memcpy(target, pointer, lower);
			target += lower;
		}
	}
	*target = '\0';
	return buffer;
}

char *p_string_format_skip(char *buffer, char *symbols) {
	int error = d_false;
	while (strchr("#0-+ '", *buffer))
		buffer++;
	while (isdigit(*buffer))
		buffer++;
	if (*buffer == '.') {
		buffer++;
		if (isdigit(*buffer))
			while (isdigit(*buffer))
				buffer++;
		else
			error = d_true;
	}
	if (!error) {
		while (strchr("hljztLq", *buffer))
			buffer++;
		if ((strchr("diouXxfFeEgGaAcsb%", *buffer)) || (strchr(symbols, *buffer)))
			buffer++;
		else
			error = d_true;
	}
	return (error)?NULL:buffer;
}

