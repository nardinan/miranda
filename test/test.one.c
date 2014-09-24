#include "ground.h"
#include "objects/objects.h"
int main (int argc, char *argv[]) {
	s_object *string_one, *string_two, *string_three, *string_four, *string_five;
	f_memory_init();
	string_one = f_string_new(d_new(string), "name of the program is: %s         ", argv[0]);
	string_five = d_retain(string_one);
	string_two = f_string_new(d_new(string), "string_one contains:");
	string_three = f_string_new_constant(d_new(string), "      prova   ");
	string_four = f_string_new_constant(d_new(string), d_string_cstring(string_one));
	printf("string one: \"%s\"\nstring two: \"%s\"\n", d_string_cstring(string_one), d_string_cstring(string_two));
	d_call(string_two, m_string_append, string_one);
	printf("after MERGE: \"%s\"\n", d_string_cstring(string_two));
	printf("string K: \"%s\"\n", d_string_cstring(string_three));
	d_call(string_three, m_string_trim, NULL);
	d_call(string_two, m_string_trim, NULL);
	printf("after TRIM K: \"%s\"\nafter TRIM two: \"%s\"\n", d_string_cstring(string_three), d_string_cstring(string_two));
	printf("as you can see, the constant is still the same!\n");
	d_call(string_one, m_string_trim, NULL);
	printf("compare between:\n\t\"%s\"\n\t\"%s\"\n%s\n", d_string_cstring(string_one), d_string_cstring(string_four),
			((d_call(string_one, m_object_compare, string_four))?"different":"same"));
	d_delete(string_one);
	d_delete(string_two);
	d_delete(string_three);
	d_delete(string_four);
	d_delete(string_five);
	f_memory_destroy();
	return 0;
}
