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
#ifndef miranda_object_h
#define miranda_object_h
#include "../list.h"
#include "../hash.h"
#include "../exception.h"
#include "../memory.h"
typedef enum e_flag {
	e_flag_null		= 0X00000000,
	e_flag_private		= 0x00000001,
	e_flag_public		= 0x00000002,
	e_flag_hashed		= 0x00000004,
	e_flag_pooled		= 0x00000008,
	e_flag_placeholder	= 0x00000010
} e_flag;
typedef void *(*t_class_method)();
typedef struct s_object {
	struct s_list_node head;
	const char *type, *file;
	unsigned int line;
	struct s_attributes *last_attributes;
	struct s_list virtual_tables, attributes;
	t_hash_value hash_value;
	int flags;
} s_object;
typedef struct s_method {
	const char *symbol, *file;
	enum e_flag flag;
	t_class_method method;
} s_method;
typedef struct s_virtual_table {
	struct s_list_node head;
	struct s_method *virtual_table;
} s_virtual_table;
typedef struct s_attributes {
	struct s_list_node head;
	const char *type;
} s_attributes;
extern const char m_object_delete[];
extern const char m_object_hash[];
extern const char m_object_compare[];
d_exception_declare(undefined_method);
d_exception_declare(private_method);
d_exception_declare(wrong_casting);
#define d_call(obj,sym,...) (p_object_recall(__FILE__,__LINE__,(obj),(sym))->method((obj),__VA_ARGS__))
extern const struct s_method *p_object_recall(const char *file, int line, struct s_object *object, const char *symbol);
#define d_new(kind) (p_object_malloc(__FILE__,__LINE__,v_##kind##_type,e_flag_null))
extern __attribute__ ((warn_unused_result)) struct s_object *p_object_malloc(const char *file, int line, const char *type, int flags);
#define d_prepare(obj,kind) ((struct s_##kind##_attributes *)p_object_setup((obj),v_##kind##_vtable,\
			p_object_attributes_malloc(sizeof(struct s_##kind##_attributes),v_##kind##_type)))
extern __attribute__ ((warn_unused_result)) struct s_attributes *p_object_attributes_malloc(size_t size, const char *type);
extern struct s_attributes *p_object_setup(struct s_object *object, struct s_method *virtual_table, struct s_attributes *attributes);
#define d_cast(obj,kind) ((struct s_##kind##_attributes *)p_object_cast((obj),v_##kind##_type))
extern struct s_attributes *p_object_cast(struct s_object *object, const char *type);
extern void f_object_delete(struct s_object *object);
extern t_hash_value f_object_hash(struct s_object *object);
extern struct s_object *p_object_compare_single(struct s_object *object, struct s_object *other);
extern struct s_object *f_object_compare(struct s_object *object, struct s_object *other);
#define d_declare_class(kind)\
	extern const char v_##kind##_type[];\
	extern struct s_method v_##kind##_vtable[];\
	typedef struct s_##kind##_attributes
#define d_declare_class_tail(kind) s_##kind##_attributes
#define d_define_class(kind)\
	const char v_##kind##_type[]=#kind;\
	struct s_method v_##kind##_vtable[]=
#define d_hook_method(kind,flag,sym) {m_##kind##_##sym,__FILE__,(flag),(t_class_method)&(p_##kind##_##sym)}
#define d_hook_delete(kind) {m_object_delete,__FILE__,e_flag_public,(t_class_method)&(p_##kind##_delete)}
#define d_hook_hash(kind) {m_object_hash,__FILE__,e_flag_public,(t_class_method)&(p_##kind##_hash)}
#define d_hook_compare(kind) {m_object_compare,__FILE__,e_flag_public,(t_class_method)&(p_##kind##_compare)}
#define d_hook_method_tail {NULL,__FILE__,e_flag_private,NULL}
#define d_declare_method(kind,sym)\
	extern const char m_##kind##_##sym[];\
	struct s_object *p_##kind##_##sym
#define d_define_method(kind,sym)\
	const char m_##kind##_##sym[]=#sym;\
	struct s_object *p_##kind##_##sym
#define d_using(kind) struct s_##kind##_attributes *kind##_attributes = d_cast(self, kind)
#endif

