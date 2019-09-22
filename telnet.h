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
#ifndef miranda_telnet_h
#define miranda_telnet_h
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include "memory.h"
#include "string.local.h"
#define d_telnet_stream_null (-1)
#define d_telnet_port 8090
#define d_telnet_queue 5
#define d_telnet_timeout 100 /* microseconds */
#define d_telnet_clients 256
typedef struct s_telnet_socket {
  int socket;
  t_boolean initialized;
} s_telnet_socket;
typedef struct s_telnet_client {
  struct s_telnet_socket socket;
  char buffer[d_string_buffer_size];
  unsigned int data_pointer;
  size_t buffer_bytes;
} s_telnet_client;
typedef void (*t_telnet_new_action)(struct s_telnet_client *, int);
typedef void (*t_telnet_quit_action)(struct s_telnet_client *, int);
typedef void (*t_telnet_buffer_action)(struct s_telnet_client *, int, const char *, size_t);
typedef struct s_telnet {
  struct s_telnet_socket socket;
  struct s_telnet_client clients[d_telnet_clients];
  struct sockaddr_in address;
} s_telnet;
extern int f_telnet_initialize(struct s_telnet **telnet);
extern void p_telnet_destroy_client(struct s_telnet *telnet, int client);
extern void f_telnet_destroy(struct s_telnet **telnet);
extern void f_telnet_refresh_incoming(struct s_telnet *telnet, t_telnet_new_action new_action);
extern void p_telnet_refresh_readout_client(struct s_telnet *telnet, int client, t_telnet_quit_action quit_action, t_telnet_buffer_action buffer_action);
extern void f_telnet_refresh_readout(struct s_telnet *telnet, t_telnet_quit_action quit_action, t_telnet_buffer_action buffer_action);
extern void f_telnet_write(struct s_telnet *telnet, int client, t_telnet_quit_action quit_action, char *format, ...);
#endif
