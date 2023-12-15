#ifndef NODE_H
#define NODE_H

#include "Slot.h"
#include "Move.h"

class Node
{
public:
    Slot shipState[204];
    Slot containerToDrop;
    Node* parent;
    int totalCost_f=0;
    int cost_g=0;
    int heuristic_h=0;
    bool notValid=false;
    Move* operation=NULL;

    vector<Container *> containersToLoad;
    vector<Container *> containersToUnload;

    Node();
};

#endif // NODE_H
