#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <time.h>
#include <cmath>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include "assets.h"
#include "file-io.h"
#include "options.h"
#include "input-manager.h"
#include "geometry.h"
#include "interface.h"
#include "objects.h"

using namespace std;

void draw_text(const char*, int, float, float, int, bool = false);
void draw_ustr(ALLEGRO_USTR*, int, float, float, int, bool = false);
void draw_mouse(int);

int update_from_event(ALLEGRO_EVENT);

void load_assets();

void load_objects();


int run_game(ALLEGRO_EVENT_QUEUE* events) {
    cout << "[GAME] Game begun.\n";
    set_center(get_player()->get_position());
    set_main_area(new Area(1));
    get_main_area()->load_area();
    // pc = new Player();
    get_main_area()->add_object(get_player());

    cout << "[GAME] Now drawing...\n";
    draw_at_center();
    al_flip_display();

    bool update = true;
    while (true) {
        ALLEGRO_EVENT e;
        al_wait_for_event(events, &e);
        if (e.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            close_all_windows();
            return 0;
        } else if (e.type == ALLEGRO_EVENT_TIMER) {
            if (!get_paused()) {
                get_player()->update_player(e);
                get_main_area()->update_objects(e);
            }
            if (get_any_key_down()) { evaluate_mouse_text(e); }
            update = true;
        } else {
            update = true;
            switch (update_from_event(e)) {
                case 1: // PAUSE key
                    toggle_pause();
                    break;
                case 2: // ENTER key, interprets text
                    if (get_command_line()) {
                        ALLEGRO_USTR* input = get_input();
                        if (al_ustr_find_cstr(input, 0, "spawn ") >= 0) {
                            al_ustr_find_replace_cstr(input, 0, "spawn ", "");
                            set_dragging(create_object(atoi(&al_cstr(input)[0])),
                                -al_get_bitmap_width(get_dragging()->get_sprite())/2,
                                -al_get_bitmap_height(get_dragging()->get_sprite())/2);
                        } else if (al_ustr_find_cstr(input, 0, "toggle ") >= 0) {
                            if (al_ustr_find_cstr(input, 7, "hm") >= 0) {
                                toggle_hack_mover();
                            }
                        }
                        clear_input();
                    }
                    break;
                case 3: // F1, starts or stops command line
                    toggle_command_line();
                    break;
                case 4: // OTHER
                    if (update_windows(e) == 0) {
                        // do something???
                        get_main_area()->update_objects(e);
                    }
                    break;
                default: // NONE
                    update = false;
            }
            if (e.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP && !is_dragging(NULL)) {
                set_dragging(NULL, 0, 0);
            }
        }
        get_main_area()->resort_objects();
        if (update) {
            update = false;
            draw_at_center();
            draw_windows();
            if (get_command_line()) {
                draw_ustr(get_input(), 0, 10, 10, 0, true);
            }
            draw_mouse(0);
            al_flip_display();
        }
    }
    return 1;
}

int new_game(ALLEGRO_EVENT_QUEUE* events) {
    add_new_input_string();
    srand(time(NULL));

    add_new_input_string();
    // set_input(true);
    int spr = get_sprite_by_id(1);

    int hair_color = 1;
    int hair_style = 0;
    int skin_color = 2;
    int portrait_type = 0;

    int dir = SOUTHWEST;
    int frame = 0;

    bool update = true;
    while (true) {
        ALLEGRO_EVENT e;
        al_wait_for_event(events, &e);
        if (e.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            close_all_windows();
            return 0;
        } else if (e.type == ALLEGRO_EVENT_TIMER) {
            frame = (frame+1)%40;
            if (frame%10 == 0) { update = true; }
        } else if (e.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP) {
            if (e.mouse.y >= 252 && e.mouse.y <= 272) {
                if (e.mouse.x >= 228 && e.mouse.x <= 289) { // CONFIRM button
                    break;
                } else if (e.mouse.x >= 349 && e.mouse.x <= 410) { // CANCEL button
                    return 1;
                }
            } else if (e.mouse.x >= 263 && e.mouse.x <= 269) {
                if (e.mouse.y >= 179 && e.mouse.y <= 187) { // LEFT for HAIR STYLE
                    hair_style = (hair_style+1)%2;
                } else if (e.mouse.y >= 195 && e.mouse.y <= 203) { // LEFT for HAIR COLOR
                    hair_color = (hair_color+5)%6;
                } else if (e.mouse.y >= 211 && e.mouse.y <= 219) { // LEFT for SKIN COLOR
                    skin_color = (skin_color+3)%4;
                }
            } else if (e.mouse.x >= 335 && e.mouse.x <= 341) {
                if (e.mouse.y >= 179 && e.mouse.y <= 187) { // RIGHT for HAIR STYLE
                    hair_style = (hair_style+1)%2;
                } else if (e.mouse.y >= 195 && e.mouse.y <= 203) { // RIGHT for HAIR COLOR
                    hair_color = (hair_color+1)%6;
                } else if (e.mouse.y >= 211 && e.mouse.y <= 219) { // RIGHT for SKIN COLOR
                    skin_color = (skin_color+1)%4;
                }
            } else if (e.mouse.y >= 217 && e.mouse.y <= 226) {
                if (e.mouse.x >= 202 && e.mouse.x <= 213) { // ROTATE CLOCKWISE
                    dir = (dir+7)%8;
                } else if (e.mouse.x >= 232 && e.mouse.x <= 243) { // ROTATE COUNTERCLOCKWISE
                    dir = (dir+1)%8;
                }
            } else if (e.mouse.y >= 195 && e.mouse.y <= 203) {
                if (e.mouse.x >= 376 && e.mouse.x <= 382) { // LEFT for PORTRAIT
                    portrait_type = ((portrait_type+1)%2);
                } else if (e.mouse.x >= 428 && e.mouse.x <= 434) { // RIGHT for PORTRAIT
                    portrait_type = (portrait_type+1)%2;
                }
            }
            if (e.mouse.x >= 191 && e.mouse.x <= 451 && e.mouse.y >= 138 && e.mouse.y <= 160) {
                set_input_string(1);
                // TODO calc where to place cursor
            } else {
                set_input_string(-1);
            }
            update = true;
        } else {
            update = (update_from_event(e) > 0);
        }

        if (update) {
            al_clear_to_color(al_map_rgb(179, 152, 125));
            al_draw_bitmap(get_interface_asset(0), 138, 106, 0);

            al_draw_ustr(get_font(0), al_map_rgb(255, 255, 255), 197, 145, 0, get_input(1));

            //al_draw_bitmap(get_image_asset(8), 200, 200, 0);
            int d = dir;
            d = ((d <= 1 || d >= 7) ? 8-((d+6)%8) : d-2);
            al_draw_tinted_bitmap(get_image_sprite(spr+(frame/10)+(4*d)), *get_skin_pigment(skin_color), 216, 177, (dir <= 1 || dir >= 7) ? ALLEGRO_FLIP_HORIZONTAL : 0);

            al_draw_bitmap(get_image_sprite(get_params_by_id(7)[1]+(frame/10)+(4*d)), 216, 177, (dir <= 1 || dir >= 7) ? ALLEGRO_FLIP_HORIZONTAL : 0);
            al_draw_bitmap(get_image_sprite(get_params_by_id(11)[1]+(frame/10)+(4*d)), 216, 177, (dir <= 1 || dir >= 7) ? ALLEGRO_FLIP_HORIZONTAL : 0);

            al_draw_bitmap(get_image_sprite(spr+20+(frame/10)+(4*d)), 216, 177, (dir <= 1 || dir >= 7) ? ALLEGRO_FLIP_HORIZONTAL : 0);
            al_draw_tinted_bitmap(get_image_sprite(spr+40+(40*hair_style)+(frame/10)+(4*d)), *get_hair_pigment(hair_color), 216, 177, (dir <= 1 || dir >= 7) ? ALLEGRO_FLIP_HORIZONTAL : 0);
            al_draw_bitmap(get_image_sprite(spr+60+(40*hair_style)+(frame/10)+(4*d)), 216, 177, (dir <= 1 || dir >= 7) ? ALLEGRO_FLIP_HORIZONTAL : 0);

            al_draw_bitmap(get_portrait(portrait_type+1), 391, 182, 0);

            draw_mouse(0);

            al_flip_display();
            update = false;
        }
    }

    set_player(new Player(rand() % 100, get_input(1), skin_color, hair_color, 40+(40*hair_style), portrait_type));
    cout << "[GAME] Equipping player...\n";
    get_player()->equip_item(dynamic_cast<Equipment*>(create_object(7)), 1);
    get_player()->equip_item(dynamic_cast<Equipment*>(create_object(11)), 5);

    set_input_string(-1);
    return run_game(events);
}

int load_game(ALLEGRO_EVENT_QUEUE* events) {
    add_new_input_string();
    srand(time(NULL));

    set_player(new Player(rand() % 100, al_ustr_new("Ashe"), 2, 1, 69, 1));
    return run_game(events);
}
