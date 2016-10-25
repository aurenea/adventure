#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <allegro5/allegro.h>

class Graphics {
private:
    static ALLEGRO_COLOR* tint;
    static float res;

    Graphics() {}
    static ALLEGRO_COLOR blend(ALLEGRO_COLOR*, ALLEGRO_COLOR*);

public:
    static void init();

    static void draw(ALLEGRO_BITMAP*, float, float, int=0, ALLEGRO_COLOR* = NULL, bool = false, float = 1.0, float = 1.0, bool = false);
};

#endif // GRAPHICS_H
