#include <iostream>
#include <allegro5/allegro.h>

int run_game(ALLEGRO_DISPLAY*, ALLEGRO_EVENT_QUEUE*);

void run_menu(ALLEGRO_DISPLAY *display, ALLEGRO_EVENT_QUEUE *events) {
    while (true) {
        ALLEGRO_EVENT e;
        al_wait_for_event(events, &e);
        if (e.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            break;
        } else if (e.type == ALLEGRO_EVENT_TIMER) {
            // TODO do something
            std::cout << "[MENU] Running menu...\n";
            if (run_game(display, events) == 0) {
                break;
            }
        }
    }
}
