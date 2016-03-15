#include <iostream>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include "input-manager.h"
#include "interface.h"
#include "objects.h"
#include "geometry.h"

int get_mouse_x();
int get_mouse_y();

ALLEGRO_BITMAP* get_interface_asset(int);
ALLEGRO_FONT* get_font(int);


ALLEGRO_COLOR white = al_map_rgb(255, 255, 255);
ALLEGRO_COLOR* overlay = NULL;

void set_overlay_color(ALLEGRO_COLOR* color) {
    overlay = color;
}

void clear_overlay_color() {
    overlay = NULL;
}


ALLEGRO_USTR* mouse_text = NULL;

void set_mouse_text(ALLEGRO_USTR* ustr) {
    mouse_text = ustr;
}

void clear_mouse_text() {
    mouse_text = NULL;
}


Object* dragging = NULL;
Point affixation(0, 0);

bool is_dragging(Object* o, bool strictly) {
    return (dragging == o || (strictly && dragging == NULL));
}

Object* get_dragging() {
    return dragging;
}

void set_dragging(Object* o, int x, int y) {
    dragging = o;
    affixation.set_to(x, y);
}

Point* get_affixation() {
    return &affixation;
}


void draw_bitmap(ALLEGRO_BITMAP* bitmap, float x, float y, int flags) {
    if (bitmap != NULL) {
        if (overlay == NULL) {
            al_draw_bitmap(bitmap, x, y, flags);
        } else {
            al_draw_tinted_bitmap(bitmap, *overlay, x, y, flags);
        }
    }
}

void draw_tinted_bitmap(ALLEGRO_BITMAP* bitmap, ALLEGRO_COLOR color, float x, float y, int flags) {
    if (bitmap != NULL) {
        if (overlay == NULL) {
            al_draw_tinted_bitmap(bitmap, color, x, y, flags);
        } else {
            float r1, r2, b1, b2, g1, g2, a1, a2;
            al_unmap_rgba_f(color, &r1, &b1, &g1, &a1);
            al_unmap_rgba_f(*overlay, &r2, &b2, &g2, &a2);
            al_draw_tinted_bitmap(bitmap, al_map_rgba_f(r1*r2, b1*b2, g1*g2, a1*a2), x, y, flags);
        }
    }
}

void draw_text(const char* c, int font, float x, float y, int flags, bool overwrite = false) {
    al_draw_text(get_font(font), (overlay == NULL || overwrite) ? white : *overlay, x, y, flags, c);
}

void draw_ustr(ALLEGRO_USTR* ustr, int font, float x, float y, int flags, bool overwrite = false) {
    al_draw_ustr(get_font(font), (overlay == NULL || overwrite) ? white : *overlay, x, y, flags, ustr);
}

void draw_mouse(int type) {
    // draw object being dragged
    if (dragging != NULL) {
        draw_tinted_bitmap(dragging->get_sprite(), *get_color(2),
            get_mouse_x()+affixation.x, get_mouse_y()+affixation.y, 0);
    }
    // draw the mouse itself
    al_draw_bitmap(get_interface_asset(type), get_mouse_x(), get_mouse_y(), 0);
    // draw the mouse text
    if (mouse_text != NULL) {
        if (get_mouse_x()+12+al_get_ustr_width(get_font(0), mouse_text) > 640) {
            draw_ustr(mouse_text, 0, get_mouse_x()-al_get_ustr_width(get_font(0), mouse_text),
                get_mouse_y(), 0);
        } else {
            draw_ustr(mouse_text, 0, get_mouse_x()+12, get_mouse_y(), 0);
        }
    }
}

void draw_interface() {
    // stuff
    draw_mouse(0);
}



UIWidget::UIWidget(int s, int xb, int yb) {
    s_default = s;
    sprite = NULL;
    x_bound = xb;
    y_bound = yb;
}

int UIWidget::update(ALLEGRO_EVENT e, int x, int y) {
    if (e.mouse.x >= x+x_bound && e.mouse.y >= y+y_bound
            && e.mouse.x <= x+x_bound+al_get_bitmap_width(sprite == NULL ? get_interface_asset(s_default) : sprite)
            && e.mouse.y <= y+y_bound+al_get_bitmap_height(sprite == NULL ? get_interface_asset(s_default) : sprite)) {
        return 1;
    }
    return 0;
}

void UIWidget::draw(int x, int y) {
    draw_bitmap(sprite != NULL ? sprite : get_interface_asset(s_default), x, y, 0);
}



int Label::update(ALLEGRO_EVENT e, int x, int y) {
    if (UIWidget::update(e, x, y) > 0) {
        set_mouse_text(mouseover_text);
        return 1;
    } else if (mouse_text == mouseover_text) {
        clear_mouse_text();
    }
    return 0;
}



Button::Button(int sd, int sp, int xb, int yb) : UIWidget(sd, xb, yb) {
    pressed = false;
    s_pressed = sp;
}

int Button::update(ALLEGRO_EVENT e, int x, int y) {
    if (UIWidget::update(e, x, y) > 0) {
        if (e.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
            pressed = true;
            return 1;
        } else if (e.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP && pressed) {
            pressed = false;
            return 1;
        }
    }
    if (e.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP) {
        pressed = false;
    }
    return 0;
}

void Button::draw(int x, int y) {
    draw_bitmap(sprite != NULL ? sprite : (!pressed ? get_interface_asset(s_default) : get_interface_asset(s_pressed)),
        x+x_bound, y+y_bound, 0);
}



Window::Window() {
    x = y = 100;
    mouse_fixed = false;
}



void ContainerWindow::draw() {
    draw_bitmap(get_bitmap(), x, y, 0);
    ALLEGRO_BITMAP* bmp = cont->get_sprite();
    draw_bitmap(bmp, x+69-(al_get_bitmap_width(bmp)/2), y+25-(al_get_bitmap_height(bmp)/2), 0);

    int xd = 69-(5*cont->width);
    int yd = 83-(5*cont->height);
    for (int i = 0; i < cont->width; i++) {
        for (int j = 0; j < cont->height; j++) {
            Item* item = cont->get_item(i, j);
            if (item == NULL) {
                draw_bitmap(get_interface_asset(2), x+xd+(10*i), y+yd+(10*j), 0);
            } else {
                if (item->get_position()->x == i && item->get_position()->y == j) {
                    draw_bitmap(item->get_sprite(), x+xd+(10*i), y+yd+(10*j), 0);
                }
            }
        }
    }
}

int ContainerWindow::update_window(ALLEGRO_EVENT e) {
    int xd = 69-(5*cont->width);
    int yd = 83-(5*cont->height);
    if (e.type == ALLEGRO_EVENT_MOUSE_AXES) {
        int i = (e.mouse.x-x-xd)/10;
        int j = (e.mouse.y-y-yd)/10;
        if (i >= 0 && i < cont->width && j >= 0 && j < cont->height) {
            Item* item = cont->get_item(i, j);
            if (item == NULL) {
                clear_mouse_text();
            } else {
                set_mouse_text(item->get_name());
            }
            return 1;
        }
    } else if (e.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
        int i = (e.mouse.x-x-xd)/10;
        int j = (e.mouse.y-y-yd)/10;
        if (i >= 0 && i < cont->width && j >= 0 && j < cont->height) {
            Item* item = cont->get_item(i, j);
            if (item != NULL && dragging == NULL) {
                dragging = item;
                Point* p = item->get_position();
                affixation.set_to(x+xd+(10*p->x)-e.mouse.x, y+yd+(10*p->y)-e.mouse.y);
                delete p;
                return 1;
            }
        }
    } else if (e.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP) {
        Point* p = new Point((get_mouse_x()+affixation.x-(x+xd-5))/10, (get_mouse_y()+affixation.y-(y+yd-5))/10);
        if (dragging != NULL && cont->get_item(p->x, p->y) != NULL) {
            cont->get_item(p->x, p->y)->Object::on_click(e);
            delete p;
            return 1;
        } else if (Item* item = dynamic_cast<Item*>(dragging)) {
            cont->add_item(item, p->x, p->y);
            delete p;
            return 1;
        }
        // do stuff
        // real specific past me thanks
        delete p;
    }
    return 0;
}



void DescriptionWindow::draw() {
    draw_bitmap(get_bitmap(), x, y, 0);
    ALLEGRO_BITMAP* bmp = object->get_sprite();
    draw_bitmap(bmp, x+69-(al_get_bitmap_width(bmp)/2), y+25-(al_get_bitmap_height(bmp)/2),
        (((rel_direction(object->get_direction())+6)%8 >= 5) ? ALLEGRO_FLIP_HORIZONTAL : 0));
    draw_ustr(text, 0, x+9, y+53, 0);
}



void ConversationWindow::draw() {
    draw_bitmap(get_bitmap(), x, y, 0);
    ALLEGRO_BITMAP* bmp = npc->get_sprite();
    draw_bitmap(bmp, x+69-(al_get_bitmap_width(bmp)/2), y+25-(al_get_bitmap_height(bmp)/2),
        (((rel_direction(npc->get_direction())+6)%8 >= 5) ? ALLEGRO_FLIP_HORIZONTAL : 0));
}



void CharacterWindow::draw() {
    draw_bitmap(get_bitmap(), x, y, 0);
    draw_bitmap(get_portrait(0), x+33, y+16, 0);
    draw_bitmap(person->get_port(), x+36, y+19, 0);
    draw_ustr(person->get_name(), 1, x+51, y+62, ALLEGRO_ALIGN_CENTER);
    draw_ustr(person->get_title(), 2, x+51, y+73, ALLEGRO_ALIGN_CENTER);
    /* ALLEGRO_BITMAP* bmp = person->get_sprite();
    draw_bitmap(bmp, x+69-(al_get_bitmap_width(bmp)/2), y+25-(al_get_bitmap_height(bmp)/2),
        (((rel_direction(person->get_direction())+6)%8 >= 5) ? ALLEGRO_FLIP_HORIZONTAL : 0));
        */
    /** DRAW SKILLS **/
    if (person == get_player()) {
        draw_bitmap(get_interface_asset(10), x+21, y+86, 0);
    } else if (Person* prs = dynamic_cast<Person*>(person)) {
        // draw Person's main skills
    }
    /** DRAW HEALTH AND OTHER METERS **/
    for (int i = 0; i < 3; i++) {
        draw_bitmap(get_interface_asset(7+i), x+15, y+109+(10*i), 0);
        // fill in the meters with <115, 86, 57>
    }
    /** DRAW EQUIPMENT **/
    person->draw(new Point(x+133, y+61), 12);
    for (int i = 0; i < 8; i++) {
        Point* p = NULL;
        if (i < 4) {
            p = new Point(134, 46);
            p = translate(p, (2*(i+1))%8, (i == 0 ? 27 : 29));
        } else {
            p = new Point(113, 27);
            if (i == 5 || i == 6) { p = translate(p, SOUTH, 39); }
            if (i == 6 || i == 7) { p = translate(p, EAST, 42); }
        }
        Equipment* eq = person->get_equip(i);
        ALLEGRO_BITMAP* bmp = (eq != NULL ? eq->get_sprite() : get_interface_asset(2));
        draw_bitmap(bmp, x+p->x-(al_get_bitmap_width(bmp)/2), y+p->y-(ceil(al_get_bitmap_height(bmp)/10.0)*5), 0);
    }
    for (int i = 0; i < 2; i++) {
        ALLEGRO_BITMAP* bmp = (person->get_hands(i) != NULL ? person->get_hands(i)->get_sprite() : get_interface_asset(2));
        draw_bitmap(bmp, x+113+(42*i)-(al_get_bitmap_width(bmp)/2), y+86-(al_get_bitmap_height(bmp)/2), 0);
    }
    /** DRAW STATS **/
    // do that here
    char* s = new char[4];
    for (int i = 0; i < 4; i++) {
        memset(&s[0], '0', sizeof(s));
        s[0] = ' ';
        s[3] = '\0';
        int stb = 0;
        switch (i) {
            case 0:
                stb = person->get_DMG();
                break;
            case 1:
                stb = person->get_HIT();
                break;
            case 2:
                stb = person->get_EVA(0);
                break;
            case 3:
                stb = person->get_DEF(0, 0);
                break;
        }
        itoa(stb, &s[(stb >= 100 ? 0 : (stb >= 10 ? 1 : 2))], 10);
        // TODO fix later
        draw_text((const char*)(s), 3, x+114+(42*(i/2)), y+110+(9*(i%2)), 0);
    }
    delete s;
}

int CharacterWindow::update_window(ALLEGRO_EVENT e) {
    if (e.type == ALLEGRO_EVENT_MOUSE_AXES) {
        if (e.mouse.x >= x+14 && e.mouse.x <= x+82) {
            if (e.mouse.y >= y+108 && e.mouse.y <= y+116) {
                set_mouse_text(get_description(0));
            } else if (e.mouse.y >= y+118 && e.mouse.y <= y+126) {
                set_mouse_text(get_description(1));
            } else if (e.mouse.y >= y+128 && e.mouse.y <= y+136) {
                set_mouse_text(get_description(2));
            } else {
                clear_mouse_text();
            }
        } else if (e.mouse.x >= x+95 && e.mouse.x <= x+173) {
            if (e.mouse.y >= y+76 && e.mouse.y <= y+96 && (e.mouse.x < x+124 || e.mouse.x > x+144)) {
                Weapon* hand = (e.mouse.x < x+124 ? person->get_hands(0) : person->get_hands(1));
                if (hand != NULL) {
                    Point* p = (e.mouse.x < x+124 ? new Point(x+113, y+86) : new Point(x+155, y+86));
                    ALLEGRO_BITMAP* bmp = hand->get_sprite();
                    int w = al_get_bitmap_width(bmp)/2;
                    int h = al_get_bitmap_height(bmp)/2;
                    if (e.mouse.x >= p->x-w && e.mouse.x <= p->x+w && e.mouse.y >= p->y-h && e.mouse.y <= p->y+h) {
                        set_mouse_text(hand->get_name());
                    } else {
                        clear_mouse_text();
                    }
                    delete p;
                }
            } else if (e.mouse.y >= y+9 && e.mouse.y <= y+85) {
                Point* p = new Point(e.mouse.x-x-134, e.mouse.y-y-46);
                int i = (get_angle_of_vector(p)+6)%8;
                i = ((9*(i/2))+(i%2 == 1 ? 4 : 0)) % 8;
                delete p;
                p = NULL;
                if (i < 4) {
                    p = new Point(134, 46);
                    p = translate(p, (2*(i+1))%8, (i == 0 ? 27 : 29));
                } else {
                    p = new Point(113, 27);
                    if (i == 5 || i == 6) { p = translate(p, SOUTH, 39); }
                    if (i == 6 || i == 7) { p = translate(p, EAST, 42); }
                }
                Equipment* eq = person->get_equip(i);
                if (eq != NULL) {
                    ALLEGRO_BITMAP* bmp = eq->get_sprite();
                    int w = al_get_bitmap_width(bmp)/2;
                    int h = ceil(al_get_bitmap_height(bmp)/10.0)*5;
                    if (e.mouse.x >= x+p->x-w && e.mouse.x <= x+p->x+w && e.mouse.y >= y+p->y-h && e.mouse.y <= y+p->y+h) {
                        set_mouse_text(eq->get_name());
                    } else {
                        clear_mouse_text();
                    }
                }
            }
        } else {
            clear_mouse_text();
        }
        return 1;
    } else if (e.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
        if (e.mouse.x >= x+95 && e.mouse.x <= x+173) {
            if (e.mouse.y >= y+76 && e.mouse.y <= y+96 && (e.mouse.x < x+124 || e.mouse.x > x+144)) {
                Weapon* hand = (e.mouse.x < x+124 ? person->get_hands(0) : person->get_hands(1));
                if (hand != NULL) {
                    Point* p = (e.mouse.x < x+124 ? new Point(x+113, y+86) : new Point(x+155, y+86));
                    ALLEGRO_BITMAP* bmp = hand->get_sprite();
                    int w = al_get_bitmap_width(bmp)/2;
                    int h = al_get_bitmap_height(bmp)/2;
                    if (e.mouse.x >= p->x-w && e.mouse.x <= p->x+w && e.mouse.y >= p->y-h && e.mouse.y <= p->y+h) {
                        dragging = hand;
                        affixation.set_to(p->x-w-e.mouse.x, p->y-h-e.mouse.y);
                    }
                    delete p;
                    return 1;
                }
            } else if (e.mouse.y >= y+9 && e.mouse.y <= y+85) {
                Point* p = new Point(e.mouse.x-x-134, e.mouse.y-y-46);
                int i = (get_angle_of_vector(p)+6)%8;
                i = ((9*(i/2))+(i%2 == 1 ? 4 : 0)) % 8;
                delete p;
                p = NULL;
                if (i < 4) {
                    p = new Point(x+134, y+46);
                    p = translate(p, (2*(i+1))%8, (i == 0 ? 27 : 29));
                } else {
                    p = new Point(x+113, y+27);
                    if (i == 5 || i == 6) { p = translate(p, SOUTH, 39); }
                    if (i == 6 || i == 7) { p = translate(p, EAST, 42); }
                }
                Equipment* eq = person->get_equip(i);
                if (eq != NULL) {
                    ALLEGRO_BITMAP* bmp = eq->get_sprite();
                    int w = al_get_bitmap_width(bmp)/2;
                    int h = ceil(al_get_bitmap_height(bmp)/10.0)*5;
                    if (e.mouse.x >= p->x-w && e.mouse.x <= p->x+w && e.mouse.y >= p->y-h && e.mouse.y <= p->y+h) {
                        dragging = eq;
                        affixation.set_to(p->x-w-e.mouse.x, p->y-h-e.mouse.y);
                        return 1;
                    }
                }
            }
        }
    } else if (e.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP) {
        if (e.mouse.x >= x+21 && e.mouse.x <= x+80) {
            if (e.mouse.y >= y+86 && e.mouse.y <= y+102){
                add_window(new SkillsWindow());
                return 1;
            }
        } else if (e.mouse.x >= x+95 && e.mouse.x <= x+173) {
            if (dragging != NULL) {
                if (e.mouse.y >= y+76 && e.mouse.y <= y+96 && (e.mouse.x < x+124 || e.mouse.x > x+144)) {
                    Weapon* hand = (e.mouse.x < x+124 ? person->get_hands(0) : person->get_hands(1));
                    Point* p = (e.mouse.x < x+124 ? new Point(x+113, y+86) : new Point(x+155, y+86));
                    ALLEGRO_BITMAP* bmp = (hand == NULL ? get_interface_asset(2) : hand->get_sprite());
                    int w = al_get_bitmap_width(bmp)/2;
                    int h = al_get_bitmap_height(bmp)/2;
                    if (e.mouse.x >= p->x-w && e.mouse.x <= p->x+w && e.mouse.y >= p->y-h && e.mouse.y <= p->y+h) {
                        person->equip_item(dragging, (e.mouse.x < x+124 ? 0 : 1));
                    }
                    delete p;
                    return 1;
                } else if (e.mouse.y >= y+9 && e.mouse.y <= y+85) {
                    Point* p = new Point(e.mouse.x-x-134, e.mouse.y-y-46);
                    int i = (get_angle_of_vector(p)+6)%8;
                    i = ((9*(i/2))+(i%2 == 1 ? 4 : 0)) % 8;
                    delete p;
                    p = NULL;
                    if (i < 4) {
                        p = new Point(x+134, y+46);
                        p = translate(p, (2*(i+1))%8, (i == 0 ? 27 : 29));
                    } else {
                        p = new Point(x+113, y+27);
                        if (i == 5 || i == 6) { p = translate(p, SOUTH, 39); }
                        if (i == 6 || i == 7) { p = translate(p, EAST, 42); }
                    }
                    Equipment* eq = person->get_equip(i);
                    ALLEGRO_BITMAP* bmp = (eq != NULL ? eq->get_sprite() : get_interface_asset(2));
                    int w = al_get_bitmap_width(bmp)/2;
                    int h = ceil(al_get_bitmap_height(bmp)/10.0)*5;
                    if (e.mouse.x >= p->x-w && e.mouse.x <= p->x+w && e.mouse.y >= p->y-h && e.mouse.y <= p->y+h) {
                        person->equip_item(dragging, i);
                        return 1;
                    }
                }
            }
        }
    }
    // TODO stuff later
    return 0;
}



void SkillsWindow::draw() {
    draw_bitmap(get_bitmap(), x, y, 0);
    /** DRAW NAME **/
    // TODO that^
    /** FILL IN SKILL PROGRESS BUBBLES **/
    // start at (42, 30)
    // add (6, 0) for each bubble; see below for inter-skill relations
    /** FILL IN SKILL PROGRESS BARS **/
    // start at (39, 36)
    // add (70, 0)*(i%2)
    // add (0, 14)*(i/2)
}

int SkillsWindow::update_window(ALLEGRO_EVENT e) {
    if (e.type == ALLEGRO_EVENT_MOUSE_AXES) {
        if (e.mouse.x >= x+16 && e.mouse.x <= x+74) {
            int i = (e.mouse.y-y-26)/14;
            if (i >= 0 && i < 9 && e.mouse.y >= y+26) {
                set_mouse_text(get_description(3+(2*i)));
                return 1;
            } else {
                clear_mouse_text();
                return 1;
            }
        } else if (e.mouse.x >= x+86 && e.mouse.x <= x+144) {
            int i = (e.mouse.y-y-26)/14;
            if (i >= 0 && i < 9 && e.mouse.y >= y+26) {
                set_mouse_text(get_description(4+(2*i)));
                return 1;
            } else {
                clear_mouse_text();
                return 1;
            }
        }
    }
    return 0;
}



int num_windows = 0;
Window **windows = 0;

void add_window(Window* window) {
    num_windows++;
    Window **temp_windows = new Window*[num_windows];
    for (int i = 0; i < num_windows-1; i++) {
        temp_windows[i] = windows[i];
    }
    temp_windows[num_windows-1] = window;
    delete[] windows;
    windows = temp_windows;
}

void remove_window(Window* window) {
    // TODO fix later
    num_windows--;
    Window **temp_windows = new Window*[num_windows];
    bool removed = false;
    for (int i = 0; i < num_windows+1; i++) {
        if (windows[i] == window && !removed) {
            removed = true;
        } else if (!removed && i == num_windows) {
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

void draw_windows() {
    for (int i = 0; i < num_windows; i++) {
        windows[i]->draw();
    }
}

int update_windows(ALLEGRO_EVENT e) {
    if (e.type == ALLEGRO_EVENT_MOUSE_AXES) {
        for (int i = num_windows-1; i >= 0; i--) {
            if (windows[i]->mouse_fixed && get_mouse_pressed()) {
                windows[i]->x += e.mouse.dx;
                windows[i]->y += e.mouse.dy;
                return 1;
            }
            ALLEGRO_BITMAP* bg = windows[i]->get_bitmap();
            if (e.mouse.x >= windows[i]->x && e.mouse.x <= windows[i]->x+al_get_bitmap_width(bg)
                && e.mouse.y >= windows[i]->y && e.mouse.y <= windows[i]->y+al_get_bitmap_height(bg)) {
                windows[i]->update_window(e);
                return 1;
            }
        }
    } else if (e.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
        for (int i = num_windows-1; i >= 0; i--) {
            if (windows[i]->get_in_bounds(e.mouse.x, e.mouse.y)) {
                // pushes the window clicked to front of queue
                Window* q = windows[i];
                for (int j = i; j < num_windows-1; j++) { windows[j] = windows[j+1]; }
                windows[num_windows-1] = q;
                // checks what exactly was clicked in the window
                if (!windows[num_windows-1]->update_window(e)) { q->mouse_fixed = true; }
                return 1;
            }
        }
    } else if (e.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP) {
        for (int i = num_windows-1; i >= 0; i--) {
            ALLEGRO_BITMAP* bg = windows[i]->get_bitmap();
            if (e.mouse.x >= windows[i]->x && e.mouse.x <= windows[i]->x+al_get_bitmap_width(bg)
                && e.mouse.y >= windows[i]->y && e.mouse.y <= windows[i]->y+al_get_bitmap_height(bg)) {
                int width = al_get_bitmap_width(bg);
                if (e.mouse.x >= windows[i]->x+width-16 && e.mouse.x <= windows[i]->x+width-6
                    && e.mouse.y >= windows[i]->y+6 && e.mouse.y <= windows[i]->y+16) {
                    remove_window(windows[i]);
                } else if (!windows[i]->update_window(e)) {
                    if (windows[i]->mouse_fixed) {
                        windows[i]->mouse_fixed = false;
                    }
                }
                return 1;
            }
        }
    }
    return 0;
}

void close_all_windows() {
    delete[] windows;
}
