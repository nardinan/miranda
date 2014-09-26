#include "ground.h"
#include "objects/objects.h"
int main (int argc, char *argv[]) {
	f_memory_init();
	s_object *array_strings, *array_substrings;
	s_object *string_singleton, *string_pool[] = {
		f_string_new(d_new(string), "name of the program is %s      ", argv[0]),
		f_string_new(d_new(string), "string contains:"),
		f_string_new(d_new(string), "     prova      "),
		f_string_new(d_new(string), "sono:senza:parole:aloha signori:come:::andiamo:?::"),
		NULL
	};
	int index;
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
