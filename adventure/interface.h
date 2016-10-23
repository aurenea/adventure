#ifndef INTERFACE_H
#define INTERFACE_H

#include <allegro5/allegro.h>
#include "run.h"

/**
 *  Class for holding and updating UI elements
 */
class UIController {
public:
    //~Interface();
    RunResult update_event(ALLEGRO_EVENT*);
};

#endif // INTERFACE_H
