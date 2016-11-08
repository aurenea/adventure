#ifndef LOADER_H
#define LOADER_H

#include <allegro5/allegro.h>
#include <string>
#include <unordered_map>
// #include "script.h"
#include "data.h"
#include "module.h"

const int FILE_LOAD_BUFFER_SIZE = 128;

class FileLoader {
private:
    Module* module;
    ALLEGRO_FILE* file;

    std::unordered_map<std::string, unsigned int> keymap;
    // unordered_map<std::string, void*> global_vars;
    unsigned int add_key(std::string);
    unsigned int get_key(std::string);
    void* get_value(std::string);

    char buffer[FILE_LOAD_BUFFER_SIZE];
    int b_index;
    int l_no;

    bool add_char(std::string*);
    char* get_char();

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
