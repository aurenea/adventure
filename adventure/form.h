#ifndef FORM_H
#define FORM_H

#include <string>
#include "param.h"
#include "lord.h"
#include "geometry.h"

class Form: public Article {
protected:
    Lord<Form*, Point*>* lord;

    Point pos;
    Shape shape;

public:
    Form() {}
    Form(Parametrized*);

    Point* get_pos() { return &pos; }

    void execute(unsigned int);
};

#endif // FORM_H
