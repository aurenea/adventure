#ifndef GEOMETRY_H
#define GEOMETRY_H

const int MAX_VALUE = 2048;

enum class Direction : int {
    EAST = 0,
    NORTHEAST = 1,
    NORTH = 2,
    NORTHWEST = 3,
    WEST = 4,
    SOUTHWEST = 5,
    SOUTH = 6,
    SOUTHEAST = 7
};

struct Point {
    int x;
    int y;
    int z;
    Point() {}
    Point(int a, int b) : x(a%MAX_VALUE), y(b%MAX_VALUE) {}
    Point(int a, int b, int c) : x(a%MAX_VALUE), y(b%MAX_VALUE), z(c%MAX_VALUE) {}
};

struct Shape {
    Point* center;
    int dx;
    int dy;
    int dz;
};

Direction rel_dir(Direction, Direction);
Direction point_to_dir(Point*);
Point* dir_to_point(Direction);
Point* translate(Point*, Direction, int=1);

int compare(Point*, Point*, Direction=Direction::NORTH);

#endif // GEOMETRY_H
