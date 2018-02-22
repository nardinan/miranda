/*
 * miranda
 * Copyright (C) 2015 Andrea Nardinocchi (andrea@nardinan.it)
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
#ifndef miranda_huffman_h
#define miranda_huffman_h
#include "types.h"
#include "memory.h"
#include "endian.local.h"
#define d_huffman_elements 256
#define d_huffman_encode_bytes 4
typedef struct s_huffman_node {
  size_t elements, height;
  t_boolean leaf;
  struct s_huffman_node *father;
  union {
    struct s_huffman_node *childs[2];
    unsigned char code;
  };
} s_huffman_node;
typedef struct s_huffman_code {
  unsigned char code, bytes_encode[d_huffman_encode_bytes], bits_number;
} s_huffman_code;
extern void p_huffman_destroy(struct s_huffman_node *node);
extern void p_huffman_compression_append(struct s_huffman_code code, unsigned char *output, size_t *current_byte, size_t *current_bit);
extern int p_huffman_compression_code(struct s_huffman_node *node, struct s_huffman_code *code, unsigned char byte[], unsigned char bytes_number,
                                      unsigned char bits_number);
extern struct s_huffman_node *p_huffman_compression_aggregate(struct s_huffman_node **nodes, int elements);
extern int f_huffman_compression(unsigned char *block, size_t size, unsigned char **output, size_t *output_string);
extern void p_huffman_decompression_expand_append(struct s_huffman_node *root, struct s_huffman_code code);
extern struct s_huffman_node *p_huffman_decompression_expand(unsigned char *block, int elements);
extern int p_huffman_decompression_append(struct s_huffman_node *root, unsigned char *output, size_t *current_output_byte, unsigned char *block, size_t size,
                                          size_t *current_block_byte, size_t *current_block_bit);
extern int f_huffman_decompression(unsigned char *block, size_t size, unsigned char **output, size_t *output_size);
#endif

