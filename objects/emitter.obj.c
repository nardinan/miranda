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
#include "emitter.obj.h"
struct s_object *f_emitter_new(struct s_object *self) {
	struct s_emitter_attributes *attributes = d_prepare(self, emitter);
	f_list_init(&(attributes->signals));
	return self;
}

d_define_method(emitter, record)(struct s_object *self, const char *id) {
	d_using(emitter);
	struct s_signal *signal;
	if ((signal = (struct s_signal *) d_malloc(sizeof(struct s_signal)))) {
		strncpy(signal->id, id, d_emitter_name_size);
		f_list_append(emitter_attributes->signals, (struct s_list_node *)signal, e_list_insert_head);
	}
	return (void *)signal;
}

d_define_method(emitter, get)(struct s_object *self, const char *id) {
	d_using(emitter);
	struct s_signal *signal = NULL;
	d_foreach(emitter_attributes->signals, signal, struct s_signal)
		if (f_string_strcmp(signal->id, id) == 0)
			break;
	return (void *)signal;
}

d_define_method(emitter, embed_parameter)(struct s_object *self, const char *id, void *parameter) {
	struct s_signal *signal;
	void **new_parameters;
	if ((signal = d_call(self, m_emitter_get, id))) {
		signal->parameters_size++;
		if ((new_parameters = (void **) d_realloc(signal->parameters, signal->parameters_size*sizeof(void *)))) {
			signal->parameters = new_parameters;
			signal->parameters[signal->parameters_size-1] = parameter;
		}
	}
	return (void *)signal;
}

d_define_method(emitter, embed_function)(struct s_object *self, const char *id, f_emitter_action action) {
	struct s_signal *signal;
	if ((signal = d_call(self, m_emitter_get, id)))
		signal->action = action;
	return (void *)signal;
}

d_define_method(emitter, raise)(struct s_object *self, const char *id) {
	struct s_signal *signal;
	void *result = NULL;
	if ((signal = d_call(self, m_emitter_get, id))) {
		d_log(e_log_level_medium, "signal '%s' has been raised from %s (%s::%d)", id, self->type, self->file, self->line);
		if (signal->action)
			result = signal->action(self, signal->parameters, signal->parameters_size);
	}
	return result;
}

d_define_method(emitter, delete)(struct s_object *self, struct s_emitter_attributes *attributes) {
	struct s_signal *signal;
	while (attributes->signals->head)
		if ((signal = (struct s_signal *)f_list_delete(attributes->signals, attributes->signals->head))) {
			if (signal->parameters_size)
				d_free(signal->parameters);
			d_free(signal);
		}
	f_list_destroy(&(attributes->signals));
	return NULL;
}

d_define_class(emitter) {
	d_hook_method(emitter, e_flag_public, record),
	d_hook_method(emitter, e_flag_private, get),
	d_hook_method(emitter, e_flag_public, embed_parameter),
	d_hook_method(emitter, e_flag_public, embed_function),
	d_hook_method(emitter, e_flag_public, raise),
	d_hook_delete(emitter),
	d_hook_method_tail
};
