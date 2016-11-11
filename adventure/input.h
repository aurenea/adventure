#ifndef INPUT_H
#define INPUT_H

#include <memory>
#include <allegro5/allegro.h>
#include "options.h"

enum class InputClass {
    NONE,
    MOUSE_DRAG,
    MOUSE_DRAG_RELEASE,
    MOUSE_LEFT_CLICK,
    MOUSE_RIGHT_CLICK,
    KEY_PRESSED,
    KEY_RELEASED
};

/**
 *  Basic input controller
 */
class InputController {
protected:
    std::shared_ptr<Options> options;

    bool frozen;

    int inputs_held;
    short int crd_left[2];
    short int crd_right[2];
    unsigned short int frames_since_press[2];

    void set_input_held(int, bool);

public:
    InputController(Options*);

    void update_frame();
    InputClass update_event(ALLEGRO_EVENT*);

    void set_frozen(bool b) { frozen = b; }

    bool get_input_held(int);

};


#endif // INPUT_H
