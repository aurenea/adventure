#include <allegro5/allegro.h>
#include "assets.h"
#include "lord.h"
#include "script.h"

#ifndef FORM_H
#define FORM_H

enum FORM_FLAG {
    FORM_UNFIXED = 1,       // can be moved
    OBJECT_WALK = 2,        // (Object) can be walked through
    OBJECT_ON_AXES = 4      // (Object) has multiple sprites for different angles
};

class Form {
private:
    int id;
    int flags;

    Lord<Form*>* lord;
    Point pos;
    Domain dom;

    ScriptLord* scripts;

    int base_sprite;

public:
    Form(int);

    bool get_flag(FORM_FLAG);
    void set_flag(FORM_FLAG, bool);

    Lord<Form*>* get_lord() { return lord; }
    void set_lord(Lord<Form*>*);
    Lord<Form*>* get_highest_lord();
    Point* get_position() { return &pos; }
    void set_position(float x, float y) { pos.set_to(x, y); }
    void set_position(Point*);
    Domain* get_domain() { return &dom; }
    bool get_intersect(Point*);

    virtual ALLEGRO_BITMAP* get_sprite() { return get_image_asset(base_sprite); }
    virtual int get_image_flags() { return 0; }
};

#endif // FORM_H
