#include <allegro5/allegro5.h>

#ifndef FILEIO_H
#define FILEIO_H

int get_class_by_id(int);
int get_sprite_by_id(int);
char* get_name_by_id(int);
ALLEGRO_USTR* get_desc_by_id(int);
int* get_params_by_id(int);

#endif // FILEIO_H
