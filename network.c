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
#include "network.h"
int f_network_serverize(const char *address, const char *port, unsigned int queue) {
	struct addrinfo hints = {AI_PASSIVE, AF_INET, SOCK_STREAM}, *response = NULL;
	int result = d_network_null_socket, value = 1;
#ifdef _WIN32
	WSADATA data_socket;
	WSAStartup(MAKEWORD(1,1), &data_socket);
	WSACleanup();
#endif
	if (getaddrinfo(address, port, &hints, &response) == 0) {
		if ((result = socket(response->ai_family, response->ai_socktype, response->ai_protocol)) != d_network_null_socket) {
			if (setsockopt(result, SOL_SOCKET, SO_REUSEADDR, (char *)&value, sizeof(int)) != -1) {
#ifdef SO_NOSIGPIPE
				setsockopt(result, SOL_SOCKET, SO_NOSIGPIPE, (char *)&value, sizeof(int));
#endif
				if (bind(result, response->ai_addr, response->ai_addrlen) == 0) {
					if (listen(result, queue) != 0) {
						freeaddrinfo(response);
						result = d_network_null_socket;
					}
				} else if (result != d_network_null_socket) {
					d_socket_close(result);
					result = d_network_null_socket;
				}
			} else if (result != d_network_null_socket) {
				d_socket_close(result);
				result = d_network_null_socket;
			}
		}
		freeaddrinfo(response);
	}
	return result;
}

int f_network_search(int descriptor) {
	fd_set rdset;
	int flags, result = d_network_null_socket;
	unsigned long mode = 1;
	FD_ZERO(&rdset);
	FD_SET(descriptor,&rdset);
	if ((result = accept(descriptor, NULL, NULL)) != d_network_null_socket) {
#ifndef _WIN32                                                                                                                                                  
		mode = 0;
		if ((flags = fcntl(result, F_GETFL)) != -1)                                                                                     
			fcntl(result, F_SETFL, flags|O_NONBLOCK);                                                                                                   
#else                                                                                                                                                           
		ioctlsocket(result, FIONBIO, &mode);                                                                                                            
#endif
	}
	return result;
}

int f_network_connect(const char *address, const char *port) {
	struct addrinfo hints = {AI_PASSIVE, AF_INET, SOCK_STREAM}, *response = NULL;
	int flags, result = d_network_null_socket, value = 1;
	unsigned long mode = 1;
#ifdef _WIN32
	WSADATA data_socket;
	WSAStartup(MAKEWORD(1,1), &data_socket);
	WSACleanup();
#endif
	if (getaddrinfo(address, port, &hints, &response) == 0) {
		if ((result = socket(response->ai_family, response->ai_socktype, response->ai_protocol)) != d_network_null_socket) {
			if (connect(result, response->ai_addr, response->ai_addrlen) == 0) {
#ifndef _WIN32
				mode = 0; /* warning avoid */
				if ((flags = fcntl(result, F_GETFL)) != -1)
					fcntl(result, F_SETFL, flags|O_NONBLOCK);
#else
				ioctlsocket(result, FIONBIO, &mode);
#endif
#ifdef SO_NOSIGPIPE
				setsockopt(result, SOL_SOCKET, SO_NOSIGPIPE, (char *)&value, sizeof(int));
#endif
			} else {
				d_socket_close(result);
				result = d_network_null_socket;
			}
		}
		freeaddrinfo(response);
	}
	return result;
}

int f_network_recv(int descriptor, void **buffer, size_t *readed_size, time_t sec, time_t usec) {
	struct timeval timeout = {sec, usec};
	int descriptors, result = d_true, readed = EAGAIN;
	char *pointer, *backup;
	int32_t size;
	fd_set rdset;
	if (descriptor != d_network_null_socket) {
		FD_ZERO(&rdset);
		FD_SET(descriptor, &rdset);
		if ((descriptors = select(descriptor+1, &rdset, NULL, NULL, &timeout)) > 0) {
			if (d_socket_recv(descriptor, &size, d_network_intro_size) == d_network_intro_size) {
				if (f_endian_check() != d_network_default_endian)
					d_swap(size);
				*readed_size = size;
				if ((pointer = (void *) d_malloc(size+1))) {
					backup = pointer;
					while ((size > 0) && (readed == EAGAIN)) {
						timeout.tv_sec = sec;
						timeout.tv_usec = usec;
						if ((descriptors = select(descriptor+1, &rdset, NULL, NULL, &timeout)) > 0) {
							while ((readed = d_socket_recv(descriptor, backup, size)) > 0) {
								size -= readed;
								backup += readed;
							}
							if ((readed < 0) && (errno == EAGAIN))
								readed = EAGAIN;
						} else {
							if (descriptors < 0)
								result = d_false;
							break;
						}
					}
					*buffer = pointer;
					if (size)
						result = d_false;
				}
			} else
				result = d_false;
		} else if (descriptors < 0)
			result = d_false;
	} else
		result = d_false;
	return result;
}

ssize_t f_network_send(int descriptor, void *buffer, size_t size) {
	size_t local_size = size;
	ssize_t readed = EAGAIN, result = 0;
	int32_t real_size = size;
	if (descriptor != d_network_null_socket) {
		if (f_endian_check() != d_network_default_endian)
			d_swap(real_size);
		if ((result = d_socket_send(descriptor, &real_size, d_network_intro_size)) == d_network_intro_size)
			readed = d_socket_send(descriptor, buffer, local_size);
	}
	return readed;
}

