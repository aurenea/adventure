#ifndef MODULE_H
#define MODULE_H

#include <allegro5/allegro.h>
#include <memory>
#include <vector>
#include <unordered_map>
#include "data.h"

class Module {
private:
    std::unordered_map<unsigned int, std::shared_ptr<Parametrized> > data;

    std::shared_ptr<Parametrized> add_data(unsigned int);
public:
    std::shared_ptr<Parametrized> get_data(unsigned int);

    void load_modules(std::vector<ALLEGRO_FS_ENTRY*>);
};

#endif // MODULE_H
