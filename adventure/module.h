#ifndef MODULE_H
#define MODULE_H

#include <allegro5/allegro.h>
#include <memory>
#include <vector>
#include <unordered_map>
#include "param.h"

class Module: public Parametrized {
private:
    unsigned int min_key; // the minimum key not currently in the module data
    unsigned int max_key; // the maximum key currently in the module data
    std::unordered_map<unsigned int, std::shared_ptr<Parametrized> > data;

    std::shared_ptr<Parametrized> add_data(unsigned int);
    void load_module(ALLEGRO_FS_ENTRY*);
public:
    void add_param(unsigned int, std::shared_ptr<Param>);
    std::shared_ptr<Parametrized> get_data(unsigned int);

    bool check_key(unsigned int);
    unsigned int get_next_key();

    void load_modules(std::vector<ALLEGRO_FS_ENTRY*>);
};

#endif // MODULE_H
