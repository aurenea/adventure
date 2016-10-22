#include <cmath>
#include "geometry.h"

Point::Point(Point* p) {
    x = p->x;
    y = p->y;
    z = p->z;
}

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

bool Point::operator!=(const Point& point) {
    return (x != point.x || y != point.y || z != point.z);
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


DIRECTION Geometry::relative_direction(DIRECTION d, DIRECTION v) {
    return static_cast<DIRECTION>((d-v+10)%8);
}

DIRECTION Geometry::convert_point_to_direction(Point* p) {
    int a = (int)round(atan2(-p->y, p->x)*4/3.1415926535);
    return static_cast<DIRECTION>((a+8)%8);
}

Point* Geometry::convert_direction_to_point(DIRECTION d) {
    Point* temp = new Point();
    if (d >= NORTHWEST && d <= SOUTHWEST) {
        temp->x = -1;
    } else if ((d >= EAST && d <= NORTHEAST) || d == SOUTHEAST) {
        temp->x = 1;
    }
    if (d >= NORTHEAST && d <= NORTHWEST) {
        temp->y = -1;
    } else if (d >= SOUTHWEST && d <= SOUTHEAST) {
        temp->y = 1;
    }
    return temp;
}

/**
 * Returns:
 *      1 if p1 is in front of p2
 *      -1 if p1 is in front of p1
 *      0 if the two are equal
 */
int Geometry::compare(Point* p1, Point* p2, DIRECTION d) {
    if (d != NO_DIRECTION) {
        if (p1->x == p2->x && p1->y == p2->y && p1->z == p2->z) { return 0; }
        float x1 = 0;
        float x2 = 0;
        if (d >= NORTH && d <= SOUTHWEST) {
            x1 = p1->x;
            x2 = p2->x;
        } else {
            x1 = Geometry::MAX_INT-p1->x;
            x2 = Geometry::MAX_INT-p2->x;
        }
        float y1 = 0;
        float y2 = 0;
        if (d >= EAST && d <= NORTHWEST) {
            y1 = p1->y;
            y2 = p2->y;
        } else {
            y1 = Geometry::MAX_INT-p1->y;
            y2 = Geometry::MAX_INT-p2->y;
        }
        float v11 = (d%2 == 1 ? x1+y1 : (d%4 == 0 ? x1 : y1));
        float v21 = (d%2 == 1 ? x2+y2 : (d%4 == 0 ? x2 : y2));
        if (v11 == v21) {
            float v12 = ((d/2)%2 == 0 ? y1 : x1);
            float v12 = ((d/2)%2 == 0 ? y2 : x2);
            if (v12 == v22) {
                if (p1->z == p2->z) {
                    return 0;
                } else if (p1->z > p2->z) {
                    return 1;
                } else {
                    return -1;
                }
            } else if (v12 > v21) {
                return 1;
            } else {
                return -1;
            }
        } else if (v11 > v21) {
            return 1;
        } else {
            return -1;
        }
    }
}

Point* Geometry::translate(Point* point, DIRECTION direction, int distance) {
    Point* temp = Geometry::convert_direction_to_point(direction);
    temp->set_to((distance*temp->x)+point->x, (distance*temp->y)+point->y);
    return temp;
}






Point* center;

Point* get_center() {
    return center;
}

void set_center(Point* p) {
    center = p;
}

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
