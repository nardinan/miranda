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
#include "memory.obj.h"
struct s_object *f_memory_new(struct s_object *self) {
  struct s_memory_attributes *attributes = d_prepare(self, memory);
  attributes->references = 1; /* first reference */
  return self;
}
d_define_method(memory, retain)(struct s_object *self) {
  d_using(memory);
  ++(memory_attributes->references);
  return self;
}
d_define_method(memory, release)(struct s_object *self) {
  struct s_memory_attributes *memory_attributes;
  struct s_object *result = NULL;
  if ((memory_attributes = d_cast(self, memory)))
    if (memory_attributes->references > 0) {
      memory_attributes->references--;
      if (memory_attributes->references > 0)
        result = self;
    }
  return result;
}
d_define_class(memory) {d_hook_method(memory, e_flag_public, retain),
                        d_hook_method(memory, e_flag_public, release),
                        d_hook_method_tail};
