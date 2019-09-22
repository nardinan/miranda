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
#include "huffman.h"
static int p_huffman_compare(const void *value, const void *reference) {
  const struct s_huffman_node *values[2] = {
    *(const struct s_huffman_node **)value,
    *(const struct s_huffman_node **)reference
  };
  int result;
  if ((values[0]) && (values[1])) {
    if (values[0]->elements > values[1]->elements)
      result = 1;
    else if (values[0]->elements < values[1]->elements)
      result = -1;
    else
      result = 0;
  } else if (values[1])
    result = 1;
  else if (values[0])
    result = -1;
  else
    result = 0;
  return result;
}
void p_huffman_destroy(struct s_huffman_node *node) {
  if (node) {
    if (!node->leaf) {
      p_huffman_destroy(node->childs[0]);
      p_huffman_destroy(node->childs[1]);
    }
    d_free(node);
  }
}
void p_huffman_compression_append(struct s_huffman_code code, unsigned char *output, size_t *current_byte, size_t *current_bit) {
  unsigned char current_local_byte, current_local_bit, bits;
  for (current_local_bit = 8, current_local_byte = ((code.bits_number - 1) / 8), bits = 0; bits < code.bits_number; ++bits) {
    if (current_local_bit == 0) {
      current_local_bit = 8;
      --current_local_byte;
    }
    --current_local_bit;
    if ((++(*current_bit)) > 8) {
      ++(*current_byte);
      *current_bit = 1;
    } else
      output[*current_byte] <<= 1;
    output[*current_byte] |= (code.bytes_encode[current_local_byte] >> current_local_bit) & 0x01;
  }
}
int p_huffman_compression_code(struct s_huffman_node *node, struct s_huffman_code *code, unsigned char *bytes, unsigned char bytes_number,
  unsigned char bits_number) {
  int result = 0;
  unsigned char bytes_right[d_huffman_encode_bytes] = {0}, bytes_left[d_huffman_encode_bytes] = {0};
  if (node) {
    if (node->leaf) {
      code[node->code].code = node->code;
      bytes[0] = (bytes[0] << (8 - bits_number));
      memcpy(code[node->code].bytes_encode, bytes, d_huffman_encode_bytes);
      code[node->code].bits_number = (bits_number + (bytes_number * 8));
      result = 1;
    } else {
      if ((++bits_number) > 8) {
        ++bytes_number;
        bits_number = 1; // first new position is 1
        if (bytes) {
          memcpy((bytes_right + 1), bytes, (d_huffman_encode_bytes - 1));
          memcpy((bytes_left + 1), bytes, (d_huffman_encode_bytes - 1));
        }
      } else if (bytes) {
        memcpy(bytes_right, bytes, d_huffman_encode_bytes);
        memcpy(bytes_left, bytes, d_huffman_encode_bytes);
      }
      bytes_right[0] = ((bytes_right[0] << 1) | 1);
      bytes_left[0] = ((bytes_left[0] << 1) | 0);
      result = (p_huffman_compression_code(node->childs[1], code, bytes_right, bytes_number, bits_number) +
                p_huffman_compression_code(node->childs[0], code, bytes_left, bytes_number, bits_number));
    }
  }
  return result;
}
struct s_huffman_node *p_huffman_compression_aggregate(struct s_huffman_node **nodes, int elements) {
  struct s_huffman_node *current;
  int index, remaining;
  qsort(nodes, d_huffman_elements, sizeof(struct s_huffman_node *), p_huffman_compare);
  for (index = 0; index < (elements - 1); ++index)
    if ((current = (struct s_huffman_node *)d_malloc(sizeof(struct s_huffman_node)))) {
      current->leaf = d_false;
      current->childs[0] = nodes[0];
      current->childs[1] = nodes[1];
      current->elements = (nodes[0]->elements + nodes[1]->elements);
      current->height = ((nodes[0]->height > nodes[1]->height) ? nodes[0]->height : nodes[1]->height) + 1;
      nodes[0]->father = nodes[1]->father = current;
      nodes[0] = current;
      nodes[1] = NULL;
      if ((remaining = (elements - index)) > 1)
        qsort(nodes, remaining, sizeof(struct s_huffman_node *), p_huffman_compare);
    } else
      d_die(d_error_malloc);
  return nodes[0];
}
int f_huffman_compression(unsigned char *block, size_t size, unsigned char **output, size_t *output_size) {
  struct s_huffman_node *nodes[d_huffman_elements], *root;
  struct s_huffman_code codes[d_huffman_elements];
  size_t index, elements, bits, current_byte = 0, current_bit = 0, endian_size;
  memset(nodes, 0, (sizeof(struct s_huffman_node *) * d_huffman_elements));
  memset(codes, 0, (sizeof(struct s_huffman_code) * d_huffman_elements));
  for (index = 0, elements = 0; index < size; ++index) {
    if (!(nodes[block[index]])) {
      if ((nodes[block[index]] = (struct s_huffman_node *)d_malloc(sizeof(struct s_huffman_node)))) {
        nodes[block[index]]->code = block[index];
        nodes[block[index]]->leaf = d_true;
        ++elements;
      } else
        d_die(d_error_malloc);
    }
    ++nodes[block[index]]->elements;
  }
  if ((root = p_huffman_compression_aggregate(nodes, elements))) {
    if (d_huffman_encode_bytes >= ((root->height / 8) + 1))
      if ((elements = p_huffman_compression_code(root, codes, NULL, 0, 0))) {
        for (index = 0, bits = 0; index < size; ++index)
          bits += codes[block[index]].bits_number;
        *output_size = sizeof(unsigned char) + sizeof(size_t) + (sizeof(struct s_huffman_code) * elements) + ((bits / 8) + 1);
        if ((*output = (unsigned char *)d_malloc(*output_size))) {
          (*output)[current_byte++] = elements;
          endian_size = size;
          if (f_endian_check() == d_big_endian)
            d_swap(endian_size);
          memcpy(&((*output)[current_byte]), &endian_size, sizeof(size_t));
          current_byte += sizeof(size_t);
          for (index = 0; index < d_huffman_elements; ++index)
            if (codes[index].bits_number) {
              memcpy(&((*output)[current_byte]), &(codes[index]), sizeof(struct s_huffman_code));
              current_byte += sizeof(struct s_huffman_code);
            }
          for (index = 0; index < size; ++index)
            p_huffman_compression_append(codes[block[index]], (*output), &current_byte, &current_bit);
        } else
          d_die(d_error_malloc);
      }
    p_huffman_destroy(root);
  } else
    for (index = 0; index < d_huffman_elements; ++index)
      if (nodes[index])
        d_free(nodes[index]);
  return elements;
}
void p_huffman_decompression_expand_append(struct s_huffman_node *root, struct s_huffman_code code) {
  struct s_huffman_node *current_node = root;
  int bits, current_byte, current_bit, child;
  for (current_bit = 8, current_byte = ((code.bits_number - 1) / 8), bits = 0; bits < code.bits_number; ++bits) {
    if (current_bit == 0) {
      current_bit = 8;
      --current_byte;
    }
    --current_bit;
    child = ((code.bytes_encode[current_byte] >> current_bit) & 0x01);
    if (!current_node->childs[child])
      if (!(current_node->childs[child] = (struct s_huffman_node *)d_malloc(sizeof(struct s_huffman_node))))
        d_die(d_error_malloc);
    current_node = current_node->childs[child];
  }
  if (current_node) {
    current_node->leaf = d_true;
    current_node->code = code.code;
  }
}
struct s_huffman_node *p_huffman_decompression_expand(unsigned char *block, int elements) {
  struct s_huffman_code codes[d_huffman_elements];
  struct s_huffman_node *root;
  int index;
  memcpy(codes, block, (sizeof(struct s_huffman_code) * elements));
  if ((root = (struct s_huffman_node *)d_malloc(sizeof(struct s_huffman_node)))) {
    for (index = 0; index < elements; ++index)
      p_huffman_decompression_expand_append(root, codes[index]);
  } else
    d_die(d_error_malloc);
  return root;
}
int p_huffman_decompression_append(struct s_huffman_node *root, unsigned char *output, size_t *current_output_byte, unsigned char *block, size_t size,
  size_t *current_block_byte, size_t *current_block_bit) {
  int result = d_false, child;
  if (root->leaf) {
    output[*current_output_byte] = root->code;
    ++(*current_output_byte);
    result = d_true;
  } else if (*current_output_byte < size) {
    if ((++(*current_block_bit)) > 8) {
      ++(*current_block_byte);
      *current_block_bit = 1; // first new position is 1
    }
    child = (block[*current_block_byte] >> (8 - (*current_block_bit))) & 0x01;
    if (root->childs[child])
      result = p_huffman_decompression_append(root->childs[child], output, current_output_byte, block, size, current_block_byte, current_block_bit);
  }
  return result;
}
int f_huffman_decompression(unsigned char *block, size_t size, unsigned char **output, size_t *output_size) {
  struct s_huffman_node *root;
  unsigned char codes_elements;
  size_t bytes_elements, endian_size, current_output_byte = 0, current_block_byte = 0, current_block_bit = 0;
  if ((codes_elements = block[current_block_byte++])) {
    memcpy(&endian_size, &(block[current_block_byte]), sizeof(size_t));
    if (f_endian_check() == d_big_endian)
      d_swap(endian_size);
    if ((*output_size = endian_size)) {
      current_block_byte += sizeof(size_t);
      if ((bytes_elements = (sizeof(struct s_huffman_code) * codes_elements)) < (size - current_block_byte))
        if ((root = p_huffman_decompression_expand(&(block[current_block_byte]), codes_elements))) {
          current_block_byte += bytes_elements;
          if ((*output = (unsigned char *)d_malloc(*output_size))) {
            while (p_huffman_decompression_append(root, *output, &current_output_byte, block, *output_size, &current_block_byte, &current_block_bit));
          } else
            d_die(d_error_malloc);
          p_huffman_destroy(root);
        }
    }
  }
  return current_block_byte;
}

