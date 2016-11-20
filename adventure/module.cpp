#include "module.h"
#include "loader.h"


void Module::add_param(unsigned int key, std::shared_ptr<Param> value) {
    if (key == min_key) {
        if (min_key > max_key) { max_key = min_key; }
        for (unsigned int i = min_key; i <= max_key; i++) {
            if (!check_param(i) && data.find(i) == data.end()) {
                min_key = i;
                break;
            }
        }
        if (key == min_key) { min_key = max_key+1; }
    } else if (key > max_key) {
        max_key = key;
    }
    Parametrized::add_param(key, value);
}

std::shared_ptr<Parametrized> Module::add_data(unsigned int key) {
    std::shared_ptr<Parametrized> p(new Parametrized());
    pair<unsigned int, std::shared_ptr<Parametrized> > q(key, p);
    data.insert(q);
    return p;
}

std::shared_ptr<Parametrized> Module::get_data(unsigned int key) {
    std::unordered_map<unsigned int, std::shared_ptr<Parametrized> >::iterator iter = data.find(key);
    if (iter == data.end()) {
        return add_data(key);
    } else {
        return iter->second;
    }
}

bool Module::check_key(unsigned int key) {
    if (key < min_key) {
        return true;
    } else if (key > max_key) {
        return false;
    }
    return (check_param(key) || data.find(key) != data.end());
}

unsigned int Module::get_next_key() {
    unsigned int former = min_key;
    for (unsigned int i = min_key+1; i <= max_key; i++) {
        if (!check_key(i)) {
            min_key = i;
            break;
        }
    }
    if (min_key == former) { min_key = max_key+1; }
    return former;
}

void Module::load_module(ALLEGRO_FS_ENTRY* fs_entry) {
    ALLEGRO_FILE* f = al_open_fs_entry(fs_entry, "r");
    FileLoader(this, f);
    // TODO close file
}

void Module::load_modules(vector<ALLEGRO_FS_ENTRY*> files) {
    for (std::vector<ALLEGRO_FS_ENTRY*>::iterator iter = files.begin(); iter < files.end(); ++iter) {
        load_module(*iter);
    }
}
