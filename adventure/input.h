#include <allegro5/allegro.h>

enum class KeyHandler {
    UP = 0,
    LEFT = 1,
    DOWN = 2,
    RIGHT = 3,
    PAUSE = 4,
    MENU = 5,
    WAIT = 6,
    REST = 7,
    CAMP = 8,
    COMBAT = 9,
    C_ROTATE = 10,
    CC_ROTATE = 11,
    CHARACTER = 12,
    SKILLS = 13,
    INVENTORY = 14,
    JOURNAL = 15,
    MAP = 16,
    ENTER = 30,
    DEBUG = 31
};

class InputHandler {
private:
    static int keymap[16];

    static bool frozen;
    static bool mouse_pressed;
    static int keys_pressed;

    static int mouse_x;
    static int mouse_y;

    InputHandler() {}

    static KeyHandler get_keycode(int);

public:
    static void init();
    static void update(ALLEGRO_EVENT*, State*);
};
