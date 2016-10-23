#include "input.h"

InputController::InputController(Options* o) {
    options = o;
    inputs_held = 0;
}

void InputController::set_input_held(int index, bool b) {
    if (b) {
        inputs_held |= (1 << index);
    } else {
        inputs_held &= ~(1 << index);
    }
}

bool InputController::get_input_held(int index) {
    return ((inputs_held & (1 << index)) != 0);
}

void InputController::update_frame() {
    frames_since_press[0]++;
    frames_since_press[1]++;
}

InputClass InputController::update_event(ALLEGRO_EVENT* e) {
    // do stuff
    switch (e->type) {
    case ALLEGRO_EVENT_MOUSE_AXES:
        if (!frozen && get_input_held((int)KeyBindings::LEFT_MOUSE)) {
            return InputClass::MOUSE_DRAG;
        }
        break;
    case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
        if (!frozen) {
            if (e->mouse.button == 2) {
                set_input_held((int)KeyBindings::RIGHT_MOUSE, true);
                crd_right[0] = e->mouse.x;
                crd_right[1] = e->mouse.y;
                frames_since_press[1] = 0;
            } else {
                set_input_held((int)KeyBindings::LEFT_MOUSE, true);
                crd_left[0] = e->mouse.x;
                crd_left[1] = e->mouse.y;
                frames_since_press[0] = 0;
            }
        }
        break;
    case ALLEGRO_EVENT_MOUSE_BUTTON_UP:
        if (e->mouse.button == 2) {
            set_input_held((int)KeyBindings::RIGHT_MOUSE, false);
            if (!frozen) {
                if (crd_right[0] == e->mouse.x && crd_right[1] == e->mouse.y && options->get_frame_rate() >= 2*frames_since_press[1]) {
                    return InputClass::MOUSE_RIGHT_CLICK;
                }
            }
        } else {
            set_input_held((int)KeyBindings::LEFT_MOUSE, false);
            if (!frozen) {
                if (crd_left[0] == e->mouse.x && crd_left[1] == e->mouse.y && options->get_frame_rate() >= 2*frames_since_press[0]) {
                    return InputClass::MOUSE_LEFT_CLICK;
                } else if (crd_left[0] != e->mouse.x || crd_left[1] != e->mouse.y) {
                    return InputClass::MOUSE_DRAG_RELEASE;
                }
            }
        }
        break;
    case ALLEGRO_EVENT_KEY_DOWN:
        {
            int key_binding = options->get_key_binding(e->keyboard.keycode);
            if (key_binding > -1) {
                inputs_held |= (1 << (key_binding+2));
                return InputClass::KEY_PRESSED;
            }
            break;
        }
    case ALLEGRO_EVENT_KEY_UP:
        {
            int key_binding = options->get_key_binding(e->keyboard.keycode);
            if (key_binding > -1) {
                inputs_held &= ~(1 << (key_binding+2));
                return InputClass::KEY_RELEASED;
            }
            break;
        }
    }
    return InputClass::NONE;
}
