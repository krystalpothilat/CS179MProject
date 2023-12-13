#ifndef SEARCH_H
#define SEARCH_H

using namespace std;
#include <string>
#include <iostream>
#include <vector>
#include <fstream>
#include "Move.h"
#include "Slot.h"
#include "Node.h"
#include "Container.h"

class Search
{
public:
    string manifestPath;
    vector<Move*>getMovesList();
    void printState(Node n, int slotsNum);
    vector<Node> expand(Node n, int totalCols, int slotsNum);
    int getEmptyLoc(int col, Node n);
    bool isGoalState(Node n, string type,int orderedWeights[]);
    vector<Move*> trace(Node n);
    int getContainerLoc(int col, Node n);
    int getHeuristic(Node n);
    Node getGoalNode(Node root,int totalCols, int slotNums, string type,int orderedWeights[]);
    Search();
};

#endif // SEARCH_H
