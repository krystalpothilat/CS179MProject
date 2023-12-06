#include "Move.h"

Move::Move() {
    container = new Container();
    final_location = "";
    time = 0;
}

Move::Move(Container* c, string fl, unsigned int t) {
    container = c;
    final_location = fl;
    time = t;
}

Container* Move::get_container() {
    return container;
}

string Move::get_final_location() {
    return final_location;
}

unsigned int Move::get_time() {
    return time;
}
