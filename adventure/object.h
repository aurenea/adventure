#ifndef OBJECT_H
#define OBJECT_H

#include "form.h"
#include "intelligence.h"

/**
 *  Basic physical object
 */
class Object: public Form {

};

/**
 *  An inanimate object
 */
class Item: public Object {

};

/**
 *  An Item that can store other items
 */
class Container: public Item, public Lord<Item*, Point*> {

};

/**
 *  An animate object with some sort of agency
 */
class Entity: public Object {
private:
    Agent* ai;
};

/**
 *  An Entity which can equip items
 */
class Attired: public Entity, public Lord<Item*, int> {

};

#endif // OBJECT_H
