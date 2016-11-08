#ifndef AREA_H
#define AREA_H

#include "lord.h"
#include "geometry.h"
#include "object.h"

class Area: public Lord<Object*, Point*> {
private:
    unsigned int first;
    unsigned int last;

    Direction* face;

    void swap_objects(unsigned int, unsigned int);
    unsigned int find_index(Point*, unsigned int, unsigned int);
    void resort_object(unsigned int);
    void resort(unsigned int, unsigned int);
    unsigned int find_index(Point*);

public:
    Area(Direction*);

    bool add(Object*);
    bool add(Object*, Point*);
    Object* discard(Object*);
    Object* get(Point*);
    void resort_all() { resort(0, last-1); }

    void set_face(Direction* d) { face = d; }
};

#endif // AREA_H
