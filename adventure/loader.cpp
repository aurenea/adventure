#include "loader.h"

#include <string>
#include <regex>

using namespace std;

Script* load_script(ALLEGRO_FILE*, char*, string);

BooleanScript* load_boolean_script(ALLEGRO_FILE* file, char* buffer) {
    string input;
    while (al_fread(file, buffer, sizeof(buffer)) == sizeof(buffer)) {
        if (!regex_match(buffer, regex("\s"))) {
            input.append(buffer);
            if (regex_match(input, regex("\("))) {


            }
        }
    }
}

ConditionalScript* load_conditional_script(ALLEGRO_FILE* file, char* buffer, bool after) {
    BooleanScript* b = load_boolean_script(file, buffer);
    Script* t = NULL;
    Script* f = NULL;
    string input;
    while (al_fread(file, buffer, sizeof(buffer)) == sizeof(buffer)) {
        if (regex_match(buffer, regex("[^elsif\{\(\s]"))) {
            return new ConditionalScript((after ? load_script(file, buffer, input) : NULL), b, t, f);
        }
        if (!regex_match(buffer, regex("\s"))) {
            input.append(buffer);
            if (regex_match(input, regex("^elseif\("))) {
                f = load_conditional_script(file, buffer, false);
                return new ConditionalScript((after ? load_script(file, buffer) : NULL), b, t, f);
            } else if (regex_match(input, regex("^else\{"))) {
                f = load_script(file, buffer);
                return new ConditionalScript(load_script(file, buffer), b, t, f);
            } else if (regex_match(input, regex("^\{"))) {
                t = load_script(file, buffer);
                input.clear();
            }
        }
    }
}

Script* load_script(ALLEGRO_FILE* file, char* buffer, string input) {
    while (al_fread(file, buffer, sizeof(buffer)) == sizeof(buffer)) {
        if (!regex_match(buffer, regex("\s"))) {
            input.append(buffer);
            if (regex_match(input, regex("\bif\("))) {
                return load_conditional_script(file, buffer, true);
            }
        }
    }
}

Script* load_script(ALLEGRO_FILE* file, char* buffer) {
    load_script(file, buffer, string(""));
}
