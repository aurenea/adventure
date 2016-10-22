#include "input.h"

void InputHandler::init() {
    InputHandler::frozen = false;
    InputHandler::mouse_pressed = false;
    InputHandler::keys_pressed = 0;
}

void InputHandler::update(ALLEGRO_EVENT* e, State* s) {
    switch (e->type) {
    case ALLEGRO_EVENT_KEY_DOWN:
        if (!InputHandler::frozen) {
            // do stuff
        }
        break;
    case ALLEGRO_EVENT_KEY_UP:
        KeyHandler key = InputHandler::get_keycode(e->keyboard.keycode);
        if (key > KeyHandler::NONE) {
            InputHandler::keys_pressed &= ~(1 << key);
        }
        if (!InputHandler::frozen) {
            // do stuff
        }
        break;
    case ALLEGRO_EVENT_MOUSE_AXES:
        InputHandler::mouse_x = e->mouse.x;
        InputHandler::mouse_y = e->mouse.y;
        break;
    case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
        if (!InputHandler::frozen) {
            InputHandler::mouse_pressed = true;
        }
        break;
    case ALLEGRO_EVENT_MOUSE_BUTTON_UP:
        InputHandler::mouse_pressed = false;
        break;
    }
}

KeyHandler InputHandler::get_keycode(int keycode) {
    if (keycode == ALLEGRO_KEY_ENTER) {
        return KeyHandler::ENTER;
    } else if (keycode == ALLEGRO_KEY_F1) {
        return KeyHandler::DEBUG;
    } else {
        for (int i = 0; i < 16; i++) {
            if (keycode == InputHandler::keymap[i]) {
                return static_cast<KeyHandler>(i);
            }
        }
        return KeyHandler::NONE;
    }
}
