#include <iostream>
#include <allegro5/allegro.h>

using namespace std;

ALLEGRO_BITMAP* get_image_asset(int key);

bool get_mouse_pressed();
int get_mouse_x();
int get_mouse_y();

class Window {
    public:
        int x;
        int y;
        bool mouse_fixed;
        Window();
};

Window::Window() {
    this->x = 100;
    this->y = 100;
    this->mouse_fixed = false;
}

int num_windows = 0;
Window **windows = 0;

void add_window(Window window) {
    Window **temp_windows = new Window*[num_windows++];
    for (int i = 0; i < num_windows-1; i++) {
        temp_windows[i] = windows[i];
    }
    temp_windows[num_windows-1] = &window;
    delete[] windows;
    windows = temp_windows;
}

void remove_window(Window window) {
    Window **temp_windows = new Window*[num_windows--];
    bool removed = false;
    for (int i = 0; i < num_windows; i++) {
        if (windows[i] == &window && !removed) {
            removed = true;
        } else if (!removed && i == num_windows-1) {
            delete[] temp_windows;
            return;
        } else if (!removed) {
            temp_windows[i] = windows[i];
        } else {
            temp_windows[i-1] = windows[i];
        }
    }
    delete[] windows;
    windows = temp_windows;
}

int update_windows(ALLEGRO_EVENT e) {
    if (e.type == ALLEGRO_EVENT_MOUSE_AXES && get_mouse_pressed()) {
        for (int i = 0; i < num_windows; i++) {
            if (windows[i]->mouse_fixed) {
                windows[i]->x += e.mouse.dx;
                windows[i]->y += e.mouse.dy;
                return 1;
            }
        }
    } else if (e.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
        // check to see if any windows were clicked
    }
    return 0;
}

void draw_windows() {
    for (int i = 0; i < num_windows; i++) {
        al_draw_bitmap(get_image_asset(0), windows[i]->x, windows[i]->y, 0);
    }
}
