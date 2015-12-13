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
#ifndef miranda_list_h
#define miranda_list_h
#include "memory.h"
#include "string.local.h"
#define d_foreach(l,n,k) for((l)->current=(l)->head;(((n)=(k *)(l)->current));(l)->current=(l)->current->next)
#define d_reverse_foreach(l,n,k) for((l)->current=(l)->tail;(((n)=(k *)(l)->current));(l)->current=(l)->current->back)
#define d_list_rewind(l) (l)->current=((l)->current->back)?(l)->current->back:(l)->current
#define d_list_node_in(l,n) (((n)->next!=NULL)||((n)->back!=NULL)||((l)->head==(n)))
#define d_list_node_head struct s_list_node head
#define d_list_node_constant {NULL, NULL}
typedef size_t t_list_value;
typedef enum e_list_insert_kind {
	e_list_insert_head,
	e_list_insert_tail
} e_list_insert_kind;
typedef struct s_list_node {
	struct s_list_node *next, *back;
} s_list_node;
typedef struct s_list {
	struct s_list_node *head, *tail, *current;
	t_list_value fill;
} s_list;
extern void f_list_init(struct s_list **list);
extern void f_list_destroy(struct s_list **list);
extern void f_list_append(struct s_list *list, struct s_list_node *node, enum e_list_insert_kind kind);
extern void f_list_insert(struct s_list *list, struct s_list_node *node, struct s_list_node *left);
extern struct s_list_node *f_list_delete(struct s_list *list, struct s_list_node *node);
#endif

