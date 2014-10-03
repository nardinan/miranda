#include "ground.h"
#include "objects/objects.h"
#define d_size 1024
int main (int argc, char *argv[]) {
	f_memory_init();
	s_object *pool = f_pool_new(d_new(pool));
	d_pool_begin(pool) {
		int index;
		s_object *array_strings, *array_substrings;
		s_object *string_pool[] = {
			f_string_new(d_new(string), "name of the program is '%s'      ", argv[0]),
			f_string_new(d_new(string), "string contains:"),
			f_string_new(d_new(string), "      prova     "),
			f_string_new(d_new(string), "sono:senza:parole,::e::voi:?::"),
			NULL
		}, *string_singleton, *string_readed = NULL, *string_current;
		s_object *stream_pool[] = {
			f_stream_new_file(d_new(stream), d_P(d_KSTR("./database.txt")), "r", 0777),
			f_stream_new_temporary(d_new(stream), d_P(d_KSTR("temporary_file"))),
			NULL
		};
		while ((string_current = d_call(stream_pool[0], m_stream_read_string, string_readed, d_size))) {
			string_readed = string_current;
			printf("[file row: '%s']\n", d_string_cstring(string_readed));
			d_call(stream_pool[1], m_stream_write_string, string_readed, NULL);
		}
		if (string_readed)
			d_delete(string_readed);
		for (index = 0; stream_pool[index]; ++index)
			d_delete(stream_pool[index]);
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
	} d_pool_end_kill;
	d_delete(pool);
	f_memory_destroy();
	return 0;
}
