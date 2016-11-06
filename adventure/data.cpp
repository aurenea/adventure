#include <regex>
#include "data.h"

using namespace std;

void Parametrized::add_param(string key, void* value) {
    params.insert( {{key, value}} );
}

void Parametrized::set_param(string key, void* value) {
    unordered_map<string, void*>::iterator iter = params.find(key);
    if (iter == params.end()) {
        add_param(key, value);
    } else {
        iter->second = value;
    }
}

void* Parametrized::get_param(string key) {
    unordered_map<string, void*>::iterator iter = params.find(key);
    if (iter == params.end()) {
        if (inherit == NULL) {
            throw string("Attempted to access undefined parameter ").append(key).append(".");
        } else {
            return inherit->get_param(key);
        }
    } else {
        return iter->second;
    }
}

/*
void FormData::execute(string key) {
    if (Script* value = dynamic_cast<Script*>(get_param(key))) {
        value->execute();
    }
}

Form* FormData::create() {
    switch (underlying) {
        case UnderlyingClassData::MENU:
            return new Menu(this);
        case UnderlyingClassData::WIDGET:
            return new Widget(this);
        case UnderlyingClassData::ITEM:
            return new Item(this);
        case UnderlyingClassData::CONTAINER:
            return new Container(this);
        case UnderlyingClassData::ENTITY:
            return new Entity(this);
        case UnderlyingClassData::ATTIRED:
            return new Attired(this);
        default:
            throw string("FormData does not have proper underlying class type.");
    }
}
*/

void* Article::get_param(string key) {
    smatch match;
    if (regex_match(key, match, regex("(.*)\.(.*)"))) {
        void* first = get_param(match.str(1));
        if (Parametrized* prmtrs = static_cast<Parametrized*>(first)) {
            return prmtrs->get_param(match.str(2));
        } else {
            throw string(match.str(1)).append(" has no parameter ").append(match.str(2)).append(".");
        }
    } else {
        return Parametrized::get_param(key);
    }
}
