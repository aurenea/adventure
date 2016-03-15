#include <allegro5/allegro5.h>

#ifndef GEOMETRY_H
#define GEOMETRY_H

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
        void set_to(float, float, int = -1);
        float length(bool = false);
};

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

const int EAST = 0;
const int NORTHEAST = 1;
const int NORTH = 2;
const int NORTHWEST = 3;
const int WEST = 4;
const int SOUTHWEST = 5;
const int SOUTH = 6;
const int SOUTHEAST = 7;

int view_direction();
void rotate_view_clockwise();
void rotate_view_counterclockwise();
int rel_direction(int);
Point* translate(Point*, int, int = 1);
int get_angle_of_vector(Point*);

Point* get_center();
void set_center(Point*);
Point* convert_to_screen_coordinates(Point);
Point* convert_from_screen_coordinates(Point);

bool get_within_sprite(int, int, ALLEGRO_BITMAP*, int, int);
bool get_within_stage(ALLEGRO_BITMAP*, int, int);

#endif // GEOMETRY_H
