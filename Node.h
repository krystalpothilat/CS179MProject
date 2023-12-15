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
    int costNoH=0;
    int depth=0;
    int totalCost_f=0;
    int cost_g=0;
    int heuristic_h=0;
    bool notValid=false;
    Move* operation=NULL;

    vector<Container *> containersToLoad;
    vector<Container *> containersToUnload;
    vector<Node*> alreadyExpanded;

    Node();
};

#endif // NODE_H
