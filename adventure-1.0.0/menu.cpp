#include <iostream>
#include <allegro5/allegro.h>

int new_game(ALLEGRO_EVENT_QUEUE*);
int load_game(ALLEGRO_EVENT_QUEUE*);

void run_menu(ALLEGRO_EVENT_QUEUE *events) {
    while (true) {
        ALLEGRO_EVENT e;
        al_wait_for_event(events, &e);
        if (e.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            break;
        } else if (e.type == ALLEGRO_EVENT_TIMER) {
            // TODO do something
            std::cout << "[MENU] Running menu...\n";
            if (new_game(events) == 0) {
                break;
            }
        }
    }
}
