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
#ifndef miranda_log_h
#define miranda_log_h
#include <time.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#define d_log_time_size 64
#define d_log_description_size 32
#define d_log_new_line '\n'
#define d_log_level_default e_log_level_medium
#define d_log(l,c...) p_log_write(stdout,l,"\x1B[32mlog\x1B[0m",__FILE__,__FUNCTION__,__LINE__,##c)
#define d_war(l,c...) p_log_write(stdout,l,"\x1B[33mwar\x1B[0m",__FILE__,__FUNCTION__,__LINE__,##c)
#define d_err(l,c...) p_log_write(stderr,l,"\x1B[31merr\x1B[0m",__FILE__,__FUNCTION__,__LINE__,##c)
#define d_die(f...)\
	do{\
		d_err(e_log_level_ever,##f);\
		exit(1);\
	}while(0);
#define d_assert(ass)\
	do{\
		if(!(ass))\
			d_die("[assertion] - failing testing following condition: %s",#ass);\
	}while(0)
typedef enum e_log_level {
	e_log_level_ever = 0,
	e_log_level_low,
	e_log_level_medium,
	e_log_level_high
} e_log_level;
extern enum e_log_level v_log_level;
extern const char v_log_level_description[][d_log_description_size];
extern void p_log_write(FILE *stream, enum e_log_level level, const char *prefix, const char *file, const char *function, unsigned int line, 
	const char *format, ...);
#endif
