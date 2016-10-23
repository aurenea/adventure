#ifndef STATE_H
#define STATE_H

#include <allegro5/allegro.h>
#include "input.h"
#include "interface.h"
#include "run.h"

/**
 *  Superclass for states of the game: the main menu, the load game screen, the new game screens, the actual game
 */
class State {
protected:
    ALLEGRO_BITMAP* bg;

    Options* options;
    InputController* input_controller;
    UIController* ui_controller;

public:
    State(Options*);
    virtual ~State();

    RunResult run(ALLEGRO_EVENT*);
    virtual RunResult update_frame();
    virtual RunResult update_event(ALLEGRO_EVENT*);

};

/**
 *  The State for the main menu of the game
 */
class MainMenuState : public State {
public:
    MainMenuState();

};

#endif // STATE_H
