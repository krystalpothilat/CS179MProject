#include "Search.h"

Search::Search() {

}

void Search::printState(Node n, int slotsNum){
    if(n.containerToDrop.location==""){
        for(int j=0; j</*slotsNum*/24;j++){
            cout<<"\n"<<n.shipState[j].location<<" "<<n.shipState[j].description<<" "<<n.shipState[j].weight;
        }
    }else{
        cout<<"\nlocation of container to drop:"<<n.containerToDrop.location;
    }
}

int Search::getEmptyLoc(int col, Node n){
    if(n.shipState[col+0].description=="UNUSED"){
        return col+0;
    }else if(n.shipState[col+12].description=="UNUSED"){
        return col+12;
    }else if(n.shipState[col+24].description=="UNUSED"){
        return col+24;
    }else if(n.shipState[col+36].description=="UNUSED"){
        return col+36;
    }else if(n.shipState[col+48].description=="UNUSED"){
        return col+48;
    }else if(n.shipState[col+60].description=="UNUSED"){
        return col+60;
    }else if(n.shipState[col+72].description=="UNUSED"){
        return col+72;
    }else if(n.shipState[col+84].description=="UNUSED"){
        return col+84;
    }else if(n.shipState[col+96].description=="UNUSED"){
        return col+96;
    }else{
        return -1; //column is all full
    }
}

int Search::getContainerLoc(int col, Node n){
    if(n.shipState[col+96].description!="UNUSED" && n.shipState[col+96].description!="NAN"){
        return col+96;
    }else if(n.shipState[col+84].description!="UNUSED" && n.shipState[col+84].description!="NAN"){
        return col+84;
    }else if(n.shipState[col+72].description!="UNUSED" && n.shipState[col+72].description!="NAN"){
        return col+72;
    }else if(n.shipState[col+60].description!="UNUSED" && n.shipState[col+60].description!="NAN"){
        return col+60;
    }else if(n.shipState[col+48].description!="UNUSED" && n.shipState[col+48].description!="NAN"){
        return col+48;
    }else if(n.shipState[col+36].description!="UNUSED" && n.shipState[col+36].description!="NAN"){
        return col+36;
    }else if(n.shipState[col+24].description!="UNUSED" && n.shipState[col+96].description!="NAN"){
        return col+24;
    }else if(n.shipState[col+12].description!="UNUSED" && n.shipState[col+12].description!="NAN"){
        return col+12;
    }else if(n.shipState[col+0].description!="UNUSED" && n.shipState[col+0].description!="NAN"){
        return col+0;
    }else{
        return -1; //no containers to pick up
    }
}

vector<Node> Search::expand(Node n, int totalCols, int slotsNum){
    vector<Node> newNodes;
    int containerLoc;
    int openSpot;
    if(n.containerToDrop.arrayLoc==-1){
        for(int a=0; a<totalCols;a++){
            containerLoc=getContainerLoc(a,n);
            if(containerLoc!=-1){
                Node newNode;
                newNode.containerToDrop.description=n.shipState[containerLoc].description;
                newNode.containerToDrop.location=n.shipState[containerLoc].location;
                newNode.containerToDrop.weight=n.shipState[containerLoc].weight;
                newNode.containerToDrop.arrayLoc=containerLoc;
                newNode.parent=&n;
                for(int j=0; j<slotsNum;j++){
                    newNode.shipState[j]=n.shipState[j];
                }
                newNodes.push_back(newNode);
            }
        }
    }else{
        //get the columns that aren't full, put the container there and update the state and add it
        for(int b=0;b<totalCols;b++){
            openSpot=getEmptyLoc(b,n);
            if(openSpot!=-1 && openSpot!=n.containerToDrop.arrayLoc+12){ //spot is open and its a new spot
                Node newNode;
                for(int j=0; j<slotsNum;j++){
                    newNode.shipState[j]=n.shipState[j];
                }
                //for move object
                string currLoc=n.containerToDrop.location;
                int currLocY=stoi(currLoc.substr(0,2));
                int currLocX=stoi(currLoc.substr(3,2));
                if(n.containerToDrop.arrayLoc<108){ //is on the ship
                    currLoc="s " + currLoc;
                }else{ //only other option is buffer since no loading/unloading
                    currLoc="b " + currLoc;
                }
                Container* mContainer= new Container(currLoc,n.containerToDrop.description,stoi(n.containerToDrop.weight));
                string finalLoc=n.containerToDrop.location; //removes brackets
                int finalLocY=stoi(finalLoc.substr(0,2)); //row = y axis
                int finalLocX=stoi(finalLoc.substr(3,2)); //col = x axis
                if(openSpot<108){ //is on the ship
                    finalLoc="s " + currLoc;
                }else{ //only other option is buffer since no loading/unloading
                    finalLoc="b " + currLoc;
                }
                int cost=0;
                if(finalLoc.at(0)=='s' && currLoc.at(0)=='s'){ //inside ship move
                    cost=abs(finalLocX-currLocX)+abs(finalLocY-currLocY);
                }else if(finalLoc.at(0)=='s' && currLoc.at(0)=='b'){
                    cost=abs(9-finalLocY)+abs(1-finalLocX)+4+abs(5-currLocY)+abs(24-currLocX);
                }else if(finalLoc.at(0)=='b' && currLoc.at(0)=='s'){
                    cost=abs(5-finalLocY)+abs(24-finalLocX)+4+abs(9-currLocY)+abs(1-currLocX);
                }else if(finalLoc.at(0)=='b' && currLoc.at(0)=='b'){
                    cost=abs(finalLocX-currLocX)+abs(finalLocY-currLocY);
                }
                newNode.cost_g=cost;
                newNode.operation = new Move(mContainer, finalLoc,cost);

                newNode.shipState[openSpot].description=n.containerToDrop.description;
                newNode.shipState[openSpot].weight=n.containerToDrop.weight;
                newNode.shipState[n.containerToDrop.arrayLoc].description="UNUSED";
                newNode.shipState[n.containerToDrop.arrayLoc].weight="00000";
                newNode.containerToDrop.arrayLoc=-1;
                newNode.containerToDrop.weight="";
                newNode.containerToDrop.description="";
                newNode.containerToDrop.location="";
                newNode.parent=&n;

                newNodes.push_back(newNode);

            }
        }
    }
    return newNodes;
}

bool Search::isGoalState(Node n, string type, int orderedWeights[]){
    //if goal state, otherwise false
    if(n.containerToDrop.arrayLoc==-1){ //not holding a container
        if(n.shipState[96].description=="UNUSED" && //nothing in extra 9th row
           n.shipState[97].description=="UNUSED" &&
           n.shipState[98].description=="UNUSED" &&
           n.shipState[99].description=="UNUSED" &&
           n.shipState[100].description=="UNUSED" &&
           n.shipState[101].description=="UNUSED" &&
           n.shipState[102].description=="UNUSED" &&
           n.shipState[103].description=="UNUSED" &&
           n.shipState[104].description=="UNUSED" &&
           n.shipState[105].description=="UNUSED" &&
           n.shipState[106].description=="UNUSED" &&
           n.shipState[107].description=="UNUSED"){
            //check if balanced, if yes then return true
            if(type=="normal balance"){
                int portSideWeight=0;
                for(int i=0;i<9;i++){
                    portSideWeight+=stoi(n.shipState[i*12].weight);
                    portSideWeight+=stoi(n.shipState[(i*12)+1].weight);
                    portSideWeight+=stoi(n.shipState[(i*12)+2].weight);
                    portSideWeight+=stoi(n.shipState[(i*12)+3].weight);
                    portSideWeight+=stoi(n.shipState[(i*12)+4].weight);
                    portSideWeight+=stoi(n.shipState[(i*12)+5].weight);
                }
                int starSideWeight=0;
                for(int j=0;j<9;j++){
                    starSideWeight+=stoi(n.shipState[(j*12)+6].weight);
                    starSideWeight+=stoi(n.shipState[(j*12)+7].weight);
                    starSideWeight+=stoi(n.shipState[(j*12)+8].weight);
                    starSideWeight+=stoi(n.shipState[(j*12)+9].weight);
                    starSideWeight+=stoi(n.shipState[(j*12)+10].weight);
                    starSideWeight+=stoi(n.shipState[(j*12)+11].weight);
                }
                if(portSideWeight==0&&starSideWeight==0){ //ship is empty
                    return true;
                }else if(((double)min(portSideWeight,starSideWeight)/(double)max(portSideWeight,starSideWeight))>=0.9){
                    cout<<"\n normal goal\n";
                    cout<<"star: "<<starSideWeight;
                    cout<<"\n port: "<<portSideWeight;
                    return true;
                }
            }else if(type=="sift"){
                int weightPos=0;
                //bool isSiftGoal=true;
                for(int c=0; c<9; c++){
                    if(n.shipState[5+(c*12)].description!="NAN"){
                        if(n.shipState[5+(c*12)].description!="UNUSED" && stoi(n.shipState[5+(c*12)].weight)!=orderedWeights[weightPos]){
                            //isSiftGoal=false;
                            return false;
                        }
                        weightPos++;
                    }
                    if(n.shipState[6+(c*12)].description!="NAN"){
                        if(n.shipState[6+(c*12)].description!="UNUSED" && stoi(n.shipState[6+(c*12)].weight)!=orderedWeights[weightPos]){
                            //isSiftGoal=false;
                            return false;
                        }
                        weightPos++;
                    }
                    if(n.shipState[4+(c*12)].description!="NAN"){
                        if(n.shipState[4+(c*12)].description!="UNUSED" && stoi(n.shipState[4+(c*12)].weight)!=orderedWeights[weightPos]){
                            //isSiftGoal=false;
                            return false;
                        }
                        weightPos++;
                    }
                    if(n.shipState[4+(c*12)].description!="NAN"){
                        if(n.shipState[4+(c*12)].description!="UNUSED" && stoi(n.shipState[4+(c*12)].weight)!=orderedWeights[weightPos]){
                            //isSiftGoal=false;
                            return false;
                        }
                        weightPos++;
                    }
                    if(n.shipState[7+(c*12)].description!="NAN"){
                        if(n.shipState[7+(c*12)].description!="UNUSED" && stoi(n.shipState[7+(c*12)].weight)!=orderedWeights[weightPos]){
                            //isSiftGoal=false;
                            return false;
                        }
                        weightPos++;
                    }
                    if(n.shipState[3+(c*12)].description!="NAN"){
                        if(n.shipState[3+(c*12)].description!="UNUSED" && stoi(n.shipState[3+(c*12)].weight)!=orderedWeights[weightPos]){
                            //isSiftGoal=false;
                            return false;
                        }
                        weightPos++;
                    }
                    if(n.shipState[8+(c*12)].description!="NAN"){
                        if(n.shipState[8+(c*12)].description!="UNUSED" && stoi(n.shipState[8+(c*12)].weight)!=orderedWeights[weightPos]){
                            //isSiftGoal=false;
                            return false;
                        }
                        weightPos++;
                    }
                }
                return true;
            }
        }
    }
    return false;
}

vector<Move*> Search::trace(Node n){
    //trace and convert to move objects
    vector<Move*> moves;
    Node* currNode = &n;
    while(currNode->parent!=NULL){
        moves.push_back(currNode->operation);
        currNode=currNode->parent;
    }
    reverse(moves.begin(), moves.end());
    return moves;
}

int Search::getHeuristic(Node n, int slotsNum){
    //see slides?
    //sort all mases on the right side
    //calculate mass of both sides
    //Balance mass = left + right
    //compute the deficit of the smaller side
    //slide down the list, to the first value <= deficit
    //deficit-prevFoundValue = new Def
    // contue previous 2 steps until new def satisfied?
    //get count of how many containers had to be moved to get that value
    //get closet available col for each container
    //add everything together
    int rightSideSum=0;
    for(int i=0;i<8;i++){ //sum of "correctly placed" right side
        rightSideSum+=stoi(n.shipState[0].weight);

    }

    return 0;
}

Node Search::getGoalNode(Node root, int totalCols, int slotsNum, string type, int orderedWeights[]){
    vector<Node> queue;
    queue.push_back(root);
    int minPos;
    Node currNode;
    vector<Node> expandedNodeOptions;
    while(!queue.empty()){
        minPos = 0;
        for(int k=0;k<queue.size();k++){
            //queue[k].heuristic_h = getHeuristic(queue[k]);
            queue[k].heuristic_h = 0;
            queue[k].totalCost_f = queue[k].heuristic_h+ queue[k].cost_g;
            if(queue[k].totalCost_f < queue[minPos].totalCost_f){
                minPos=k;
            }
        }
        currNode=queue[minPos];
        cout<<"\npopping a node\n";
        printState(currNode,slotsNum);
        queue.erase(queue.begin()+minPos);

        if(isGoalState(currNode,type,orderedWeights)){
            cout<<"\ngoal node found!\n";
            return currNode;
        }else{
            expandedNodeOptions=expand(currNode, totalCols, slotsNum);
            cout<<"\nexapnded nodes:\n";
            for(int i=0; i<expandedNodeOptions.size();i++){
                printState(expandedNodeOptions[i],slotsNum);
                cout<<"\n";
                queue.push_back(expandedNodeOptions[i]);
            }
         }
    }

    currNode.notValid=true; //no goal state found
    cout<<"\nno goal state found\n";
    return currNode;
}

vector<Move*> Search::getMovesList(){
    vector<Move*> moves;
    int totalCols = 12;
    int slotsNum =108;
    Slot slotsArray[slotsNum];
    int i=0;
    string line;
    ifstream manifestFile(manifestPath);
    while(getline(manifestFile, line)){
        slotsArray[i].location=line.substr(1,5);
        slotsArray[i].description=line.substr(18);
        slotsArray[i].weight=line.substr(10,5);
        i++;
    }
    //cout<<"this is i: "<<i;
    for(int l=0; l<12;l++){
        slotsArray[l+i].description="UNUSED";
        slotsArray[l+i].weight="00000";
    }
    i=i+12;
    slotsArray[96].location="09,01";
    slotsArray[97].location="09,02";
    slotsArray[98].location="09,03";
    slotsArray[99].location="09,04";
    slotsArray[100].location="09,05";
    slotsArray[101].location="09,06";
    slotsArray[102].location="09,07";
    slotsArray[103].location="09,08";
    slotsArray[104].location="09,09";
    slotsArray[105].location="09,10";
    slotsArray[106].location="09,11";
    slotsArray[107].location="09,12";


    Node root;
    for(int j=0; j<slotsNum;j++){
        root.shipState[j]=slotsArray[j];
    }
    root.containerToDrop.arrayLoc=-1;
    root.containerToDrop.description="";
    root.containerToDrop.location="";
    root.containerToDrop.weight="";
    root.parent=NULL;

    int temp[0];
    cout<<"test";
    Node goal = getGoalNode(root, totalCols, slotsNum, "normal balance", temp);
    /*if(goal.notValid){
        cout<<"\nDoing SIFT...\n";
        int orderedWeights[96];
        for (int c = 0; c < 96; c++) {
            orderedWeights[c]=stoi(slotsArray[c].weight);
        }

        for (int i = 0; i < 96; i++)
            for (int j = 0; j < 96 - i; j++)
                if (orderedWeights[j] > orderedWeights[j + 1])
                    swap(orderedWeights[j], orderedWeights[j + 1]);

        goal = getGoalNode(root, totalCols, slotsNum, "sift",orderedWeights);
    }*/
    //call trace function
    return trace(goal);

    /*Container* container1 = new Container("s 01,02","Cat",99);
    Container* container2 = new Container("s 01,03","Dog",100);
    Container* container3 = new Container("s 01,07","Rats",-1);
    Move *move1 = new Move(container1, "b 01,01", 7);
    moves.push_back(move1);
    Move *move2 = new Move(container2, "t", 8);
    moves.push_back(move2);
    Move *move3 = new Move(container3, "s 01,05", 5);
    moves.push_back(move3);


    return moves;*/
}
