#include <iostream>
#include <fstream>
#include "objects.h"
#include "geometry.h"
#include "file-io.h"
#include "options.h"
#include "interface.h"
#include "input-manager.h"

using namespace std;

Object::Object(int d) {
    id = d;
    dir = view_direction();
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
    Point* draw_at = convert_to_screen_coordinates(pos-*get_center());
    if (get_within_stage(this->get_sprite(), draw_at->x, draw_at->y)) {
        ALLEGRO_BITMAP* bitmap = this->get_sprite();
        if (get_flag(1)) {
            draw_tinted_bitmap(bitmap, *get_color(3), draw_at->x-(al_get_bitmap_width(bitmap)/2),
                draw_at->y-al_get_bitmap_height(bitmap), this->get_sprite_flipped());
        } else {
            draw_bitmap(bitmap, draw_at->x-(al_get_bitmap_width(bitmap)/2),
                draw_at->y-al_get_bitmap_height(bitmap), this->get_sprite_flipped());
        }
    }
    delete draw_at;
}

void Object::update() {
    if (mov.x != 0 || mov.y != 0) {
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
                        std::cout << "[GAME::Object] All collision checks failed.\n";
                    }
                }
            }
        }
        if (mov.x != 0 || mov.y != 0) {
            dir = get_angle_of_vector(&mov);
        }
    }
}

int Object::on_click(ALLEGRO_EVENT e) {
    cout << "Object clicked.\n";
    if (is_dragging(this) && e.mouse.button == 2) {
        add_window(new DescriptionWindow(this));
        return 1;
    }
    return 0;
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
    get_main_area()->add_object(thingy);
}

ALLEGRO_USTR* Item::get_description() {
    return get_desc_by_id(id);
}

int Item::on_click(ALLEGRO_EVENT e) {
    cout << "Item clicked.\n";
    return Object::on_click(e);
}

ItemContainer::ItemContainer(int d) : Item(d) {
    num_items = 0;
    items = new Item*[0];
    width = get_params_by_id(id)[0];
    height = get_params_by_id(id)[1];
    remove_flag(2);
}

int ItemContainer::on_click(ALLEGRO_EVENT e) {
    cout << "Item container clicked.\n";
    if (Object::on_click(e)) {
        return 1;
    } else if (is_dragging(this)) {
        // TODO if window is already open, remove window
        add_window(new ContainerWindow(this));
        return 1;
    } else if (Item* item = dynamic_cast<Item*>(get_dragging())) {
        add_item(item);
        return 1;
    }
    cout << "Function of click executed.\n";
    return 0;
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

int Openable::on_click(ALLEGRO_EVENT e) {
    // TODO add collision testing so you can't glitch into a door
    if (!locked && is_dragging(NULL)) {
        opened = !opened;
        return 1;
    } else if (Key* key = dynamic_cast<Key*>(get_dragging())) {
        if (lock_key == key->lock_key) {
            locked = !locked;
            opened = locked;
            return 1;
        }
    }
    // add function for lockpicks here
    return 0;
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

Foodstuff::Foodstuff(int d) : Item(d) {
    hunger = get_params_by_id(id)[0];
}

Plant::Plant(int d) : Item(d) {
    harvest_id = get_params_by_id(id)[0];
    harvested = -1;
    add_flag(0);
}

int Plant::on_click(ALLEGRO_EVENT e) {
    if (Object::on_click(e)) { return 1; }
    Object* hrvst = create_object(harvest());
    if (hrvst != NULL) {
        hrvst->set_position(translate(&pos, (view_direction()+4)%8));
        loc->add_object(hrvst);
    }
    return 1;
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

void Entity::cancel_action() {
    delete action;
    action = NULL;
}

void Entity::harm(int body_part, int d_type) {

}

/*int NPC::on_click(ALLEGRO_EVENT e) {
    if (Object::on_click(e)) {
        return 1;
    } else {
        // TODO insert clause for ATTACK MODE here
    }
    return 0;
}*/

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

    Point* draw_at = convert_to_screen_coordinates(pos-*get_center());
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
        al_unmap_rgb(*get_skin_pigment(s_color), r, g, b);
        draw_tinted_bitmap(bmp1, al_map_rgba((int)r/4, (int)g/4, (int)b/4, 64), draw_at->x, draw_at->y, flip);
        // draw clothing (except head)
        if (equip[7] != NULL && i >= 12) {
            draw_tinted_bitmap(get_image_sprite(equip[7]->w_sprite+i), *get_color(3), draw_at->x, draw_at->y, flip);
        }
        for (int a = 5; a > 0; a--) {
            if (equip[a] != NULL) {
                draw_tinted_bitmap(get_image_sprite(equip[a]->w_sprite+i), *get_color(3), draw_at->x, draw_at->y, flip);
            }
        }
        if (equip[6] != NULL) {
            draw_tinted_bitmap(get_image_sprite(equip[6]->w_sprite+i), *get_color(3), draw_at->x, draw_at->y, flip);
        }
        if (equip[7] != NULL && i < 12) {
            draw_tinted_bitmap(get_image_sprite(equip[7]->w_sprite+i), *get_color(3), draw_at->x, draw_at->y, flip);
        }
        // draw shadowing
        draw_tinted_bitmap(bmp2, *get_color(3), draw_at->x, draw_at->y, flip);
        // draw hair (if they have any)
        if (h_type >= 0) {
            al_unmap_rgb(*get_hair_pigment(h_color), r, g, b);
            draw_tinted_bitmap(get_image_sprite(h_type+i), al_map_rgba((int)r/4, (int)g/4, (int)b/4, 64), draw_at->x, draw_at->y, flip);
            draw_tinted_bitmap(get_image_sprite(h_type+i+20), *get_color(3), draw_at->x, draw_at->y, flip);
        }
        // draw head equipment
        if (equip[0] != NULL) {
            draw_tinted_bitmap(get_image_sprite(equip[0]->w_sprite+i), *get_color(3), draw_at->x, draw_at->y, flip);
        }
    } else {
        // draw body
        draw_tinted_bitmap(bmp1, *get_skin_pigment(s_color), draw_at->x, draw_at->y, flip);
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
            draw_tinted_bitmap(get_image_sprite(h_type+i), *get_hair_pigment(h_color), draw_at->x, draw_at->y, flip);
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

int Person::on_click(ALLEGRO_EVENT e) {
    // right click: followers = CharacterWindow
    // right click: other NPC = pickpocket
    if (is_dragging(this) && e.mouse.button == 2) {
        // TODO insert clause for only followers
        add_window(new CharacterWindow(this));
        return 1;
    }
    if (Object::on_click(e)) { // TODO fix later, I guess????
        return 1;
    } else if (is_dragging(this)) {
        add_window(new ConversationWindow(this));
        return 1;
    }
    return 0;
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
        mov += *translate(origin, view_direction());
    } else if (get_key_down(2) && !get_key_down(0)) { // DOWN key
        mov += *translate(origin, (view_direction()+4)%8);
    }
    if (get_key_down(1) && !get_key_down(3)) { // LEFT key
        mov += *translate(origin, (view_direction()+2)%8);
    } else if (get_key_down(3) && !get_key_down(1)) { // RIGHT key
        mov += *translate(origin, (view_direction()+6)%8);
    }
    delete origin;
    if (mov.length() != 0) {
        cancel_action();
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

int Player::on_click(ALLEGRO_EVENT e) {
    if (is_dragging(this) && e.mouse.button == 2) {
        add_window(new CharacterWindow(this));
        return 1;
    }
    return 0;
}



Player* pc;

Player* get_player() {
    return pc;
}

void set_player(Player* p) {
    pc = p;
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
    Point* o1_pos = convert_to_screen_coordinates(*(o1->get_position())-*get_center());
    Point* o2_pos = convert_to_screen_coordinates(*(o2->get_position())-*get_center());
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
        if (*convert_to_screen_coordinates(*p-*get_center()) >=
            *convert_to_screen_coordinates(*(compare_to->get_position())-*get_center())) {
            return find_object_by_position(p, (si+ei)/2, ei);
        } else {
            return find_object_by_position(p, si, (si+ei)/2);
        }
    }
}

void Area::resort_objects() {
    for (int i = 0; i < num_objects; i++) {
        if (Wall* wall = dynamic_cast<Wall*>(objects[i])) {
            if (*wall->get_position() >= *get_center()) {
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
        return false;
    }
    int i = index-1;
    Point* pos = o->get_position();
    Domain* dom = o->get_domain();
    while ((i >= 0 && i < num_objects) && (abs(objects[i]->get_position()->x-pos->x) <= 100 ||
           abs(objects[i]->get_position()->y-pos->y) <= 100)) {
        // cout << "[GAME::Area] Checking for collisions at index " << i << ", id = " << objects[i]->id << "\n";
        if (objects[i] != o && objects[i]->get_domain()->intersects(dom) && !objects[i]->get_flag(2)) {
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
            if (i < 0 || (abs(objects[i]->get_position()->x-pos->x) > 100 && abs(objects[i]->get_position()->y-pos->y) > 100)) {
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
    int len = max(abs(v->x), abs(v->y));
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



const int DRAW_DISTANCE = 13;
Area* main_area;

Area* get_main_area() {
    return main_area;
}

void set_main_area(Area* new_main) {
    main_area = new_main;
}

void Area::update_objects(ALLEGRO_EVENT e) {
    if (e.type == ALLEGRO_EVENT_TIMER) {
        for (int i = 0; i < num_objects; i++) {
            objects[i]->update();
        }
    }
    if (e.type == ALLEGRO_EVENT_MOUSE_AXES || (e.type == ALLEGRO_EVENT_TIMER && get_any_key_down())) {
        evaluate_mouse_text(e);
    } else if (e.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
        for (int i = num_objects-1; i >= 0; i--) {
            Point* p = convert_to_screen_coordinates(*(objects[i]->get_position())-*get_center());
            if (get_within_sprite(e.mouse.x, e.mouse.y, objects[i]->get_sprite(), p->x, p->y) && !objects[i]->get_flag(1)) {
                if (get_hack_mover()) {
                    ALLEGRO_BITMAP* bmp = objects[i]->get_sprite();
                    set_dragging(objects[i], (p->x-(al_get_bitmap_width(bmp)/2))-e.mouse.x,
                        (p->y-al_get_bitmap_height(bmp))-e.mouse.y);
                } else if (Item* item = dynamic_cast<Item*>(objects[i])) {
                    if (!item->get_flag(0) && is_dragging(NULL)) {
                        ALLEGRO_BITMAP* bmp = item->get_sprite();
                        set_dragging(item, (p->x-(al_get_bitmap_width(bmp)/2))-e.mouse.x,
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
            Point* p = convert_to_screen_coordinates(*(objects[i]->get_position())-*get_center());
            if (get_within_sprite(e.mouse.x, e.mouse.y, objects[i]->get_sprite(), p->x, p->y) && !objects[i]->get_flag(1)) {
                // do the click function
                objects[i]->on_click(e);
                delete p;
                return;
            }
            delete p;
        }
        if (!is_dragging(NULL)) {
            ALLEGRO_BITMAP* bmp = get_dragging()->get_sprite();
            Point* p = new Point(e.mouse.x+get_affixation()->x+(al_get_bitmap_width(bmp)/2),
                e.mouse.y+get_affixation()->y+al_get_bitmap_height(bmp));
            get_dragging()->set_position(convert_from_screen_coordinates(*p));
            cout << "<" << get_dragging()->get_position()->x << "," << get_dragging()->get_position()->y << ">\n";
            delete p;
            main_area->add_object(get_dragging());
        } else {
            Point* p = new Point(e.mouse.x, e.mouse.y);
            get_player()->set_destination(convert_from_screen_coordinates(*p));
            delete p;
        }
    }
}

void draw_at_center() {
    al_clear_to_color(*get_color(0));

    // DRAW THE TILES OF THE TERRAIN
    Point* indices = new Point(get_center()->x/32, get_center()->y/32);
    Point* offset = new Point((int)get_center()->x%32, (int)get_center()->y%32);
    // if facing NW or SE, calculates offset from y=x line; else if facing SW or NE, calculates offset from y=-x line
    // then, if facing SW or SE, inverts this value
    int disx = (view_direction()%4 == 3 ? offset->x-offset->y : offset->x+offset->y)*(view_direction() > 4 ? 1 : -1);
    disx += (view_direction() == NORTHEAST ? 32 : (view_direction() == SOUTHWEST ? -32 : 0));
    // calculates offset from farthest point in tile: (for indices j and k)
    //      for NW3, (32*j,32*k)
    //      for SW5, (32*j,32*(k+1))
    //      for SE7, (32*(j+1),32*(k+1))
    //      for NE1, (32*(j+1),32*k)
    int disy = -((view_direction()%6 == 1 ? 32-offset->x : offset->x)+(view_direction() > 4 ? 32-offset->y : offset->y))/2;
    disy += get_center()->z;
    Point* basis = translate(indices, view_direction(), DRAW_DISTANCE);
    for (int i = -DRAW_DISTANCE; i <= DRAW_DISTANCE; i++) {
        if (i != -DRAW_DISTANCE) { basis = translate(basis, (view_direction()+5)%8); }
        for (int j = -DRAW_DISTANCE; j <= DRAW_DISTANCE; j++) {
            int x = 320+(32*(i-j))+disx-32;
            int y = 200+(16*(i+j))+disy;
            if (x > -64 && x <= 640 && y > -32 && y <= 400) {
                Point* p = translate(basis, (view_direction()+3)%8, j+DRAW_DISTANCE);
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

void evaluate_mouse_text(ALLEGRO_EVENT e) {
    for (int i = main_area->num_objects-1; i >= 0; i--) {
        Point* p = convert_to_screen_coordinates(*(main_area->objects[i]->get_position())-*get_center());
        if (main_area->objects[i]->get_sprite() == NULL) {
            cout << al_cstr(main_area->objects[i]->get_name()) << "\n";
        }
        if (get_within_sprite(e.mouse.x, e.mouse.y, main_area->objects[i]->get_sprite(), p->x, p->y)
                && !main_area->objects[i]->get_flag(1)) {
            set_mouse_text(main_area->objects[i]->get_name());
            delete p;
            return;
        }
        delete p;
    }
    clear_mouse_text();
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
    // cout << "[GAME::MoveAction] d = <" << d->x << ", " << d->y << ">\n";
    time_remaining = -1;
    next_action = n_act;
    destination = new Point(0, 0);
    towards = new Point(0, 0);
    evaluate_direction(s, d);
}

void MoveAction::evaluate_direction(Point* s, Point* d) {
    Point* v = new Point(d->x-s->x, d->y-s->y);
    // cout << "[GAME::MoveAction] v = <" << v->x << ", " << v->y << ">\n";
    int dir = get_angle_of_vector(v);
    // cout << "[GAME::MoveAction] dir = " << dir << "\n";
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
        // cout << "[GAME::MoveAction] uh oh\n";
    } else {
        towards->set_to(destination->x/abs(destination->x == 0 ? 1 : destination->x),
            destination->y/abs(destination->y == 0 ? 1 : destination->y));
        // cout << "[GAME::MoveAction] towards = <" << towards->x << ", " << towards->y << ">\n";
        destination->set_to(destination->x+s->x, destination->y+s->y);
        // cout << "[GAME::MoveAction] destination = <" << destination->x << ", " << destination->y << ">\n";
        next_action = (*destination == *d ? next_action : new MoveAction(actor, destination, d, next_action));
    }
    delete v;
}

Point* MoveAction::get_direction() {
    return towards;
}

void MoveAction::update() {
    Action::update();
    Point* p = actor->get_position();
    if (*p == *destination) {
        completed = true;
    } else {
        if (towards->x != 0) {
            if ((destination->x-p->x)/towards->x <= 0 || (towards->y != 0 && (destination->x-p->x)/towards->x != (destination->y-p->y)/towards->y)) {
                evaluate_direction(p, destination);
            }
        } else if (towards->y != 0) {
            if ((destination->y-p->y)/towards->y <= 0) {
                evaluate_direction(p, destination);
            }
        }
    }
}

Attack::Attack(Entity* e, Object* o, Action* n_act) : Action(e, o, n_act) {
    // TODO more here probably?
}

int Attack::execute() {
    if (can_be_executed() && time_remaining == 0) {
        if (Entity* ent = dynamic_cast<Entity*>(obj)) {
            if (actor == get_player()) {
                if (get_player()->get_hands(0) != NULL) {
                    get_player()->add_CMB(get_player()->get_hands(0)->get_type());
                }
                if (get_player()->get_hands(1) != NULL) {
                    get_player()->add_CMB(get_player()->get_hands(1)->get_type());
                }
                if (get_player()->get_hands(0) == NULL && get_player()->get_hands(1) == NULL) {
                    get_player()->add_CMB(0);
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
        if (ent == get_player()) { get_player()->add_EVA(); }
    }
}
