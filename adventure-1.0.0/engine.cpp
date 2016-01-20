#include <iostream>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_native_dialog.h>

using namespace std;

void load_options();
void load_assets();
void load_objects();

int get_FPS();
int get_screen_width();
int get_screen_height();

void run_menu(ALLEGRO_EVENT_QUEUE*);

int main() {

    // load all this shit, because if you load it later weird stuff starts happening for some reason
    load_options();
    load_objects();

    // initialize display
    ALLEGRO_DISPLAY *display = NULL;
    ALLEGRO_EVENT_QUEUE *events = NULL;
    ALLEGRO_TIMER *timer = NULL;

    if (!al_init()) {
        al_show_native_message_box(display, "Error", "", "Fatal Error: Allegro failed to initialize.", NULL, ALLEGRO_MESSAGEBOX_ERROR);
        return -1;
    }
    if (!al_install_mouse()) {
        al_show_native_message_box(display, "Error", "", "Fatal Error: Allegro mouse failed to initialize.", NULL, ALLEGRO_MESSAGEBOX_ERROR);
        return -1;
    }
    if (!al_install_keyboard()) {
        al_show_native_message_box(display, "Error", "", "Fatal Error: Allegro keyboard failed to initialize.", NULL, ALLEGRO_MESSAGEBOX_ERROR);
        return -1;
    }
    if (!al_init_image_addon()) {
        al_show_native_message_box(display, "Error", "", "Fatal Error: Allegro image addon failed to initialize.", NULL, ALLEGRO_MESSAGEBOX_ERROR);
        return -1;
    }
    al_init_font_addon();
    if (!al_init_ttf_addon()) {
        al_show_native_message_box(display, "Error", "", "Fatal Error: Allegro ttf font addon failed to initialize.", NULL, ALLEGRO_MESSAGEBOX_ERROR);
        return -1;
    }

    timer = al_create_timer(1.0/get_FPS());
    if (!timer) {
        al_show_native_message_box(display, "Error", "", "Fatal Error: Allegro timer failed to initialize.", NULL, ALLEGRO_MESSAGEBOX_ERROR);
        return -1;
    }

    display = al_create_display(get_screen_width(), get_screen_height());
    if (!display) {
        al_show_native_message_box(display, "Error", "", "Fatal Error: Display failed to initialize.", NULL, ALLEGRO_MESSAGEBOX_ERROR);
        al_destroy_timer(timer);
        return -1;
    }

    events = al_create_event_queue();
    if (!events) {
        al_show_native_message_box(display, "Error", "", "Fatal Error: Allegro event queue failed to initialize.", NULL, ALLEGRO_MESSAGEBOX_ERROR);
        al_destroy_display(display);
        al_destroy_timer(timer);
        return -1;
    }
    al_register_event_source(events, al_get_display_event_source(display));
    al_register_event_source(events, al_get_timer_event_source(timer));
    al_register_event_source(events, al_get_mouse_event_source());
    al_register_event_source(events, al_get_keyboard_event_source());

    // load images and stuff
    load_assets();

    al_start_timer(timer);

    run_menu(events);

    al_destroy_display(display);
    al_destroy_timer(timer);
    al_destroy_event_queue(events);
    return 0;
}
