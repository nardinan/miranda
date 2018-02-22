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
#include "list.h"
void f_list_init(struct s_list **list) {
  if ((*list = (struct s_list *) d_malloc(sizeof(struct s_list))))
    memset((*list), 0, sizeof(struct s_list));
  else
    d_die(d_error_malloc);
}
void f_list_destroy(struct s_list **list) {
  if (*list)
    d_free(*list);
  *list = NULL;
}
void f_list_append(struct s_list *list, struct s_list_node *node, enum e_list_insert_kind kind) {
  switch (kind) {
    case e_list_insert_head:
      if (list->head)
        list->head->back = node;
      else
        list->tail = node;
      node->next = list->head;
      list->head = node;
      break;
    case e_list_insert_tail:
      if (list->tail)
        list->tail->next = node;
      else
        list->head = node;
      node->back = list->tail;
      list->tail = node;
      break;
  }
  ++(list->fill);
}
void f_list_insert(struct s_list *list, struct s_list_node *node, struct s_list_node *left) {
  if (left) {
    if (left->next) {
      node->next = left->next;
      node->next->back = node;
      node->back = left;
      left->next = node;
    } else
      f_list_append(list, node, e_list_insert_tail);
  } else
    f_list_append(list, node, e_list_insert_head);
  ++(list->fill);
}
struct s_list_node *f_list_delete(struct s_list *list, struct s_list_node *node) {
  if (d_list_node_in(list, node)) {
    if (node->next)
      node->next->back = node->back;
    else
      list->tail = node->back;
    if (node->back)
      node->back->next = node->next;
    else
      list->head = node->next;
    node->next = NULL;
    node->back = NULL;
    --(list->fill);
  }
  return node;
}

