#include <iostream>
#include <string>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>

using namespace std;

ALLEGRO_BITMAP* sprite_sheets[10];
ALLEGRO_BITMAP* asset_sprites[180];
ALLEGRO_BITMAP* asset_tiles[4];
ALLEGRO_BITMAP* asset_images[8];
// I don't know why, but include an extra pointer on the end. Just go with it, the program crashes if you don't.
ALLEGRO_BITMAP* portraits[4];
ALLEGRO_BITMAP* asset_interface[12];

ALLEGRO_FONT* font[4];

ALLEGRO_COLOR black;
ALLEGRO_COLOR dark;
ALLEGRO_COLOR transparent;
ALLEGRO_COLOR invisible;

ALLEGRO_COLOR s_pigment[4];
ALLEGRO_COLOR h_pigment[6];

ALLEGRO_USTR* description[21];

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
    sprite_sheets[9] = al_load_bitmap("lib/entities/bunny.png");
    for (int k = 0; k < 8; k++) {
        for (int i = 0; i <= 4; i++) {
            for (int j = 0; j < 4; j++) {
                // cout << "[ASSETS] <" << (20*k)+(4*i)+j << ">\n";
                asset_sprites[(20*k)+(4*i)+j] = al_create_sub_bitmap(sprite_sheets[k+1],
                    13*((j%2) == 0 ? 0 : (j == 3 ? 2 : j)), 29*i, 13, 29);
            }
        }
    }
    for (int k = 8; k < 9; k++) {
        int wdt = al_get_bitmap_width(sprite_sheets[k+1])/4;
        int hgt = al_get_bitmap_height(sprite_sheets[k+1])/5;
        for (int i = 0; i <= 4; i++) {
            for (int j = 0; j < 4; j++) {
                asset_sprites[(20*k)+(4*i)+j] = al_create_sub_bitmap(sprite_sheets[k+1],
                    wdt*j, hgt*i, wdt, hgt);
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
    font[1] = al_load_font("lib/font2.png", 40, 0);
    font[2] = al_load_font("lib/font3.png", 12, 0);
    font[3] = al_load_font("lib/font4.png", 12, 0);

    /** LOAD COLORS **/
    black = al_map_rgb(0, 0, 0);
    dark = al_map_rgb(128, 128, 128);
    transparent = al_map_rgba(128, 128, 128, 128);
    invisible = al_map_rgba(64, 64, 64, 64);

    s_pigment[0] = al_map_rgb(230, 179, 126);
    s_pigment[1] = al_map_rgb(184, 128, 69);
    s_pigment[2] = al_map_rgb(122, 85, 46);
    s_pigment[3] = al_map_rgb(82, 57, 31);

    h_pigment[0] = al_map_rgb(46, 24, 0); // dark brown
    h_pigment[1] = al_map_rgb(77, 40, 0); // brown
    h_pigment[2] = al_map_rgb(13, 7, 0); // black
    h_pigment[3] = al_map_rgb(191, 156, 48); // blond
    h_pigment[4] = al_map_rgb(191, 64, 0); // red
    h_pigment[5] = al_map_rgb(230, 230, 230); // silver or white

    /** LOAD DESCRIPTIONS **/
    description[0] = al_ustr_new("Health");
    description[1] = al_ustr_new("Fatigue");
    description[2] = al_ustr_new("Hunger");
    description[3] = al_ustr_new("Unarmed: Punching and grappling attacks");
    description[4] = al_ustr_new("Blunt: Maces, flails, and other blunt weapons");
    description[5] = al_ustr_new("Dagger: Daggers and rapiers");
    description[6] = al_ustr_new("Spear: Spears and halberds");
    description[7] = al_ustr_new("Sword: Light and heavy swords");
    description[8] = al_ustr_new("Axe: Handaxes and battleaxes");
    description[9] = al_ustr_new("Ranged: Bows, crossbows, and firearms");
    description[10] = al_ustr_new("Thrown: Slings and thrown weapons");
    description[11] = al_ustr_new("Shields: Blocking and shield bashing");
    description[12] = al_ustr_new("Armor: Proficiency with armor");
    description[13] = al_ustr_new("Smithing: Crafting of new weapons and armor");
    description[14] = al_ustr_new("Negotiation: Better prices with buying and selling");
    description[15] = al_ustr_new("Healing: Ability to treat wounds");
    description[16] = al_ustr_new("Stealth: Hiding and sneaking without alerting others");
    description[17] = al_ustr_new("Pickpocket: Filching items from people's pockets");
    description[18] = al_ustr_new("Lockpicking: Opening locks and disabling traps");
    description[19] = al_ustr_new("Strength: Ability to carry items without encumberance");
    description[20] = al_ustr_new("Athletics: Sprinting and swimming");
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

ALLEGRO_COLOR* get_color(int key) {
    if (key == 0) {
        return &black;
    } else if (key == 1) {
        return &dark;
    } else if (key == 2) {
        return &transparent;
    } else if (key == 3) {
        return &invisible;
    } else {
        return NULL;
    }
}

ALLEGRO_COLOR* get_skin_pigment(int key) {
    return &s_pigment[key];
}

ALLEGRO_COLOR* get_hair_pigment(int key) {
    return &h_pigment[key];
}

ALLEGRO_USTR* get_description(int key) {
    return description[key];
}
