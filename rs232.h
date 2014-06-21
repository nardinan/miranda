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
#ifndef miranda_rs232_h
#define miranda_rs232_h
#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/time.h>
#include "string.local.h"
#define d_rs232_usecs(tv) (((tv).tv_sec*1000000)+(tv).tv_usec)
#define d_rs232_null -1
#define d_rs232_vtime 1
#define d_rs232_vmin 6
typedef enum e_rs232_baud {
	e_rs232_baud_1200 = 1200,
	e_rs232_baud_1800 = 1800,
	e_rs232_baud_2400 = 2400,
	e_rs232_baud_3600 = 3600,
	e_rs232_baud_4800 = 4800,
	e_rs232_baud_7200 = 7200,
	e_rs232_baud_9600 = 9600,
	e_rs232_baud_14400 = 14400,
	e_rs232_baud_19200 = 19200,
	e_rs232_baud_28800 = 28800,
	e_rs232_baud_38400 = 38400,
	e_rs232_baud_56000 = 56000,
	e_rs232_baud_57600 = 57600,
	e_rs232_baud_115200 = 115200
} e_rs232_baud;
typedef enum e_rs232_parity {
	e_rs232_parity_no,
	e_rs232_parity_odd,
	e_rs232_parity_even
} e_rs232_parity;
typedef enum e_rs232_stops {
	e_rs232_stops_1_bit,
	e_rs232_stops_2_bit
} e_rs232_stops;
typedef enum e_rs232_bits {
	e_rs232_bits_5 = CS5,
	e_rs232_bits_6 = CS6,
	e_rs232_bits_7 = CS7,
	e_rs232_bits_8 = CS8
} e_rs232_bits;
extern int f_rs232_open(const char *port, enum e_rs232_baud baud, enum e_rs232_bits bits, enum e_rs232_stops stops, enum e_rs232_parity parity,
		int flow_control, int *device, struct termios *before_tty);
extern void f_rs232_close_termios(int device, struct termios before_tty);
extern void f_rs232_close(int device);
extern int f_rs232_write(int device, const unsigned char *message, size_t size);
extern int f_rs232_read(int device, unsigned char *message, size_t size);
#endif

