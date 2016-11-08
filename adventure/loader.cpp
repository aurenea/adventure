#include "loader.h"

#include <string>
#include <regex>

using namespace std;


FileLoader::FileLoader(Module* m) {
    module = m;
    add_key(string("this"));
    add_key(string("inherit"));
    add_key(string("Form"));
    add_key(string("Widget"));
    add_key(string("Menu"));
    add_key(string("Object"));
    add_key(string("Item"));
    add_key(string("Container"));
    add_key(string("Entity"));
    add_key(string("Attired"));
}

unsigned int FileLoader::add_key(string key) {
    unsigned int ikey = (unsigned int)keymap.size();
    keymap.insert( {{ key, ikey }} );
    return ikey;
}

unsigned int FileLoader::get_key(string key) {
    unordered_map<string, unsigned int>::iterator iter = keymap.find(key);
    if (iter == keymap.end()) {
        return add_key(key);
    } else {
        return iter->second;
    }
}


/*
Script* FileLoader::load_script() {
    load_script(string(""));
}

Script* FileLoader::load_script(string input) {
    char c;
    while (c = get_char()) {
        if (!regex_match(c, regex("\s"))) {
            input.append(c);
            if (regex_match(input, regex("\bif\("))) {
                return load_conditional_script(true);
            }
        }
    }
}

BooleanScript* load_boolean_script() {
    string input;
    BooleanScript* b = NULL;
    char c;
    while (c = get_char()) {
        if (!regex_match(c, regex("\s"))) {
            input.append(c);
            if (regex_match(input, regex("\("))) {
                if (b == NULL) {
                    b = load_boolean_script(file, buffer);
                    input.clear();
                } else {
                    throw string("Expected ")
                }
            } else if (regex_match(input, regex("&&"))) {
                if (b != NULL) {

                } else {

                }
                return new IntersectScript((b == NULL ? new TrueScript() : b), load_boolean_script(file, buffer));
            } else if (regex_match(input, regex("\|\|"))) {
                return new UnionScript((b == NULL ? new TrueScript() : b))
            }
            if (regex_match(input, regex("\)"))) {
                if (b != NULL) {
                    return b;
                } else {

                }
            }
        }
    }
}

ConditionalScript* FileLoader::load_conditional_script(bool after) {
    BooleanScript* b = load_boolean_script();
    Script* t = NULL;
    Script* f = NULL;
    string input;
    char c;
    while (c = get_char()) {
        if (regex_match(c, regex("[^elsif\{\(\s]"))) {
            return new ConditionalScript((after ? load_script(input) : NULL), b, t, f);
        }
        if (!regex_match(c, regex("\s"))) {
            input.append(c);
            if (regex_match(input, regex("^elseif\("))) {
                f = load_conditional_script(false);
                return new ConditionalScript((after ? load_script() : NULL), b, t, f);
            } else if (regex_match(input, regex("^else\{"))) {
                f = load_script();
                return new ConditionalScript(load_script(), b, t, f);
            } else if (regex_match(input, regex("^\{"))) {
                t = load_script();
                input.clear();
            }
        }
    }
}*/

/*

void* FileLoader::load_value(string s, unordered_map<std::string, void*> local_vars) {
    cmatch match;
    if (s.empty() || s.compare("null") == 0) { return NULL; }
    if (regex_match(s, match, regex("(.+)\.(([^\(\)]+)(\(.*\))?)"))) {
        if (TypeData* d = dynamic_cast<TypeData*>(load_value(match.str(1)))) {
            if (match.str(4).compare(string("")) != 0) {
                // 5 = parameters for function
            } else {
                return d->get_param(match.str(3));
            }
        } else {
            throw string(match.str(1)).append(" does not have member ").append(match.str(3)).append(".");
        }
    }
    unordered_map<std::string, void*>::iterator it = local_vars.find(s);
    if (it != local_vars.end()) {
        return it->second;
    }
    it = global_vars.find(s);
    if (it != global_vars.end()) {
        return it->second;
    }
}

unsigned int FileLoader::load_conditional(Script* s, string* str, string::iterator* iter) {
    unsigned int r_value = 0;
    regex or_c("\|\|");
    regex and_c("&&");

    unsigned int cnd = 0;
    string input;
    while (*iter < str->end()) {
        if (**iter == '(') {
            if (cnd > 0) { throw string("Unexpected token '('."); }
            cnd = load_conditional(s, str, iter);
        } else {
            input.append(**iter);
            ++(*iter);
            if (regex_match(*str, or_c)) {
                r_value = s->append(ScriptType::OR);
                if (cnd == 0) {
                    delete s;
                    throw string("No first clause for '||' token.");
                }
                s->append(cnd);
                unsigned int second = s->append(ScriptType::END);
                s->set_value(second, load_conditional(s, str, iter));
                return second;
            } else if (regex_match(*str, and_c)) {
                r_value = s->append(ScriptType::AND);
                if (cnd == 0) {
                    delete s;
                    throw string("No first clause for '&&' token.");
                }
                s->append(cnd);
                unsigned int second = s->append(ScriptType::END);
                s->set_value(second, load_conditional(s, str, iter));
                return second;
            }
        }
    }
}

unsigned int FileLoader::load_conditional(Script* s) {

}

unsigned int FileLoader::load_script(Script* s, unsigned int fsi) {
    unsigned int first_index = s->last_index();
    regex end_script("\}");
    regex if_c("\bif\(");
    regex else_c("\belse\{");
    regex elseif_c("\belseif\(");
    regex for_c("\bfor\((.*);(.*);(.*)\)\{");

    string input;
    while (add_char(&input) && !regex_match(input, end_script)) {
        if (regex_match(input, if_c) || (regex_match(input, elseif_c) && fsi > 0)) {
            if (fsi > 0) {
                s->set_value(fsi, s->append(ScriptType::IF));
                fsi = 0;
            } else {
                s->append(ScriptType::IF);
            }
            unsigned int eval = s->append(ScriptType::END);
            unsigned int ts = s->append(ScriptType::END);
            unsigned int fs = s->append(ScriptType::END);
            unsigned int as = s->append(ScriptType::END);
            input.clear();
            s->set_value(eval, load_conditional(s));
            if (add_char(&input) && regex_match(input, regex("\{")) {
                s->set_value(ts, load_script(s));
            } else {
                delete s;
                throw string("Expected '{', received '").append(input).append("'.");
            }
            s->set_value(as, load_script(s, fs));
        } else if (regex_match(input, else_c) && fsi > 0) {
            s->set_value(fsi, load_script(s));
            fsi = 0;
            first_index = s->last_index();
        } else if regex_match(input, match, for_c) {

        }
    }

    s->append(ScriptType::END);
    return first_index;
}*/

regex whitespace("\s");
regex definition("(.*)\{");
regex line_end("(.*);");
regex var_def("(\w[^=]*)(=(.+))?");

void FileLoader::load(ALLEGRO_FILE* f) {
    file = f;
    l_no = -1;

    try {
        /*
        smatch match;
        regex type_def("type(\w.*)");
        regex form_def("def(\w.*)");
        regex script_def("(\w[^=]*)=(script)?");

        string input;
        while (add_char(&input)) {
            if (regex_match(input, match, definition)) {
                smatch def_match;
                if (regex_match(match.str(1), def_match, type_def)) {
                    data.insert( {{ def_match.str(1), load_type() }} );
                    input.clear();
                } else if (regex_match(match.str(1), def_match, form_def)) {
                    data.insert( {{ def_match.str(1), load_form() }} );
                    input.clear();
                } else if (regex_match(match.str(1), def_match, script_def)) {
                    global_vars.insert( {{ def_match.str(1), load_script() }} );
                } else {
                    throw string("Unrecognized qualifier before '{' token.");
                }
            } else if (regex_match(input, match, line_end)) {
                cmatch def_match;
                if (regex_match(match.str(1), def_match, var_def)) {
                    global_vars.insert( {{ def_match.str(1), load_value(def_match.str(3)) }} );
                } else {
                    throw string("Unrecognized line format.");
                }
            }
        }
        */
    } catch (string s) {
        // generate error message
    }
}

bool FileLoader::add_char(string* s) {
    if (b_index < 127 && buffer[b_index] == '/' && buffer[b_index+1] == '*') {
        while (true) {
            if (b_index < 127 && buffer[b_index] == '*' && buffer[b_index+1] == '/') {
                b_index += 2;
                break;
            } else {
                if (b_index >= 128 || buffer[b_index] == '\0'){
                    if (al_fgets(file, buffer, sizeof(buffer))) {
                        l_no++;
                        b_index = 0;
                    } else if (al_feof(file)) {
                        return false;
                    } else if (al_ferror(file)) {
                        throw string(al_ferrmsg(file));
                    }
                } else {
                    b_index++;
                }
            }
        }
    }
    if (b_index >= 128 || buffer[b_index] == '\0' || l_no < 0 || (b_index < 127 && buffer[b_index] == '/' && buffer[b_index+1] == '/')) {
        if (al_fgets(file, buffer, sizeof(buffer))) {
            l_no++;
            b_index = 0;
            return add_char(s);
        } else if (al_feof(file)) {
            return false;
        } else if (al_ferror(file)) {
            throw string(al_ferrmsg(file));
        }
    } else {
        if (!regex_match(&buffer[b_index], whitespace)) {
            s->append(&buffer[b_index++]);
            return true;
        } else {
            b_index++;
            return add_char(s);
        }
    }
}
