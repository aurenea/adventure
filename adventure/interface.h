#ifndef INTERFACE_H
#define INTERFACE_H

#include <allegro5/allegro.h>
#include "options.h"
#include "run.h"

/**
 *  Class for holding and updating UI elements
 */
class UIController {
private:
    Options* options;

public:
    UIController(Options*);

    RunResult update_event(ALLEGRO_EVENT*);
};

#endif // INTERFACE_H
