#include <iostream>
#include <cmath>
#include <allegro5/allegro5.h>
#include "objects.h"
#include "assets.h"

#ifndef INTERFACE_H
#define INTERFACE_H

class UIWidget {
    protected:
        int s_default;
        ALLEGRO_BITMAP* sprite;
        int x_bound;
        int y_bound;
    public:
        UIWidget(int, int, int);
        virtual int update(ALLEGRO_EVENT, int, int);
        virtual void draw(int, int);
        void set_sprite(ALLEGRO_BITMAP* spr) { sprite = spr; }
        void clear_sprite() { sprite = NULL; }
};

class Label: public UIWidget {
    protected:
        ALLEGRO_USTR* mouseover_text;
    public:
        Label(int s, int x, int y, ALLEGRO_USTR* mt) : UIWidget(s, x, y), mouseover_text(mt) {}
        int update(ALLEGRO_EVENT, int, int);
};

class Button: public UIWidget {
    protected:
        bool pressed;
        int s_pressed;  // pressed sprite
    public:
        Button(int, int, int, int);
        int update(ALLEGRO_EVENT, int, int);
        void draw(int, int);
};



class Window {
    public:
        int x;
        int y;
        UIWidget* widgets;
        bool mouse_fixed;
        Window();
        virtual int update_window(ALLEGRO_EVENT) = 0;
        virtual ALLEGRO_BITMAP* get_bitmap() { return get_interface_asset(1); }
        virtual void draw() = 0;
        virtual bool get_in_bounds(int ax, int ay) {
            ALLEGRO_BITMAP* bg = get_bitmap();
            return (ax >= x && ax <= x+al_get_bitmap_width(bg) && ay >= y && ay <= y+al_get_bitmap_height(bg));
        }
};

class ContainerWindow: public Window {
    private:
        ItemContainer* cont;
    public:
        ContainerWindow(ItemContainer* ic) : cont(ic) { std::cout << "New ContainerWindow.\n"; }
        int update_window(ALLEGRO_EVENT);
        void draw();
        ItemContainer* get_container() { return cont; }
};

class DescriptionWindow: public Window {
    private:
        Object* object;
        ALLEGRO_USTR* text;
    public:
        DescriptionWindow(Object* obj) : object(obj) { text = object->get_description(); }
        int update_window(ALLEGRO_EVENT e) { return 0; }
        void draw();
};

class ConversationWindow: public Window {
    private:
        Person* npc;
    public:
        ConversationWindow(Person* n) : npc(n) { x = y = 0; }
        int update_window(ALLEGRO_EVENT) { return 0; }
        void draw();
};

class CharacterWindow: public Window {
    private:
        Humanoid* person;
    public:
        CharacterWindow(Humanoid* h) : person(h) {}
        ALLEGRO_BITMAP* get_bitmap() { return get_interface_asset(3); }
        int update_window(ALLEGRO_EVENT);
        void draw();
};

class SkillsWindow: public Window {
    public:
        SkillsWindow() {}
        ALLEGRO_BITMAP* get_bitmap() { return get_interface_asset(11); }
        int update_window(ALLEGRO_EVENT);
        void draw();
};

void add_window(Window*);
void remove_window(Window*);
void draw_windows();
int update_windows(ALLEGRO_EVENT);
void close_all_windows();

bool is_dragging(Object*, bool = false);
Object* get_dragging();
void set_dragging(Object*, int, int);
Point* get_affixation();

void set_overlay_color(ALLEGRO_COLOR*);
void clear_overlay_color();
void set_mouse_text(ALLEGRO_USTR*);
void clear_mouse_text();

void draw_bitmap(ALLEGRO_BITMAP*, float, float, int);
void draw_tinted_bitmap(ALLEGRO_BITMAP*, ALLEGRO_COLOR, float, float, int);
void draw_mouse(int);

#endif // INTERFACE_H
