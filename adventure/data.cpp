#include <regex>
#include "data.h"

using namespace std;


Parametrized::Parametrized() {
    add_param<shared_ptr<Parametrized> >(0, shared_ptr<Parametrized>(this));
    add_param<shared_ptr<Parametrized> >(1, shared_ptr<Parametrized>(nullptr));
}

template <class T>
void Parametrized::add_param(unsigned int key, T value) {
    shared_ptr<Param> p(new TypedParam<T>(value));
    pair<unsigned int, shared_ptr<Param> > q(key, p);
    params.insert(q);
}

template <>
void Parametrized::set_param<Param*>(unsigned int key, Param* value) {
    unordered_map<unsigned int, shared_ptr<Param> >::iterator iter = params.find(key);
    if (iter == params.end()) {
        add_param(key, value);
    } else {
        iter->second = shared_ptr<Param>(value);
    }
}

template <class T>
void Parametrized::set_param(unsigned int key, T value) {
    set_param(key, static_cast<Param*>(new TypedParam<T>(value)));
}

bool Parametrized::check_param(unsigned int key) {
    unordered_map<unsigned int, shared_ptr<Param> >::iterator iter = params.find(key);
    return (iter != params.end());
}

template <>
shared_ptr<Param> Parametrized::get_param<shared_ptr<Param> >(unsigned int key) {
    unordered_map<unsigned int, shared_ptr<Param> >::iterator iter = params.find(key);
    if (iter == params.end()) {
        if (shared_ptr<TypedParam<shared_ptr<Parametrized> > > inherit =
                static_pointer_cast<TypedParam<shared_ptr<Parametrized> > >(get_param<shared_ptr<Param> >(1))) {
            return inherit->data->get_param<shared_ptr<Param> >(key);
        } else {
            throw string("Attempted to access undefined parameter.");
        }
    } else {
        return iter->second;
    }
}

template <class T>
T Parametrized::get_param(unsigned int key) {
    shared_ptr<Param> value = get_param<shared_ptr<Param> >(key);
    if (value == nullptr) {
        return nullptr;
    } else if (shared_ptr<TypedParam<T> > typed_value = static_pointer_cast<TypedParam<T> >(value)) {
        return typed_value->data;
    } else {
        throw string("Could not cast member to desired type.");
    }
}


Article::Article(Parametrized* p) {
    set_param<shared_ptr<Parametrized> >(1, shared_ptr<Parametrized>(p));
}

template <>
shared_ptr<Param> Article::get_param_chain<shared_ptr<Param> >(vector<unsigned int>* keys) {
    shared_ptr<Param> p = get_param<shared_ptr<Param> >(0);
    for (vector<unsigned int>::iterator iter = keys->begin(); iter < keys->end(); ++iter) {
        if (shared_ptr<TypedParam<shared_ptr<Parametrized> > > tp =
                static_pointer_cast<TypedParam<shared_ptr<Parametrized> > >(p)) {
            p = tp->data->get_param<shared_ptr<Param> >(*iter);
        } else {
            throw string("Tried to access member which does not exist.");
        }
    }
    return p;
}

template <class T>
T Article::get_param_chain(vector<unsigned int>* keys) {
    shared_ptr<Param> value = get_param_chain<shared_ptr<Param> >(keys);
    if (value == nullptr) {
        return nullptr;
    } else if (shared_ptr<TypedParam<T> > typed_value = static_pointer_cast<TypedParam<T> >(value)) {
        return typed_value->data;
    } else {
        throw string("Could not cast member to desired type.");
    }
}
