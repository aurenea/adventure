#include <iostream>
#include <fstream>
#include <allegro5/allegro.h>
#include "input-manager.h"
#include "assets.h"
#include "interface.h"

using namespace std;

int FPS = 30;
int screen_width = 640;
int screen_height = 400;

int get_FPS() {
    return FPS;
}

int get_screen_width() {
    return screen_width;
}

int get_screen_height() {
    return screen_height;
}


// UP, DOWN, LEFT, RIGHT, PAUSE
int keymap[5];
// 0 to not show, 1 to show on mouse-over, 2 to show all the time
int show_action_icon;

int get_function_of_keycode(int keycode) {
    if (keycode == ALLEGRO_KEY_ENTER) { return 5; }
    else if (keycode == ALLEGRO_KEY_F1) { return 6; }
    for (int i = 0; i < 5; i++) {
        if (keycode == keymap[i]) {
            return i;
        }
    }
    return -1;
}

void default_options() {
    keymap[0] = ALLEGRO_KEY_UP;
    keymap[1] = ALLEGRO_KEY_LEFT;
    keymap[2] = ALLEGRO_KEY_DOWN;
    keymap[3] = ALLEGRO_KEY_RIGHT;
    keymap[4] = ALLEGRO_KEY_SPACE;

    show_action_icon = 1;
}

void load_options() {
    ifstream inpt("config.ini");
    if (inpt.is_open()) {
        string line;
        char* s = new char[4];
        int* p;
        int index;
        while (getline(inpt, line)) {
            if (line[0] != '[') {
                if (line.find("KeyUp") >= 0) {
                    p = &keymap[0];
                } else if (line.find("KeyLeft") >= 0) {
                    p = &keymap[1];
                } else if (line.find("KeyDown") >= 0) {
                    p = &keymap[2];
                } else if (line.find("KeyRight") >= 0) {
                    p = &keymap[3];
                } else if (line.find("KeyPause") >= 0) {
                    p = &keymap[4];
                }
                index = line.find_last_of(' ')+1;
                line.copy(s, line.length()-index, index);
                *p = atoi(s);
                memset(&s[0], 0, sizeof(s));
            }
        }
        inpt.close();
    } else {
        default_options();
    }
}

void save_options() {
    ofstream output("config.ini", ios::out | ios::trunc);
    if (output.is_open()) {
        output << "[CONTROLS]\n";
        output << "KeyUp = " << keymap[0] << "\n";
        output << "KeyLeft = " << keymap[1] << "\n";
        output << "KeyDown = " << keymap[2] << "\n";
        output << "KeyRight = " << keymap[3] << "\n";
        output << "KeyPause = " << keymap[4] << "\n";
        output.close();
    }
}



int mode = 0;

int get_paused() {
    return ((mode & 1) != 0);
}

int get_command_line() {
    return ((mode & 2) != 0);
}

int get_hack_mover() {
    return ((mode & 4) != 0);
}

void toggle_pause() {
    if (!get_paused() || get_command_line()) {
        mode |= 1;
        freeze_input();
        set_overlay_color(get_color(1));
    } else {
        mode &= ~1;
        unfreeze_input();
        clear_overlay_color();
    }
}

void toggle_command_line() {
    mode ^= 2;
    toggle_pause();
    set_input_string(get_command_line() ? 0 : -1);
}

void toggle_hack_mover() {
    mode ^= 4;
}
