#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <cmath>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>

using namespace std;

ALLEGRO_BITMAP* get_tile_asset(int);
ALLEGRO_BITMAP* get_image_asset(int);
ALLEGRO_FONT* get_font();

bool get_mouse_pressed();
int get_mouse_x();
int get_mouse_y();

bool get_any_key_down();
bool get_key_down(int);

void set_input(bool);
ALLEGRO_USTR* get_input(bool);
void clear_input();

int update_from_event(ALLEGRO_EVENT);

void load_assets();

void load_objects();
int get_class_by_id(int);
int get_sprite_by_id(int);
char* get_name_by_id(int);
ALLEGRO_USTR* get_desc_by_id(int);
int* get_params_by_id(int);

int mode = 0;

int get_paused() {
    return ((mode & 1) != 0);
}
int get_command_line() {
    return ((mode & 2) != 0);
}
int get_hack_mover() {
    return ((mode & 4) != 0);
}

void toggle_pause() {
    if (!get_paused() || get_command_line()) {
        mode |= 1;
    } else {
        mode &= ~1;
    }
}
void toggle_command_line() {
    mode ^= 2;
    toggle_pause();
    set_input(get_command_line());
}
void toggle_hack_mover() {
    mode ^= 4;
}

ALLEGRO_COLOR dark;
ALLEGRO_COLOR transparent;
ALLEGRO_COLOR invisible;
ALLEGRO_COLOR sky;

ALLEGRO_COLOR s_pigment[4];
ALLEGRO_COLOR h_pigment[6];

void init_game() {
    dark = al_map_rgb(128, 128, 128);
    transparent = al_map_rgba(128, 128, 128, 128);
    invisible = al_map_rgba(64, 64, 64, 64);

    s_pigment[0] = al_map_rgb(230, 179, 126);
    s_pigment[1] = al_map_rgb(184, 128, 69);
    s_pigment[2] = al_map_rgb(122, 85, 46);
    s_pigment[3] = al_map_rgb(82, 57, 31);

    h_pigment[0] = al_map_rgb(46, 24, 0);
    h_pigment[1] = al_map_rgb(77, 40, 0);
    h_pigment[2] = al_map_rgb(13, 7, 0);
    h_pigment[3] = al_map_rgb(191, 156, 48);
    h_pigment[4] = al_map_rgb(191, 64, 0);
    h_pigment[5] = al_map_rgb(230, 230, 230);

    srand(time(NULL));
}

void draw_bitmap(ALLEGRO_BITMAP* bmp, float x, float y, int flags) {
    if (bmp == NULL) { cout << "ERROR: Tried to draw NULL bitmap.\n"; }
    else if (get_paused()) { al_draw_tinted_bitmap(bmp, dark, x, y, flags); }
    else { al_draw_bitmap(bmp, x, y, flags); }
}

void draw_tinted_bitmap(ALLEGRO_BITMAP* bmp, ALLEGRO_COLOR color, float x, float y, int flags) {
    if (bmp == NULL) { cout << "ERROR: Tried to draw NULL bitmap.\n"; }
    else { al_draw_tinted_bitmap(bmp, color, x, y, flags); }
}

void draw_ustr(ALLEGRO_USTR* ustr, float x, float y, int flags) {
    if (get_paused()) { al_draw_ustr(get_font(), dark, x, y, flags, ustr); }
    else { al_draw_ustr(get_font(), al_map_rgb(255, 255, 255), x, y, flags, ustr); }
}

class Point {
    public:
        int x;
        int y;
        Point() { x = 0; y = 0; }
        Point(int a,int b) : x(a), y(b) {};
        Point operator +(const Point&);
        Point operator -(const Point&);
        void operator +=(const Point&);
        void operator -=(const Point&);
        bool operator ==(const Point&);
        bool operator >=(const Point&);
        void set_to(int, int);
};

Point Point::operator+(const Point& point) {
    Point temp;
    temp.x = x + point.x;
    temp.y = y + point.y;
    return temp;
}

Point Point::operator-(const Point& point) {
    Point temp;
    temp.x = x - point.x;
    temp.y = y - point.y;
    return temp;
}

void Point::set_to(int i, int j) {
    x = i;
    y = j;
}

void Point::operator+=(const Point& point) {
    x += point.x;
    y += point.y;
}

void Point::operator-=(const Point& point) {
    x -= point.x;
    y -= point.y;
}

bool Point::operator==(const Point& point) {
    return (x == point.x && y == point.y);
}

bool Point::operator>=(const Point& point) {
    return (y >= point.y || (y == point.y && x >= point.x));
}

const int EAST = 0;
const int NORTHEAST = 1;
const int NORTH = 2;
const int NORTHWEST = 3;
const int WEST = 4;
const int SOUTHWEST = 5;
const int SOUTH = 6;
const int SOUTHEAST = 7;
int view_angle = SOUTHWEST;

int rel_direction(int dir) {
    return (dir-view_angle+10)%8;
}

Point* translate(Point* point, int angle, int distance = 1) {
    Point* temp = new Point();
    if (angle >= NORTHWEST && angle <= SOUTHWEST) {
        temp->x = -distance;
    } else if ((angle >= EAST && angle <= NORTHEAST) || angle == SOUTHEAST) {
        temp->x = distance;
    }
    if (angle >= NORTHEAST && angle <= NORTHWEST) {
        temp->y = -distance;
    } else if (angle >= SOUTHWEST && angle <= SOUTHEAST) {
        temp->y = distance;
    }
    *temp += *point;
    return temp;
}

int get_angle_of_vector(Point* p) {
    int a = (int)(atan2(-p->y, p->x)*4/3.1415926535);
    return (a+8)%8;
}

Point* center;

Point* convert_to_screen_coordinates(Point offset) {
    Point* temp = new Point(320, 201);
    temp->x += (offset.x+((view_angle%4 == 3 ? -1 : 1)*offset.y))*(view_angle > 4 ? -1 : 1);
    temp->y += (offset.y+((view_angle%4 == 1 ? -1 : 1)*offset.x))*(view_angle > 4 ? -1 : 1)/2;
    return temp;
}

Point* convert_from_screen_coordinates(Point crd) {
    Point* temp = new Point(center->x, center->y);
    temp->x += (((crd.x-320)/2)+((view_angle%4 == 1 ? -1 : 1)*(crd.y-201)))*(view_angle > 4 ? -1 : 1);
    temp->y += ((crd.y-201)+((view_angle%4 == 3 ? -1 : 1)*(crd.x-320)/2))*(view_angle > 4 ? -1 : 1);
    return temp;
}

bool get_within_sprite(int x, int y, ALLEGRO_BITMAP* bmp, int i, int j) {
    int width = al_get_bitmap_width(bmp);
    return (x >= i-(width/2) && x <= i+(width/2) && y >= j-al_get_bitmap_height(bmp) && y <= j);
}

bool get_within_stage(ALLEGRO_BITMAP* bitmap, int x, int y) {
    return ((x+(al_get_bitmap_width(bitmap)/2) >= 0 && y >= 0)
        || (x-(al_get_bitmap_width(bitmap)/2) <= 640 && y-al_get_bitmap_height(bitmap) <= 400));
}

class Domain {
    private:
        Point* pos;
        int dx;
        int dy;
        bool elliptical;
    public:
        Domain() {}
        void set_to(Point* p, int i, int j, bool e) { pos = p; dx = i; dy = j; elliptical = e; }
        bool get_point(Point*);
        bool intersects(Domain*);
};

bool Domain::get_point(Point* p) {
    if (p->x >= pos->x-dx && p->x <= pos->x+dx && p->y >= pos->y-dy && p->y <= pos->y+dy) {
        if (!elliptical || pow(1.0*(p->x-pos->x)/dx, 2.0)+pow(1.0*(p->y-pos->y)/dy, 2.0) <= 1) {
            return true;
        }
    }
    return false;
}

bool Domain::intersects(Domain* d) {
    if (pos->x-dx <= d->pos->x+d->dx && pos->x+dx >= d->pos->x-d->dx
        && pos->y-dy <= d->pos->y+d->dy && pos->y+dy >= d->pos->y-d->dy) {
        if (!elliptical && !d->elliptical) { return true; }
        else { return true; } // TODO change later???
    }
    // cout << "D1: <" << pos->x-dx << "," << pos->y-dy << "> -> <" << pos->x+dx << "," << pos->y+dy << ">\n";
    // cout << "D2: <" << d->pos->x-d->dx << "," << d->pos->y-d->dy << "> -> <" << d->pos->x+d->dx << "," << d->pos->y+d->dy << ">\n";
    return false;
}

class Object;
class Item;
class ItemContainer;
class Wall;
class Plant;
class Entity;
class Player;

Object* create_object(int);

class Area;

class Window;
class ContainerWindow;
class DescriptionWindow;
class ConversationWindow;

/**
 * Abstract, generic class for representing objects
 */
class Object {
    public:
        int id;
        int flags;
        int sprite;
        Point pos;
        Point mov;
        int dir;
        Domain dom;
        Area* loc;
        ALLEGRO_USTR* name;
    //public:
        Object(int);
        void add_flag(int);
        void remove_flag(int);
        bool get_flag(int);
        Point* get_position();
        void set_position(int, int);
        void set_position(Point*);
        int get_direction() { return dir; }
        Domain* get_domain();
        Area* get_locale();
        void set_locale(Area*);
        ALLEGRO_USTR* get_name();
        int get_sprite_flipped();
        void draw();
        void update();
        int on_click(ALLEGRO_EVENT);
        virtual int on_click() = 0;
        virtual ALLEGRO_BITMAP* get_sprite() = 0;
        virtual ALLEGRO_USTR* get_description() = 0;
};

/**
 * (mostly?) abstract class for representing ... stuff
 */
class Item: public Object {
    private:
        int space;
    public:
        ItemContainer* within;
        Item(int);
        ALLEGRO_BITMAP* get_sprite();
        bool get_within_space(int, int);
        int on_click();
        ALLEGRO_USTR* get_description();
};

class CompositeItem: public Item {
    private:
        int num_composed;
        Item* composed;
    public:
        CompositeItem(int, Item*);
};

/**
 * Class for items that contain other items, like a barrel
 */
class ItemContainer: public Item {
    friend class ContainerWindow;
    private:
        int num_items;
        Item** items;
        int width;
        int height;
    public:
        ItemContainer(int);
        Item* get_item(int, int);
        int add_item(Item*);
        int add_item(Item*, int, int);
        void remove_item(Item*);
        int on_click();
};

class Weapon: public Item {
    public:
        Weapon(int);
};

class Wall: public Item {
    public:
        Wall(int d) : Item(d) {}
        bool get_flag(int);
};

class Foodstuff: public Item {
    private:
        int hunger;
    public:
        Foodstuff(int d) : Item(d) { hunger = get_params_by_id(id)[0]; }
};

class Plant: public Item {
    private:
        int harvested;
        int harvest_id;
    public:
        Plant(int);
        int harvest();
        int on_click();
};

/**
 * Abstract class for representing people and creatures
 */
class Entity: public Object {
    private:
        float hunger;
        float thirst;
        float fatigue;
    public:
        int frame;
        Entity(int d) : Object(d), frame(0) {}
        ALLEGRO_BITMAP* get_sprite();
        void update();
        int on_click();
        ALLEGRO_USTR* get_description();
};

/**
 * Class for representing humanoid characters with the ability to equip items
 */
class Humanoid: public Entity {
    protected:
        int s_color;
        int h_color;
        int h_type;
        /**
         * LIST OF EQUIP AREAS
         *  0   Head
         *  1   Neck
         *  2   Chest
         *  3   Back
         *  4   Arms
         *  5   R. Hand
         *  6   L. Hand
         *  7   Waist
         *  8   Legs
         *  9   Feet
         */
        Item* equip[10];
    public:
        Humanoid(int d, int sc, int hc, int ht) : Entity(d), s_color(sc), h_color(hc), h_type(ht) {}
        void draw();
        void equip_to(Item*, int);
};

/**
 * The player character
 */
class Player: public Humanoid {
    private:
        int identifier;
    public:
        Player(int i, int sc, int hc, int ht) : Humanoid(1, sc, hc, ht) {}
        void update_player(ALLEGRO_EVENT);
};

/**
 * For Entities not controlled by the player (essentially, NPC AI)
 */
class NPC: public Entity {
    public:
        NPC(int d) : Entity(d) {}
};

/**
 * For human and kobold NPCs
 */
class Person: public NPC, public Humanoid {
    private:
        int interests;
        int personality;
    public:
        // TODO fix next line
        Person(int d) : NPC(d), Humanoid(d, 2, 1, 69) {}
};

/**
 * class for representing 64x64 tile / 2048x2048 unit spaces
 */
class Area {
    protected:
        int tiles[64][64];
    public:
        int num_objects;
        Object** objects;
        Area();
        ALLEGRO_BITMAP* get_tile(int, int);
        void add_object(Object*);
        void remove_object(Object*);
        void resort_objects();
        bool check_collisions(Object*);
        void update_objects(ALLEGRO_EVENT);
};

class OutdoorArea: public Area {
    protected:
        Point pos;
        Area* borders[8];
    public:
        OutdoorArea(int, int);
        Area* get_border(int);
};

Player* pc;

const int DRAW_DISTANCE = 13;
Area* main_area;

Object::Object(int d) {
    id = d;
    dir = view_angle;
    pos.set_to(1024, 1024);
    mov.set_to(0, 0);
    dom.set_to(&pos, 3, 3, false);
    loc = main_area;
    flags = 0;
    sprite = get_sprite_by_id(id);
    name = al_ustr_new(get_name_by_id(id));
}

/**
 * LIST OF FLAGS
 *  0   fixed/immobile
 *  1   invisible (or, basically, really REALLY translucent)
 *  2   can be walked through
 *  3   has multiple axes (different sprites at different angles vs. same sprite for all angles)
 *  4   flammable
 *  5   (Item only) bladed
 */

void Object::add_flag(int key) {
    flags |= (1 << key);
}

void Object::remove_flag(int key) {
    flags &= ~(1 << key);
}

bool Object::get_flag(int key) {
    return ((flags & (1 << key)) != 0);
}

Point* Object::get_position() {
    return &pos;
}

void Object::set_position(int i, int j) {
    pos.set_to(i, j);
}

void Object::set_position(Point* p) {
    pos = *p;
}

Domain* Object::get_domain() {
    return &dom;
}

Area* Object::get_locale() {
    return loc;
}

void Object::set_locale(Area* l) {
    loc = l;
}

ALLEGRO_USTR* Object::get_name() {
    return name;
}

int Object::get_sprite_flipped() {
    return (((rel_direction(dir)+6)%8 >= 5) ? ALLEGRO_FLIP_HORIZONTAL : 0);
}

void Object::draw() {
    Point* draw_at = convert_to_screen_coordinates(pos-*center);
    if (get_within_stage(get_sprite(), draw_at->x, draw_at->y)) {
        ALLEGRO_BITMAP* bitmap = get_sprite();
        if (get_flag(1)) {
            draw_tinted_bitmap(bitmap, invisible, draw_at->x-(al_get_bitmap_width(bitmap)/2),
                draw_at->y-al_get_bitmap_height(bitmap), get_sprite_flipped());
        } else {
            draw_bitmap(bitmap, draw_at->x-(al_get_bitmap_width(bitmap)/2),
                draw_at->y-al_get_bitmap_height(bitmap), get_sprite_flipped());
        }
    }
    delete draw_at;
}

void Object::update() {
    pos += mov;
    if (!loc->check_collisions(this)) {
        pos -= mov;
        int just_y = mov.y;
        mov.set_to(mov.x, 0);
        pos += mov;
        if (!loc->check_collisions(this)) {
            pos -= mov;
            mov.set_to(0, just_y);
            pos += mov;
            if (!loc->check_collisions(this)) { pos -= mov; }
        }
    }
    if (mov.x != 0 || mov.y != 0) {
        dir = get_angle_of_vector(&mov);
    }
}

Item::Item(int d) : Object(d) {
    add_flag(2);
    space = 0;
    int width = al_get_bitmap_width(get_sprite());
    int height = al_get_bitmap_height(get_sprite());
    for (int j = 0; j < 4; j++) {
        for (int i = 0; i < 4; i++) {
            if (width/10 > i && (height/10) >= j) {
                space |= (1 << ((4*(3-j))+(3-i)));
            }
        }
    }
    within = NULL;
}

ALLEGRO_BITMAP* Item::get_sprite() {
    return get_image_asset(sprite);
}

bool Item::get_within_space(int i, int j) {
    if (i < 0 || i >= 4 || j < 0 || j >= 4) { return false; }
    return ((space & (1 << ((3-i)+(4*(3-j))))) != 0);
}

ALLEGRO_USTR* Item::get_description() {
    return get_desc_by_id(id);
}

ItemContainer::ItemContainer(int d) : Item(d) {
    num_items = 0;
    items = new Item*[0];
    width = get_params_by_id(id)[0];
    height = get_params_by_id(id)[1];
    remove_flag(2);
}

Item* ItemContainer::get_item(int i, int j) {
    if (num_items == 0) { return NULL; }
    int index1 = 0, index2 = num_items-1;
    while (index1 != index2) {
        Point* p = items[(index1+index2)/2]->get_position();
        if (items[(index1+index2)/2]->get_within_space(i-p->x, j-p->y)) {
            delete p;
            return items[(index1+index2)/2];
        } else if (p->y < j || (p->y == j && p->x < i)) {
            index1 = (index1+index2+1)/2;
        } else if (p->y > j || (p->y == j && p->x > i)) {
            index2 = (index1+index2)/2;
        }
        delete p;
    }
    if (items[index1]->get_within_space(i-items[index1]->get_position()->x, j-items[index1]->get_position()->y)) {
        return items[(index1+index2)/2];
    } else {
        for (int k = 0; k < num_items; k++) {
            if (items[k]->get_within_space(i-items[k]->get_position()->x, j-items[k]->get_position()->y)) {
                return items[k];
            }
        }
        return NULL;
    }
}

int ItemContainer::add_item(Item* item) {
    cout << "Adding item (general): " << item << ".\n";
    for (int j = 0; j < height; j++) {
        for (int i = 0; i < width; i++) {
            if (get_item(i, j) == item) { cout << "Item already contained.\n"; return 1; }
            if (add_item(item, i, j)) { cout << "Item added (general).\n"; return 1; }
        }
    }
    return 0;
}

int ItemContainer::add_item(Item* item, int x, int y) {
    cout << "Adding item <" << x << "," << y << ">: " << item << "\n";
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (get_item(x+i, y+j) != NULL) {
                if (item->get_within_space(i, j)) {
                    if (item != get_item(x+i, y+j)) {
                        cout << "FAILURE TO ADD: INTERSECTION " << get_item(x+i, y+j) <<
                            " AT <" << x+i << "," << y+j << ">\n";
                        return 0;
                    }
                }
            } else if (x+i < 0 || x+i >= width || y+j < 0 || y+j >= height) {
                if (item->get_within_space(i, j)) { cout << "FAILURE TO ADD: OUT OF BOUNDS\n"; return 0; }
            }
        }
    }
    cout << "Item checked and go.\n";
    num_items++;
    Item** holder = new Item*[num_items];
    bool added = false;
    for (int i = 0; i < num_items; i++) {
        holder[i] = NULL;
        if (i < num_items-1) {
            Point* p = items[i]->get_position();
            if (!added && (p->y > y || (p->y == y && p->x > x))) {
                holder[i] = item;
                added = true;
            } else if (!added) {
                holder[i] = items[i];
            }
            delete p;
        }
        if (added && holder[i] == NULL) { holder[i] = items[i-1]; }
    }
    if (!added) { holder[num_items-1] = item; }
    item->set_position(x, y);
    delete[] items;
    cout << "Items position set.\n";
    items = holder;
    if (item->within != NULL) { item->within->remove_item(item); }
    item->within = this;
    return 1;
}

void ItemContainer::remove_item(Item* item) {
    num_items--;
    Item** temp = new Item*[num_items];
    bool removed = false;
    for (int i = 0; i < num_items+1; i++) {
        if (!removed && items[i] == item) {
            removed = true;
        } else if (!removed && i == num_items) {
            delete[] temp;
            return;
        } else if (!removed) {
            temp[i] = items[i];
        } else {
            temp[i-1] = items[i];
        }
    }
    item->within = NULL;
    delete[] items;
    items = temp;
}

Weapon::Weapon(int d) : Item(d) {

}

Plant::Plant(int d) : Item(d) {
    harvest_id = get_params_by_id(id)[0];
    harvested = -1;
    add_flag(0);
}

int Plant::harvest() {
    if (harvested < 0) {
        harvested = 0;
        return harvest_id;
    } else {
        return -1;
    }
}

void Entity::update() {
    Object::update();
    if (mov.x != 0 || mov.y != 0) { frame = (frame+1)%60; }
    else { frame = 0; }

}

ALLEGRO_BITMAP* Entity::get_sprite() {
    int d = rel_direction(dir);
    d = ((d <= 1 || d >= 7) ? 8-((d+6)%8) : d-2);
    return get_image_asset(sprite+((frame/5)%4)+(4*d));
}

ALLEGRO_USTR* Entity::get_description() {
    // TODO fix later
    return get_desc_by_id(id);
}

void Humanoid::draw() {
    int d = rel_direction(dir);
    d = ((d <= 1 || d >= 7) ? (8-((d+6)%8)) : (d-2));
    int i = ((frame/5)%4)+(4*d);

    Point* draw_at = convert_to_screen_coordinates(pos-*center);
    if (get_within_stage(get_image_asset(i), draw_at->x, draw_at->y)) {
        ALLEGRO_BITMAP *bmp1 = get_image_asset(sprite+i);
        ALLEGRO_BITMAP *bmp2 = get_image_asset(sprite+i+20);
        ALLEGRO_BITMAP *bmp3, *bmp4;
        if (h_type >= 0) {
            bmp3 = get_image_asset(h_type+i);
            bmp4 = get_image_asset(h_type+i+20);
        }
        draw_at->set_to(draw_at->x-(al_get_bitmap_width(bmp1)/2), draw_at->y-al_get_bitmap_height(bmp1));
        int flip = get_sprite_flipped();
        if (get_flag(1)) {
            unsigned char *r, *g, *b;
            al_unmap_rgb(s_pigment[s_color], r, g, b);
            draw_tinted_bitmap(bmp1, al_map_rgba((int)r/4, (int)g/4, (int)b/4, 64), draw_at->x, draw_at->y, flip);
            draw_tinted_bitmap(bmp2, invisible, draw_at->x, draw_at->y, flip);
            // draw clothing
            if (h_type >= 0) {
                al_unmap_rgb(h_pigment[h_color], r, g, b);
                draw_tinted_bitmap(bmp3, al_map_rgba((int)r/4, (int)g/4, (int)b/4, 64), draw_at->x, draw_at->y, flip);
                draw_tinted_bitmap(bmp4, invisible, draw_at->x, draw_at->y, flip);
            }
        } else {
            draw_tinted_bitmap(bmp1, s_pigment[s_color], draw_at->x, draw_at->y, flip);
            draw_bitmap(bmp2, draw_at->x, draw_at->y, flip);
            // draw clothing
            if (h_type >= 0) {
                draw_tinted_bitmap(bmp3, h_pigment[h_color], draw_at->x, draw_at->y, flip);
                draw_bitmap(bmp4, draw_at->x, draw_at->y, flip);
            }
        }
    }
    delete draw_at;
}

void Humanoid::equip_to(Item* item, int a) {
    if (a >= 10 || a < 0) { return; }
    if (a == 3) {
        if (equip[a] != NULL) {
            equip[a]->set_position(translate(&pos, (view_angle+4)%8));
            loc->add_object(equip[a]);
        }
    } else {
        if (equip[a] != NULL) {
            if (ItemContainer* cont = dynamic_cast<ItemContainer*>(equip[3])) {
                if (!cont->add_item(equip[a])) {
                    equip[a]->set_position(translate(&pos, (view_angle+4)%8));
                    loc->add_object(equip[a]);
                }
            }
        }
    }
    equip[a] = item;
}

void Player::update_player(ALLEGRO_EVENT e) {
    mov.set_to(0, 0);
    Point* origin = new Point();
    if (get_key_down(0) && !get_key_down(2)) { // UP key
        mov += *translate(origin, view_angle);
    } else if (get_key_down(2) && !get_key_down(0)) { // DOWN key
        mov += *translate(origin, (view_angle+4)%8);
    }
    if (get_key_down(1) && !get_key_down(3)) { // LEFT key
        mov += *translate(origin, (view_angle+2)%8);
    } else if (get_key_down(3) && !get_key_down(1)) { // RIGHT key
        mov += *translate(origin, (view_angle+6)%8);
    }
    delete origin;
    update();
}

Area::Area() {
    // load the area here
    for (int i = 0; i < 64; i++) {
        for (int j = 0; j < 64; j++) { tiles[i][j] = (i+j)%3; }
    }
    num_objects = 0;
}

ALLEGRO_BITMAP* Area::get_tile(int x, int y) {
    // TODO transfer to OutdoorArea
    /*if (x >= 64) { return borders[0]->get_tile(x-64, y); }
    else if (y < 0) { return borders[1]->get_tile(x, y+64); }
    else if (x < 0) { return borders[2]->get_tile(x+64, y); }
    else if (y >= 64) { return borders[3]->get_tile(x, y-64); }*/
    if (x < 0 || x >= 64 || y < 0 || y >= 64) { return NULL; }
    else { return get_tile_asset(tiles[x][y]); }
}

/**
 * @return true if o1 should be in front of o2, false otherwise
 */
bool evaluate_objects(Object* o1, Object* o2) {
    // TODO fix latter part of evaluation???
    Point* o1_pos = convert_to_screen_coordinates(*(o1->get_position())-*center);
    Point* o2_pos = convert_to_screen_coordinates(*(o2->get_position())-*center);
    bool b = *o1_pos >= *o2_pos;
    delete o1_pos;
    delete o2_pos;
    return b;
    /*Point* o1_pos = o1->get_position();
    Point* o2_pos = o2->get_position();
    if (view_angle == NORTHWEST || view_angle == SOUTHEAST) {
        return (o1_pos->x+o1_pos->y >= o2_pos->x+o2_pos->y);
    } else if (view_angle == SOUTHEAST) {
        return (o1_pos->x+o1_pos->y <= o2_pos->x+o2_pos->y);
    } else if (view_angle == NORTHEAST) {
        return (o1_pos->x-o1_pos->y >= o2_pos->x-o2_pos->y);
    } else {
        return (o1_pos->x-o1_pos->y <= o2_pos->x-o2_pos->y);
    }*/
}

void Area::add_object(Object* object) {
    num_objects++;
    Object** temp = new Object*[num_objects];
    bool added = false;
    for (int i = 0; i < num_objects; i++) {
        if (!added && i < num_objects-1 && evaluate_objects(objects[i], object)) {
            added = true;
            temp[i] = object;
        } else if (!added) {
            if (i == num_objects-1) { temp[i] = object; added = true; }
            else { temp[i] = objects[i]; }
        } else {
            temp[i] = objects[i-1];
        }
    }
    if (Item* item = dynamic_cast<Item*>(object)) {
        if (item->within != NULL) { item->within->remove_item(item); }
    }
    object->set_locale(this);
    delete[] objects;
    objects = temp;
}

void Area::remove_object(Object* object) {
    num_objects--;
    Object** temp = new Object*[num_objects];
    bool removed = false;
    for (int i = 0; i < num_objects; i++) {
        if (!removed && objects[i] == object) {
            removed = true;
        } else if (!removed && i == num_objects) {
            delete[] temp;
            return;
        } else if (!removed) {
            temp[i] = objects[i];
        } else {
            temp[i-1] = objects[i];
        }
    }
    object->set_locale(NULL);
    delete[] objects;
    objects = temp;
}

void Area::resort_objects() {
    for (int i = 0; i < num_objects; i++) {
        if (Wall* wall = dynamic_cast<Wall*>(objects[i])) {
            if (*wall->get_position() >= *center) {
                wall->add_flag(1);
            } else {
                wall->remove_flag(1);
            }
        }
        if (i < num_objects-1 && evaluate_objects(objects[i], objects[i+1])) {
            Object* holder = objects[i];
            objects[i] = objects[i+1];
            objects[i+1] = holder;
        } else if (i > 0 && evaluate_objects(objects[i-1], objects[i])
            && !evaluate_objects(objects[i], objects[i-1])) {
            Object* holder = objects[i];
            objects[i] = objects[i-1];
            objects[i-1] = holder;
            i -= 2;
        }
    }
}

/**
 * Returns false if collision detected, true if none
 */
bool Area::check_collisions(Object* o) {
    for (int i = 0; i < num_objects; i++) {
        if (o->get_domain()->intersects(objects[i]->get_domain()) && !objects[i]->get_flag(2)) {
            if (o != objects[i]) { return false; }
        }
    }
    return true;
}

OutdoorArea::OutdoorArea(int x, int y) : Area() {
    pos.set_to(x, y);
}

Area* OutdoorArea::get_border(int dir) {
    return borders[dir];
}

void draw_at_center() {
    // DRAW THE TILES OF THE TERRAIN
    Point* indices = new Point(center->x/32, center->y/32);
    Point* offset = new Point(center->x%32, center->y%32);
    // if facing NW or SE, calculates offset from y=x line; else if facing SW or NE, calculates offset from y=-x line
    // then, if facing SW or SE, inverts this value
    int disx = (view_angle%4 == 3 ? offset->x-offset->y : offset->x+offset->y)*(view_angle > 4 ? 1 : -1);
    disx += (view_angle == NORTHEAST ? 32 : (view_angle == SOUTHWEST ? -32 : 0));
    // calculates offset from farthest point in tile: (for indices j and k)
    //      for NW3, (32*j,32*k)
    //      for SW5, (32*j,32*(k+1))
    //      for SE7, (32*(j+1),32*(k+1))
    //      for NE1, (32*(j+1),32*k)
    int disy = -((view_angle%6 == 1 ? 32-offset->x : offset->x)+(view_angle > 4 ? 32-offset->y : offset->y))/2;
    Point* basis = translate(indices, view_angle, DRAW_DISTANCE);
    for (int i = -DRAW_DISTANCE; i <= DRAW_DISTANCE; i++) {
        if (i != -DRAW_DISTANCE) { basis = translate(basis, (view_angle+5)%8); }
        for (int j = -DRAW_DISTANCE; j <= DRAW_DISTANCE; j++) {
            int x = 320+(32*(i-j))+disx-32;
            int y = 200+(16*(i+j))+disy;
            if (x > -64 && x <= 640 && y > -32 && y <= 400) {
                Point* p = translate(basis, (view_angle+3)%8, j+DRAW_DISTANCE);
                ALLEGRO_BITMAP *tile = main_area->get_tile(p->x, p->y);
                draw_bitmap(tile, x, y, 0);

                /* char* s = new char[5];
                itoa(p->x, &s[0], 10);
                s[2] = ',';
                itoa(p->y, &s[3], 10);
                ALLEGRO_USTR* lca = al_ustr_new(s);
                draw_ustr(lca, x+16, y+10, 0);
                al_ustr_free(lca);
                delete s; */

                delete p;
            }
        }
    }

    // DRAW THE OBJECTS IN THE AREA
    for (int i = 0; i < main_area->num_objects; i++) {
        if (Humanoid* hmnd = dynamic_cast<Humanoid*>(main_area->objects[i])) {
            hmnd->draw();
        } else {
            main_area->objects[i]->draw();
        }
    }

    // cleanup
    delete indices;
    delete offset;
    delete basis;

    // remove this later
    al_put_pixel(320, 200, al_map_rgb(0, 0, 0));
}

Object* dragging = NULL;
Point affixation(0, 0);
ALLEGRO_USTR* mouse_desc = NULL;

class Window {
    public:
        int x;
        int y;
        bool mouse_fixed;
        Window();
        virtual int update_window(ALLEGRO_EVENT) = 0;
        virtual ALLEGRO_BITMAP* get_bitmap() = 0;
        virtual void draw() = 0;
};

Window::Window() {
    x = y = 100;
    mouse_fixed = false;
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

class ContainerWindow: public Window {
    private:
        ItemContainer* cont;
    public:
        ContainerWindow(ItemContainer* ic) : cont(ic) { cout << "New ContainerWindow."; }
        int update_window(ALLEGRO_EVENT);
        ALLEGRO_BITMAP* get_bitmap() { return get_image_asset(1); }
        void draw();
};

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
                draw_bitmap(get_image_asset(2), x+xd+(10*i), y+yd+(10*j), 0);
            } else {
                if (item->get_position()->x == i && item->get_position()->y == j) {
                    draw_bitmap(item->get_sprite(), x+xd+(10*i), y+yd+(10*j), 0);
                }
            }
        }
    }
}

class DescriptionWindow: public Window {
    private:
        Object* object;
        ALLEGRO_USTR* text;
    public:
        DescriptionWindow(Object* obj) : object(obj) { text = object->get_description(); }
        int update_window(ALLEGRO_EVENT e) { return 0; }
        ALLEGRO_BITMAP* get_bitmap() { return get_image_asset(1); }
        void draw();
};

void DescriptionWindow::draw() {
    draw_bitmap(get_bitmap(), x, y, 0);
    ALLEGRO_BITMAP* bmp = object->get_sprite();
    draw_bitmap(bmp, x+69-(al_get_bitmap_width(bmp)/2), y+25-(al_get_bitmap_height(bmp)/2),
        (((rel_direction(object->get_direction())+6)%8 >= 5) ? ALLEGRO_FLIP_HORIZONTAL : 0));
    draw_ustr(text, x+9, y+53, 0);
}

class ConversationWindow: public Window {
    private:
        Entity* ent;
    public:
        ConversationWindow(Entity* e) : ent(e) { x = y = 0; }
        int update_window(ALLEGRO_EVENT) { return 0; }
        ALLEGRO_BITMAP* get_bitmap() { return get_image_asset(1); }
        void draw();
};

void ConversationWindow::draw() {
    draw_bitmap(get_bitmap(), x, y, 0);
    ALLEGRO_BITMAP* bmp = ent->get_sprite();
    draw_bitmap(bmp, x+69-(al_get_bitmap_width(bmp)/2), y+25-(al_get_bitmap_height(bmp)/2),
        (((rel_direction(ent->get_direction())+6)%8 >= 5) ? ALLEGRO_FLIP_HORIZONTAL : 0));
}

class EquipmentWindow: public Window {
    private:
        Humanoid* person;
    public:
        EquipmentWindow(Humanoid* h) : person(h) {}
        int update_window(ALLEGRO_EVENT) { return 0; }
        ALLEGRO_BITMAP* get_bitmap() { return get_image_asset(1); }
        void draw();
};

void EquipmentWindow::draw() {
    draw_bitmap(get_bitmap(), x, y, 0);
    ALLEGRO_BITMAP* bmp = person->get_sprite();
    draw_bitmap(bmp, x+69-(al_get_bitmap_width(bmp)/2), y+25-(al_get_bitmap_height(bmp)/2),
        (((rel_direction(person->get_direction())+6)%8 >= 5) ? ALLEGRO_FLIP_HORIZONTAL : 0));
}

int Object::on_click(ALLEGRO_EVENT e) {
    cout << "Object clicked.\n";
    if ((dragging == NULL || dragging == this) && e.mouse.button == 2) {
        add_window(new DescriptionWindow(this));
        return 1;
    } else {
        return this->on_click();
    }
    return 0;
}

int Entity::on_click() {
    if (dragging == NULL || dragging == this) {
        add_window(new ConversationWindow(this));
        return 1;
    }
    return 0;
}

int Item::on_click() {
    cout << "Item clicked.\n";
    if (ItemContainer* cont = dynamic_cast<ItemContainer*>(this)) {
        return cont->on_click();
    }
    return 0;
}

int ItemContainer::on_click() {
    cout << "Item container clicked.\n";
    if (dragging == NULL || dragging == this) {
        add_window(new ContainerWindow(this));
        return 1;
    } else if (Item* item = dynamic_cast<Item*>(dragging)) {
        add_item(item);
        return 1;
    }
    cout << "Function of click executed.\n";
    return 0;
}

int Plant::on_click() {
    Object* hrvst = create_object(harvest());
    if (hrvst != NULL) {
        hrvst->set_position(translate(&pos, (view_angle+4)%8));
        loc->add_object(hrvst);
    }
    return 1;
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
            ALLEGRO_BITMAP* bg = windows[i]->get_bitmap();
            if (e.mouse.x >= windows[i]->x && e.mouse.x <= windows[i]->x+al_get_bitmap_width(bg)
                && e.mouse.y >= windows[i]->y && e.mouse.y <= windows[i]->y+al_get_bitmap_height(bg)) {
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

int ContainerWindow::update_window(ALLEGRO_EVENT e) {
    int xd = 69-(5*cont->width);
    int yd = 83-(5*cont->height);
    if (e.type == ALLEGRO_EVENT_MOUSE_AXES) {
        int i = (e.mouse.x-x-xd)/10;
        int j = (e.mouse.y-y-yd)/10;
        if (i >= 0 && i < cont->width && j >= 0 && j < cont->height) {
            Item* item = cont->get_item(i, j);
            if (item == NULL) {
                mouse_desc = NULL;
            } else {
                mouse_desc = item->get_name();
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
        delete p;
    }
    return 0;
}

void Area::update_objects(ALLEGRO_EVENT e) {
    if (e.type == ALLEGRO_EVENT_MOUSE_AXES) {
        for (int i = num_objects-1; i >= 0; i--) {
            Point* p = convert_to_screen_coordinates(*(objects[i]->get_position())-*center);
            if (get_within_sprite(e.mouse.x, e.mouse.y, objects[i]->get_sprite(), p->x, p->y) && !objects[i]->get_flag(1)) {
                mouse_desc = objects[i]->get_name();
                delete p;
                return;
            }
            delete p;
        }
        mouse_desc = NULL;
    } else if (e.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
        for (int i = num_objects-1; i >= 0; i--) {
            Point* p = convert_to_screen_coordinates(*(objects[i]->get_position())-*center);
            if (get_within_sprite(e.mouse.x, e.mouse.y, objects[i]->get_sprite(), p->x, p->y) && !objects[i]->get_flag(1)) {
                if (get_hack_mover()) {
                    dragging = objects[i];
                    ALLEGRO_BITMAP* bmp = dragging->get_sprite();
                    affixation.set_to((p->x-(al_get_bitmap_width(bmp)/2))-e.mouse.x,
                        (p->y-al_get_bitmap_height(bmp))-e.mouse.y);
                } else if (Item* item = dynamic_cast<Item*>(objects[i])) {
                    if (!item->get_flag(0) && dragging == NULL) {
                        dragging = item;
                        ALLEGRO_BITMAP* bmp = item->get_sprite();
                        affixation.set_to((p->x-(al_get_bitmap_width(bmp)/2))-e.mouse.x,
                            (p->y-al_get_bitmap_height(bmp))-e.mouse.y);
                    }
                }
                delete p;
                return;
            }
            delete p;
        }
    } else if (e.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP) {
        for (int i = num_objects-1; i >= 0; i--) {
            Point* p = convert_to_screen_coordinates(*(objects[i]->get_position())-*center);
            if (get_within_sprite(e.mouse.x, e.mouse.y, objects[i]->get_sprite(), p->x, p->y) && !objects[i]->get_flag(1)) {
                // do the click function
                objects[i]->on_click(e);
                delete p;
                return;
            }
            delete p;
        }
        if (dragging != NULL) {
            ALLEGRO_BITMAP* bmp = dragging->get_sprite();
            Point* p = new Point(e.mouse.x+affixation.x+(al_get_bitmap_width(bmp)/2),
                e.mouse.y+affixation.y+al_get_bitmap_height(bmp));
            dragging->set_position(convert_from_screen_coordinates(*p));
            cout << "<" << dragging->get_position()->x << "," << dragging->get_position()->y << ">\n";
            delete p;
            main_area->add_object(dragging);
        }
    }
}

Object* create_object(int id) {
    switch (get_class_by_id(id)) {
        case 1:
            return new Item(id);
        case 2:
            return new ItemContainer(id);
        case 3:
            return new Plant(id);
        case 4:
            return new Entity(id);
        default:
            return new Item(0);
    }
}

int run_game(ALLEGRO_EVENT_QUEUE* events) {
    cout << "[GAME] Game begun.\n";
    main_area = new Area();
    // pc = new Player();
    center = pc->get_position();
    main_area->add_object(pc);

    cout << "[GAME] Now drawing...\n";
    draw_at_center();
    al_flip_display();

    bool update = true;
    while (true) {
        ALLEGRO_EVENT e;
        al_wait_for_event(events, &e);
        if (e.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            delete[] windows;
            return 0;
        } else if (e.type == ALLEGRO_EVENT_TIMER) {
            // do something
            update = get_any_key_down();
        } else {
            update = true;
            switch (update_from_event(e)) {
                case 1: // PAUSE key
                    toggle_pause();
                    break;
                case 2: // ENTER key, interprets text
                    if (get_command_line()) {
                        ALLEGRO_USTR* input = get_input(false);
                        if (al_ustr_find_cstr(input, 0, "spawn ") >= 0) {
                            al_ustr_find_replace_cstr(input, 0, "spawn ", "");
                            dragging = create_object(atoi(&al_cstr(input)[0]));
                            affixation.set_to(-al_get_bitmap_width(dragging->get_sprite())/2,
                                -al_get_bitmap_height(dragging->get_sprite())/2);
                        } else if (al_ustr_find_cstr(input, 0, "toggle ") >= 0) {
                            if (al_ustr_find_cstr(input, 7, "hm") >= 0) {
                                toggle_hack_mover();
                            }
                        }
                        clear_input();
                    }
                    break;
                case 3: // F1, starts or stops command line
                    toggle_command_line();
                    break;
                case 4: // OTHER
                    if (update_windows(e) == 0) {
                        // do something???
                        main_area->update_objects(e);
                    }
                    break;
                default: // NONE
                    update = false;
            }
            if (e.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP && dragging != NULL) {
                dragging = NULL;
            }
        }
        main_area->resort_objects();
        if (update) {
            update = false;
            if (!get_paused()) { pc->update_player(e); }
            al_clear_to_color(dark);
            draw_at_center();
            draw_windows();
            if (get_command_line()) {
                al_draw_ustr(get_font(), al_map_rgb(255, 255, 255), 10, 10, 0, get_input(true));
            }
            if (dragging != NULL) {
                draw_tinted_bitmap(dragging->get_sprite(), transparent,
                    get_mouse_x()+affixation.x, get_mouse_y()+affixation.y, 0);
            }
            if (mouse_desc != NULL) {
                if (get_mouse_x()+12+(6*(int)al_ustr_length(mouse_desc)) > 640) {
                    draw_ustr(mouse_desc, get_mouse_x()-(6*(int)al_ustr_length(mouse_desc)),
                        get_mouse_y(), 0);
                } else {
                    draw_ustr(mouse_desc, get_mouse_x()+12, get_mouse_y(), 0);
                }
            }
            al_flip_display();
        }
    }
    return 1;
}

int new_game(ALLEGRO_EVENT_QUEUE* events) {
    init_game();

    int hair_color = 1;
    int hair_style = 0;
    int skin_color = 2;
    int portrait_type = 0;

    int dir = SOUTHWEST;
    int frame = 0;

    bool update = true;
    while (true) {
        ALLEGRO_EVENT e;
        al_wait_for_event(events, &e);
        if (e.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            delete[] windows;
            return 0;
        } else if (e.type == ALLEGRO_EVENT_TIMER) {
            frame = (frame+1)%40;
            if (frame%10 == 0) { update = true; }
        } else if (e.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP) {
            if (e.mouse.y >= 282 && e.mouse.y <= 300) {
                if (e.mouse.x >= 229 && e.mouse.x <= 289) { // CONFIRM button
                    break;
                } else if (e.mouse.x >= 349 && e.mouse.x <= 409) { // CANCEL button
                    return 1;
                }
            } else if (e.mouse.x >= 263 && e.mouse.x <= 269) {
                if (e.mouse.y >= 151 && e.mouse.y <= 159) { // LEFT for HAIR STYLE
                    hair_style = (hair_style+1)%2;
                } else if (e.mouse.y >= 167 && e.mouse.y <= 175) { // LEFT for HAIR COLOR
                    hair_color = (hair_color+5)%6;
                } else if (e.mouse.y >= 183 && e.mouse.y <= 191) { // LEFT for SKIN COLOR
                    skin_color = (skin_color+3)%4;
                }
            } else if (e.mouse.x >= 335 && e.mouse.x <= 341) {
                if (e.mouse.y >= 151 && e.mouse.y <= 159) { // RIGHT for HAIR STYLE
                    hair_style = (hair_style+1)%2;
                } else if (e.mouse.y >= 167 && e.mouse.y <= 175) { // RIGHT for HAIR COLOR
                    hair_color = (hair_color+1)%6;
                } else if (e.mouse.y >= 183 && e.mouse.y <= 191) { // RIGHT for SKIN COLOR
                    skin_color = (skin_color+1)%4;
                }
            } else if (e.mouse.y >= 188 && e.mouse.y <= 197) {
                if (e.mouse.x >= 202 && e.mouse.x <= 213) { // ROTATE CLOCKWISE
                    dir = (dir+7)%8;
                } else if (e.mouse.x >= 232 && e.mouse.x <= 243) { // ROTATE COUNTERCLOCKWISE
                    dir = (dir+1)%8;
                }
            } else if (e.mouse.y >= 167 && e.mouse.y <= 175) {
                if (e.mouse.x >= 376 && e.mouse.x <= 382) { // LEFT for PORTRAIT
                    portrait_type = (portrait_type+1)%2;
                } else if (e.mouse.x >= 428 && e.mouse.x <= 434) { // RIGHT for PORTRAIT
                    portrait_type = (portrait_type+1)%2;
                }
            }
            if (e.mouse.x >= 191 && e.mouse.x <= 451 && e.mouse.y >= 112 && e.mouse.y <= 131) {
                set_input(true);
                // TODO calc where to place cursor
            } else {
                set_input(false);
            }
        }

        if (update) {
            al_clear_to_color(al_map_rgb(179, 152, 125));
            al_draw_bitmap(get_image_asset(1), 140, 80, 0);

            //al_draw_bitmap(get_image_asset(8), 200, 200, 0);
            int d = dir;
            d = ((d <= 1 || d >= 7) ? 8-((d+6)%8) : d-2);
            al_draw_tinted_bitmap(get_image_asset(9+(frame/10)+(4*d)), s_pigment[skin_color], 216, 149, (dir <= 1 || dir >= 7) ? ALLEGRO_FLIP_HORIZONTAL : 0);

            al_draw_bitmap(get_image_asset(149+(frame/10)+(4*d)), 216, 149, (dir <= 1 || dir >= 7) ? ALLEGRO_FLIP_HORIZONTAL : 0);
            al_draw_bitmap(get_image_asset(129+(frame/10)+(4*d)), 216, 149, (dir <= 1 || dir >= 7) ? ALLEGRO_FLIP_HORIZONTAL : 0);

            al_draw_bitmap(get_image_asset(29+(frame/10)+(4*d)), 216, 149, (dir <= 1 || dir >= 7) ? ALLEGRO_FLIP_HORIZONTAL : 0);
            al_draw_tinted_bitmap(get_image_asset(49+(40*hair_style)+(frame/10)+(4*d)), h_pigment[hair_color], 216, 149, (dir <= 1 || dir >= 7) ? ALLEGRO_FLIP_HORIZONTAL : 0);
            al_draw_bitmap(get_image_asset(69+(40*hair_style)+(frame/10)+(4*d)), 216, 149, (dir <= 1 || dir >= 7) ? ALLEGRO_FLIP_HORIZONTAL : 0);

            al_flip_display();
            update = false;
        }
    }

    pc = new Player(rand() % 100, skin_color, hair_color, 49+(40*hair_style));
    return run_game(events);
}

int load_game(ALLEGRO_EVENT_QUEUE* events) {
    init_game();

    pc = new Player(rand() % 100, 2, 1, 69);
    return run_game(events);
}
