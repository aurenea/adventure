#include "object.h"

Object::Object(int id) : Form(id) {
    name = al_ustr_new(get_name_by_id(id));
}
