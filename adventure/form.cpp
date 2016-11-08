#include <memory>
#include "form.h"
#include "script.h"

Form::Form(Parametrized* p) : Article(p) {
    shape.center = &pos;
}

void Form::execute(unsigned int key) {
    std::shared_ptr<Script> s = nullptr;// this->get_param<std::shared_ptr<Script> >(key);
    //get_param<std::shared_ptr<Script> >(key);
    if (s != nullptr) {
        s->execute();
    }
}
