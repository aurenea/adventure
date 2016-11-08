#include <cmath>
#include "geometry.h"

/**
 *  Returns the relative direction of v with respect to d
 */
Direction rel_dir(Direction v, Direction d) {
    return static_cast<Direction>(((int)d-(int)v+10)%8);
}

/**
 *  Converts Point p to Direction
 */
Direction point_to_dir(Point* p) {
    int a = (int)round(atan2(-p->y, p->x)*4/3.1415926535);
    return static_cast<Direction>((a+8)%8);
}

/**
 *  Converts Direction d to Point
 */
Point* dir_to_point(Direction d) {
    Point* p = new Point();
    if (d >= Direction::NORTHWEST && d <= Direction::SOUTHWEST) {
        p->x = -1;
    } else if ((d >= Direction::EAST && d <= Direction::NORTHEAST) || d == Direction::SOUTHEAST) {
        p->x = 1;
    }
    if (d >= Direction::NORTHEAST && d <= Direction::NORTHWEST) {
        p->y = -1;
    } else if (d >= Direction::SOUTHWEST && d <= Direction::SOUTHEAST) {
        p->y = 1;
    }
    return p;
}

/**
 *  Returns new Point translated in Direction d by distance dist
 */
Point* translate(Point* p, Direction d, int dist) {
    Point* v = dir_to_point(d);
    return new Point(p->x+(dist*v->x), p->y+(dist*v->y), p->z);
}

/**
 *  Returns     1 if p1 is in front of p2
 *              -1 if p1 is behind p2
 *              0 if p1 equals p2           relative to dir
 */
int compare(Point* p1, Point* p2, Direction dir) {
    if (p1->x == p2->x && p1->y == p2->y && p1->z == p2->z) { return 0; }
    int x1 = p1->x;
    int y1 = p1->y;
    int x2 = p2->x;
    int y2 = p2->y;
    if (dir >= Direction::SOUTH || dir <= Direction::NORTHEAST) {
        x1 = MAX_VALUE-x1;
        x2 = MAX_VALUE-x2;
    }
    if (dir >= Direction::WEST) {
        y1 = MAX_VALUE-y1;
        y2 = MAX_VALUE-y2;
    }
    int v1 = x1+y1;
    int v2 = x2+y2;
    int d = static_cast<int>(dir);
    if (d%2 == 0) {
        if (d%4 == 0) {
            v1 = x1;
            v2 = x2;
        } else {
            v1 = y1;
            v2 = y2;
        }
    }
    if (v1 > v2) {
        return 1;
    } else if (v1 < v2) {
        return -1;
    } else {
        if ((d/2)%2 == 0) {
            v1 = y1;
            v2 = y2;
        } else {
            v1 = x1;
            v2 = x2;
        }
        if (v1 > v2) {
            return 1;
        } else if (v1 < v2) {
            return -1;
        } else {
            if (p1->z > p2->z) {
                return 1;
            } else if (p1->z < p2->z) {
                return -1;
            } else {
                return 0;
            }
        }
    }
}
