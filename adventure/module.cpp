#include <cmath>
#include "module.h"

unsigned int data_hash(std::string formID, unsigned int value) {
    unsigned int hashing = 0;
    for (int i = 0; i < formID.length(); i++) {
        hashing = (hashing + (int)((1 + (pow(value, 0.25)*i) + (pow(value, 0.33)*pow(i, 2)) + (pow(value, 0.5)*pow(i, 3))) *
                                   pow((int)(formID[i]), 2))) % value;
    }
    return hashing;
}


Module::Module() {
    form_data = new FormData*[10]();
}

unsigned int Module::add(FormData* data) {
    unsigned int index = data_hash(data->formID, length);
    while (form_data[index] != NULL) {
        index = (index+1)%length;
    }
    form_data[index] = data;
    load++;
    if (load > (unsigned int)(0.75*length)) {
        return rehash(index);
    } else {
        return index;
    }
}

unsigned int Module::rehash(unsigned int idx) {
    unsigned int return_value;
    unsigned int new_length = (unsigned int)(length * 1.75);
    FormData** new_data = new FormData*[new_length]();
    for (unsigned int i = 0; i < length; i++) {
        unsigned int index = data_hash(form_data[i]->formID, new_length);
        while (index != NULL) {
            index = (index+1)%new_length;
        }
        new_data[index] = form_data[i];
        new_data[index]->hashID = index;
        if (i == idx) { return_value = index; }
    }
    // reassign hash pointers
    delete form_data;
    form_data = new_data;
    return return_value;
}

FormData* Module::get(std::string formID) {
    unsigned int index = data_hash(formID, length);
    unsigned int number = 0;
    while (number < length && form_data[index] != NULL && form_data[index]->formID.compare(formID) != 0) {
        index = (index + 1)%length;
        number++;
    }
    if (form_data[index]->formID.compare(formID) == 0) {
        return form_data[index];
    } else {
        return NULL;
    }
}

FormData* Module::get(unsigned int index) {
    return form_data[index];
}

void Module::load_form_data(ALLEGRO_FS_ENTRY* filepath) {
    ALLEGRO_FILE* file = al_open_fs_entry(filepath, "r");
    char buffer[128];

}
