#ifndef LOADER_H
#define LOADER_H

#include <allegro5/allegro.h>
#include <string>
#include <unordered_map>

#include "data.h"
#include "module.h"
#include "script.h"

enum class EndStatement {
    SEMICOLON,
    COMMA,
    OPEN_BRACKET,
    CLOSE_BRACKET,
    END_OF_FILE
};

const int FILE_LOAD_BUFFER_SIZE = 128;

class FileLoader {
private:
    Module* module;
    ALLEGRO_FILE* file;

    std::unordered_map<std::string, unsigned int> keymap;
    unsigned int add_key(std::string);
    unsigned int get_key(std::string);

    std::string line;
    int line_count;
    EndStatement get_line();

    void load_define(Parametrized*);
    void load_params(Parametrized*);
    void load_param_assignment(Parametrized*, unsigned int, std::string);
    void load_scripts(Parametrized*);
    void load_script(Script*);

public:
    FileLoader(Module*);

    void load(ALLEGRO_FILE*);

};

/*
    Script* load_script();
    Script* load_script(std::string);
    BooleanScript* load_boolean_script();
    ConditionalScript* load_conditional_script(bool);
*/

    /*unsigned int load_script(Script*);
    unsigned int load_conditional(Script*);

    void* load_var();

    FormData* load_form();*/


#endif // LOADER_H
