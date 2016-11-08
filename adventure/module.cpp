#include "module.h"
#include "loader.h"

using namespace std;


shared_ptr<Parametrized> Module::add_data(unsigned int key) {
    shared_ptr<Parametrized> p(new Parametrized());
    pair<unsigned int, shared_ptr<Parametrized> > q(key, p);
    data.insert(q);
    return p;
}

shared_ptr<Parametrized> Module::get_data(unsigned int key) {
    unordered_map<unsigned int, shared_ptr<Parametrized> >::iterator iter = data.find(key);
    if (iter == data.end()) {
        return add_data(key);
    } else {
        return iter->second;
    }
}

void Module::load_modules(vector<ALLEGRO_FS_ENTRY*> files) {
    FileLoader loader(this);
    for (vector<ALLEGRO_FS_ENTRY*>::iterator iter = files.begin(); iter < files.end(); ++iter) {
        ALLEGRO_FILE* file = al_open_fs_entry(*iter, "r");
        loader.load(file);
        // TODO close file
    }
}
