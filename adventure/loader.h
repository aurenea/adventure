#ifndef LOADER_H
#define LOADER_H

#include <allegro5/allegro.h>
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

FormData* load_form(ALLEGRO_FILE*, char*);
Script* load_script(ALLEGRO_FILE*, char*);

#endif // LOADER_H
