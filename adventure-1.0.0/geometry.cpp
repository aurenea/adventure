#include <cmath>
#include "geometry.h"

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



int view_angle = NORTHWEST;

int view_direction() {
    return view_angle;
}

void rotate_view_clockwise() {
    view_angle = (view_angle+6)%8;
}

void rotate_view_counterclockwise() {
    view_angle = (view_angle+2)%8;
}

int rel_direction(int dir) {
    return (dir-view_angle+10)%8;
}

Point* translate(Point* point, int angle, int distance) {
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
