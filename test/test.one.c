#define d_miranda_debug 1
#include <miranda/ground.h>
#include <miranda/objects/objects.h>
#include <miranda/objects/io/io.h>
#include <miranda/objects/geometry/geometry.h>
#define d_size 1024
unsigned char *huffman_string =
  (unsigned char *) "Lorem ipsum dolor sit amet,consectetur adipiscing elit. Non enim iam stirpis bonum quaeret, sed animalis. Qui ita affectus, beatum esse;   "\
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
int main(int argc, char *argv[]) {
  d_pool_init;
  d_pool_begin("main context")
  {
#ifdef d_miranda_debug
    printf("[debug mode with safe pointers]\n");
#endif
    struct s_exception *exception;
    unsigned char *huffman_compressed = NULL, *huffman_decompressed = NULL;
    char *string_supply = NULL;
    size_t huffman_length = 0, again_length;
    int index = 0;
    s_object *array_strings, *array_substrings, *json_object = NULL;
    s_object *string_pool[] = {
      f_string_new(d_new(string), "name of the program is '%s'      ", argv[0]),
      f_string_new(d_new(string), "string contains:"),
      f_string_new(d_new(string), "      test      "),
      f_string_new(d_new(string), "Ihave:now:words,::what::about:you?::"),
      NULL
    }, *string_singleton, *string_readed = NULL, *string_current;
    s_object *stream_pool[] = {
      f_stream_new_file(d_new(stream), d_pkstr("./database.txt"), "r", 0777),
      f_stream_new_file(d_new(stream), d_pkstr("./database.json"), "r", 0777),
      f_stream_new_temporary(d_new(stream), d_pkstr("temporary_file")),
      f_stream_new(d_new(stream), d_pkstr("stdout"), STDOUT_FILENO),
      NULL
    };
    s_object *resources = f_resources_new(d_new(resources), d_pkstr("/home/god/Pictures"), ".jpg");
    s_object *line_A = f_line_new(d_new(line), -40, -75, 150, 120), *line_B = f_line_new(d_new(line), -40, 75, 100, 0),
      *line_C = f_line_new(d_new(line), -40, 75, -100, 75);
    if (d_call(line_A, m_line_intersect, line_B))
      printf("intersection between line A and line B\n");
    if (d_call(line_A, m_line_intersect, line_C))
      printf("intersection between line A and line C\n");
    d_delete(line_A);
    d_delete(line_B);
    d_delete(line_C);
    d_delete(resources);
    d_try
    {
      json_object = f_json_new_stream(d_new(json), stream_pool[1]);
      d_call(json_object, m_json_write, stream_pool[3]);
      printf("\n");
      index = 0;
      while (d_true)
        if (d_call(json_object, m_json_get_string, &string_supply, "sds", "skills", index++, "category"))
          printf("skills[%d].category: %s\n", (index - 1), string_supply);
        else
          break;
      index = 0;
      while (d_true)
        if (d_call(json_object, m_json_get_string, &string_supply, "sd", "interests", index++))
          printf("interests[%d]: %s\n", (index - 1), string_supply);
        else
          break;
      d_call(json_object, m_json_set_string, "Physics", "s", "interests");
      d_call(json_object, m_json_insert_value, "languages", "s", "favorites");
      d_call(json_object, m_json_set_array, "ss", "favorites", "languages");
      d_call(json_object, m_json_set_string, "C", "ss", "favorites", "languages");
      d_call(json_object, m_json_set_string, "C++", "ss", "favorites", "languages");
      d_call(json_object, m_json_set_string, "bash", "ss", "favorites", "languages");
      d_call(json_object, m_json_delete_value, "sds", "skills", 0, "values");
      d_call(json_object, m_json_delete_value, "sdsd", "skills", 1, "values", 0);
      d_call(json_object, m_json_write, stream_pool[3]);
      printf("\n");
    }
    d_catch(exception)
    {
      d_exception_dump(stderr, exception);
      d_raise;
    }
    d_endtry;
    if (json_object)
      d_delete(json_object);
    f_huffman_compression(huffman_string, f_string_strlen((const char *) huffman_string) + 1, &huffman_compressed, &huffman_length);
    f_huffman_decompression(huffman_compressed, huffman_length, &huffman_decompressed, &again_length);
    printf("[huffman compression: %zu bytes VS %zu bytes compressed string]\n", (f_string_strlen((const char *) huffman_string) + 1), huffman_length);
    printf("[and again: %zu]\n", again_length);
    d_free(huffman_compressed);
    d_free(huffman_decompressed);
    d_pool_begin("another context")
    {
      while ((string_current = d_call(stream_pool[0], m_stream_read_string, string_readed, d_size))) {
        string_readed = string_current;
        printf("[file row: '%s']\n", d_string_cstring(string_readed));
        d_call(stream_pool[2], m_stream_write_string, string_readed, NULL);
      }
      if (string_readed)
        d_delete(string_readed);
      for (index = 0; stream_pool[index]; ++index)
        d_delete(stream_pool[index]);
    }
    d_pool_end;
    (d_true);
    array_strings = f_array_new(d_new(array), 4);
    for (index = 0; string_pool[index]; ++index) {
      d_call(string_pool[index], m_string_trim, NULL);
      d_call(array_strings, m_array_insert, string_pool[index], index);
    }
    d_array_foreach(array_strings, string_singleton)
    {
      printf("our string: %s\n", d_string_cstring(string_singleton));
    }
    if ((array_substrings = d_call(string_pool[index - 1], m_string_split, ':'))) {
      printf("\tEXPLODE: OK\n");
      d_array_foreach(array_substrings, string_singleton)
      {
        printf("\tvalue is %s\n", d_string_cstring(string_singleton));
      }
      d_delete(array_substrings);
    }
    d_delete(array_strings);
    for (index = 0; string_pool[index]; ++index)
      d_delete(string_pool[index]);
    d_assert(d_false);
  }
  d_pool_end;
  d_pool_destroy;
  f_memory_destroy();
  return 0;
}
