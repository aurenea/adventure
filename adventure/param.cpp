#include "param.h"

using namespace std;

Parametrized::Parametrized() {
    set_param<shared_ptr<Parametrized> >((unsigned int)ParamSpecific::INHERIT, nullptr);
    inherit = nullptr;
}

void Parametrized::add_param(unsigned int key, shared_ptr<Param> value) {
    pair<unsigned int, shared_ptr<Param> > p(key, value);
    params.insert(p);
}

template <>
void Parametrized::set_param<Param*>(unsigned int key, Param* value) {
    shared_ptr<Param> ptr(value);
    unordered_map<unsigned int, shared_ptr<Param> >::iterator it = params.find(key);
    if (it == params.end()) {
        add_param(key, ptr);
    } else {
        if (key == (unsigned int)ParamSpecific::INHERIT) {
            if (TypedParam<shared_ptr<Parametrized> >* inh =
                    dynamic_cast<TypedParam<shared_ptr<Parametrized> >*>(value)) {
                inherit = (inh->data).get();
            } else {
                throw string("Inherit must be of parametrized type.");
            }
        }
        it->second = ptr;
    }
}

template <class T>
void Parametrized::set_param(unsigned int key, T value) {
    set_param<Param*>(key, new TypedParam<T>(value));
}

template <>
Param* Parametrized::get_param<Param*>(unsigned int key) {
    unordered_map<unsigned int, shared_ptr<Param> >::iterator it = params.find(key);
    if (it == params.end()) {
        if (inherit != nullptr) {
            return inherit->get_param<Param*>(key);
        } else {
            throw string("Attempted to access undefined parameter.");
        }
    } else {
        return (it->second).get();
    }
}

template <class T>
T Parametrized::get_param(unsigned int key) {
    Param* p = get_param<Param*>(key);
    if (TypedParam<T>* typed = dynamic_cast<TypedParam<T>*>(p)) {
        return typed->data;
    } else if (p == nullptr) {
        return nullptr;
    } else {
        throw string("Member could not be casted to requested type.");
    }
}

bool Parametrized::check_param(unsigned int key) {
    if (params.find(key) != params.end()) {
        return true;
    } else {
        if (inherit != nullptr) {
            return inherit->check_param(key);
        } else {
            return false;
        }
    }
}


Article::Article(Parametrized* p) {
    set_param((unsigned int)ParamSpecific::INHERIT, shared_ptr<Parametrized>(p));
}

template <>
Param* Article::get_param_chain<Param*>(vector<unsigned int>* keys) {
    Param* cur = nullptr;
    for (vector<unsigned int>::iterator it = keys->begin(); it < keys->end(); ++it) {
        if (it == keys->begin()) {
            cur = get_param<Param*>(*it);
        } else if (TypedParam<shared_ptr<Parametrized> >* typed = static_cast<TypedParam<shared_ptr<Parametrized> >*>(cur)) {
            cur = typed->data->get_param<Param*>(*it);
        } else {
            throw string("Attempted to access member which does not exist.");
        }
    }
    return cur;
}

template <class T>
T Article::get_param_chain(vector<unsigned int>* keys) {
    Param* p = get_param_chain<Param*>(keys);
    if (TypedParam<T>* typed = dynamic_cast<TypedParam<T>*>(p)) {
        return typed->data;
    } else {
        throw string("Member could not be casted to requested type.");
    }
}
