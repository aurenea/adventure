#include <allegro5/allegro5.h>
#include "geometry.h"
#include "assets.h"

#ifndef OBJECTS_H
#define OBJECTS_H

class Area;
class Action;

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
        Point* get_position() { return &pos; }
        void set_position(int, int);
        void set_position(Point*);
        Point* get_movement() { return &mov; }
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
        Foodstuff(int);
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
        void cancel_action();
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

Player* get_player();
void set_player(Player*);

Object* create_object(int);


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

Area* get_main_area();
void set_main_area(Area*);
void draw_at_center();
void evaluate_mouse_text(ALLEGRO_EVENT);


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
        void evaluate_direction(Point*, Point*);
        Point* get_direction();
        void update();
        int execute() { return 1; }
};

class Attack: public Action {
    protected:
        int body_part = -1;
    public:
        Attack(Entity*, Object*, Action* = NULL);
        int execute();
        void make_attack(Entity*, bool);
};

#endif // OBJECTS_H
