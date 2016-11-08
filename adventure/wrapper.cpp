#include "wrapper.h"

Area* load_area(Area* a, Direction d) { return nullptr; }

/**
 *  Sets a as current Area
 */
bool Wrapper::add(Area* a) {
    delete current;
    current = a;
    for (int i = 0; i < 8; i++) {
        delete vassals[i];
        vassals[i] = load_area(current, static_cast<Direction>(i));
    }
}

/**
 *  Sets a as the Area in direction d
 */
bool Wrapper::add(Area* a, Direction d) {
    delete vassals[(int)d];
    vassals[(int)d] = a;
}

/**
 *  Sets the Area in direction d as current
 */
bool Wrapper::add(Direction dir) {
    int d = (int)dir;
    delete vassals[(d+4)%8];
    vassals[(d+4)%8] = current;
    current = vassals[d];
    vassals[d] = load_area(current, static_cast<Direction>(d));
    if (d%2 == 0) {
        delete vassals[(d+3)%8];
        vassals[(d+3)%8] = vassals[(d+2)%8];
        vassals[(d+2)%8] = vassals[(d+1)%8];
        vassals[(d+1)%8] = load_area(current, static_cast<Direction>((d+1)%8));
        delete vassals[(d+5)%8];
        vassals[(d+5)%8] = vassals[(d+6)%8];
        vassals[(d+6)%8] = vassals[(d+7)%8];
        vassals[(d+7)%8] = load_area(current, static_cast<Direction>((d+7)%8));
    } else {
        delete vassals[(d+2)%8];
        vassals[(d+2)%8] = load_area(current, static_cast<Direction>((d+2)%8));
        delete vassals[(d+3)%8];
        vassals[(d+3)%8] = vassals[(d+1)%2];
        vassals[(d+1)%8] = load_area(current, static_cast<Direction>((d+1)%8));
        delete vassals[(d+5)%8];
        vassals[(d+5)%8] = vassals[(d+7)%8];
        vassals[(d+7)%8] = load_area(current, static_cast<Direction>((d+7)%8));
        delete vassals[(d+6)%8];
        vassals[(d+6)%8] = load_area(current, static_cast<Direction>((d+6)%8));
    }
}

/**
 *  Deletes a
 *  Returns NULL
 */
Area* Wrapper::discard(Area* a) {
    if (current == a) { delete a; }
    else {
        for (int i = 0; i < 8; i++) {
            if (vassals[i] == a) {
                delete vassals[i];
                break;
            }
        }
    }
    return NULL;
}

/**
 *  Retrieves the Area in Direction d
 */
Area* Wrapper::get(Direction d) {
    return vassals[(int)d];
}


bool Wrapper::add(Object* o) {
    return add(o, o->get_pos());
}

bool Wrapper::add(Object* o, Point* p) {
    if (p->x >= 0 && p->y >= 0 && p->x < MAX_VALUE && p->y < MAX_VALUE) {
        return current->add(o, p);
    }
    int dx = -1;
    int dy = -1;
    if (p->x < 0) {
        dx = (int)Direction::WEST;
        p->x = (MAX_VALUE+p->x) % MAX_VALUE;
    } else if (p->x >= MAX_VALUE) {
        dx = (int)Direction::EAST;
        p->x = p->x % MAX_VALUE;
    }
    if (p->y < 0) {
        dy = (int)Direction::NORTH;
        p->y = (MAX_VALUE+p->y) % MAX_VALUE;
    } else if (p->y >= MAX_VALUE) {
        dy = (int)Direction::SOUTH;
        p->y = p->y % MAX_VALUE;
    }
    return vassals[((dx < 0) ? dy : ((dy < 0) ? dx : (dx+dy)/2))]->add(o, p);
}


/**
 *  Rotate view clockwise
 */
void Wrapper::c_rotate() {
    face = static_cast<Direction>((6+(int)face)%8);
    current->resort_all();
    for (int i = 0; i < 8; i++) {
        vassals[i]->resort_all();
    }
}

/**
 *  Rotate view counter-clockwise
 */
void Wrapper::cc_rotate() {
    face = static_cast<Direction>((2+(int)face)%8);
    current->resort_all();
    for (int i = 0; i < 8; i++) {
        vassals[i]->resort_all();
    }
}
