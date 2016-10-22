#include <iostream>
#include <allegro5/allegro.h>
#include "input-manager.h"

using namespace std;

int get_function_of_keycode(int);

bool input_frozen = false;

bool mouse_pressed = false;

int mouse_x;
int mouse_y;

int key_down = 0;

/*bool take_input_string = false;
int cursor = 0;
ALLEGRO_USTR* input = al_ustr_new("");*/

class TextInput {
    protected:
        ALLEGRO_USTR* input;
        int cursor;
    public:
        TextInput();
        ~TextInput();
        void add_at_cursor(int32_t);
        void remove_at_cursor();
        void remove_after_cursor();
        void clear_input();
        void cursor_left();
        void cursor_right();
        ALLEGRO_USTR* get_input() { return input; }
};

TextInput::TextInput() {
    input = al_ustr_new("");
    cursor = 0;
}

void TextInput::add_at_cursor(int32_t chr) {
    if (al_ustr_length(input) < 40) {
        al_ustr_insert_chr(input, cursor++, chr);
    }
}

void TextInput::remove_at_cursor() {
    al_ustr_remove_chr(input, --cursor);
}

void TextInput::remove_after_cursor() {
    al_ustr_remove_chr(input, cursor--);
}

void TextInput::clear_input() {
    al_ustr_truncate(input, 0);
    cursor = 0;
}

void TextInput::cursor_left() {
    cursor = std::max(0, cursor-1);
}

void TextInput::cursor_right() {
    cursor = std::min((int)al_ustr_length(input), cursor+1);
}

TextInput::~TextInput() {
    al_ustr_free(input);
}

TextInput** input_strings;
int current = -1;
int num_input = 0;

ALLEGRO_USTR* get_input() {
    if (current >= 0 && current < num_input) {
        if (input_strings[current] != NULL) {
            return input_strings[current]->get_input();
        }
    }
    return NULL;
}

ALLEGRO_USTR* get_input(int index) {
    if (index >= 0 && index < num_input) {
        if (input_strings[index] != NULL) {
            return input_strings[index]->get_input();
        }
    }
    return NULL;
}

void clear_input() {
    if (current >= 0 && current < num_input) {
        if (input_strings[current] != NULL) {
            input_strings[current]->clear_input();
        }
    }
}

int set_input_string(int loc) {
    if (loc >= -1 && loc < num_input) {
        current = loc;
        return 1;
    }
    return 0;
}

int add_new_input_string() {
    for (int i = 0; i < num_input; i++) {
        if (input_strings[i] == NULL) {
            input_strings[i] = new TextInput();
            return i;
        }
    }
    TextInput** temp = new TextInput*[num_input+1];
    for (int i = 0; i < num_input; i++) {
        temp[i] = input_strings[i];
    }
    temp[num_input] = new TextInput();
    input_strings = temp;
    set_input_string(num_input++);
    return current;
}

void remove_input_string_at(int index) {
    if (index >= 0 && index < num_input) {
        delete input_strings[index];
        if (input_strings[index] == NULL) { cout << "[INPUT] Deleted pointer gives NULL.\n"; }
        else { cout << "[INPUT] Deleted pointer does not give NULL.\n"; }
        input_strings[index] = NULL;
        current = -1;
    }
}

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

void freeze_input() { input_frozen = true; }
void unfreeze_input() { input_frozen = false; }

/*ALLEGRO_USTR* get_input(bool with_cursor) {
    if (with_cursor && al_ustr_get(input, cursor) != 124) { al_ustr_insert_chr(input, cursor, 124); }
    if (!with_cursor && al_ustr_get(input, cursor) == 124) { al_ustr_remove_chr(input, cursor); }
    return input;
}

void set_input(bool b) {
    take_input_string = b;
    if (!b) { clear_input(); }
}*/

int update_from_event(ALLEGRO_EVENT* e) {
    if (e->type == ALLEGRO_EVENT_MOUSE_BUTTON_UP) {
        mouse_pressed = false;
    } else if (e->type == ALLEGRO_EVENT_KEY_UP) {
        COMMAND_FUNCTION key = get_function_of_keycode(e->keyboard.keycode);
        if (key >= COMMAND_FUNCTION::UP && key <= COMMAND_FUNCTION::RIGHT) {
            key_down &= ~(1 << (int)key);
        }
    } else if (e->type == ALLEGRO_EVENT_MOUSE_AXES) {
        mouse_x = e->mouse.x;
        mouse_y = e->mouse.y;
    } else if (!input_frozen) {
        if (e->type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
            mouse_pressed = true;
        } else if (e->type == ALLEGRO_EVENT_KEY_CHAR && current > -1 && input_strings[current] != NULL) {
            // if (al_ustr_get(input, cursor) == 124) { al_ustr_remove_chr(input, cursor); }
            if (e->keyboard.keycode == ALLEGRO_KEY_BACKSPACE) {
                input_strings[current]->remove_at_cursor();
                // al_ustr_remove_chr(input, --cursor);
            } else if (e->keyboard.keycode == ALLEGRO_KEY_DELETE) {
                input_strings[current]->remove_after_cursor();
                // al_ustr_remove_chr(input, cursor--);
            } else if (e->keyboard.keycode == ALLEGRO_KEY_LEFT) {
                input_strings[current]->cursor_left();
                // cursor = std::max(0, cursor-1);
            } else if (e->keyboard.keycode == ALLEGRO_KEY_RIGHT) {
                input_strings[current]->cursor_right();
                // cursor = std::min((int)al_ustr_length(input), cursor+1);
            } else if (e->keyboard.unichar >= 32 && e->keyboard.unichar <= 126 && e->keyboard.unichar != 124) {
                input_strings[current]->add_at_cursor(e->keyboard.unichar);
                // al_ustr_insert_chr(input, cursor++, e.keyboard.unichar);
            }
        } else if (e->type == ALLEGRO_EVENT_KEY_DOWN) {
            COMMAND_FUNCTION key = get_function_of_keycode(e->keyboard.keycode);
            if (key >= COMMAND_FUNCTION::UP && key <= COMMAND_FUNCTION::RIGHT) {
                key_down |= (1 << (int)key);
                return COMMAND_FUNCTION::NONE;
            } else {
                return key;
            }
        } else { return 0; }
    }
    return 4;
}
