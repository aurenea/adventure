#include <string>
#include "area.h"

Area::Area(Direction* d) {
    first = capacity/2;
    last = first;
    face = d;
}

void Area::swap_objects(unsigned int i, unsigned int j) {
    Object* temp = vassals[i];
    vassals[i] = vassals[j];
    vassals[j] = temp;
}

/**
 *  Returns the index of the first found Object with Point p,
 *      or index of first Object with Point in front of p (if no such Object exists in vassals)
 *  Searches from start_i to end_i (exclusive)
 */
unsigned int Area::find_index(Point* p, unsigned int start_i, unsigned int end_i) {
    unsigned int si = start_i, ei = end_i;
    while (ei-si > 1) {
        int c = compare(p, vassals[(si+ei)/2]->get_pos(), *face);
        if (c == 0) { return (si+ei)/2; }
        else if (c > 0) { si = (si+ei)/2; }
        else { ei = (si+ei)/2; }
    }
    if (compare(p, vassals[si]->get_pos(), *face) >= 0) {
        while (si >= start_i && compare(p, vassals[si]->get_pos(), *face) >= 0) {
            si--;
        }
        return (si+1);
    } else {
        while (si < end_i && compare(p, vassals[si]->get_pos(), *face) < 0) {
            si++;
        }
        return si;
    }
}

/**
 *  Finds the specific place for the object and sorts it to that location
 */
void Area::resort_object(unsigned int index) {
    unsigned int k = find_index(vassals[index]->get_pos(), first, index);
    if (k < index) {
        Object* temp = vassals[index];
        for (unsigned int i = index; i > k; i--) {
            vassals[i] = vassals[i-1];
        }
        vassals[k] = temp;
    } else {
        k = find_index(vassals[index]->get_pos(), index+1, last);
        if (k > index) {
            Object* temp = vassals[index];
            for (unsigned int i = index; i < k-1; i++) {
                vassals[i] = vassals[i+1];
            }
            vassals[k-1] = temp;
        }
    }
}

/**
 *  Sorts everything from si to ei, ei inclusive
 *      Uses quicksort, always choosing (si+ei)/2 as the pivot
 */
void Area::resort(unsigned int si, unsigned int ei) {
    if (ei-si <= 1) { return; }
    swap_objects((si+ei)/2, ei);
    Point* pivot = vassals[ei]->get_pos();
    unsigned int i = si;
    for (unsigned int j = i; j < ei; j++) {
        if (compare(vassals[j]->get_pos(), pivot, *face) <= 0) {
            swap_objects(j, i++);
        }
    }
    swap_objects(i, ei);
    resort(si, i-1);
    resort(i+1, ei);

    /*
    unsigned int mi = (si+ei)/2;
    resort(si, mi);
    resort(mi, ei);
    unsigned int l = si;
    unsigned int r = mi;
    if (compare(vassals[l]->get_pos(), vassals[r]->get_pos(), wrapper->get_face()) <= 0) { return true; }
    while (l < mi && r < ei) {
        if (compare(vassals[l]->get_pos(), vassals[r]->get_pos(), wrapper->get_face()) > 0) {
            Object* temp = vassals[r];
            for (int i = r; i > l; i--) {
                vassals[i] = vassals[i-1];
            }
            vassals[l] = temp;
            mi++; r++;
        }
        l++;
    }
    return false;
    */
}

unsigned int Area::find_index(Point* p) {
    return find_index(p, first, last);
}

bool Area::add(Object* o) {
    return add(o, o->get_pos());
}

bool Area::add(Object* o, Point* p) {
    bool add_before = (compare(p, vassals[(first+last)/2]->get_pos(), *face) <= 0);
    if ((add_before && first == 0) || (!add_before && last == capacity)) {
        unsigned int new_capacity = capacity*3/2;
        if (new_capacity < capacity) { throw std::string("Too many objects in area."); }
        Object** new_vassals = new Object*[new_capacity]();
        unsigned int m = (add_before ? (capacity/2)-1 : 0);
        for (unsigned int i = first+m; i < last; i++) {
            if ((m == 0 || m == (capacity/2)-1) && compare(p, vassals[i-m]->get_pos(), *face) < 0) {
                new_vassals[i] = o;
                m++;
            } else {
                new_vassals[i] = vassals[i-m];
            }
        }
        if (add_before) { first--; }
        else { last++; }

        delete[] vassals;
        vassals = new_vassals;
        capacity = new_capacity;
    } else {
        unsigned int index = (add_before ? (--first) : (last++));
        vassals[index] = o;
        resort_object(index);
    }
    return true;

    /*
    if (last == capacity) {
        unsigned int new_capacity = capacity*7/4;
        if (new_capacity < capacity) { throw std::string("Too many objects in area."); }
        Object** new_vassals = new Object*[new_capacity];
        bool added = false;
        for (int i = 0; i < last; i++) {
            if (added) {
                new_vassals[i+1] = vassals[i];
            } else if (compare(p, vassals[i]->get_pos(), face) < 0) {
                new_vassals[i] = o;
                new_vassals[i+1] = vassals[i];
                added = true;
            } else {
                new_vassals[i] = vassals[i];
            }
        }
    } else {
        unsigned int index = find_index(p);
        for (unsigned int i = last-1; i > index; i--) {
            vassals[i] = vassals[i-1];
        }
        vassals[index] = o;
    }
    last++;
    return true;
    */
}

Object* Area::discard(Object* o) {
    unsigned int index = find_index(o->get_pos());
    if (vassals[index] != o) {
        for (unsigned int i = index; i < last; i++) {
            if (vassals[i] == o) {
                index = i;
                break;
            } else if (i == last-1 || compare(o->get_pos(), vassals[i]->get_pos(), *face) != 0) {
                throw std::string("Attempted to delete Object not in Area.");
            }
        }
    }

    if (index <= (first+last)/2) {
        for (unsigned int i = index; i > first; i--) {
            vassals[i] = vassals[i-1];
        }
        vassals[first++] = NULL;
    } else {
        for (unsigned int i = index; i < last-1; i++) {
            vassals[i] = vassals[i+1];
        }
        vassals[--last] = NULL;
    }
    return o;

    /*
        unsigned int i = index;
        while (vassals[i] != o) {
            if (i >= last-1 || compare(o->get_pos(), vassals[i]->get_pos(), wrapper->get_face()) != 0) {
                break;
            } else {
                i++;
            }
        }
        if (vassals[i] == o) {
            index = i;
        } else {
            i = index;
            while (vassals[i] != o) {
                if (i <= first || compare(o->get_pos(), vassals[i]->get_pos(), wrapper->get_face()) != 0) {
                    break;
                } else {
                    i--;
                }
            }
            if (vassals[i] == o) {
                index = i
            } else {
                throw std::string("Attempted to delete Object not in Area.");
            }
        }
    }
    bool del_before =
    for (unsigned int i = last-1; i > index; i--) {
        vassals[i-1] = vassals[i];
    }
    return o;
    */
}

Object* Area::get(Point* p) {
    unsigned int index = find_index(p);
    if (compare(p, vassals[index]->get_pos(), *face) != 0) {
        throw 0;
    } else {
        return vassals[index];
    }
}

