#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <time.h>
#include <cmath>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>

using namespace std;

ALLEGRO_BITMAP* get_tile_asset(int);
ALLEGRO_BITMAP* get_image_asset(int);
ALLEGRO_BITMAP* get_image_sprite(int);
ALLEGRO_BITMAP* get_portrait(int);
ALLEGRO_BITMAP* get_interface_asset(int);
ALLEGRO_FONT* get_font(int);

bool get_mouse_pressed();
int get_mouse_x();
int get_mouse_y();

bool get_any_key_down();
bool get_key_down(int);

int add_new_input_string();
void remove_input_string_at(int);
int set_input_string(int);
ALLEGRO_USTR* get_input();
ALLEGRO_USTR* get_input(int);
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
    set_input_string(get_command_line() ? 0 : -1);
}
void toggle_hack_mover() {
    mode ^= 4;
}

ALLEGRO_COLOR white;
ALLEGRO_COLOR black;
ALLEGRO_COLOR dark;
ALLEGRO_COLOR transparent;
ALLEGRO_COLOR invisible;
ALLEGRO_COLOR sky;

ALLEGRO_COLOR s_pigment[4];
ALLEGRO_COLOR h_pigment[6];

ALLEGRO_USTR* description[21];

void init_game() {
    add_new_input_string();

    white = al_map_rgb(255, 255, 255);
    black = al_map_rgb(0, 0, 0);
    dark = al_map_rgb(128, 128, 128);
    transparent = al_map_rgba(128, 128, 128, 128);
    invisible = al_map_rgba(64, 64, 64, 64);

    s_pigment[0] = al_map_rgb(230, 179, 126);
    s_pigment[1] = al_map_rgb(184, 128, 69);
    s_pigment[2] = al_map_rgb(122, 85, 46);
    s_pigment[3] = al_map_rgb(82, 57, 31);

    h_pigment[0] = al_map_rgb(46, 24, 0); // dark brown
    h_pigment[1] = al_map_rgb(77, 40, 0); // brown
    h_pigment[2] = al_map_rgb(13, 7, 0); // black
    h_pigment[3] = al_map_rgb(191, 156, 48); // blond
    h_pigment[4] = al_map_rgb(191, 64, 0); // red
    h_pigment[5] = al_map_rgb(230, 230, 230); // silver or white

    description[0] = al_ustr_new("Health");
    description[1] = al_ustr_new("Fatigue");
    description[2] = al_ustr_new("Hunger");
    description[3] = al_ustr_new("Unarmed: Punching and grappling attacks");
    description[4] = al_ustr_new("Blunt: Maces, flails, and other blunt weapons");
    description[5] = al_ustr_new("Dagger: Daggers and rapiers");
    description[6] = al_ustr_new("Spear: Spears and halberds");
    description[7] = al_ustr_new("Sword: Light and heavy swords");
    description[8] = al_ustr_new("Axe: Handaxes and battleaxes");
    description[9] = al_ustr_new("Ranged: Bows, crossbows, and firearms");
    description[10] = al_ustr_new("Thrown: Slings and thrown weapons");
    description[11] = al_ustr_new("Shields: Blocking and shield bashing");
    description[12] = al_ustr_new("Armor: Proficiency with armor");
    description[13] = al_ustr_new("Smithing: Crafting of new weapons and armor");
    description[14] = al_ustr_new("Negotiation: Better prices with buying and selling");
    description[15] = al_ustr_new("Healing: Ability to treat wounds");
    description[16] = al_ustr_new("Stealth: Hiding and sneaking without alerting others");
    description[17] = al_ustr_new("Pickpocket: Filching items from people's pockets");
    description[18] = al_ustr_new("Lockpicking: Opening locks and disabling traps");
    description[19] = al_ustr_new("Strength: Ability to carry items without encumberance");
    description[20] = al_ustr_new("Athletics: Sprinting and swimming");

    srand(time(NULL));
}

void draw_bitmap(ALLEGRO_BITMAP* bmp, float x, float y, int flags) {
    if (bmp == NULL) { return; cout << "ERROR: Tried to draw NULL bitmap.\n"; }
    else if (get_paused()) { al_draw_tinted_bitmap(bmp, dark, x, y, flags); }
    else { al_draw_bitmap(bmp, x, y, flags); }
}

void draw_tinted_bitmap(ALLEGRO_BITMAP* bmp, ALLEGRO_COLOR color, float x, float y, int flags) {
    if (bmp == NULL) { return; cout << "ERROR: Tried to draw NULL bitmap.\n"; }
    else { al_draw_tinted_bitmap(bmp, color, x, y, flags); }
}

void draw_ustr(ALLEGRO_USTR* ustr, float x, float y, int flags, int font = 0) {
    if (get_paused()) { al_draw_ustr(get_font(font), dark, x, y, flags, ustr); }
    else { al_draw_ustr(get_font(font), al_map_rgb(255, 255, 255), x, y, flags, ustr); }
}

class Point {
    public:
        float x;
        float y;
        int z;
        Point() { x = 0; y = 0; z = 0; }
        Point(float a, float b) : x(a), y(b), z(0) {};
        Point(float a, float b, int c) : x(a), y(b), z(c) {};
        Point operator +(const Point&);
        Point operator -(const Point&);
        void operator +=(const Point&);
        void operator -=(const Point&);
        bool operator ==(const Point&);
        bool operator >=(const Point&);
        void set_to(float, float, int=-1);
        float length(bool=false);
};

Point Point::operator+(const Point& point) {
    Point temp;
    temp.x = x + point.x;
    temp.y = y + point.y;
    temp.z = z + point.z;
    return temp;
}

Point Point::operator-(const Point& point) {
    Point temp;
    temp.x = x - point.x;
    temp.y = y - point.y;
    temp.z = z - point.z;
    return temp;
}

void Point::set_to(float i, float j, int k) {
    x = i;
    y = j;
    if (k >= 0) { z = k; }
}

void Point::operator+=(const Point& point) {
    x += point.x;
    y += point.y;
    z += point.z;
}

void Point::operator-=(const Point& point) {
    x -= point.x;
    y -= point.y;
    z -= point.z;
}

bool Point::operator==(const Point& point) {
    return (x == point.x && y == point.y && z == point.z);
}

bool Point::operator>=(const Point& point) {
    return (y >= point.y || (y == point.y && x >= point.x) || (y == point.y && x == point.x && z >= point.z));
}

float Point::length(bool include_z) {
    if (include_z) {
        return sqrt(pow(x, 2)+pow(y, 2)+pow(z, 2));
    } else {
        return sqrt(pow(x, 2)+pow(y, 2));
    }
}

const int EAST = 0;
const int NORTHEAST = 1;
const int NORTH = 2;
const int NORTHWEST = 3;
const int WEST = 4;
const int SOUTHWEST = 5;
const int SOUTH = 6;
const int SOUTHEAST = 7;
int view_angle = NORTHWEST;

void rotate_view_clockwise() {
    view_angle = (view_angle+6)%8;
}

void rotate_view_counterclockwise() {
    view_angle = (view_angle+2)%8;
}

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
    int a = (int)round(atan2(-p->y, p->x)*4/3.1415926535);
    return (a+8)%8;
}

Point* center;

Point* convert_to_screen_coordinates(Point offset) {
    // when center.x and center.y have different parity, results in odd graphical errors
    Point* temp = new Point(320, 201);
    temp->x += (offset.x+((view_angle%4 == 3 ? -1 : 1)*offset.y))*(view_angle > 4 ? -1 : 1);
    temp->y += (int) ((offset.y+((view_angle%4 == 1 ? -1 : 1)*offset.x))*(view_angle > 4 ? -1 : 1)/2);
    // UGGHHH CAN'T FIGURE THIS OUT TODO LATER
    temp->y -= offset.z;
    return temp;
}

Point* convert_from_screen_coordinates(Point crd) {
    Point* temp = new Point(center->x, center->y, center->z);
    temp->x += (int)(((crd.x-320)/2)+((view_angle%4 == 1 ? -1 : 1)*(crd.y-201)))*(view_angle > 4 ? -1 : 1);
    temp->y += (int)((crd.y-201)+((view_angle%4 == 3 ? -1 : 1)*(crd.x-320)/2))*(view_angle > 4 ? -1 : 1);
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
    public:
        Point* pos; // center point in xy-plane at minimum relative to z-axis
        int dx; // outward radius from center in x-direction
        int dy; // outward radius from center in y-direction
        int dz; // upward height in z-direction
        bool elliptical;
        Domain() {}
        void set_to(Point* p, int i, int j, bool e) { pos = p; dx = i; dy = j; dz = 0; elliptical = e; }
        void set_to(Point* p, int i, int j, int k, bool e) { pos = p; dx = i; dy = j; dz = k; elliptical = e; }
        bool get_point(Point*);
        bool intersects(Domain*);
        bool intersects(Point*, Point*);
};

bool Domain::get_point(Point* p) {
    if (p->x >= pos->x-dx && p->x <= pos->x+dx && p->y >= pos->y-dy && p->y <= pos->y+dy && p->z >= pos->z && p->z <= pos->z+dz) {
        if (!elliptical || pow(1.0*(p->x-pos->x)/dx, 2.0)+pow(1.0*(p->y-pos->y)/dy, 2.0) <= 1) {
            return true;
        }
    }
    return false;
}

bool Domain::intersects(Domain* d) {
    if (pos->x-dx <= d->pos->x+d->dx && pos->x+dx >= d->pos->x-d->dx
        && pos->y-dy <= d->pos->y+d->dy && pos->y+dy >= d->pos->y-d->dy
        && pos->z < d->pos->z+d->dz && pos->z+dz >= d->pos->z) {
        if (!elliptical && !d->elliptical) { return true; }
        else { return true; } // TODO change later???
    }
    // cout << "D1: <" << pos->x-dx << "," << pos->y-dy << "> -> <" << pos->x+dx << "," << pos->y+dy << ">\n";
    // cout << "D2: <" << d->pos->x-d->dx << "," << d->pos->y-d->dy << "> -> <" << d->pos->x+d->dx << "," << d->pos->y+d->dy << ">\n";
    return false;
}

bool Domain::intersects(Point* s, Point* v) {
    float det1 = -2*dy*v->x;
    float det2 = -2*dx*v->y;
    Point* sVert = new Point(pos->x-dx, pos->y-dy);
    Point* nVert = new Point(pos->x+dx, pos->y+dy);
    // check bottom edges
    float m = ((v->y*(s->x-sVert->x))-(v->x*(s->y-sVert->y)));
    if (((m >= 0 && m <= det1) || (m <= 0 && m >= det1)) && det1 != 0) {
        float n = (2*dy*(s->x-sVert->x));
        if ((n >= 0 && n <= det1) || (n <= 0 && n >= det1)) { return true; }
    }
    if (((m >= 0 && m <= det2) || (m <= 0 && m >= det2)) && det2 != 0) {
        float n = (2*dx*(s->y-sVert->y));
        if ((n >= 0 && n <= det2) || (n <= 0 && n >= det2)) { return true; }
    }
    // check top edges
    m = ((v->y*(s->x-nVert->x))-(v->x*(s->y-nVert->y)));
    if (((m >= 0 && m <= det1) || (m <= 0 && m >= det1)) && det1 != 0) {
        float n = (-2*dy*(s->x-sVert->x));
        if ((n >= 0 && n <= det1) || (n <= 0 && n >= det1)) { return true; }
    }
    if (((m >= 0 && m <= det2) || (m <= 0 && m >= det2)) && det2 != 0) {
        float n = (-2*dx*(s->y-sVert->y));
        if ((n >= 0 && n <= det2) || (n <= 0 && n >= det2)) { return true; }
    }
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

class Action;

class Area;
class OutdoorArea;

class Window;
class ContainerWindow;
class DescriptionWindow;
class ConversationWindow;

/**
 * Abstract, generic class for representing objects
 */
class Object {
    protected:
        int flags;
        int sprite;
        Point pos;
        Point mov;
        int z;
        int dir;
        Domain dom;
        Area* loc;
        ALLEGRO_USTR* name;
    public:
        int id;
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
        virtual int get_sprite_flipped();
        virtual void draw();
        virtual void update();
        virtual int on_click(ALLEGRO_EVENT);
        // virtual int on_click() = 0;
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
        Object* within;
        Item(int);
        virtual ALLEGRO_BITMAP* get_sprite();
        bool get_within_space(int, int);
        void remove_from_within(int = -1);
        void add_to_within(Item*);
        virtual int on_click(ALLEGRO_EVENT);
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
        virtual int on_click(ALLEGRO_EVENT);
};

class Equipment: public Item {
    friend class Humanoid;
    private:
        int w_sprite;
        int e_area;
        int mDEF[4];
    public:
        Equipment(int);
};

class Weapon: public Item {
    private:
        int type;
        int d_type; // 0 BLUNT, 1 SLASHING, 2 PIERCING
        int mHIT;
        int mDMG;
        int mCRT;
    public:
        Weapon(int);
        int get_type() { return type; }
        int get_damage() { return d_type; }
        int get_HIT() { return mHIT; }
        int get_DMG() { return mDMG; }
        int get_CRT() { return mCRT; }
};

class Wall: public Item {
    public:
        Wall(int d) : Item(d) {}
        bool get_flag(int);
};

class Openable: public Item {
    protected:
        int open_sprite; // -1 if just rotated when open, e.g. a door
        Domain* open_dom;
        bool locked;
        int lock_key;
    public:
        bool opened;
        Openable(int);
        ALLEGRO_BITMAP* get_sprite();
        int get_sprite_flipped();
        virtual int on_click(ALLEGRO_EVENT);
        void set_key(int);
};

class Key: public Item {
    friend class Openable;
    protected:
        int lock_key;
    public:
        Key(int d) : Item(d) {}
        void set_key(int);
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
        int on_click(ALLEGRO_EVENT);
};

/**
 * Abstract class for representing people and creatures
 */
class Entity: public Object {
    protected:
        ALLEGRO_USTR* char_name;
        float hunger;
        float fatigue;
        int* max_health;
        Action* action;
    public:
        int frame;
        Entity(int);
        ALLEGRO_BITMAP* get_sprite();
        virtual void update();
        ALLEGRO_USTR* get_name() { return char_name; }
        ALLEGRO_USTR* get_description();
        void set_destination(Point*);
        void harm(int, int);
        // virtual void evaluate_response(int);
        virtual int get_CMB(int) = 0;
        virtual int get_HIT(bool=false) = 0;
        virtual int get_DMG(bool=false) = 0;
        virtual int get_CRT(bool=false) = 0;
        virtual int get_attack_type(bool=false) = 0;
        virtual int get_EVA(int) = 0;
        virtual int get_DEF(int, int) = 0;
};

/**
 * For Entities not controlled by the player (essentially, NPC AI)
 */
class NPC {
    protected:
        int bCMB;
        int bEVA;
        float agg_response; // when HP < agg_response*MAXHP, they flee
        int hostility;
    public:
        NPC() {}
        // void update();
        // virtual int on_click(ALLEGRO_EVENT);
};

/**
 * For NPCs which are not humanoids/can't be spoken with
 */
class BasicNPC: public Entity, public NPC {
    protected:
        int bDEF[4];
        Weapon* nat_weapon;
    public:
        BasicNPC(int d) : Entity(d), NPC() {}
        int on_click(ALLEGRO_EVENT e) { return 0; /*NPC::on_click(e);*/ }
        void update();
        void evaluate_response(int);
        int get_CMB(int type) { return bCMB; }
        int get_EVA(int b_part) { return bEVA*100/max_health[b_part]; }
        int get_HIT(bool=false);
        int get_DMG(bool=false);
        int get_CRT(bool=false);
        int get_DEF(int b_part, int d_type) { return bDEF[d_type]; }
        int get_attack_type(bool offhand = false) { return nat_weapon->get_damage(); }
};

/**
 * Class for representing humanoid characters with the ability to equip items
 */
class Humanoid: public Entity {
    protected:
        ALLEGRO_USTR* c_title;
        int s_color;
        int h_color;
        int h_type;
        int p_type;
        /**
         * LIST OF EQUIP AREAS
         *  0   Head
         *  4   Neck
         *  1   Chest
         *  7   Back
         *  3   Arms
         *  6   Waist
         *  5   Legs
         *  2   Feet
         */
        Equipment* equip[8];
        Weapon* hands[2];
    public:
        Humanoid(int, int, int, int, int);
        ALLEGRO_BITMAP* get_port() { return get_portrait(p_type+1); }
        void draw();
        void draw(Point*, int, int = 0);
        Item* equip_item(Object*, int);
        void deequip_item(Object*, int = -1);
        Equipment* get_equip(int a) { return equip[a]; }
        Weapon* get_hands(int a) { return hands[a]; }
        ALLEGRO_USTR* get_title() { return c_title; }
        int get_HIT(bool = false);
        int get_DMG(bool = false);
        int get_CRT(bool = false);
        int get_attack_type(bool = false);
        int get_DEF(int, int);
};

/**
 * For human and kobold NPCs that you can/are meant to talk to
 */
class Person: public NPC, public Humanoid {
    private:
        int interests;
        int personality;
    public:
        Person(int);
        void evaluate_response(int i) {}
        int on_click(ALLEGRO_EVENT);
        int get_CMB(int);
        int get_EVA(int);
};

/**
 * The player character
 */
class Player: public Humanoid {
    private:
        int identifier;
        int sXP[18]; // skill experience
        int cXP; // experience to raise CMB
        int eXP; // experience to raise EVA
    public:
        Player(int, ALLEGRO_USTR*, int, int, int, int);
        void update_player(ALLEGRO_EVENT);
        int on_click(ALLEGRO_EVENT);
        int get_CMB(int);
        int get_EVA(int);
        void add_CMB(int);
        void add_EVA();
};

/**
 * Class for non-immediate actions, like attacking or lockpicking
 */
class Action {
    protected:
        Entity* actor;
        Object* obj;
        int max_distance;
        int time_remaining = 0; // -1 for indefinite, >0 for timed
        bool completed;
        Action* next_action;
    public:
        Action(Entity*, Object*, Action* = NULL);
        Action* get_next_action() { return next_action; }
        bool is_completed() { return completed; }
        bool can_be_executed();
        virtual void update();
        virtual int execute() = 0;
};

class MoveAction: public Action {
    protected:
        Point* destination;
        Point* towards;
    public:
        MoveAction(Entity*, Point*, Point*, Action* = NULL);
        Point* get_direction();
        void update();
        int execute() {}
};

class Attack: public Action {
    protected:
        int body_part = -1;
    public:
        Attack(Entity*, Object*, Action* = NULL);
        int execute();
        void make_attack(Entity*, bool);
};

/**
 * class for representing 64x64 tile / 2048x2048 unit spaces
 */
class Area {
    protected:
        int id;
        int tiles[64][64];
    public:
        int num_objects;
        Object** objects;
        Area(int);
        ALLEGRO_BITMAP* get_tile(int, int);
        void load_area();
        void add_object(Object*);
        void remove_object(Object*);
        int find_object(Object*, int, int);
        int find_object_by_position(Point*, int, int);
        void resort_objects();
        int resort_object(Object*);
        bool check_collisions(Object*, bool=true);
        int check_route(Point*, Point*, Object*);
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
    sprite = get_sprite_by_id(id);
    name = al_ustr_new(get_name_by_id(id));
    dom.set_to(&pos, 3, 3, 6, false);
    loc = NULL;
    flags = 0;
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
    if (get_within_stage(this->get_sprite(), draw_at->x, draw_at->y)) {
        ALLEGRO_BITMAP* bitmap = this->get_sprite();
        if (get_flag(1)) {
            draw_tinted_bitmap(bitmap, invisible, draw_at->x-(al_get_bitmap_width(bitmap)/2),
                draw_at->y-al_get_bitmap_height(bitmap), this->get_sprite_flipped());
        } else {
            draw_bitmap(bitmap, draw_at->x-(al_get_bitmap_width(bitmap)/2),
                draw_at->y-al_get_bitmap_height(bitmap), this->get_sprite_flipped());
        }
    }
    delete draw_at;
}

void Object::update() {
    pos += mov;
    if (loc != NULL) {
        if (!loc->check_collisions(this)) {
            pos -= mov;
            int just_y = mov.y;
            mov.set_to(mov.x, 0);
            pos += mov;
            if (!loc->check_collisions(this)) {
                pos -= mov;
                mov.set_to(0, just_y);
                pos += mov;
                if (!loc->check_collisions(this)) {
                    pos -= mov;
                    mov.set_to(0, 0);
                    loc->resort_object(this);
                }
            }
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

void Item::remove_from_within(int slot) {
    if (loc != NULL) {
        loc->remove_object(this);
    } else if (ItemContainer* item_container = dynamic_cast<ItemContainer*>(within)) {
        item_container->remove_item(this);
    } else if (Humanoid* humanoid = dynamic_cast<Humanoid*>(within)) {
        humanoid->deequip_item(this, slot);
    }
}

void Item::add_to_within(Item* thingy) {
    if (loc != NULL) {
        loc->add_object(thingy);
        return;
    } else if (ItemContainer* item_container = dynamic_cast<ItemContainer*>(within)) {
        item_container->add_item(thingy);
        return;
    } else if (Humanoid* humanoid = dynamic_cast<Humanoid*>(within)) {
        /*TODO if (Equipment* eq = dynamic_cast<Equipment*>(thingy)) {
            if (humanoid->get_equip(eq->e_area) == NULL) {
                humanoid->equip_item(eq, eq->e_area);
                return;
            }
        } else if (Weapon* wp = dynamic_cast<Weapon*>(thingy)) {
            if (humanoid->get_hands(0) == NULL) {
                humanoid->equip_item(wp, 0);
                return;
            } else if (humanoid->get_hands(1) == NULL) {
                humanoid->equip_item(wp, 1);
                return;
            }
        }*/
    }
    main_area->add_object(thingy);
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
    if (item == this) { cout << "FAILURE TO ADD: INSERTING THIS INTO THIS\n"; return 0; }
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
    item->remove_from_within();
    item->within = this;
    item->set_position(x, y);
    delete[] items;
    cout << "Items position set.\n";
    items = holder;
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

Equipment::Equipment(int d) : Item(d) {
    int* params = get_params_by_id(id);
    e_area = params[0];
    w_sprite = params[1];
}

Weapon::Weapon(int d) : Item(d) {
    int* params = get_params_by_id(id);
    mHIT = params[0];
    mDMG = params[1];
    mCRT = params[2];
    type = params[3];
    d_type = params[4];
}

Openable::Openable(int d) : Item(d), locked(false), opened(false) {
    int* params = get_params_by_id(id);
    open_sprite = params[0];
    if (open_sprite == 0) { open_sprite = -1; }

}

ALLEGRO_BITMAP* Openable::get_sprite() {
    if (open_sprite == -1 || !opened) {
        return Item::get_sprite();
    } else {
        return get_image_asset(open_sprite);
    }
}

int Openable::get_sprite_flipped() {
    bool cond1 = ((rel_direction(dir)+6)%8 >= 5);
    return (((!cond1 || (open_sprite < 0 && opened)) && (cond1 || !(open_sprite < 0 && opened)))
        ? ALLEGRO_FLIP_HORIZONTAL : 0);
}

void Openable::set_key(int k) {
    lock_key = k;
}

void Key::set_key(int k) {
    lock_key = k;
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

Entity::Entity(int d) : Object(d) {
    frame = 0;
    action = NULL;
}

void Entity::update() {
    if (action != NULL) {
        action->update();
        if (action->is_completed()) {
            action = action->get_next_action();
        }
    }
    if (MoveAction* ma = dynamic_cast<MoveAction*>(action)) {
        mov.set_to(ma->get_direction()->x, ma->get_direction()->y);
    }
    Object::update();
    if (mov.x != 0 || mov.y != 0) { frame = (frame+1)%60; }
    else { frame = 0; }
}

ALLEGRO_BITMAP* Entity::get_sprite() {
    int d = rel_direction(dir);
    d = ((d <= 1 || d >= 7) ? 8-((d+6)%8) : d-2);
    return get_image_sprite(sprite+((frame/5)%4)+(4*d));
}

ALLEGRO_USTR* Entity::get_description() {
    // TODO fix later
    return get_desc_by_id(id);
}

void Entity::set_destination(Point* p) {
    action = new MoveAction(this, &pos, p);
}

void Entity::harm(int body_part, int d_type) {

}

void BasicNPC::evaluate_response(int a_type) {
    switch (a_type) {
        case 0:
            set_destination(new Point(pos.x+((40+(rand()%40))*(rand()%2 == 0 ? -1 : 1)), pos.y+((40+(rand()%40))*(rand()%2 == 0 ? -1 : 1))));
        case 1:
            if (max_health[0] < agg_response*max_health[0]) {
                // run away
            } else {
                // fight
            }
            break;
    }
}

void BasicNPC::update() {
    if (action == NULL) {
        evaluate_response(0);
    }
    Entity::update();
}

// TODO stuff with this??? maybe???????
/*void NPC::update() {
    if (hostility > 0) {
        if (aggression) {
            // TODO make this better
            action = new Attack(this, pc);
        }
    }
    Entity::update();
}*/

int BasicNPC::get_HIT(bool offhand) {
    return get_CMB(-1)+nat_weapon->get_HIT();
}

int BasicNPC::get_DMG(bool offhand) {
    return ((get_CMB(-1)-40)/5)+nat_weapon->get_DMG();
}

int BasicNPC::get_CRT(bool offhand) {
    return get_CMB(-1)+nat_weapon->get_CRT();
}

Humanoid::Humanoid(int d, int sc, int hc, int ht, int pt) : Entity(d), s_color(sc), h_color(hc), h_type(ht), p_type(pt) {
    for (int i = 0; i < 8; i++) {
        equip[i] = NULL;
    }
    hands[0] = hands[1] = NULL;
    max_health = new int[6];
    max_health[0] = 100;
    max_health[1] = 25;
    for (int i = 2; i < 6; i++) { max_health[i] = 50; }
}

void Humanoid::draw() {
    int d = rel_direction(dir);
    d = ((d <= 1 || d >= 7) ? (8-((d+6)%8)) : (d-2));
    int i = ((frame/5)%4)+(4*d);

    Point* draw_at = convert_to_screen_coordinates(pos-*center);
    if (get_within_stage(get_image_sprite(sprite+i), draw_at->x, draw_at->y)) {
        draw(draw_at, i, get_sprite_flipped());
    }
    delete draw_at;
}

void Humanoid::draw(Point* draw_at, int i, int flip) {
    // sprite for body
    ALLEGRO_BITMAP* bmp1 = get_image_sprite(sprite+i);
    // sprite for shadowing
    ALLEGRO_BITMAP* bmp2 = get_image_sprite(sprite+i+20);
    draw_at->set_to(draw_at->x-(al_get_bitmap_width(bmp1)/2), draw_at->y-al_get_bitmap_height(bmp1));
    if (get_flag(1)) {
        // draw body
        unsigned char *r, *g, *b;
        al_unmap_rgb(s_pigment[s_color], r, g, b);
        draw_tinted_bitmap(bmp1, al_map_rgba((int)r/4, (int)g/4, (int)b/4, 64), draw_at->x, draw_at->y, flip);
        // draw clothing (except head)
        if (equip[7] != NULL && i >= 12) {
            draw_tinted_bitmap(get_image_sprite(equip[7]->w_sprite+i), invisible, draw_at->x, draw_at->y, flip);
        }
        for (int a = 5; a > 0; a--) {
            if (equip[a] != NULL) {
                draw_tinted_bitmap(get_image_sprite(equip[a]->w_sprite+i), invisible, draw_at->x, draw_at->y, flip);
            }
        }
        if (equip[6] != NULL) {
            draw_tinted_bitmap(get_image_sprite(equip[6]->w_sprite+i), invisible, draw_at->x, draw_at->y, flip);
        }
        if (equip[7] != NULL && i < 12) {
            draw_tinted_bitmap(get_image_sprite(equip[7]->w_sprite+i), invisible, draw_at->x, draw_at->y, flip);
        }
        // draw shadowing
        draw_tinted_bitmap(bmp2, invisible, draw_at->x, draw_at->y, flip);
        // draw hair (if they have any)
        if (h_type >= 0) {
            al_unmap_rgb(h_pigment[h_color], r, g, b);
            draw_tinted_bitmap(get_image_sprite(h_type+i), al_map_rgba((int)r/4, (int)g/4, (int)b/4, 64), draw_at->x, draw_at->y, flip);
            draw_tinted_bitmap(get_image_sprite(h_type+i+20), invisible, draw_at->x, draw_at->y, flip);
        }
        // draw head equipment
        if (equip[0] != NULL) {
            draw_tinted_bitmap(get_image_sprite(equip[0]->w_sprite+i), invisible, draw_at->x, draw_at->y, flip);
        }
    } else {
        // draw body
        draw_tinted_bitmap(bmp1, s_pigment[s_color], draw_at->x, draw_at->y, flip);
        // draw clothing
        if (equip[7] != NULL && i >= 12) {
            draw_bitmap(get_image_sprite(equip[7]->w_sprite+i), draw_at->x, draw_at->y, flip);
        }
        for (int a = 5; a > 0; a--) {
            if (equip[a] != NULL) {
                draw_bitmap(get_image_sprite(equip[a]->w_sprite+i), draw_at->x, draw_at->y, flip);
            }
        }
        if (equip[6] != NULL) {
            draw_bitmap(get_image_sprite(equip[6]->w_sprite+i), draw_at->x, draw_at->y, flip);
        }
        if (equip[7] != NULL && i < 12) {
            draw_bitmap(get_image_sprite(equip[7]->w_sprite+i), draw_at->x, draw_at->y, flip);
        }
        // draw shadowing
        draw_bitmap(bmp2, draw_at->x, draw_at->y, flip);
        // draw hair (if they have any)
        if (h_type >= 0) {
            draw_tinted_bitmap(get_image_sprite(h_type+i), h_pigment[h_color], draw_at->x, draw_at->y, flip);
            draw_bitmap(get_image_sprite(h_type+i+20), draw_at->x, draw_at->y, flip);
        }
        // draw head equipment
        if (equip[0] != NULL) {
            draw_bitmap(get_image_sprite(equip[0]->w_sprite+i), draw_at->x, draw_at->y, flip);
        }
    }
}

/**
 * Equips the given item, returns what was in the equip slot before.
 */
Item* Humanoid::equip_item(Object* thingy, int slot) {
    cout << "[GAME::Humanoid] Equip called with slot " << slot << ".\n";
    if (Equipment* eq = dynamic_cast<Equipment*>(thingy)) {
        if (slot == eq->e_area) {
            Equipment* former = equip[eq->e_area];
            equip[eq->e_area] = eq;
            eq->remove_from_within();
            if (former != NULL) { eq->add_to_within(former); }
            eq->within = this;
            return former;
        }
    } else if (Weapon* wp = dynamic_cast<Weapon*>(thingy)) {
        Weapon* formr = hands[slot];
        hands[slot] = wp;
        wp->remove_from_within(slot == 0 ? 1 : 0);
        if (formr != NULL) { wp->add_to_within(formr); }
        wp->within = this;
        return formr;
    }
    return NULL;

    /* if (a >= 10 || a < 0) { return; }
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
    equip[a] = item; */
}

void Humanoid::deequip_item(Object* item, int slot) {
    if (Equipment* eq = dynamic_cast<Equipment*>(item)) {
        if (equip[eq->e_area] == eq) {
            equip[eq->e_area] = NULL;
            eq->within = NULL;
        }
    } else if (Weapon* wp = dynamic_cast<Weapon*>(item)) {
        if (hands[1] == wp && slot != 0) {
            hands[1] = NULL;
            wp->within = NULL;
        } else if (hands[0] == wp) {
            hands[0] = NULL;
            wp->within = NULL;
        }
    }
}

int Humanoid::get_HIT(bool offhand) {
    if (hands[offhand ? 1 : 0] == NULL) { return get_CMB(0); }
    else { return get_CMB(hands[offhand ? 1 : 0]->get_type())+hands[offhand ? 1 : 0]->get_HIT(); }
}

int Humanoid::get_DMG(bool offhand) {
    if (hands[offhand ? 1 : 0] == NULL) { return (int)max(1, (get_CMB(0)-40)/5); }
    else { return (int)max(1, ((get_CMB(hands[offhand ? 1 : 0]->get_type())-40)/5)+hands[offhand ? 1 : 0]->get_DMG()); }
}

int Humanoid::get_CRT(bool offhand) {
    if (hands[offhand ? 1 : 0] == NULL) { return 0; }
    else {
        if (hands[offhand ? 1 : 0]->get_CRT() == 0) { return 0; }
        else { return get_CMB(hands[offhand ? 1 : 0]->get_type())+hands[offhand ? 1 : 0]->get_CRT(); }
    }
}

int Humanoid::get_DEF(int b_part, int d_type) {
    if (equip[b_part] != NULL) {
        return equip[b_part]->mDEF[d_type];
    }
    return 0;
}

int Humanoid::get_attack_type(bool offhand) {
    if (hands[offhand ? 1 : 0] != NULL) { return hands[offhand ? 1 : 0]->get_damage(); }
    else { return 2; }
}

Person::Person(int d) : NPC(), Humanoid(d, 2, 1, 69, 1) {
    // create NPC here
    c_title = al_ustr_new("NPC");
}

int Person::get_CMB(int type) {
    return bCMB; // TODO replace later
}

int Person::get_EVA(int b_part) {
    return bEVA*100/max_health[b_part]; // TODO factor in weight
}

Player::Player(int i, ALLEGRO_USTR* n, int sc, int hc, int ht, int pt) : Humanoid(1, sc, hc, ht, pt) {
    char_name = n;
    c_title = al_ustr_new("Adventurer");
    cXP = eXP = 0;
    for (int i = 0; i < 18; i++) {
        sXP[i] = 0;
    }
}

void Player::update_player(ALLEGRO_EVENT e) {
    mov.set_to(0, 0);
    Point* origin = new Point();
    if (get_key_down(0) && !get_key_down(2)) { // UP key
        mov += *translate(origin, view_angle);
    } else
    if (get_key_down(2) && !get_key_down(0)) { // DOWN key
        mov += *translate(origin, (view_angle+4)%8);
    }
    if (get_key_down(1) && !get_key_down(3)) { // LEFT key
        mov += *translate(origin, (view_angle+2)%8);
    } else if (get_key_down(3) && !get_key_down(1)) { // RIGHT key
        mov += *translate(origin, (view_angle+6)%8);
    }
    delete origin;
    if (mov.length() != 0) {
        action = NULL;
        cout << "[GAME] (Player) " << pos.x << ", " << pos.y << ", " << pos.z << "\n";
    }
}

int Player::get_CMB(int type) {
    return (int)(40+min(35.0, 0.35*sqrt(sXP[type]))+min(15.0, 0.15*sqrt(cXP)));
}

int Player::get_EVA(int b_part) {
    return (int)(30+min(60.0, 0.6*sqrt(eXP)))*100/max_health[b_part];
}

void Player::add_CMB(int type) {
    cout << "[GAME::Player] Adding XP to CMB for type " << type << "...\n";
    cXP++;
    sXP[type]++;
}

void Player::add_EVA() {
    cout << "[GAME::Player] Adding XP to EVA...\n";
    eXP++;
}

Action::Action(Entity* e, Object* o, Action* n_act) {
    actor = e;
    obj = o;
    next_action = n_act;
    completed = false;
}

void Action::update() {
    if (time_remaining > 0) {
        time_remaining--;
    }
}

bool Action::can_be_executed() {
    return (obj == NULL || (*actor->get_position()-*obj->get_position()).length() <= max_distance);
}

MoveAction::MoveAction(Entity* e, Point* s, Point* d, Action* n_act) : Action(e, NULL) {
    cout << "[GAME::MoveAction] d = <" << d->x << ", " << d->y << ">\n";
    time_remaining = -1;
    next_action = n_act;
    destination = new Point(0, 0);
    towards = new Point(0, 0);
    evaluate_direction(s, d, 0);
}

void MoveAction::evaluate_direction(Point* s, Point* d, int flags) {
    Point* v = new Point(d->x-s->x, d->y-s->y);
    cout << "[GAME::MoveAction] v = <" << v->x << ", " << v->y << ">\n";
    int dir = get_angle_of_vector(v);
    cout << "[GAME::MoveAction] dir = " << dir << "\n";
    if (v->x == 0 || v->y == 0 || abs(v->x) == abs(v->y)) {
        destination->set_to(v->x, v->y);
    } else if (dir % 2 == 0) {
        if (dir == NORTH || dir == SOUTH) {
            destination->set_to(v->x, dir == SOUTH ? abs(v->x) : -abs(v->x));
        } else {
            destination->set_to(dir == EAST ? abs(v->y) : -abs(v->y), v->y);
        }
    } else {
        if (v->x < 0) {
            if (v->y-v->x < 0 || v->y+v->x > 0) {
                destination->set_to(0, (v->y-abs(v->x))*v->y/abs(v->y));
            } else {
                destination->set_to((v->x-abs(v->y))*v->x/abs(v->x), 0);
            }
        } else {
            if (v->y-v->x > 0 || v->y+v->x < 0) {
                destination->set_to(0, (v->y-abs(v->x))*v->y/abs(v->y));
            } else {
                destination->set_to((v->x-abs(v->y))*v->x/abs(v->x), 0);
            }
        }
    }
    int index = (actor->get_locale() == NULL ? -1 : actor->get_locale()->check_route(s, destination, actor));
    if (index >= 0) {
        if (~flags ^ 1 != 0) {
            evaluate_direction(s, d, 1);
        }
    } else {
        towards->set_to(destination->x/abs(destination->x == 0 ? 1 : destination->x),
            destination->y/abs(destination->y == 0 ? 1 : destination->y));
        cout << "[GAME::MoveAction] towards = <" << towards->x << ", " << towards->y << ">\n";
        destination->set_to(destination->x+s->x, destination->y+s->y);
        cout << "[GAME::MoveAction] destination = <" << destination->x << ", " << destination->y << ">\n";
        next_action = (*destination == *d ? next_action : new MoveAction(actor, destination, d, next_action));
    }
    delete v;
}

Point* MoveAction::get_direction() {
    return towards;
}

void MoveAction::update() {
    Action::update();
    if (*actor->get_position() == *destination) {
        completed = true;
    }
}

Attack::Attack(Entity* e, Object* o, Action* n_act) : Action(e, o, n_act) {
    // TODO more here probably?
}

int Attack::execute() {
    if (can_be_executed() && time_remaining == 0) {
        if (Entity* ent = dynamic_cast<Entity*>(obj)) {
            if (actor == pc) {
                if (pc->get_hands(0) != NULL) {
                    pc->add_CMB(pc->get_hands(0)->get_type());
                }
                if (pc->get_hands(1) != NULL) {
                    pc->add_CMB(pc->get_hands(1)->get_type());
                }
                if (pc->get_hands(0) == NULL && pc->get_hands(1) == NULL) {
                    pc->add_CMB(0);
                }
            }
            if (Humanoid* hmnd = dynamic_cast<Humanoid*>(actor)) {
                if (hmnd->get_hands(0) != NULL || hmnd->get_hands(1) == NULL) {
                    make_attack(ent, false);
                }
                if (hmnd->get_hands(1) != NULL) {
                    make_attack(ent, true);
                }
            } else {
                make_attack(ent, false);
            }
        }
        return 1;
    } else {
        return 0;
    }
}

void Attack::make_attack(Entity* ent, bool offhand) {
    float p_hit = pow(max(0.32, min((actor->get_HIT(offhand)/ent->get_EVA(body_part))-0.3, 0.995)), 2);
    cout << "[GAME::Attack] P(hit) = " << p_hit << "\n";
    if (p_hit*1000 > rand()%1000) { // successful hit or failed dodge
        int dmg = actor->get_DMG(offhand);
        dmg = (dmg*19/20)+(rand()%(dmg/10));
        dmg = max(0, dmg-ent->get_DEF(body_part, actor->get_attack_type(offhand)));
        cout << "[GAME::Attack] HIT for " << dmg << " damage\n";
        if (dmg > 0 && actor->get_CRT(offhand) > 0) {
            p_hit = pow(max(0.1, min(actor->get_CRT(offhand)/(ent->get_EVA(body_part)+ent->get_DEF(body_part, actor->get_attack_type(offhand)))-0.5, 0.99)), 2);
            cout << "[GAME::Attack] P(bleed) = " << p_hit << "\n";
            if (p_hit*1000 > rand()%1000) {
                // bleed damage
                cout << "[GAME::Attack] BLEED for " << "" << " damage\n";
            } else {
                cout << "[GAME::Attack] NO BLEED\n";
            }
        }
    } else { // failed hit or successful dodge
        cout << "[GAME::Attack] MISS\n";
        if (ent == pc) { pc->add_EVA(); }
    }
}

Area::Area(int identifier) {
    id = identifier;
    // load the area here
    for (int i = 0; i < 64; i++) {
        for (int j = 0; j < 64; j++) { tiles[i][j] = 0; }
    }
    num_objects = 0;
}

ALLEGRO_BITMAP* Area::get_tile(int x, int y) {
    // TODO transfer to OutdoorArea
    /*if (x >= 64) { return borders[0]->get_tile(x-64, y); }
    else if (y < 0) { return borders[1]->get_tile(x, y+64); }
    else if (x < 0) { return borders[2]->get_tile(x+64, y); }
    else if (y >= 64) { return borders[3]->get_tile(x, y-64); }*/
    if (x < 0 || x >= 64 || y < 0 || y >= 64 || tiles[x][y] == 0) { return NULL; }
    else { return get_tile_asset(tiles[x][y]-1); }
}

void Area::load_area() {
    string line;
    char* s = new char[8];
    memset(&s[0], 0, sizeof(s));
    itoa(id, s, 10);
    char* filename = new char[23];
    strcpy(filename, "data/area/area");
    filename[14] = s[0];
    if (s[1] != 0) {
        filename[15] = s[1];
        if (s[2] >= '0' && s[2] <= '9') { filename[16] = s[2]; filename[17] = '\0'; }
        else { filename[16] = '\0'; }
    } else { filename[15] = '\0'; }
    strcat(filename, ".txt");
    ifstream inpt(filename);
    if (inpt.is_open()) {
        memset(&s[0], 0, sizeof(s));
        while (getline(inpt, line)) {
            if (line[0] == '-') {
                if (line[2] == 'd') { // default
                    int index = line.find(' ', 2)+1;
                    line.copy(s, line.find(' ', index)-index, index);
                    int def = atoi(s);
                    for (int i = 0; i < 64; i++) {
                        for (int j = 0; j < 64; j++) {
                            if (tiles[i][j] == 0) { tiles[i][j] = def; }
                        }
                    }
                } else {
                    int index = line.find(' ', 2)+1;
                    line.copy(s, index-3, 2);
                    int xmin = atoi(s);
                    memset(&s[0], 0, sizeof(s));

                    line.copy(s, line.find(' ', index)-index, index);
                    int ymin = atoi(s);
                    memset(&s[0], 0, sizeof(s));
                    index = line.find(' ', index)+1;

                    line.copy(s, line.find(' ', index)-index, index);
                    int xmax = atoi(s);
                    memset(&s[0], 0, sizeof(s));
                    index = line.find(' ', index)+1;

                    line.copy(s, line.find(' ', index)-index, index);
                    int ymax = atoi(s);
                    memset(&s[0], 0, sizeof(s));
                    index = line.find(' ', index)+1;

                    line.copy(s, line.find(' ', index)-index, index);
                    int q = atoi(s);
                    memset(&s[0], 0, sizeof(s));

                    for (int i = xmin; i <= xmax; i++) {
                        for (int j = ymin; j <= ymax; j++) {
                            tiles[i][j] = q;
                        }
                    }
                }
            } else if (line[0] != '/') {
                int index = line.find(' ')+1;
                line.copy(s, index-1);
                int identifier = atoi(s);
                memset(&s[0], 0, sizeof(s));
                Object* new_object = create_object(identifier);

                line.copy(s, line.find(' ', index)-index, index);
                int xpos = atoi(s);
                memset(&s[0], 0, sizeof(s));
                index = line.find(' ', index)+1;

                line.copy(s, line.find(' ', index)-index, index);
                int ypos = atoi(s);
                memset(&s[0], 0, sizeof(s));
                new_object->set_position(xpos, ypos);
                add_object(new_object);

                // if (get_class_by_id(identifier) == 5)
            }
        }
    } else { cout << "[GAME] Failed to load file \"" << filename << "\".\n"; }
}

/**
 * @return true if o1 should be in front of o2, false otherwise
 */
bool evaluate_objects(Object* o1, Object* o2) {
    // TODO fix latter part of evaluation???
    Point* o1_pos = convert_to_screen_coordinates(*(o1->get_position())-*center);
    if (center == NULL) { cout << "CENTER IS NULL WHAT THE FUCK\n"; }
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
    if (Item* item = dynamic_cast<Item*>(object)) {
        item->remove_from_within();
    }
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
    object->set_locale(this);
    delete[] objects;
    objects = temp;
}

void Area::remove_object(Object* object) {
    int index = find_object(object, 0, num_objects-1);
    // cout << "[GAME::Area] Remove object called for " << object << ": index of " << index << ".\n";
    if (index >= 0) {
        num_objects--;
        for (int i = index; i < num_objects; i++) {
            objects[i] = objects[i+1];
        }
        objects[num_objects] = NULL;
        object->set_locale(NULL);
    }
}

int Area::find_object(Object* object, int si, int ei) {
    // cout << "[GAME::Area] Checking from " << si << " to " << ei << "\n";
    Object* compare_to = objects[(si+ei)/2];
    if (object == compare_to) {
        return (si+ei)/2;
    } else if (ei-si == 0) {
        if (*(object->get_position()) == *(compare_to->get_position())) {
            return find_object(object, si-1, ei-1);
        } else {
            return -1;
        }
    } else if (ei-si <= 1) {
        return find_object(object, ei, ei);
    } else if (evaluate_objects(object, compare_to)) {
        int result = find_object(object, (si+ei)/2, ei);
        if (result == -1 && *(object->get_position()) == *(compare_to->get_position())) {
            return find_object(object, ((si+ei)/2)-1, ((si+ei)/2)-1);
        } else {
            return result;
        }
    } else {
        return find_object(object, si, (si+ei)/2);
    }
}

/** Finds the approximate position of the object at the point **/
int Area::find_object_by_position(Point* p, int si, int ei) {
    Object* compare_to = objects[(si+ei)/2];
    if (compare_to->get_domain()->get_point(p)) {
        return (si+ei)/2;
    } else if (ei-si <= 0) {
        return ei;
    } else if (ei-si == 1) {
        return find_object_by_position(p, ei, ei);
    } else {
        if (*convert_to_screen_coordinates(*p-*center) >=
            *convert_to_screen_coordinates(*(compare_to->get_position())-*center)) {
            return find_object_by_position(p, (si+ei)/2, ei);
        } else {
            return find_object_by_position(p, si, (si+ei)/2);
        }
    }
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

int Area::resort_object(Object* o) {
    int index = find_object_by_position(o->get_position(), 0, num_objects-1);
    if (index < 0) { return -1; }
    int i = 0;
    while (index-i >= 0 || index+i < num_objects) {
        if (index-i >= 0 && objects[index-i] == o) {
            index -= i;
            break;
        }
        if (index+i < num_objects && objects[index+i] == o) {
            index += i;
            break;
        }
        i++;
    }
    if (objects[index] == o) {
        while (index > 0 && evaluate_objects(objects[index-1], objects[index])) {
            objects[index] = objects[index-1];
            objects[index-1] = o;
            index--;
        }
        while (index+1 < num_objects && evaluate_objects(objects[index], objects[index+1])) {
            objects[index] = objects[index+1];
            objects[index+1] = o;
            index++;
        }
        return index;
    }
    return -1;
}

/**
 * Returns false if collision detected, true if none
 */
bool Area::check_collisions(Object* o, bool change_z) {
    int index = resort_object(o); // find_object(o, 0, num_objects-1);
    if (index < 0) {
        cout << "[GAME::Area] resort_object() returned -1, collision will not be checked\n\tobject id = " << o->id << "\n";
    }
    int i = index-1;
    Point* pos = o->get_position();
    Domain* dom = o->get_domain();
    while ((i >= 0 && i < num_objects) && o != objects[i] && (abs(objects[i]->get_position()->x-pos->x) <= 100 ||
           abs(objects[i]->get_position()->y-pos->y) <= 100)) {
        // cout << "[GAME::Area] Checking for collisions at index " << i << ", id = " << objects[i]->id << "\n";
        if (objects[i]->get_domain()->intersects(dom) && !objects[i]->get_flag(2)) {
            if (change_z && pos->z+6 > objects[i]->get_position()->z+objects[i]->get_domain()->dz) {
                pos->z = objects[i]->get_domain()->dz-objects[i]->get_position()->z;
                if (check_collisions(o, false)) {
                    return true;
                } else {
                    pos->z -= objects[i]->get_domain()->dz;
                    return false;
                }
            } else { return false; }
        }
        if (i < index) {
            i--;
            if (i < 0 || abs(objects[i]->get_position()->x-pos->x) > 100 && abs(objects[i]->get_position()->y-pos->y) > 100) {
                i = index+1;
            }
        } else {
            i++;
        }
    }
    // cout << "[GAME::Area] Termination reached at i = " << i << ", id = " << (i >= 0 && i < num_objects ? objects[i]->id : -1) << "\n";

    /*for (int i = 0; i < num_objects; i++) {
        if (o != objects[i]) {
            if (o->get_domain()->intersects(objects[i]->get_domain()) && !objects[i]->get_flag(2)) {
                if (change_z && objects[i]->get_domain()->dz < 6 && pos->z >= objects[i]->get_position()->z) {
                    pos->z += objects[i]->get_domain()->dz;
                    if (check_collisions(o, false)) {
                        return true;
                    } else {
                        pos->z -= objects[i]->get_domain()->dz;
                        return false;
                    }
                } else { return false; }
            }
        }
    }*/
    if (change_z && pos->z > 0) {
        int former = pos->z;
        pos->z = (pos->z-6 < 0 ? 0 : pos->z-6);
        if (!check_collisions(o)) { pos->z = former; }
    }
    return true;
}

int Area::check_route(Point* start, Point* v, Object* ignore_this) {
    // Point* v = translate(new Point(0, 0), dir, len);
    int index = find_object_by_position(start, 0, num_objects-1);
    int d = rel_direction(get_angle_of_vector(v));
    int i = index;
    while ((i >= 0 && i < num_objects) && (abs(objects[i]->get_position()->x-start->x) <= len || abs(objects[i]->get_position()->y-start->y) <= len)) {
        if (objects[i] != ignore_this && objects[i]->get_domain()->intersects(start, v) && !objects[i]->get_flag(2)) {
            return i;
        }
        if (d > EAST && d <= WEST) {
            i--;
        } else {
            i++;
        }
    }
    return -1;
}

OutdoorArea::OutdoorArea(int x, int y) : Area(x+y) {
    // fix super() later
    pos.set_to(x, y);
}

Area* OutdoorArea::get_border(int dir) {
    return borders[dir];
}

void draw_at_center() {
    al_clear_to_color(black);

    // DRAW THE TILES OF THE TERRAIN
    Point* indices = new Point(center->x/32, center->y/32);
    Point* offset = new Point((int)center->x%32, (int)center->y%32);
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
    disy += center->z;
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
        main_area->objects[i]->draw();
        /*if (Humanoid* hmnd = dynamic_cast<Humanoid*>(main_area->objects[i])) {
            hmnd->draw();
        } else {
            main_area->objects[i]->draw();
        }*/
    }

    // cleanup
    delete indices;
    delete offset;
    delete basis;
}

Object* dragging = NULL;
Point affixation(0, 0);
Object** drag_holder = NULL;
ALLEGRO_USTR* mouse_desc = NULL;

void evaluate_mouse_desc(ALLEGRO_EVENT e) {
    for (int i = main_area->num_objects-1; i >= 0; i--) {
        Point* p = convert_to_screen_coordinates(*(main_area->objects[i]->get_position())-*center);
        if (main_area->objects[i]->get_sprite() == NULL) {
            cout << al_cstr(main_area->objects[i]->get_name()) << "\n";
        }
        if (get_within_sprite(e.mouse.x, e.mouse.y, main_area->objects[i]->get_sprite(), p->x, p->y)
                && !main_area->objects[i]->get_flag(1)) {
            mouse_desc = main_area->objects[i]->get_name();
            delete p;
            return;
        }
        delete p;
    }
    mouse_desc = NULL;
}

class Window {
    public:
        int x;
        int y;
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
        ContainerWindow(ItemContainer* ic) : cont(ic) { cout << "New ContainerWindow.\n"; }
        int update_window(ALLEGRO_EVENT);
        void draw();
        ItemContainer* get_container() { return cont; }
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
                draw_bitmap(get_interface_asset(2), x+xd+(10*i), y+yd+(10*j), 0);
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
        Person* npc;
    public:
        ConversationWindow(Person* n) : npc(n) { x = y = 0; }
        int update_window(ALLEGRO_EVENT) { return 0; }
        void draw();
};

void ConversationWindow::draw() {
    draw_bitmap(get_bitmap(), x, y, 0);
    ALLEGRO_BITMAP* bmp = npc->get_sprite();
    draw_bitmap(bmp, x+69-(al_get_bitmap_width(bmp)/2), y+25-(al_get_bitmap_height(bmp)/2),
        (((rel_direction(npc->get_direction())+6)%8 >= 5) ? ALLEGRO_FLIP_HORIZONTAL : 0));
}

class CharacterWindow: public Window {
    private:
        Humanoid* person;
    public:
        CharacterWindow(Humanoid* h) : person(h) {}
        ALLEGRO_BITMAP* get_bitmap() { return get_interface_asset(3); }
        int update_window(ALLEGRO_EVENT);
        void draw();
};

void CharacterWindow::draw() {
    draw_bitmap(get_bitmap(), x, y, 0);
    draw_bitmap(get_portrait(0), x+33, y+16, 0);
    draw_bitmap(person->get_port(), x+36, y+19, 0);
    draw_ustr(person->get_name(), x+51, y+62, ALLEGRO_ALIGN_CENTER, 1);
    draw_ustr(person->get_title(), x+51, y+73, ALLEGRO_ALIGN_CENTER, 2);
    /* ALLEGRO_BITMAP* bmp = person->get_sprite();
    draw_bitmap(bmp, x+69-(al_get_bitmap_width(bmp)/2), y+25-(al_get_bitmap_height(bmp)/2),
        (((rel_direction(person->get_direction())+6)%8 >= 5) ? ALLEGRO_FLIP_HORIZONTAL : 0));
        */
    /** DRAW SKILLS **/
    if (person == pc) {
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
        al_draw_text(get_font(3), white, x+114+(42*(i/2)), y+110+(9*(i%2)), 0, (const char*)(s));
    }
    delete s;
}

class SkillsWindow: public Window {
    public:
        SkillsWindow() {}
        ALLEGRO_BITMAP* get_bitmap() { return get_interface_asset(11); }
        int update_window(ALLEGRO_EVENT);
        void draw();
};

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

int Object::on_click(ALLEGRO_EVENT e) {
    cout << "Object clicked.\n";
    if ((dragging == NULL || dragging == this) && e.mouse.button == 2) {
        add_window(new DescriptionWindow(this));
        return 1;
    }
    return 0;
}

/*int NPC::on_click(ALLEGRO_EVENT e) {
    if (Object::on_click(e)) {
        return 1;
    } else {
        // TODO insert clause for ATTACK MODE here
    }
    return 0;
}*/

int Person::on_click(ALLEGRO_EVENT e) {
    // right click: followers = CharacterWindow
    // right click: other NPC = pickpocket
    if ((dragging == NULL || dragging == this) && e.mouse.button == 2) {
        // TODO insert clause for only followers
        add_window(new CharacterWindow(this));
        return 1;
    }
    if (Object::on_click(e)) { // TODO fix later, I guess????
        return 1;
    } else if (dragging == NULL || dragging == this) {
        add_window(new ConversationWindow(this));
        return 1;
    }
    return 0;
}

int Player::on_click(ALLEGRO_EVENT e) {
    if ((dragging == NULL || dragging == this) && e.mouse.button == 2) {
        add_window(new CharacterWindow(this));
        return 1;
    }
    return 0;
}

int Item::on_click(ALLEGRO_EVENT e) {
    cout << "Item clicked.\n";
    return Object::on_click(e);
}

int ItemContainer::on_click(ALLEGRO_EVENT e) {
    cout << "Item container clicked.\n";
    if (Object::on_click(e)) {
        return 1;
    } else if (dragging == NULL || dragging == this) {
        for (int i = 0; i < num_windows; i++) {
            if (ContainerWindow* cw = dynamic_cast<ContainerWindow*>(windows[i])) {
                if (cw->get_container() == this) {
                    remove_window(cw);
                    return 1;
                }
            }
        }
        add_window(new ContainerWindow(this));
        return 1;
    } else if (Item* item = dynamic_cast<Item*>(dragging)) {
        add_item(item);
        return 1;
    }
    cout << "Function of click executed.\n";
    return 0;
}

int Openable::on_click(ALLEGRO_EVENT e) {
    // TODO add collision testing so you can't glitch into a door
    if (!locked && dragging == NULL) {
        opened = !opened;
        return 1;
    } else if (Key* key = dynamic_cast<Key*>(dragging)) {
        if (lock_key == key->lock_key) {
            locked = !locked;
            opened = locked;
            return 1;
        }
    }
    // add function for lockpicks here
    return 0;
}

int Plant::on_click(ALLEGRO_EVENT e) {
    if (Object::on_click(e)) { return 1; }
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
        // real specific past me thanks
        delete p;
    }
    return 0;
}

int CharacterWindow::update_window(ALLEGRO_EVENT e) {
    if (e.type == ALLEGRO_EVENT_MOUSE_AXES) {
        if (e.mouse.x >= x+14 && e.mouse.x <= x+82) {
            if (e.mouse.y >= y+108 && e.mouse.y <= y+116) {
                mouse_desc = description[0];
            } else if (e.mouse.y >= y+118 && e.mouse.y <= y+126) {
                mouse_desc = description[1];
            } else if (e.mouse.y >= y+128 && e.mouse.y <= y+136) {
                mouse_desc = description[2];
            } else {
                mouse_desc = NULL;
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
                        mouse_desc = hand->get_name();
                    } else {
                        mouse_desc = NULL;
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
                        mouse_desc = eq->get_name();
                    } else {
                        mouse_desc = NULL;
                    }
                }
            }
        } else {
            mouse_desc = NULL;
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

int SkillsWindow::update_window(ALLEGRO_EVENT e) {
    if (e.type == ALLEGRO_EVENT_MOUSE_AXES) {
        if (e.mouse.x >= x+16 && e.mouse.x <= x+74) {
            int i = (e.mouse.y-y-26)/14;
            if (i >= 0 && i < 9 && e.mouse.y >= y+26) {
                mouse_desc = description[3+(2*i)];
                return 1;
            } else {
                mouse_desc = NULL;
                return 1;
            }
        } else if (e.mouse.x >= x+86 && e.mouse.x <= x+144) {
            int i = (e.mouse.y-y-26)/14;
            if (i >= 0 && i < 9 && e.mouse.y >= y+26) {
                mouse_desc = description[4+(2*i)];
                return 1;
            } else {
                mouse_desc = NULL;
                return 1;
            }
        }
    }
    return 0;
}

void Area::update_objects(ALLEGRO_EVENT e) {
    if (e.type == ALLEGRO_EVENT_TIMER) {
        for (int i = 0; i < num_objects-1; i++) {
            objects[i]->update();
        }
    }
    if (e.type == ALLEGRO_EVENT_MOUSE_AXES || (e.type == ALLEGRO_EVENT_TIMER && get_any_key_down())) {
        evaluate_mouse_desc(e);
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
        } else {
            Point* p = new Point(e.mouse.x, e.mouse.y);
            pc->set_destination(convert_from_screen_coordinates(*p));
            delete p;
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
            return new Equipment(id);
        case 5:
            return new Key(id);
        case 6:
            return new Openable(id);
        case 7:
            return new Weapon(id);
        case 100:
            return new Person(id);
        case 101:
            return new BasicNPC(id);
        case -1:
            return NULL;
        default:
            return new Item(0);
    }
}

int run_game(ALLEGRO_EVENT_QUEUE* events) {
    cout << "[GAME] Game begun.\n";
    center = pc->get_position();
    main_area = new Area(1);
    main_area->load_area();
    // pc = new Player();
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
            if (!get_paused()) {
                pc->update_player(e);
                main_area->update_objects(e);
            }
            if (get_any_key_down()) { evaluate_mouse_desc(e); }
            update = true;
        } else {
            update = true;
            switch (update_from_event(e)) {
                case 1: // PAUSE key
                    toggle_pause();
                    break;
                case 2: // ENTER key, interprets text
                    if (get_command_line()) {
                        ALLEGRO_USTR* input = get_input();
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
            draw_at_center();
            draw_windows();
            if (get_command_line()) {
                al_draw_ustr(get_font(0), al_map_rgb(255, 255, 255), 10, 10, 0, get_input());
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
    add_new_input_string();
    // set_input(true);
    int spr = get_sprite_by_id(1);

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
            if (e.mouse.y >= 252 && e.mouse.y <= 272) {
                if (e.mouse.x >= 228 && e.mouse.x <= 289) { // CONFIRM button
                    break;
                } else if (e.mouse.x >= 349 && e.mouse.x <= 410) { // CANCEL button
                    return 1;
                }
            } else if (e.mouse.x >= 263 && e.mouse.x <= 269) {
                if (e.mouse.y >= 179 && e.mouse.y <= 187) { // LEFT for HAIR STYLE
                    hair_style = (hair_style+1)%2;
                } else if (e.mouse.y >= 195 && e.mouse.y <= 203) { // LEFT for HAIR COLOR
                    hair_color = (hair_color+5)%6;
                } else if (e.mouse.y >= 211 && e.mouse.y <= 219) { // LEFT for SKIN COLOR
                    skin_color = (skin_color+3)%4;
                }
            } else if (e.mouse.x >= 335 && e.mouse.x <= 341) {
                if (e.mouse.y >= 179 && e.mouse.y <= 187) { // RIGHT for HAIR STYLE
                    hair_style = (hair_style+1)%2;
                } else if (e.mouse.y >= 195 && e.mouse.y <= 203) { // RIGHT for HAIR COLOR
                    hair_color = (hair_color+1)%6;
                } else if (e.mouse.y >= 211 && e.mouse.y <= 219) { // RIGHT for SKIN COLOR
                    skin_color = (skin_color+1)%4;
                }
            } else if (e.mouse.y >= 217 && e.mouse.y <= 226) {
                if (e.mouse.x >= 202 && e.mouse.x <= 213) { // ROTATE CLOCKWISE
                    dir = (dir+7)%8;
                } else if (e.mouse.x >= 232 && e.mouse.x <= 243) { // ROTATE COUNTERCLOCKWISE
                    dir = (dir+1)%8;
                }
            } else if (e.mouse.y >= 195 && e.mouse.y <= 203) {
                if (e.mouse.x >= 376 && e.mouse.x <= 382) { // LEFT for PORTRAIT
                    portrait_type = ((portrait_type+1)%2);
                } else if (e.mouse.x >= 428 && e.mouse.x <= 434) { // RIGHT for PORTRAIT
                    portrait_type = (portrait_type+1)%2;
                }
            }
            if (e.mouse.x >= 191 && e.mouse.x <= 451 && e.mouse.y >= 138 && e.mouse.y <= 160) {
                set_input_string(1);
                // TODO calc where to place cursor
            } else {
                set_input_string(-1);
            }
            update = true;
        } else {
            update = (update_from_event(e) > 0);
        }

        if (update) {
            al_clear_to_color(al_map_rgb(179, 152, 125));
            al_draw_bitmap(get_interface_asset(0), 138, 106, 0);

            al_draw_ustr(get_font(0), al_map_rgb(255, 255, 255), 197, 145, 0, get_input(1));

            //al_draw_bitmap(get_image_asset(8), 200, 200, 0);
            int d = dir;
            d = ((d <= 1 || d >= 7) ? 8-((d+6)%8) : d-2);
            al_draw_tinted_bitmap(get_image_sprite(spr+(frame/10)+(4*d)), s_pigment[skin_color], 216, 177, (dir <= 1 || dir >= 7) ? ALLEGRO_FLIP_HORIZONTAL : 0);

            al_draw_bitmap(get_image_sprite(get_params_by_id(7)[1]+(frame/10)+(4*d)), 216, 177, (dir <= 1 || dir >= 7) ? ALLEGRO_FLIP_HORIZONTAL : 0);
            al_draw_bitmap(get_image_sprite(get_params_by_id(11)[1]+(frame/10)+(4*d)), 216, 177, (dir <= 1 || dir >= 7) ? ALLEGRO_FLIP_HORIZONTAL : 0);

            al_draw_bitmap(get_image_sprite(spr+20+(frame/10)+(4*d)), 216, 177, (dir <= 1 || dir >= 7) ? ALLEGRO_FLIP_HORIZONTAL : 0);
            al_draw_tinted_bitmap(get_image_sprite(spr+40+(40*hair_style)+(frame/10)+(4*d)), h_pigment[hair_color], 216, 177, (dir <= 1 || dir >= 7) ? ALLEGRO_FLIP_HORIZONTAL : 0);
            al_draw_bitmap(get_image_sprite(spr+60+(40*hair_style)+(frame/10)+(4*d)), 216, 177, (dir <= 1 || dir >= 7) ? ALLEGRO_FLIP_HORIZONTAL : 0);

            al_draw_bitmap(get_portrait(portrait_type+1), 391, 182, 0);

            al_flip_display();
            update = false;
        }
    }

    pc = new Player(rand() % 100, get_input(1), skin_color, hair_color, 40+(40*hair_style), portrait_type);
    cout << "[GAME] Equipping player...\n";
    pc->equip_item(dynamic_cast<Equipment*>(create_object(7)), 1);
    pc->equip_item(dynamic_cast<Equipment*>(create_object(11)), 5);

    set_input_string(-1);
    return run_game(events);
}

int load_game(ALLEGRO_EVENT_QUEUE* events) {
    init_game();

    pc = new Player(rand() % 100, al_ustr_new("Ashe"), 2, 1, 69, 1);
    return run_game(events);
}
