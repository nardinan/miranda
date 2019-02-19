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
int f_rs232_open(const char *port, enum e_rs232_baud baud, enum e_rs232_bits bits, enum e_rs232_stops stops, enum e_rs232_parity parity,
  enum e_rs232_flow_control flow_control, int *device, struct termios *before_tty) {
  struct termios tty;
  int result = d_false;
  if ((*device = open(port, O_RDWR | O_NOCTTY | O_NDELAY)) >= 0) {
    tcflush((*device), TCIOFLUSH);
    memset(&tty, 0, sizeof(struct termios));
    if (tcgetattr(*device, &tty) == 0) {
      if (before_tty)
        memcpy(before_tty, &tty, sizeof(struct termios));
      cfsetospeed(&tty, (speed_t)baud);
      cfsetispeed(&tty, (speed_t)baud);
      tty.c_cflag &= ~CSIZE;
      tty.c_cflag |= (bits | CLOCAL | CREAD);
      switch (parity) {
        case e_rs232_parity_no:
          tty.c_cflag &= ~(PARENB | PARODD);
          break;
        case e_rs232_parity_even:
          tty.c_cflag |= PARENB;
          tty.c_cflag &= ~PARODD;
          break;
        case e_rs232_parity_odd:
          tty.c_cflag |= (PARENB | PARODD);
      }
      switch (stops) {
        case e_rs232_stops_1_bit:
          tty.c_cflag &= ~CSTOPB;
          break;
        case e_rs232_stops_2_bit:
          tty.c_cflag |= CSTOPB;
      }
      tty.c_iflag = IGNBRK;
      switch (flow_control) {
        case e_rs232_flow_control_hardware:
          tty.c_cflag |= CRTSCTS;
          tty.c_iflag &= ~(IXOFF | IXON | IXANY);
          break;
        case e_rs232_flow_control_software:
          tty.c_cflag &= ~CRTSCTS;
          tty.c_iflag |= (IXOFF | IXON | IXANY);
          break;
        case e_rs232_flow_control_no:
          tty.c_cflag &= ~CRTSCTS;
          tty.c_iflag &= ~(IXON | IXOFF | IXANY);
      }
      tty.c_lflag = 0;
      tty.c_oflag = 0;
      tty.c_cc[VTIME] = d_rs232_vtime;
      tty.c_cc[VMIN] = d_rs232_vmin;
      result = (tcsetattr(*device, TCSANOW, &tty) == 0);
    }
    if (!result)
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
  return write(device, message, size);
}
int f_rs232_read(int device, unsigned char *message, size_t size) {
  return read(device, message, size);
}
int f_rs232_read_packet(int device, unsigned char *message, size_t size, time_t timeout, unsigned char *head, unsigned char *tail, size_t sentry_size) {
  struct timeval timestamp;
  time_t begin, current;
  int readed = 0, head_checked = d_false, tail_checked = d_false, result;
  gettimeofday(&timestamp, NULL);
  begin = current = d_rs232_usecs(timestamp);
  while (((current - begin) < timeout) && (readed < size) && (!tail_checked)) {
    if ((result = f_rs232_read(device, (message + readed), (size - readed))) > 0) {
      readed += result;
      if (readed >= sentry_size) {
        if ((!head_checked) && (head))
          if (!(head_checked = (memcmp(message, head, sentry_size) == 0)))
            readed = 0;
        if (tail)
          tail_checked = (memcmp((message + (readed - sentry_size)), tail, sentry_size) == 0);
      }
      begin = current; /* reset timeout */
    } else if ((result == 0) || (errno != EAGAIN)) {
      readed = -1;
      break;
    }
    gettimeofday(&timestamp, NULL);
    current = d_rs232_usecs(timestamp);
  }
  return readed;
}

