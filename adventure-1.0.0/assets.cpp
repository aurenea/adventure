#include <iostream>
#include <string>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>

using namespace std;

ALLEGRO_BITMAP* sprite_sheets[9];
ALLEGRO_BITMAP* asset_sprites[160];
ALLEGRO_BITMAP* asset_tiles[4];
ALLEGRO_BITMAP* asset_images[8];
// I don't know why, but include an extra pointer on the end. Just go with it, the program crashes if you don't.
ALLEGRO_BITMAP* portraits[4];
ALLEGRO_BITMAP* asset_interface[12];

ALLEGRO_FONT* font[4];

void load_assets() {
    cout << "[ASSETS] Loading assets...\n";

    sprite_sheets[0] = al_load_bitmap("lib/entities/portraits.png");
    sprite_sheets[1] = al_load_bitmap("lib/entities/human.png");
    sprite_sheets[2] = al_load_bitmap("lib/entities/human_overlay.png");
    sprite_sheets[3] = al_load_bitmap("lib/entities/human_hair1.png");
    sprite_sheets[4] = al_load_bitmap("lib/entities/human_hair1_overlay.png");
    sprite_sheets[5] = al_load_bitmap("lib/entities/human_hair2.png");
    sprite_sheets[6] = al_load_bitmap("lib/entities/human_hair2_overlay.png");
    sprite_sheets[7] = al_load_bitmap("lib/entities/human_shirt1.png");
    sprite_sheets[8] = al_load_bitmap("lib/entities/human_pants1.png");
    for (int i = 0; i <= 4; i++) {
        for (int j = 0; j < 4; j++) {
            for (int k = 0; k < 8; k++) {
                // cout << "[ASSETS] <" << (20*k)+(4*i)+j << ">\n";
                asset_sprites[(20*k)+(4*i)+j] = al_create_sub_bitmap(sprite_sheets[k+1],
                    13*((j%2) == 0 ? 0 : (j == 3 ? 2 : j)), 29*i, 13, 29);
            }
        }
    }

    asset_tiles[0] = al_load_bitmap("lib/terrain/tiles/test1.png");
    asset_tiles[1] = al_load_bitmap("lib/terrain/tiles/test2.png");
    asset_tiles[2] = al_load_bitmap("lib/terrain/tiles/test3.png");
    asset_tiles[3] = al_load_bitmap("lib/terrain/tiles/grass.png");

    asset_images[0] = al_load_bitmap("lib/items/perfectly generic object.png");
    asset_images[1] = al_load_bitmap("lib/items/barrel.png");
    asset_images[2] = al_load_bitmap("lib/items/grapes.png");
    asset_images[3] = al_load_bitmap("lib/items/wheat.png");
    asset_images[4] = al_load_bitmap("lib/items/knife.png");
    asset_images[5] = al_load_bitmap("lib/items/iron key.png");
    asset_images[6] = al_load_bitmap("lib/items/shirt.png");
    asset_images[7] = al_load_bitmap("lib/items/pants.png");
    /** LOAD PORTRAITS **/
    portraits[0] = al_load_bitmap("lib/entities/portrait_frame.png");
    int w = (al_get_bitmap_width(sprite_sheets[0])-1)/30;
    int h = (al_get_bitmap_height(sprite_sheets[0])-1)/36;
    for (int i = 0; i < w; i++) {
        for (int j = 0; j < h; j++) {
            portraits[1+(i*h)+j] = al_create_sub_bitmap(sprite_sheets[0], 1+(30*i), 1+(36*j), 29, 35);
        }
    }
    /** LOAD INTERFACE ASSETS **/
    asset_interface[0] = al_load_bitmap("lib/windows/new_game.png");
    asset_interface[1] = al_load_bitmap("lib/windows/container_window.png");
    asset_interface[2] = al_load_bitmap("lib/windows/item_slot.png");
    asset_interface[3] = al_load_bitmap("lib/windows/character_window.png");
    asset_interface[4] = al_load_bitmap("lib/windows/book.png");
    asset_interface[5] = al_load_bitmap("lib/windows/book_prev.png");
    asset_interface[6] = al_load_bitmap("lib/windows/book_next.png");
    asset_interface[7] = al_load_bitmap("lib/windows/health_meter.png");
    asset_interface[8] = al_load_bitmap("lib/windows/fatigue_meter.png");
    asset_interface[9] = al_load_bitmap("lib/windows/food_meter.png");
    asset_interface[10] = al_load_bitmap("lib/windows/skills_button.png");
    asset_interface[11] = al_load_bitmap("lib/windows/skills_window.png");
    /** LOAD FONTS **/
    // font = al_load_font("c:/Windows/Fonts/calibri.ttf", 12, 0);
    font[0] = al_load_font("lib/font1.png", 12, 0);
    font[1] = al_load_font("lib/font2.png", 12, 0);
    font[2] = al_load_font("lib/font3.png", 12, 0);
    font[3] = al_load_font("lib/font4.png", 12, 0);
}

ALLEGRO_BITMAP* get_tile_asset(int key) {
    return asset_tiles[key];
}

ALLEGRO_BITMAP* get_image_asset(int key) {
    return asset_images[key];
}

ALLEGRO_BITMAP* get_image_sprite(int key) {
    return asset_sprites[key];
}

ALLEGRO_BITMAP* get_portrait(int key) {
    return portraits[key];
}

ALLEGRO_BITMAP* get_interface_asset(int key) {
    return asset_interface[key];
}

ALLEGRO_FONT* get_font(int key) {
    return font[key];
}
