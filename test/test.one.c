#include "ground.h"
#include "objects/objects.h"
#define d_size 1024
int main (int argc, char *argv[]) {
	f_memory_init();
	s_object *array_strings, *array_substrings;
	s_object *string_singleton, *string_pool[] = {
		f_string_new(d_new(string), "name of the program is '%s'      ", argv[0]),
		f_string_new(d_new(string), "string contains:"),
		f_string_new(d_new(string), "      prova     "),
		f_string_new(d_new(string), "sono:senza:parole:,:e::voi:?::"),
		NULL
	};
	s_object *string_file_name = f_string_new_constant(d_new(string), "./database.txt"),
		 *string_temp_name = f_string_new_constant(d_new(string), "temporary_file"), *string_readed = NULL, *string_current;
	s_object *stream[] = {
		f_stream_new_file(d_new(stream), string_file_name, "r", 0777),
		f_stream_new_temporary(d_new(stream), string_temp_name),
		NULL
	};
	int index;
	d_delete(string_file_name);
	d_delete(string_temp_name);
	while ((string_current = d_call(stream[0], m_stream_read_string, string_readed, d_size))) {
		string_readed = string_current;
		printf("[file row: '%s']\n", d_string_cstring(string_readed));
		d_call(stream[1], m_stream_write_string, string_readed, NULL);
	}
	if (string_readed)
		d_delete(string_readed);
	d_delete(stream[0]);
	d_delete(stream[1]);
	array_strings = f_array_new(d_new(array), 4);
	for (index = 0; string_pool[index]; ++index) {
		d_call(string_pool[index], m_string_trim, NULL);
		d_call(array_strings, m_array_insert, string_pool[index], index);
	}
	d_array_foreach(array_strings, string_singleton) {
		printf("our string: %s\n", d_string_cstring(string_singleton));
	}

	if ((array_substrings = d_call(string_pool[index-1], m_string_split, ':'))) {
		printf("\tEXPLODE: OK\n");
		d_array_foreach(array_substrings, string_singleton) {
			printf("\tvalue is %s\n", d_string_cstring(string_singleton));
		}
		d_delete(array_substrings);
	}
	d_delete(array_strings);
	for (index = 0; string_pool[index]; ++index)
		d_delete(string_pool[index]);
	f_memory_destroy();
	return 0;
}
