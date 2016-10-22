#ifndef OPTIONS_H
#define OPTIONS_H

enum class COMMAND_FUNCTION {
    NONE = -1,
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
    ENTER = 1000,
    DEBUG = 1001
};

enum class OPTION_FLAG {
    FULLSCREEN = 1,
    SIDEBAR = 2,
    PAUSE = 4,
    PAUSE_IN_UI = 8,
    DOUBLE_ZOOM = 16,
    HARDCORE = 32,
    COMBAT = 64,
    UNLIMITED_MOVER = 128
};

enum class SHOW_ACTION_ICONS {
    NEVER_SHOW = 0,
    SHOW_IN_PAUSE_WITH_MOUSEOVER = 1,
    SHOW_WITH_MOUSEOVER = 2,
    SHOW_IN_PAUSE = 3,
    ALWAYS_SHOW = 4
};

enum class REALISM_SETTING {
    CASUAL = 0,
    NORMAL = 1,
    SERIOUS = 2,
    IMMERSIVE = 3
};

class Options {
private:
    static int screen_width;
    static int screen_height;
    static float resolution;

    static int keymap[5];
    static int active_flags;
    static SHOW_ACTION_ICONS show_action_icons;
    static REALISM_SETTING realism;

    Options() {}

public:
    static void init();
    static bool load_options();
    static void default_options();
    static void save_options();

    static int get_screen_width() { return Options::screen_width; }
    static int get_screen_height() { return Options::screen_height; }
    static void set_dimensions(int, int);

    static float get_scale();
    static void set_resolution(float f) { Options::resolution = f; }

    static COMMAND_FUNCTION get_cf_of_keycode(int);
    static void execute_cf(COMMAND_FUNCTION, State*);

    static bool get_flag(OPTION_FLAG);
    static void toggle_flag(OPTION_FLAG);
    static void set_flag(OPTION_FLAG, bool);

    static bool show_action_icon(bool);
};

#endif // OPTIONS_H
