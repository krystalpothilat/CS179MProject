#ifndef MOVE_H
#define MOVE_H
#include "Container.h"
#include <vector>

class Move {

private:
    Container* container;
    string final_location;// suggested format: truck - "t", ship - "s 00,00", buffer - "b 00,00"
    unsigned int time;//time in minutes it takes for move to be completed
public:
    Move();
    Move(Container*, string, unsigned int);
    Container* get_container();
    string get_final_location();
    unsigned int get_time();
};
#endif // MOVE_H
