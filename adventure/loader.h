#ifndef LOADER_H
#define LOADER_H

#include <allegro5/allegro.h>
#include <string>
#include "script.h"

struct FormData {
    unsigned int hashID;
    std::string formID;
    int classID;
    int spriteID;

    std::string name;
    std::string desc;
    int flags;

    int* params;
};

const int FILE_LOAD_BUFFER_SIZE = 128;

class FileLoader {
private:
    ALLEGRO_FILE* file;

    char buffer[FILE_LOAD_BUFFER_SIZE];
    int b_index;
    int l_no;

    char* get_char();

    Script* load_script();
    Script* load_script(std::string);
    BooleanScript* load_boolean_script();
    ConditionalScript* load_conditional_script(bool);

    FormData* load_form();

public:
    void load(ALLEGRO_FILE*);
};

#endif // LOADER_H
