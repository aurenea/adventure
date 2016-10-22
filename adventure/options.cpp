#include <iostream>
#include <fstream>
#include <allegro5/allegro.h>
#include "input-manager.h"
#include "assets.h"
#include "interface.h"

using namespace std;

void Options::init() {
    if (!Options::load_options()) {
        Options::default_options();
        Options::save_options();
    }
}

void Options::load_options() {
    ifstream inpt("config.ini");
    if (inpt.is_open()) {
        string line;
        char* s = new char[4];
        int* p;
        int index;
        while (getline(inpt, line)) {
            if (line[0] != '[' && line[0] != '\0') {
                if (line.find("KeyUp") >= 0) {
                    p = &keymap[COMMAND_FUNCTION::UP];
                } else if (line.find("KeyLeft") >= 0) {
                    p = &keymap[COMMAND_FUNCTION::LEFT];
                } else if (line.find("KeyDown") >= 0) {
                    p = &keymap[COMMAND_FUNCTION::DOWN];
                } else if (line.find("KeyRight") >= 0) {
                    p = &keymap[COMMAND_FUNCTION::RIGHT];
                } else if (line.find("KeyPause") >= 0) {
                    p = &keymap[COMMAND_FUNCTION::PAUSE];
                }
                index = line.find_last_of(' ')+1;
                line.copy(s, line.length()-index, index);
                *p = atoi(s);
                memset(&s[0], 0, sizeof(s));
            }
        }
        inpt.close();
        return true;
    }
    return false;
}

void Options::default_options() {
    Options::screen_width = 640;
    Options::screen_height = 400;
    Options::resolution = 1.0;

    Options::active_flags = 0;

    Options::keymap[COMMAND_FUNCTION::UP] = ALLEGRO_KEY_UP;
    Options::keymap[COMMAND_FUNCTION::DOWN] = ALLEGRO_KEY_DOWN;
    Options::keymap[COMMAND_FUNCTION::LEFT] = ALLEGRO_KEY_LEFT;
    Options::keymap[COMMAND_FUNCTION::RIGHT] = ALLEGRO_KEY_RIGHT;
    Options::keymap[COMMAND_FUNCTION::PAUSE] = ALLEGRO_KEY_SPACE;

}

void Options::save_options() {
    ofstream output("config.ini", ios::out | ios::trunc);
    if (output.is_open()) {
        output << "[CONTROLS]\n";
        output << "KeyUp = " << keymap[COMMAND_FUNCTION::UP] << "\n";
        output << "KeyLeft = " << keymap[COMMAND_FUNCTION::LEFT] << "\n";
        output << "KeyDown = " << keymap[COMMAND_FUNCTION::DOWN] << "\n";
        output << "KeyRight = " << keymap[COMMAND_FUNCTION::RIGHT] << "\n";
        output << "KeyPause = " << keymap[COMMAND_FUNCTION::PAUSE] << "\n";
        output.close();
    }
}

void Options::set_dimensions(int w, int h) {
    Options::screen_width = w;
    Options::screen_height = h;
}

float Options::get_scale() {
    return Options::resolution*(Options::get_flag(OPTION_FLAG::DOUBLE_ZOOM) ? 2 : 1);
}

COMMAND_FUNCTION Options::get_function_of_keycode(int keycode) {
    if (keycode == ALLEGRO_KEY_ENTER) { return COMMAND_FUNCTION::ENTER; }
    else if (keycode == ALLEGRO_KEY_F1) { return COMMAND_FUNCTION::DEBUG; }
    for (int i = 0; i < 5; i++) {
        if (keycode == keymap[i]) {
            return static_cast<COMMAND_FUNCTION>(i);
        }
    }
    return COMMAND_FUNCTION::NONE;
}

void Options::execute_cf(COMMAND_FUNCTION cf, State* s) {
    if (GameState* gs = dynamic_cast<GameState*>(s))
        switch (cf) {
        case COMMAND_FUNCTION::PAUSE:
            Options::toggle_flag(OPTION_FLAG::PAUSE);
            break;
        case COMMAND_FUNCTION::MENU:
            // pull up Options menu
            break;
        case COMMAND_FUNCTION::WAIT:
            // pull up Wait menu
            break;
        case COMMAND_FUNCTION::REST:
            // pull up Rest menu
            break;
        case COMMAND_FUNCTION::CAMP:
            // Camp??
            break;
        case COMMAND_FUNCTION::COMBAT:
            Options::toggle_flag(OPTION_FLAG::COMBAT);
            break;
        case COMMAND_FUNCTION::C_ROTATE:
            AreaWrapper::crotate_view();
            break;
        case COMMAND_FUNCTION::CC_ROTATE:
            AreaWrapper::ccrotate_view();
            break;
        case COMMAND_FUNCTION::CHARACTER:
            // pull up Character window
            break;
        case COMMAND_FUNCTION::SKILLS:
            // pull up Skills window
            break;
        case COMMAND_FUNCTION::INVENTORY:
            // pull up Inventory window for all equipped bags
            break;
        case COMMAND_FUNCTION::JOURNAL:
            // pull up Journal window
            break;
        case COMMAND_FUNCTION::MAP:
            // pull up Map window
            break;
        case COMMAND_FUNCTION::DEBUG:
            // pull up Debug window
            break;
        }
    } else if (cf == COMMAND_FUNCTION::ENTER) {
        // send Enter command to Interface
        break;
    }
}

bool Options::get_flag(OPTION_FLAG f) {
    return (bool)(active_flags & f);
}

void Options::toggle_flag(OPTION_FLAG f) {
    active_flags ^= f;
}

void Options::set_flag(OPTION_FLAG f, bool b) {
    if (b) { active_flags |= f; }
    else { active_flags &= ~f; }
}

bool Options::show_action_icon(bool mouseover) {
    switch (Options::show_action_icons) {
    case SHOW_ACTION_ICONS::NEVER_SHOW:
        return false;
    case SHOW_ACTION_ICONS::SHOW_IN_PAUSE_WITH_MOUSEOVER:
        return (mouseover && Options::get_flag(OPTION_FLAG::PAUSE));
    case SHOW_ACTION_ICONS::SHOW_IN_PAUSE:
        return Options::get_flag(OPTION_FLAG::PAUSE);
    case SHOW_ACTION_ICONS::SHOW_WITH_MOUSEOVER:
        return mouseover;
    case SHOW_ACTION_ICONS::ALWAYS_SHOW:
        return true;
    }
}
