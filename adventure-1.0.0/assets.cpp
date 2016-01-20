#include <iostream>
#include <string>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>

using namespace std;

ALLEGRO_BITMAP* sprite_sheets[8];
ALLEGRO_BITMAP* asset_tiles[4];
ALLEGRO_BITMAP* asset_images[169];
// I don't know why, but include an extra pointer on the end. Just go with it, the program crashes if you don't.

ALLEGRO_FONT* font;

void load_assets() {
    sprite_sheets[0] = al_load_bitmap("lib/entities/human.png");
    sprite_sheets[1] = al_load_bitmap("lib/entities/human_overlay.png");
    sprite_sheets[2] = al_load_bitmap("lib/entities/human_hair1.png");
    sprite_sheets[3] = al_load_bitmap("lib/entities/human_hair1_overlay.png");
    sprite_sheets[4] = al_load_bitmap("lib/entities/human_hair2.png");
    sprite_sheets[5] = al_load_bitmap("lib/entities/human_hair2_overlay.png");
    sprite_sheets[6] = al_load_bitmap("lib/entities/human_shirt1.png");
    sprite_sheets[7] = al_load_bitmap("lib/entities/human_pants1.png");

    asset_tiles[0] = al_load_bitmap("lib/terrain/tiles/test1.png");
    asset_tiles[1] = al_load_bitmap("lib/terrain/tiles/test2.png");
    asset_tiles[2] = al_load_bitmap("lib/terrain/tiles/test3.png");
    asset_tiles[3] = al_load_bitmap("lib/terrain/tiles/grass.png");

    asset_images[0] = al_load_bitmap("lib/items/perfectly generic object.png");
    asset_images[1] = al_load_bitmap("lib/windows/new_game.png");
    asset_images[2] = al_load_bitmap("lib/windows/container_window.png");
    asset_images[3] = al_load_bitmap("lib/windows/item_slot.png");
    asset_images[4] = al_load_bitmap("lib/items/barrel.png");
    asset_images[5] = al_load_bitmap("lib/items/grapes.png");
    asset_images[6] = al_load_bitmap("lib/items/wheat.png");
    asset_images[7] = al_load_bitmap("lib/items/knife.png");
    asset_images[8] = al_load_bitmap("lib/items/character creation pillar.png");
    for (int i = 0; i <= 4; i++) {
        for (int j = 0; j < 4; j++) {
            for (int k = 0; k < 8; k++) {
                asset_images[9+(20*k)+(4*i)+j] = al_create_sub_bitmap(sprite_sheets[k],
                    13*((j%2) == 0 ? 0 : (j == 3 ? 2 : j)), 29*i, 13, 29);
            }
        }
    }
    /** LOAD FONTS **/
    // font = al_load_font("c:/Windows/Fonts/calibri.ttf", 12, 0);
    font = al_load_font("lib/font.png", 12, 0);
}

ALLEGRO_BITMAP* get_tile_asset(int key) {
    return asset_tiles[key];
}

ALLEGRO_BITMAP* get_image_asset(int key) {
    return asset_images[key];
}

ALLEGRO_FONT* get_font() {
    return font;
}
