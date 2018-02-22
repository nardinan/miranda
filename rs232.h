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
#include <errno.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/time.h>
#include "string.local.h"
#define d_rs232_usecs(tv) (((tv).tv_sec*1000000)+(tv).tv_usec)
#define d_rs232_null -1
#define d_rs232_vtime 1
#define d_rs232_vmin 60
#define d_rs232_byte_timeout 1000
typedef enum e_rs232_baud {
  e_rs232_baud_1200 = B1200,
  e_rs232_baud_1800 = B1800,
  e_rs232_baud_2400 = B2400,
  e_rs232_baud_4800 = B4800,
  e_rs232_baud_9600 = B9600,
  e_rs232_baud_19200 = B19200,
  e_rs232_baud_38400 = B38400,
  e_rs232_baud_57600 = B57600,
  e_rs232_baud_115200 = B115200
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
typedef enum e_rs232_flow_control {
  e_rs232_flow_control_no = 0,
  e_rs232_flow_control_hardware,
  e_rs232_flow_control_software
} e_rs232_flow_control;
extern int f_rs232_open(const char *port, enum e_rs232_baud baud, enum e_rs232_bits bits, enum e_rs232_stops stops, enum e_rs232_parity parity,
                        enum e_rs232_flow_control flow_control, int *device, struct termios *before_tty);
extern void f_rs232_close_termios(int device, struct termios before_tty);
extern void f_rs232_close(int device);
extern int f_rs232_write(int device, const unsigned char *message, size_t size);
extern int f_rs232_read(int device, unsigned char *message, size_t size);
extern int f_rs232_read_packet(int device, unsigned char *message, size_t size, time_t timeout, unsigned char *head, unsigned char *tail, size_t sentry_size);
#endif

