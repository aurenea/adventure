#ifndef WRAPPER_H
#define WRAPPER_H

#include "lord.h"
#include "geometry.h"
#include "area.h"
#include "object.h"

class Wrapper: public Lord<Area*, Direction> {
private:
    Area* current;
    Point* center;
    Direction face;

public:
    Wrapper(Area*);

    bool add(Area*);
    bool add(Area*, Direction);
    bool add(Direction);
    Area* discard(Area*);
    Area* get(Direction);

    bool add(Object*);
    bool add(Object*, Point*);

    Direction get_face() { return face; }
    void c_rotate();
    void cc_rotate();
};

#endif // WRAPPER_H
