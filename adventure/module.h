#ifndef MODULE_H
#define MODULE_H

#include <string>
#include <allegro5/allegro.h>

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

class Module {
private:
    unsigned int length;
    unsigned int load;
    FormData** form_data;

    void add(FormData*);
    void rehash();
    FormData* get(std::string);
public:
    Module();

    FormData* get(unsigned int);

    void load_module(ALLEGRO_FS_ENTRY*);
};

#endif // MODULE_H
