#include "graphics.h"

ALLEGRO_COLOR* Graphics::tint;
float Graphics::res;

ALLEGRO_COLOR Graphics::blend(ALLEGRO_COLOR* c1, ALLEGRO_COLOR* c2) {
    float r1, g1, b1, a1, r2, g2, b2, a2;
    al_unmap_rgba_f(*c1, &r1, &g1, &b1, &a1);
    al_unmap_rgba_f(*c2, &r2, &g2, &b2, &a2);
    return al_map_rgba_f(r1*r2, g1*g2, b1*b2, a1*a2);
}

void Graphics::draw(ALLEGRO_BITMAP* bmp, float x, float y, int flags,
                   ALLEGRO_COLOR* tint, bool tint_override,
                   float x_scale, float y_scale, bool res_override) {
    if (!tint_override && Graphics::tint != NULL) {
        if (tint != NULL) {
            ALLEGRO_COLOR color = Graphics::blend(Graphics::tint, tint);
            Graphics::draw(bmp, x, y, flags, &color, true, x_scale, y_scale, res_override);
            delete &color;
        } else {
            Graphics::draw(bmp, x, y, flags, Graphics::tint, true, x_scale, y_scale, res_override);
        }
    } else if (!res_override && Graphics::res != 1.0) {
        Graphics::draw(bmp, x, y, flags, tint, tint_override, x_scale*Graphics::res, y_scale*Graphics::res, true);
    } else {
        if (x_scale != 1.0 || y_scale != 1.0) {
            int w = al_get_bitmap_width(bmp);
            int h = al_get_bitmap_height(bmp);
            if (tint != NULL) {
                al_draw_tinted_scaled_bitmap(bmp, *tint, 0, 0, w, h, x, y, w*x_scale, h*y_scale, flags);
            } else {
                al_draw_scaled_bitmap(bmp, 0, 0, w, h, x, y, w*x_scale, h*y_scale, flags);
            }
        } else {
            if (tint != NULL) {
                al_draw_tinted_bitmap(bmp, *tint, x, y, flags);
            } else {
                al_draw_bitmap(bmp, x, y, flags);
            }
        }
    }
}

void Graphics::init() {
    Graphics::tint = NULL;
    Graphics::res = 1.0;
}
