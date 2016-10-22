#include "geometry.h"
#include "object.h"

class Area: protected Lord<Object*> {
private:
    void sort_all(int, int);
public:
    // stuff
    Object* get(Point*);
    int add(Object*);
    Object* discard(Object*);

    void sort_all();
};

class AreaWrapper {
private:
    static Area* vassals[3][3];

    static Point* center;
    static DIRECTION view_direction;

    AreaWrapper();

    static Area* get_area(DIRECTION);

public:
    static void init();

    static DIRECTION get_view() { return AreaWrapper::view_direction; }
    static void set_view(DIRECTION);
    static void crotate_view() { AreaWrapper::set_view(static_cast<DIRECTION>((AreaWrapper::view_direction+6)%8)); }
    static void ccrotate_view() { AreaWrapper::set_view(static_cast<DIRECTION>((AreaWrapper::view_direction+2)%8)); }

    static Point* get_center() { return AreaWrapper::center; }
    static void set_center(Point* p) { center = p; }
};
