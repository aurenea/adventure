#include <iostream>
#include <string>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_image.h>

using namespace std;

ALLEGRO_BITMAP *get_tile_asset(int key);

class Point {
    public:
        int x;
        int y;
        Point() { x = 0; y = 0; }
        Point(int a,int b) : x(a), y(b) {};
        Point operator +(const Point&);
        void operator +=(const Point&);
};

Point Point::operator+(const Point& point) {
    Point temp;
    temp.x = this->x + point.x;
    temp.y = this->y + point.y;
    return temp;
}

void Point::operator+=(const Point& point) {
    this->x += point.x;
    this->y += point.y;
}

class Area {
        int tiles[64][64];
    public:
        Area(int,int);
        ALLEGRO_BITMAP* get_tile(int,int);
};

Area::Area(int x, int y) {
    // load the area here
    for (int i = 0; i < 64; i++) {
        for (int j = 0; j < 64; j++) { tiles[i][j] = (i+j)%2; }
    }
}

ALLEGRO_BITMAP* Area::get_tile(int x, int y) {
    return get_tile_asset(tiles[x][y]);
}

const int EAST = 0;
const int NORTHEAST = 1;
const int NORTH = 2;
const int NORTHWEST = 3;
const int WEST = 4;
const int SOUTHWEST = 5;
const int SOUTH = 6;
const int SOUTHEAST = 7;
int view_angle = NORTHEAST;

Point translate(Point point, int angle, int distance = 1) {
    Point temp;
    if (angle >= NORTHWEST && angle <= SOUTHWEST) {
        temp.x = -distance;
    } else if ((angle >= EAST && angle <= NORTHEAST) || angle == NORTHWEST) {
        temp.x = distance;
    }
    if (angle >= NORTHEAST && angle <= NORTHWEST) {
        temp.y = -distance;
    } else if (angle >= SOUTHWEST && angle <= SOUTHEAST) {
        temp.y = distance;
    }
    return point+temp;
}

const int DRAW_DISTANCE = 6;

Area main_area(0, 0);

void draw_at_center() {
    // cout << "Drawing...\n";
    // 1040 for center, 1024 for edge
    Point center(1024, 1024);
    Point indices(center.x/32, center.y/32);
    Point offset(center.x%32, center.y%32);
    // if facing NW or SE, calculates offset from y=x line; else if facing SW or NE, calculates offset from y=-x line
    // then, if facing SW or SE, inverts this value
    int disx = (view_angle%4 == 3 ? offset.x-offset.y : offset.x+offset.y)*(view_angle > 4 ? -1 : 1);
    disx += (view_angle == NORTHEAST ? 32 : (view_angle == SOUTHWEST ? -32 : 0));
    // calculates offset from farthest point in tile: (for indices j and k)
    //      for NW3, (32*j,32*k)
    //      for SW5, (32*j,32*(k+1))
    //      for SE7, (32*(j+1),32*(k+1))
    //      for NE1, (32*(j+1),32*k)
    int disy = -((view_angle%6 == 1 ? 32-offset.x : offset.x)+(view_angle > 4 ? 32-offset.y : offset.y))/2;
    Point basis = translate(indices, view_angle, DRAW_DISTANCE);
    for (int i = -DRAW_DISTANCE; i <= DRAW_DISTANCE; i++) {
        if (i != -DRAW_DISTANCE) { basis = translate(basis, (view_angle+3)%8); }
        for (int j = -DRAW_DISTANCE; j <= DRAW_DISTANCE; j++) {
            Point p = translate(basis, (view_angle+5)%8, j+DRAW_DISTANCE);
            ALLEGRO_BITMAP *tile = main_area.get_tile(p.x, p.y);
            al_draw_bitmap(tile, 320+(32*(i-j))+disx-32, 200+(16*(i+j))+disy, 0);
        }
    }
    ALLEGRO_COLOR color = al_map_rgb(0, 0, 0);
    al_put_pixel(320, 200, color);
}
