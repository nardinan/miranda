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
#ifndef miranda_network_h
#define miranda_network_h
#ifdef _WIN32_WINNT
#undef _WIN32_WINNT
#endif
#define _WIN32_WINNT 0x0501
#include <unistd.h>
#include <stdarg.h>
#include <limits.h>
#include <pthread.h>
#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#include <io.h>
#ifndef SHUT_RDWR
#define SHUT_RDWR 2
#endif
#ifndef EAGAIN
#define EAGAIN WSAEWOULDBLOCK
#endif
#define d_socket_close(s) closesocket(s)
#define d_socket_recv(s,ptr,siz) recv((s),(char *)(ptr),(siz),0)
#define d_socket_send(s,ptr,siz) send((s),(char *)(ptr),(siz),0)
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/fcntl.h>
#include <netdb.h>
#include <errno.h>
#define d_socket_close(s) close(s)
#define d_socket_recv(s,ptr,siz) read((s),(void *)(ptr),(siz))
#define d_socket_send(s,ptr,siz) write((s),(void *)(ptr),(siz))
#endif
#include "string.local.h"
#include "endian.local.h"
#include "memory.h"
#define d_network_default_endian d_big_endian
#define d_network_null_socket -1
#define d_network_intro_size sizeof(int32_t)
extern int f_network_serverize(const char *address, const char *port, unsigned int queue);
extern int f_network_search(int descriptor);
extern int f_network_connect(const char *address, const char *port);
extern int f_network_recv(int descriptor, void **buffer, size_t *readed_size, time_t sec, time_t usec);
extern ssize_t f_network_send(int descriptor, void *buffer, size_t size);
#endif

