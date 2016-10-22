#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>

#ifndef ASSETS_H
#define ASSETS_H

ALLEGRO_BITMAP* get_tile_asset(int);
ALLEGRO_BITMAP* get_image_asset(int);
ALLEGRO_BITMAP* get_image_sprite(int);
ALLEGRO_BITMAP* get_portrait(int);
ALLEGRO_BITMAP* get_interface_asset(int);
ALLEGRO_FONT* get_font(int);
ALLEGRO_COLOR* get_color(int);
ALLEGRO_COLOR* get_skin_pigment(int);
ALLEGRO_COLOR* get_hair_pigment(int);
ALLEGRO_USTR* get_description(int);

#endif // ASSETS_H
