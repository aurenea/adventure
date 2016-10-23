#include "state.h"

void set_state(State*);


State::State(InputController* ic) {
    this->input_controller = ic;
    this->ui_controller = new UIController();
}

RunResult State::run(ALLEGRO_EVENT* e) {
    if (e->type == ALLEGRO_EVENT_TIMER) {
        return update_frame(e);
    } else {
        return update_event(e);
    }
}

RunResult State::update_frame(ALLEGRO_EVENT* e) {
    return RunResult::CONTINUE;
}

RunResult State::update_event(ALLEGRO_EVENT* e) {
    input_controller->update_event(e);
    return ui_controller->update_event(e);
}

State::~State() {
    delete ui_controller;
}


MainMenuState::MainMenuState(InputController* ic) : State(ic) {
    // initialize buttons
}
