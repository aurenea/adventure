#include <allegro5/allegro.h>
#include "assets.h"
#include "form.h"

class Object: public Form {
protected:
    Point mov;

    ALLEGRO_USTR* name;

    Object(int);
};

class Item: public Object {
protected:
    int ui_sprite;
public:
    ALLEGRO_BITMAP* get_ui_sprite() { return get_image_asset(ui_sprite); }
};

class Container: public Item, public Lord<Item*> {
public:

};
