#include <iostream>
#include <fstream>
#include <allegro5/allegro.h>

using namespace std;

int FPS = 30;
int screen_width = 640;
int screen_height = 400;

// UP, DOWN, LEFT, RIGHT, PAUSE
int keymap[5];

int flags = 0;

int get_FPS() {
    return FPS;
}

int get_screen_width() {
    return screen_width;
}

int get_screen_height() {
    return screen_height;
}

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

int get_flag(int f) {
    return ((flags & (1 << f)) != 0);
}

int set_flag(int f) {
    flags |= (1 << f);
}

void default_options() {
    keymap[0] = ALLEGRO_KEY_UP;
    keymap[1] = ALLEGRO_KEY_LEFT;
    keymap[2] = ALLEGRO_KEY_DOWN;
    keymap[3] = ALLEGRO_KEY_RIGHT;
    keymap[4] = ALLEGRO_KEY_SPACE;
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
