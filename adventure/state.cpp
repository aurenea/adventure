#include "state.h"

RUN_RESULT State::run(ALLEGRO_EVENT* e) {
    InputHandler::update(e, this);
}

RUN_RESULT MainState::run(ALLEGRO_EVENT* e) {
    State::run(e);
    Engine::set_state(new NewGameState());
}

NewGameState::NewGameState() {
    hair_color = 1;
    hair_style = 0;
    skin_color = 2;
    // other stuff: eyes, nose, mouth, head shape
    face = SOUTHWEST;
}

RUN_RESULT NewGameState::run(ALLEGRO_EVENT* e, )
