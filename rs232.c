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
#include "rs232.h"
int f_rs232_open(const char *port, enum e_rs232_baud baud, enum e_rs232_bits bits, enum e_rs232_stops stops, enum e_rs232_parity parity, int *device,
		struct termios *before_tty) {
	struct termios tty;
	int result = d_false;
	if ((*device = open(port, O_RDWR|O_NOCTTY|O_NONBLOCK)) >= 0) {
		memset(&tty, 0, sizeof(struct termios));
		if (tcgetattr(*device, &tty) == 0) {
			if (before_tty)
				memcpy(before_tty, &tty, sizeof(struct termios));
			cfsetospeed(&tty, (speed_t)baud);
			cfsetispeed(&tty, (speed_t)baud);
			tty.c_cflag &= ~CSIZE;
			tty.c_cflag |= bits;
			switch (stops) {
				case e_rs232_stops_1_bit:
					tty.c_cflag &= ~CSTOPB;
					break;
				case e_rs232_stops_2_bit:
					tty.c_cflag |= CSTOPB;
			}
			switch (parity) {
				case e_rs232_parity_no:
					tty.c_cflag &= ~PARENB;
					break;
				case e_rs232_parity_even:
					tty.c_cflag |= PARENB;
					tty.c_cflag &= ~PARODD;
					break;
				case e_rs232_parity_odd:
					tty.c_cflag |= (PARENB|PARODD);
			}
			tty.c_cflag |= (CREAD|CLOCAL);
			cfmakeraw(&tty);
			tcflush(*device, TCIFLUSH);
			if (!(result = (tcsetattr(*device, TCSAFLUSH, &tty) == 0)))
				close(*device);
		} else
			close(*device);
	}
	return result;
}

void f_rs232_close_termios(int device, struct termios before_tty) {
	if (tcsetattr(device, TCSAFLUSH, &before_tty) == 0)
		close(device);
}

void f_rs232_close(int device) {
	close(device);
}

int f_rs232_write(int device, const unsigned char *message, size_t size) {
	return (write(device, message, size)<0);
}

int f_rs232_read(int device, unsigned char *message, size_t size, unsigned char final_character, time_t sec, time_t usec) {
	unsigned char byte;
	struct timeval timeout = {sec, usec};
	int result = d_rs232_null, index = 0;
	fd_set descriptors;
	FD_ZERO(&descriptors);
	FD_SET(device, &descriptors);
	while ((index < (size-1)) && (result = select(device+1, &descriptors, NULL, NULL, &timeout)) > 0) {
		if ((result = read(device, &byte, 1)) > 0) {
			if (byte != final_character)
				message[index++] = byte;
			else
				break;
		}
		timeout.tv_sec = sec;
		timeout.tv_usec = usec;
	}
	message[index] = '\0';
	return result;
}

