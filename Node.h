#ifndef NODE_H
#define NODE_H

#include "Slot.h"
#include "Move.h"

class Node
{
public:
    Slot shipState[108];
    Slot containerToDrop;
    Node* parent=NULL;
    int totalCost_f=0;
    int cost_g=0;
    int heuristic_h=0;
    bool notValid=false;
    Move* operation=NULL;

    Node();
    void test(Slot shipState[]);
};

#endif // NODE_H
