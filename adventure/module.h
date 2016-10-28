#ifndef MODULE_H
#define MODULE_H

#include <string>
#include <allegro5/allegro.h>
#include "loader.h"

class Module {
private:
    unsigned int length;
    unsigned int load;
    FormData** form_data;

    unsigned int add(FormData*);
    unsigned int rehash(unsigned int);
    FormData* get(std::string);

    void load_type(ALLEGRO_FILE*);
    void load_form(ALLEGRO_FILE*);
    Script* load_script(ALLEGRO_FILE*);
public:
    Module();

    FormData* get(unsigned int);

    void load_form_data(ALLEGRO_FS_ENTRY*);
};

#endif // MODULE_H
