#ifndef INPUTMANAGER_H
#define INPUTMANAGER_H

bool get_mouse_pressed();
int get_mouse_x();
int get_mouse_y();

bool get_any_key_down();
bool get_key_down(int);

int add_new_input_string();
void remove_input_string_at(int);
int set_input_string(int);
ALLEGRO_USTR* get_input();
ALLEGRO_USTR* get_input(int);
void clear_input();

void freeze_input();
void unfreeze_input();

#endif // INPUTMANAGER_H
