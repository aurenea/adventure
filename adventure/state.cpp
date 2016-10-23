#include "state.h"

// TODO remove test cout functions, then this include and namespace
#include <iostream>

void set_state(State*);


State::State(Options* o) {
    options = o;
    input_controller = new InputController(o);
    ui_controller = new UIController(o);
}

State::~State() {
    delete ui_controller;
    delete input_controller;
    delete options;
    // TODO delete objects
}

RunResult State::run(ALLEGRO_EVENT* e) {
    if (e->type == ALLEGRO_EVENT_TIMER) {
        return update_frame();
    } else {
        return update_event(e);
    }
}

RunResult State::update_frame() {
    input_controller->update_frame();
    return RunResult::CONTINUE;
}

RunResult State::update_event(ALLEGRO_EVENT* e) {
    switch (input_controller->update_event(e)) {
    case InputClass::NONE:
        std::cout << "No event.\n";
        break;
    case InputClass::KEY_PRESSED:
        std::cout << "Key pressed.\n";
        break;
    case InputClass::KEY_RELEASED:
        std::cout << "Key released.\n";
        break;
    case InputClass::MOUSE_DRAG:
        std::cout << "Mouse drag.\n";
        break;
    case InputClass::MOUSE_DRAG_RELEASE:
        std::cout << "Mouse drag released.\n";
        break;
    case InputClass::MOUSE_LEFT_CLICK:
        std::cout << "Mouse left click.\n";
        break;
    case InputClass::MOUSE_RIGHT_CLICK:
        std::cout << "Mouse right click.\n";
        break;
    }
    return ui_controller->update_event(e);
}


MainMenuState::MainMenuState() : State(new Options()) {
    // initialize buttons
}
