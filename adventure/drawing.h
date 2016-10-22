#include <allegro5/allegro.h>
#include "wrapper.h"

enum DRAW_TYPE {
    DRAW_AREA = 1,
    DRAW_INTERFACE = 2,
    DRAW_DRAGGER = 4
};

enum COLOR_TYPE : ALLEGRO_COLOR {
    COLOR_WHITE,
    COLOR_BLACK,
    COLOR_DARK,
    COLOR_TRANSPARENT,
    COLOR_INVISIBLE
};

class Drawing {
private:
    static ALLEGRO_COLOR* overlay;

    Drawing() {}

    static ALLEGRO_COLOR* blend_colors(ALLEGRO_COLOR*, ALLEGRO_COLOR*);

    static void draw_bitmap(ALLEGRO_BITMAP*, float, float, int=0, bool=true);
    static void draw_tinted_bitmap(ALLEGRO_BITMAP*, ALLEGRO_COLOR*, float, float, int=0, bool=true);

    template <class T> static void draw_form(T*, float, float, DRAW_TYPE);

    static void draw_area_wrapper();
    static void draw_interface();
    static void draw_mouse();

public:
    static void init();

    static void set_overlay(ALLEGRO_COLOR* c) { overlay = c; }

    static void draw();
};
