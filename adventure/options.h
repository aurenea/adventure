#ifndef OPTIONS_H
#define OPTIONS_H

#include "module.h"

enum class KeyBindings : int {
    ENTER = 0,
    LEFT = 1,
    RIGHT = 2,
    UP = 3,
    DOWN = 4,
    LEFT_MOUSE = 30,
    RIGHT_MOUSE = 31
};

class Options {
protected:
    // global options
    static unsigned short int FRAME_RATE;

    // specific options
    int key_bindings[30];

public:
    Options();
    virtual ~Options();
    virtual void default_options();

    int get_frame_rate();
    int get_key_binding(int);
};

class ModuleOptions : public Options {
private:
    Module* mod;

public:
    ModuleOptions(Module*);
    ~ModuleOptions();
    //void default_options();
};

#endif // OPTIONS_H
