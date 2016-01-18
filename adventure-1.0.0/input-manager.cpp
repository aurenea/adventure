#include <iostream>
#include <allegro5/allegro.h>

int get_function_of_keycode(int);

bool mouse_pressed = false;

int mouse_x;
int mouse_y;

int key_down = 0;

bool take_input_string = false;
int cursor = 0;
ALLEGRO_USTR* input = al_ustr_new("");

bool get_mouse_pressed() {
    return mouse_pressed;
}

int get_mouse_x() {
    return mouse_x;
}

int get_mouse_y() {
    return mouse_y;
}

bool get_any_key_down() {
    return key_down > 0;
}

bool get_key_down(int key) {
    return (key_down & (1 << key)) != 0;
}

ALLEGRO_USTR* get_input(bool with_cursor) {
    if (with_cursor && al_ustr_get(input, cursor) != 124) { al_ustr_insert_chr(input, cursor, 124); }
    if (!with_cursor && al_ustr_get(input, cursor) == 124) { al_ustr_remove_chr(input, cursor); }
    return input;
}

void clear_input() {
    al_ustr_truncate(input, 0);
    cursor = 0;
}

void set_input(bool b) {
    take_input_string = b;
    if (!b) { clear_input(); }
}

int update_from_event(ALLEGRO_EVENT e) {
    if (e.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
        mouse_pressed = true;
    } else if (e.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP) {
        mouse_pressed = false;
    } else if (e.type == ALLEGRO_EVENT_MOUSE_AXES) {
        mouse_x = e.mouse.x;
        mouse_y = e.mouse.y;
    } else if (take_input_string && e.type == ALLEGRO_EVENT_KEY_CHAR) {
        if (al_ustr_get(input, cursor) == 124) { al_ustr_remove_chr(input, cursor); }
        if (e.keyboard.keycode == ALLEGRO_KEY_BACKSPACE) {
            al_ustr_remove_chr(input, --cursor);
        } else if (e.keyboard.keycode == ALLEGRO_KEY_DELETE) {
            al_ustr_remove_chr(input, cursor--);
        } else if (e.keyboard.keycode == ALLEGRO_KEY_LEFT) {
            cursor = std::max(0, cursor-1);
        } else if (e.keyboard.keycode == ALLEGRO_KEY_RIGHT) {
            cursor = std::min((int)al_ustr_length(input), cursor+1);
        } else if (e.keyboard.unichar >= 32 && e.keyboard.unichar <= 126 && e.keyboard.unichar != 124) {
            al_ustr_insert_chr(input, cursor++, e.keyboard.unichar);
        }
    } else if (e.type == ALLEGRO_EVENT_KEY_DOWN) {
        int key = get_function_of_keycode(e.keyboard.keycode);
        if (key == 4) { return 1; } // PAUSE
        else if (key == 5) { return 2; } // ENTER
        else if (key == 6) { return 3; } // COMMAND LINE
        else if (key >= 0) {
            key_down |= (1 << key);
        }
    } else if (e.type == ALLEGRO_EVENT_KEY_UP) {
        int key = get_function_of_keycode(e.keyboard.keycode);
        if (key >= 0) {
            key_down &= ~(1 << key);
        }
    } else { return 0; }
    return 4;
}
