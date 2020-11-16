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
#include "telnet.h"
int f_telnet_initialize(struct s_telnet **telnet) {
  int flag = 1;
  if ((*telnet = (struct s_telnet *)d_malloc(sizeof(struct s_telnet)))) {
    memset(&((*telnet)->address), 0, sizeof(struct sockaddr_in));
    if (((*telnet)->socket.socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) != d_telnet_stream_null)
      if (setsockopt((*telnet)->socket.socket, SOL_SOCKET, SO_REUSEADDR, (char *)&flag, sizeof(int)) != d_telnet_stream_null) {
        (*telnet)->address.sin_family = AF_INET;
        (*telnet)->address.sin_port = htons(d_telnet_port);
        (*telnet)->address.sin_addr.s_addr = htonl(INADDR_ANY);
        if (bind((*telnet)->socket.socket, (struct sockaddr *)&((*telnet)->address), sizeof(struct sockaddr_in)) == 0)
          if (listen((*telnet)->socket.socket, d_telnet_queue) == 0)
            (*telnet)->socket.initialized = d_true;
      }
  } else
    d_die(d_error_malloc);
  return (*telnet)->socket.initialized;
}
void p_telnet_destroy_client(struct s_telnet *telnet, int client) {
  if (telnet->clients[client].socket.initialized) {
    shutdown(telnet->clients[client].socket.socket, SHUT_RDWR);
    close(telnet->clients[client].socket.socket);
    telnet->clients[client].socket.initialized = d_false;
    telnet->clients[client].data_pointer = 0;
    memset(telnet->clients[client].buffer, 0, d_string_buffer_size);
  }
}
void f_telnet_destroy(struct s_telnet **telnet) {
  int index;
  if (*telnet) {
    for (index = 0; index < d_telnet_clients; ++index)
      p_telnet_destroy_client((*telnet), index);
    if ((*telnet)->socket.initialized) {
      shutdown((*telnet)->socket.socket, SHUT_RDWR);
      close((*telnet)->socket.socket);
    }
    d_free(*telnet);
  }
  *telnet = NULL;
}
void f_telnet_refresh_incoming(struct s_telnet *telnet, t_telnet_new_action new_action) {
  fd_set sockets;
  struct timeval timeout = {
    0,
    d_telnet_timeout
  };
  int client, index, flags;
  if (telnet->socket.initialized) {
    FD_ZERO(&sockets);
    FD_SET(telnet->socket.socket, &sockets);
    if (select(telnet->socket.socket + 1, &sockets, NULL, NULL, &timeout) > 0)
      if ((client = accept(telnet->socket.socket, NULL, NULL)) != d_telnet_stream_null)
        if ((flags = fcntl(client, F_GETFL)) != d_telnet_stream_null) {
          fcntl(client, F_SETFL, flags | O_NONBLOCK);
          for (index = 0; index < d_telnet_clients; index++)
            if (!telnet->clients[index].socket.initialized) {
              telnet->clients[index].socket.initialized = d_true;
              telnet->clients[index].socket.socket = client;
              telnet->clients[index].buffer_bytes = 0;
              if (new_action)
                new_action(&(telnet->clients[index]), client);
              break;
            }
          if (index == d_telnet_clients) {
            shutdown(client, SHUT_RDWR);
            close(client);
          }
        }
  }
}
void p_telnet_refresh_readout_client(struct s_telnet *telnet, int client, t_telnet_quit_action quit_action, t_telnet_buffer_action buffer_action) {
  fd_set sockets;
  struct timeval timeout = {
    0,
    d_telnet_timeout
  };
  char buffer[d_string_buffer_size], output_buffer[d_string_buffer_size];
  size_t buffer_bytes = 0, read_bytes, message_bytes = 0, destination_bytes = 0;
  memset(buffer, 0, d_string_buffer_size);
  if (telnet->clients[client].socket.initialized) {
    if (telnet->clients[client].buffer_bytes > 0) {
      memcpy(buffer, telnet->clients[client].buffer, telnet->clients[client].buffer_bytes);
      buffer_bytes += telnet->clients[client].buffer_bytes;
      telnet->clients[client].buffer_bytes = 0;
    }
    FD_ZERO(&sockets);
    FD_SET(telnet->clients[client].socket.socket, &sockets);
    if (buffer_bytes < d_string_buffer_size)
      if (select(telnet->clients[client].socket.socket + 1, &sockets, NULL, NULL, &timeout) > 0) {
        if ((read_bytes = read(telnet->clients[client].socket.socket, buffer + buffer_bytes, d_string_buffer_size - buffer_bytes)) <= 0) {
          if (quit_action)
            quit_action(&(telnet->clients[client]), client);
          p_telnet_destroy_client(telnet, client);
        } else
          buffer_bytes += read_bytes;
      }
    if (buffer_bytes > 0)
      if (telnet->clients[client].socket.initialized) {
        while ((message_bytes < buffer_bytes) && (buffer[message_bytes] != '\n')) {
          if (buffer[message_bytes] != '\r') {
            if (buffer[message_bytes] == '\b')
              destination_bytes--;
            if (buffer[message_bytes] != '\n')
              output_buffer[destination_bytes++] = buffer[message_bytes];
          }
          message_bytes++;
        }
        message_bytes++;
        output_buffer[destination_bytes] = '\0';
        if (message_bytes < buffer_bytes) {
          buffer_bytes -= message_bytes;
          memcpy(telnet->clients[client].buffer, buffer + message_bytes, buffer_bytes);
          telnet->clients[client].buffer_bytes = buffer_bytes;
        }
        if (buffer_action)
          buffer_action(&(telnet->clients[client]), client, output_buffer, destination_bytes);
      }
  }
}
void f_telnet_refresh_readout(struct s_telnet *telnet, t_telnet_quit_action quit_action, t_telnet_buffer_action buffer_action) {
  int client;
  for (client = 0; client < d_telnet_clients; ++client)
    p_telnet_refresh_readout_client(telnet, client, quit_action, buffer_action);
}
void p_telnet_write_args(struct s_telnet *telnet, int client, t_telnet_quit_action quit_action, char *format, va_list args) {
  va_list backup_args;
  char buffer[d_string_buffer_size], *message;
  size_t length, current_length;
  va_copy(backup_args, args);
  f_string_format_args(buffer, &length, d_string_buffer_size, NULL, NULL, format, backup_args);
  if (length > 0) {
    current_length = length;
    if ((message = (char *)d_malloc(current_length + 1))) {
      f_string_format_args(message, &length, current_length, NULL, NULL, format, args);
      if (write(telnet->clients[client].socket.socket, message, current_length) < 0) {
        if (quit_action)
          quit_action(&(telnet->clients[client]), client);
        p_telnet_destroy_client(telnet, client);
      }
      d_free(message);
    }
  }
  va_end(backup_args);
}
void f_telnet_write(struct s_telnet *telnet, int client, t_telnet_quit_action quit_action, char *format, ...) {
  va_list args;
  if (telnet->clients[client].socket.initialized) {
    va_start(args, format);
    p_telnet_write_args(telnet, client, quit_action, format, args);
    va_end(args);
  }
}

