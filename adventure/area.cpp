#include "area.h"

void Area::sort_all(int start, int stop) {
    if (start >= stop) { return; }
    int mid = (start+stop)/2;
    sort_all(start, mid);
    sort_all(mid+1, stop);
    int left = 0;
    int right = mid+1;
    if (Geometry::compare(vassals[mid]->get_position(), vassals[right]->get_position(), AreaWrapper::get_view()) <= 0) { return; }
    while (left <= mid && right <= stop) {
        if (Geometry::compare(vassals[left]->get_position(), vassals[right]->get_position(), AreaWrapper::get_view()) <= 0) {
            left++;
        } else {
            Object* tmp = vassals[right];
            for (int i = right; i > left; i--) { vassals[i] = vassals[i-1]; }
            vassals[left] = tmp;
            left++; mid++; right++;
        }
    }
}

void Area::sort_all() {
    sort_all(0, length-1);
}

void AreaWrapper::init() {
    // do stuff
}

Area* AreaWrapper::get_area(DIRECTION d) {
    Point* p = Geometry::convert_direction_to_point(d);
    int x = (int)p->x;
    int y = (int)p->y;
    delete p;
    return vassals[x][y];
}

void AreaWrapper::set_view(DIRECTION new_view) {
    AreaWrapper::view_direction = new_view;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            vassals[i][j]->sort_all();
        }
    }
}
