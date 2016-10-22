#include <allegro5/allegro.h>

#ifndef RUN_RESULT_H
#define RUN_RESULT_H

enum RUN_RESULT : int {
    RUN_NEXT = 2,           // event not yet resolved, keep searching for resolution
    RUN_CONTINUE = 1,       // event resolved, move on
    RUN_STOP = 0,           // stop running program and exit
    RUN_ERROR = -1          // error raised, shut down program
}

#endif // RUN_RESULT_H

#ifndef ENGINE_H
#define ENGINE_H

#include "state.h"

class Engine {
private:
    static ALLEGRO_DISPLAY* display;
    static ALLEGRO_EVENT_QUEUE* events;
    static ALLEGRO_TIMER* timer;

    static State* state;

    Engine() {}

public:
    static int init();
    static int run();
    static void close();

    static void set_dimensions(int, int);

    static void set_state(State*);

};

#endif // ENGINE_H
