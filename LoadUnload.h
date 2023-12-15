#ifndef LOADUNLOAD_H
#define LOADUNLOAD_H

using namespace std;
#include <string>
#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include "Move.h"
#include "Slot.h"
#include "Node.h"
#include "Container.h"

class LoadUnload
{
public:
    string manifestPath;
    vector<Move*>getMovesList(vector<Container *> containersToLoad, vector<Container *> containersToUnload);
    void printState(Node* n, int slotsNum);
    vector<Node*> expand(Node* n, int totalCols, int slotsNum);
    int getEmptyLoc(int col, Node* n);
    bool isGoalState(Node* n);
    vector<Move*> trace(Node* n);
    int getContainerLoc(int col, Node* n);
    int getHeuristic(Node* n, int slotsNum);
    Node* getGoalNode(Node* root,int totalCols, int slotNums);
    bool isSameNode(Node* newNode, Node* n, int slotsNum);
    LoadUnload();
};

#endif // LOADUNLOAD_H
