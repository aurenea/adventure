#include <allegro5/allegro.h>
#include "options.h"

unsigned short int Options::FRAME_RATE;

Options::Options() {
    default_options();
}

Options::~Options() {
    // TODO delete objects
}

void Options::default_options() {
    Options::FRAME_RATE = 30;

    for (int i = 0; i < 30; i++) {
        key_bindings[i] = -1;
    }
    key_bindings[(int)KeyBindings::ENTER] = ALLEGRO_KEY_ENTER;
    key_bindings[(int)KeyBindings::LEFT] = ALLEGRO_KEY_LEFT;
    key_bindings[(int)KeyBindings::RIGHT] = ALLEGRO_KEY_RIGHT;
    key_bindings[(int)KeyBindings::UP] = ALLEGRO_KEY_UP;
    key_bindings[(int)KeyBindings::DOWN] = ALLEGRO_KEY_DOWN;
}

/**
 *  Gets the function bound to the keycode passed,
 *      -1 if no such binding exists
 */
int Options::get_key_binding(int keycode) {
    for (int i = 0; i < 30; i++) {
        if (key_bindings[i] == keycode) {
            return i;
        }
    }
    return -1;
}

int Options::get_frame_rate() {
    return Options::FRAME_RATE;
}


ModuleOptions::ModuleOptions(Module* m) {
    mod = m;
}

ModuleOptions::~ModuleOptions() {
    delete mod;
}
