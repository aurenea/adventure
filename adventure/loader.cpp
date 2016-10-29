#include "loader.h"

#include <string>
#include <regex>

using namespace std;

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
}


char FileLoader::get_char() {
    if (buffer[b_index] == '\0' || l_no < 0) {
        if (al_fgets(file, buffer, sizeof(buffer))) {
            l_no++;
            b_index = 0;
            return buffer[b_index++];
        } else if (al_feof(file)) {
            throw 0;
        } else if (al_ferror(file)) {
            throw string(al_ferrmsg(file));
        }
    } else {
        return buffer[b_index++];
    }
}

void FileLoader::load(ALLEGRO_FILE* f) {
    file = f;
    l_no = -1;
    line.clear();
}
