#include <allegro5/allegro5.h>

#ifndef GEOMETRY_H
#define GEOMETRY_H

enum DIRECTION {
    NO_DIRECTION = -1,
    EAST = 0,
    NORTHEAST = 1,
    NORTH = 2,
    NORTHWEST = 3,
    WEST = 4,
    SOUTHWEST = 5,
    SOUTH = 6,
    SOUTHEAST = 7
};

class Point {
public:
    float x;
    float y;
    int z;

    Point() { x = 0; y = 0; z = 0; }
    Point(Point*);
    Point(float a, float b) : x(a), y(b), z(0) {};
    Point(float a, float b, int c) : x(a), y(b), z(c) {};

    Point operator +(const Point&);
    Point operator -(const Point&);
    void operator +=(const Point&);
    void operator -=(const Point&);
    bool operator ==(const Point&);
    bool operator !=(const Point&);
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

class Geometry {
private:
    static const int MAX_INT;

    Geometry() {}

public:
    static DIRECTION relative_direction(DIRECTION);
    static DIRECTION convert_point_to_direction(Point*);
    static Point* convert_direction_to_point(DIRECTION);

    static int compare(Point*, Point*, DIRECTION);

    static Point* translate(Point*, DIRECTION, int=1);
};

/**
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
**/

#endif // GEOMETRY_H
