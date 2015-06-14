#include <miranda/ground.h>
#include <miranda/objects/objects.h>
#define d_size 1024
unsigned char *huffman_string =
		"Lorem ipsum dolor sit amet,consectetur adipiscing elit. Non enim iam stirpis bonum quaeret, sed animalis. Qui ita affectus, beatum esse;   "\
		"Te ipsum, dignissimum maioribus tuis, voluptasne induxit, ut adolescentulus eriperes P. Tubulo putas dicere? Qui non moveatur et offensione"\
		" turpitudinis et comprobatione honestatis? Tamen a proposito, inquam, aberramus. Istam voluptatem, inquit, Epicurus ignorat? Nummus in Croe"\
		"si divitiis obscuratur, pars est tamen divitiarum. Efficiens dici potest. Quis Aristidem non mortuum diligit? Quarum ambarum rerum cum medi"\
		"cinam pollicetur, luxuriae licentiam pollicetur. Ut optime, secundum naturam affectum esse possit. Facillimum id quidem est, inquam. Quid a"\
		"it Aristoteles reliquique Platonis alumni? Certe non potest. Si mala non sunt, iacet omnis ratio Peripateticorum. Sed haec omittamus; Cum s"\
		"ciret confestim esse moriendum eamque mortem ardentiore studio peteret, quam Epicurus voluptatem petendam putat. Equidem etiam Epicurum, in"\
		"physicis quidem, Democriteum puto. Facit enim ille duo seiuncta ultima bonorum, quae ut essent vera, coniungi debuerunt; Theophrasti igitur"\
		", inquit, tibi liber ille placet de beata vita? Deprehensus omnem poenam contemnet. Hoc enim constituto in philosophia constituta sunt omni"\
		"a. Tu vero, inquam, ducas licet, si sequetur; Quo modo autem optimum, si bonum praeterea nullum est? Quae similitudo in genere etiam humano"\
		" apparet. Alterum significari idem, ut si diceretur, officia media omnia aut pleraque servantem vivere. Quae qui non vident, nihil umquam m"\
		"agnum ac cognitione dignum amaverunt. Duo Reges: constructio interrete. Eiuro, inquit adridens, iniquum, hac quidem de re; Et harum quidem "\
		"rerum facilis est et expedita distinctio. Vita? Deprehensus omnem poenam contemnet. Hoc enim constituto in philosophia constituta sunt omni"\
		"a. Tu vero, inquam, ducas licet, si sequetur; Quo modo autem optimum, si bonum praeterea nullum est? Quae similitudo in genere etiam humano"\
		" apparet. Alterum significari idem, ut si diceretur, officia media omnia aut pleraque servantem vivere. Quae qui non vident, nihil umquam m"\
		"agnum ac cognitione dignum amaverunt. Duo Reges: constructio interrete. Eiuro, inquit adridens, iniquum, hac quidem de re; Et harum quidem "\
		"rerum facilis est et expedita distinctio. Vita? Deprehensus omnem poenam contemnet. Hoc enim constituto in philosophia constituta sunt omni"\
		"a. Tu vero, inquam, ducas licet, si sequetur; Quo modo autem optimum, si bonum praeterea nullum est? Quae similitudo in genere etiam humano"\
		" apparet. Alterum significari idem, ut si diceretur, officia media omnia aut pleraque servantem vivere. Quae qui non vident, nihil umquam m"\
		"agnum ac cognitione dignum amaverunt. Duo Reges: constructio interrete. Eiuro, inquit adridens, iniquum, hac quidem de re; Et harum quidem "\
		"rerum facilis est et expedita distinctio. Vita? Deprehensus omnem poenam contemnet. Hoc enim constituto in philosophia constituta sunt omni"\
		"a. Tu vero, inquam, ducas licet, si sequetur; Quo modo autem optimum, si bonum praeterea nullum est? Quae similitudo in genere etiam humano"\
		" apparet. Alterum significari idem, ut si diceretur, officia media omnia aut pleraque servantem vivere. Quae qui non vident, nihil umquam m"\
		"agnum ac cognitione dignum amaverunt. Duo Reges: constructio interrete. Eiuro, inquit adridens, iniquum, hac quidem de re; Et harum quidem "\
		"rerum facilis est et expedita distinctio. Non enim iam stirpis bonum quaeret, sed animalis. Qui ita affectus, beatum esse numquam probabis;"\
		"Te ipsum, dignissimum maioribus tuis, voluptasne induxit, ut adolescentulus eriperes P. Tubulo putas dicere? Qui non moveatur et offensione"\
		" turpitudinis et comprobatione honestatis? Tamen a proposito, inquam, aberramus. Istam voluptatem, inquit, Epicurus ignorat? Nummus in Croe"\
		"si divitiis obscuratur, pars est tamen divitiarum. Efficiens dici potest. Quis Aristidem non mortuum diligit? Quarum ambarum rerum cum medi"\
		"cinam pollicetur, luxuriae licentiam pollicetur. Ut optime, secundum naturam affectum esse possit. Facillimum id quidem est, inquam. Quid a"\
		"it Aristoteles reliquique Platonis alumni? Certe non potest. Si mala non sunt, iacet omnis ratio Peripateticorum. Sed haec omittamus; Cum s"\
		"ciret confestim esse moriendum eamque mortem ardentiore studio peteret, quam Epicurus voluptatem petendam putat. Equidem etiam Epicurum, in"\
		"physicis quidem, Democriteum puto. Facit enim ille duo seiuncta ultima bonorum, quae ut essent vera, coniungi debuerunt; Theophrasti igitur"\
		", inquit, tibi liber ille placet de beata vita? Deprehensus omnem poenam contemnet. Hoc enim constituto in philosophia constituta sunt omni"\
		"a. Tu vero, inquam, ducas licet, si sequetur; Quo modo autem optimum, si bonum praeterea nullum est? Quae similitudo in genere etiam humano"\
		" apparet. Alterum significari idem, ut si diceretur, officia media omnia aut pleraque servantem vivere. Quae qui non vident, nihil umquam m"\
		"agnum ac cognitione dignum amaverunt. Duo Reges: constructio interrete. Eiuro, inquit adridens, iniquum, hac quidem de re; Et harum quidem "\
		"rerum facilis est et expedita distinctio. Vita? Deprehensus omnem poenam contemnet. Hoc enim constituto in philosophia constituta sunt omni"\
		"a. Tu vero, inquam, ducas licet, si sequetur; Quo modo autem optimum, si bonum praeterea nullum est? Quae similitudo in genere etiam humano"\
		" apparet. Alterum significari idem, ut si diceretur, officia media omnia aut pleraque servantem vivere. Quae qui non vident, nihil umquam m"\
		"agnum ac cognitione dignum amaverunt. Duo Reges: constructio interrete. Eiuro, inquit adridens, iniquum, hac quidem de re; Et harum quidem "\
		"rerum facilis est et expedita distinctio. Vita? Deprehensus omnem poenam contemnet. Hoc enim constituto in philosophia constituta sunt omni"\
		"a. Tu vero, inquam, ducas licet, si sequetur; Quo modo autem optimum, si bonum praeterea nullum est? Quae similitudo in genere etiam humano"\
		" apparet. Alterum significari idem, ut si diceretur, officia media omnia aut pleraque servantem vivere. Quae qui non vident, nihil umquam m"\
		"agnum ac cognitione dignum amaverunt. Duo Reges: constructio interrete. Eiuro, inquit adridens, iniquum, hac quidem de re; Et harum quidem "\
		"rerum facilis est et expedita distinctio. Vita? Deprehensus omnem poenam contemnet. Hoc enim constituto in philosophia constituta sunt omni"\
		"a. Tu vero, inquam, ducas licet, si sequetur; Quo modo autem optimum, si bonum praeterea nullum est? Quae similitudo in genere etiam humano"\
		" apparet. Alterum significari idem, ut si diceretur, officia media omnia aut pleraque servantem vivere. Quae qui non vident, nihil umquam m"\
		"agnum ac cognitione dignum amaverunt. Duo Reges: constructio interrete. Eiuro, inquit adridens, iniquum, hac quidem de re; Et harum quidem "\
		"rerum facilis est et expedita distinctio.";
int main (int argc, char *argv[]) {
	f_memory_init();
	d_pool_init;
	d_pool_begin("main context") {
		unsigned char *huffman_compressed = NULL, *huffman_decompressed = NULL;
		size_t huffman_length = 0, again_length;
		int index;
		s_object *array_strings, *array_substrings;
		s_object *string_pool[] = {
			f_string_new(d_new(string), "name of the program is '%s'      ", argv[0]),
			f_string_new(d_new(string), "string contains:"),
			f_string_new(d_new(string), "      test      "),
			f_string_new(d_new(string), "Ihave:now:words,::what::about:you?::"),
			NULL
		}, *string_singleton, *string_readed = NULL, *string_current;
		s_object *stream_pool[] = {
			f_stream_new_file(d_new(stream), d_P(d_KSTR("./database.txt")), "r", 0777),
			f_stream_new_temporary(d_new(stream), d_P(d_KSTR("temporary_file"))),
			NULL
		};
		f_huffman_compression(huffman_string, f_string_strlen(huffman_string)+1, &huffman_compressed, &huffman_length);
                f_huffman_decompression(huffman_compressed, huffman_length, &huffman_decompressed, &again_length);
		printf("[huffman compression: %zu bytes VS %zu bytes compressed string]\n", (f_string_strlen(huffman_string)+1), huffman_length);
                printf("[and again: %zu]\n%s", again_length, huffman_decompressed);
		d_free(huffman_compressed);
		d_free(huffman_decompressed);
		d_pool_begin("another context") {
			while ((string_current = d_call(stream_pool[0], m_stream_read_string, string_readed, d_size))) {
				string_readed = string_current;
				printf("[file row: '%s']\n", d_string_cstring(string_readed));
				d_call(stream_pool[1], m_stream_write_string, string_readed, NULL);
			}
			if (string_readed)
				d_delete(string_readed);
			for (index = 0; stream_pool[index]; ++index)
				d_delete(stream_pool[index]);
		} d_pool_end(d_true);
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
	} d_pool_end(d_true);
	d_pool_destroy;
	f_memory_destroy();
	return 0;
}
