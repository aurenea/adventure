#include "wrapper.h"

GameState::GameState() {
    Interface::init();
    AreaWrapper::init();
}

RUN_RESULT GameState::run(ALLEGRO_EVENT* e) {
    // do stuff

}

Point* GameState::convert_to_screen_coordinates(Point* p) {
    // when center.x and center.y have different parity, results in odd graphical errors
    Point* temp = new Point(320, 201);
    DIRECTION view = AreaWrapper::get_view();
    float x = p->x-AreaWrapper::get_center()->x;
    float y = p->y-AreaWrapper::get_center()->y;
    temp->x += (x+((view%4 == 3 ? -1 : 1)*y))*(view > 4 ? -1 : 1);
    temp->y += (int) ((y+((view%4 == 1 ? -1 : 1)*x))*(view > 4 ? -1 : 1)/2);
    // UGGHHH CAN'T FIGURE THIS OUT TODO LATER
    temp->y -= p->z-AreaWrapper::get_center()->z;
    return temp;
}

Point* GameState::convert_from_screen_coordinates(Point* p) {
    Point* temp = new Point(AreaWrapper::get_center());
    DIRECTION view = AreaWrapper::get_view();
    temp->x += (int)(((p->x-320)/2)+((view%4 == 1 ? -1 : 1)*(p->y-201)))*(view > 4 ? -1 : 1);
    temp->y += (int)((p->y-201)+((view%4 == 3 ? -1 : 1)*(p->x-320)/2))*(view > 4 ? -1 : 1);
    return temp;
}
