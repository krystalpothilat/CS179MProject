#include "Search2.h"

Search2::Search2() {}


void Search2::printState(Node* n, int slotsNum){
    if(n->containerToDrop.arrayLoc!=-1){
        for(int j=0; j</*slotsNum*/24;j++){
            cout<<"\n"<<n->shipState[j].location<<" "<<n->shipState[j].description<<" "<<n->shipState[j].weight;
        }
    }else{
        cout<<"\nlocation of container to drop:"<<n->containerToDrop.location;
    }
}

int Search2::getEmptyLoc(int col, Node* n){
    if(col<12){
        if(n->shipState[col+0].description=="UNUSED"){
            return col+0;
        }else if(n->shipState[col+12].description=="UNUSED"){
            return col+12;
        }else if(n->shipState[col+24].description=="UNUSED"){
            return col+24;
        }else if(n->shipState[col+36].description=="UNUSED"){
            return col+36;
        }else if(n->shipState[col+48].description=="UNUSED"){
            return col+48;
        }else if(n->shipState[col+60].description=="UNUSED"){
            return col+60;
        }else if(n->shipState[col+72].description=="UNUSED"){
            return col+72;
        }else if(n->shipState[col+84].description=="UNUSED"){
            return col+84;
        }else if(n->shipState[col+96].description=="UNUSED"){
            return col+96;
        }else{
            return -1; //column is all full
        }
    }else{//buffer
        if(n->shipState[(col-12)+0].description=="UNUSED"){
            return (col-12+108)+0;
        }else if(n->shipState[(col-12+108)+24].description=="UNUSED"){
            return (col-12+108)+24;
        }else if(n->shipState[(col-12+108)+48].description=="UNUSED"){
            return (col-12+108)+48;
        }else if(n->shipState[(col-12+108)+72].description=="UNUSED"){
            return (col-12+108)+72;
        }else{
            return -1;
        }
        return -1;
    }
}

int Search2::getContainerLoc(int col, Node* n){
    if(col<12){
        if(n->shipState[col+96].description!="UNUSED" && n->shipState[col+96].description!="NAN"){
            return col+96;
        }else if(n->shipState[col+84].description!="UNUSED" && n->shipState[col+84].description!="NAN"){
            return col+84;
        }else if(n->shipState[col+72].description!="UNUSED" && n->shipState[col+72].description!="NAN"){
            return col+72;
        }else if(n->shipState[col+60].description!="UNUSED" && n->shipState[col+60].description!="NAN"){
            return col+60;
        }else if(n->shipState[col+48].description!="UNUSED" && n->shipState[col+48].description!="NAN"){
            return col+48;
        }else if(n->shipState[col+36].description!="UNUSED" && n->shipState[col+36].description!="NAN"){
            return col+36;
        }else if(n->shipState[col+24].description!="UNUSED" && n->shipState[col+96].description!="NAN"){
            return col+24;
        }else if(n->shipState[col+12].description!="UNUSED" && n->shipState[col+12].description!="NAN"){
            return col+12;
        }else if(n->shipState[col+0].description!="UNUSED" && n->shipState[col+0].description!="NAN"){
            return col+0;
        }else{
            return -1; //no containers to pick up
        }
    }else{ //in buffer
        if(n->shipState[(col-12+108)+72].description!="UNUSED" && n->shipState[(col-12+108)+72].description!="NAN"){
            return (col-12+108)+72;
        }else if(n->shipState[(col-12+108)+48].description!="UNUSED" && n->shipState[(col-12+108)+48].description!="NAN"){
            return (col-12+108)+48;
        }else if(n->shipState[(col-12+108)+24].description!="UNUSED" && n->shipState[(col-12+108)+72].description!="NAN"){
            return (col-12+108)+24;
        }else if(n->shipState[(col-12+108)].description!="UNUSED" && n->shipState[(col-12+108)+72].description!="NAN"){
            return (col-12+108);
        }else{
            return -1;
        }
        //return -1;
    }
}

bool Search2::isSameNode(Node* newNode, Node* n, int slotsNum){
    if(newNode->containerToDrop.description==n->containerToDrop.description &&
        newNode->containerToDrop.arrayLoc==n->containerToDrop.arrayLoc &&
        newNode->containerToDrop.location==n->containerToDrop.location &&
        newNode->containerToDrop.weight==n->containerToDrop.weight &&
        newNode->containersToLoad==n->containersToLoad &&
        newNode->containersToUnload==n->containersToUnload){
        for(int l=0; l<slotsNum; l++){
            if(newNode->shipState[l].weight!=n->shipState[l].weight ||
                newNode->shipState[l].arrayLoc!=n->shipState[l].arrayLoc ||
                newNode->shipState[l].description!=n->shipState[l].description ||
                newNode->shipState[l].location!=n->shipState[l].location){
                return false;
            }
        }
    }else{
        return false;
    }
    return true;
}

vector<Node*> Search2::expand(Node* n, int totalCols, int slotsNum, vector<Node*> alreadyExpanded){
    vector<Node*> newNodes;
    int containerLoc;
    int openSpot;
    if(n->containerToDrop.arrayLoc==-1){
        //pick up a container
        for(int a=0; a<totalCols;a++){
            containerLoc=getContainerLoc(a,n);
            if(containerLoc!=-1){
                Node* newNode=new Node();
                newNode->containersToLoad=n->containersToLoad;
                newNode->containersToUnload=n->containersToUnload;
                newNode->containerToDrop.description=n->shipState[containerLoc].description;
                newNode->containerToDrop.location=n->shipState[containerLoc].location;
                newNode->containerToDrop.weight=n->shipState[containerLoc].weight;
                newNode->containerToDrop.arrayLoc=containerLoc;
                newNode->operation = NULL;
                for(int j=0; j<slotsNum;j++){
                    newNode->shipState[j]=n->shipState[j];
                }
                newNode->parent=n;
                newNodes.push_back(newNode);
            }
        }
        //pick up a container from the truck
        if(!n->containersToLoad.empty()){
            Node* newNode=new Node();
            newNode->containersToLoad=n->containersToLoad;
            newNode->containersToUnload=n->containersToUnload;
            newNode->containerToDrop.description=newNode->containersToLoad.back()->get_description();
            newNode->containerToDrop.location="t";
            newNode->containerToDrop.weight="-1";
            newNode->containerToDrop.arrayLoc=-2;
            newNode->operation = NULL;
            for(int j=0; j<slotsNum;j++){
                newNode->shipState[j]=n->shipState[j];
            }
            newNode->parent=n;
            newNodes.push_back(newNode);
        }
    }else{
        //get the columns that aren't full, put the container there and update the state and add it
        for(int b=0;b<totalCols;b++){
            openSpot=getEmptyLoc(b,n);
            if(openSpot!=-1 && ((openSpot!=-2 && openSpot!=n->containerToDrop.arrayLoc+12 &&b<12) || (openSpot!=-2 && b>=12 && openSpot!=n->containerToDrop.arrayLoc+24))){ //spot is open and its a new spot
                Node* newNode=new Node();
                newNode->containersToLoad=n->containersToLoad;
                newNode->containersToUnload=n->containersToUnload;
                for(int j=0; j<slotsNum;j++){
                    newNode->shipState[j]=n->shipState[j];
                }
                //for move object
                string currLoc=n->containerToDrop.location;
                int currLocY=0;
                int currLocX=0;
                if(currLoc!="t"){
                    currLocY=stoi(currLoc.substr(0,2));
                    currLocX=stoi(currLoc.substr(3,2));
                }else{
                    cout<<"\nFROM TRUCK";
                }
                if(n->containerToDrop.arrayLoc<108 && n->containerToDrop.arrayLoc>0){ //is on the ship
                    currLoc="s " + currLoc;
                }else if(n->containerToDrop.arrayLoc>=108 && n->containerToDrop.arrayLoc>0){ //only other option is buffer since no loading/unloading
                    currLoc="b " + currLoc;
                }
                Container* mContainer;
                if(currLoc!="t"){
                    mContainer= new Container(currLoc,n->containerToDrop.description,stoi(n->containerToDrop.weight));
                }else{
                    mContainer= new Container(currLoc,n->containerToDrop.description,-1);
                }
                string finalLoc=newNode->shipState[openSpot].location; //removes brackets
                int finalLocY=stoi(finalLoc.substr(0,2)); //row = y axis
                int finalLocX=stoi(finalLoc.substr(3,2)); //col = x axis
                if(openSpot<108){ //is on the ship
                    finalLoc="s " + finalLoc;
                }else{ //only other option is buffer since no loading/unloading
                    finalLoc="b " + finalLoc;
                }
                int cost=0;
                if(finalLoc.at(0)=='s' && currLoc.at(0)=='s'){ //inside ship move
                    cout<<"\nfinalLocX: "<<finalLocX<< "    currLocX: "<<currLocX;
                    //get highest occupied slot col between the 2 cols
                    int highestOpenSpotToCollide=-1;
                    for(int i=0; i<abs(finalLocX-currLocX)-1;i++){
                        int openColLocY=0;
                        int pos=getEmptyLoc(finalLocX+i, newNode);
                        if(pos!=-1){
                            string openColLoc = newNode->shipState[pos].location;
                            openColLocY=stoi(openColLoc.substr(0,2));
                        }else{
                            openColLocY=10;
                        }

                        if(openColLocY>highestOpenSpotToCollide){
                            highestOpenSpotToCollide=openColLocY;
                        }
                    }
                    cost=abs(finalLocX-currLocX);
                    if(highestOpenSpotToCollide>currLocY && highestOpenSpotToCollide>finalLocY){ //no direct path, other containers in the way in other cols
                        if(highestOpenSpotToCollide>finalLocY){
                            cost+=highestOpenSpotToCollide-finalLocY;
                        }
                        if(highestOpenSpotToCollide>currLocY){
                            cost+=highestOpenSpotToCollide-currLocY;
                        }
                    }else{
                        cost+=abs(finalLocY-currLocY);
                    }
                }else if(finalLoc.at(0)=='s' && currLoc.at(0)=='b'){
                    cost=abs(9-finalLocY)+abs(1-finalLocX)+4+abs(5-currLocY)+abs(24-currLocX);
                }else if(finalLoc.at(0)=='b' && currLoc.at(0)=='s'){
                    cost=abs(5-finalLocY)+abs(24-finalLocX)+4+abs(9-currLocY)+abs(1-currLocX);
                }else if(finalLoc.at(0)=='b' && currLoc.at(0)=='b'){
                    int highestOpenSpotToCollide=-1;
                    for(int i=0; i<abs(finalLocX-currLocX)-1;i++){
                        int openColLocY=0;
                        int pos=getEmptyLoc(finalLocX+i, newNode);
                        if(pos!=-1){
                            string openColLoc = newNode->shipState[pos].location;
                            openColLocY=stoi(openColLoc.substr(0,2));
                        }else{
                            openColLocY=10;
                        }
                        if(openColLocY>highestOpenSpotToCollide){
                            highestOpenSpotToCollide=openColLocY;
                        }
                    }
                    cost=abs(finalLocX-currLocX);
                    if(highestOpenSpotToCollide>currLocY && highestOpenSpotToCollide>finalLocY){ //no direct path, other containers in the way in other cols
                        if(highestOpenSpotToCollide>finalLocY){
                            cost+=highestOpenSpotToCollide-finalLocY;
                        }
                        if(highestOpenSpotToCollide>currLocY){
                            cost+=highestOpenSpotToCollide-currLocY;
                        }
                    }else{
                        cost+=abs(finalLocY-currLocY);
                    }
                }else if(currLoc.at(0)=='t'){
                    cost=abs(9-finalLocY)+abs(1-finalLocX)+2;
                }
                newNode->cost_g=cost;
                newNode->operation = new Move(mContainer, finalLoc,cost);
                newNode->shipState[openSpot].description=n->containerToDrop.description;
                newNode->shipState[openSpot].weight=n->containerToDrop.weight;
                if(currLoc!="t"){
                    newNode->shipState[n->containerToDrop.arrayLoc].description="UNUSED";
                    newNode->shipState[n->containerToDrop.arrayLoc].weight="00000";
                }else{
                    newNode->containersToLoad.pop_back();
                }
                newNode->containerToDrop.arrayLoc=-1;
                newNode->containerToDrop.weight="nothing picked up";
                newNode->containerToDrop.description="nothing picked up";
                newNode->containerToDrop.location="nothing picked up";
                newNode->parent=n;
                newNodes.push_back(newNode);
            }
        }
        //unload container
        if(!n->containersToUnload.empty()){
            for(int i=0;i<n->containersToUnload.size();i++){
                if(n->containerToDrop.description==n->containersToUnload[i]->get_description()){
                    Node* newNode=new Node();
                    newNode->containersToLoad=n->containersToLoad;
                    newNode->containersToUnload=n->containersToUnload;
                    for(int j=0; j<slotsNum;j++){
                        newNode->shipState[j]=n->shipState[j];
                    }
                    //for move object
                    string currLoc=n->containerToDrop.location;
                    if(currLoc!="t"){
                        int currLocY=stoi(currLoc.substr(0,2));
                        int currLocX=stoi(currLoc.substr(3,2));
                        if(n->containerToDrop.arrayLoc<108 && n->containerToDrop.arrayLoc>0){ //is on the ship
                            currLoc="s " + currLoc;
                        }else if(n->containerToDrop.arrayLoc>=108 && n->containerToDrop.arrayLoc>0){ //only other option is buffer since no loading/unloading
                            currLoc="b " + currLoc;
                        }
                        Container* mContainer;
                        mContainer= new Container(currLoc,n->containerToDrop.description,stoi(n->containerToDrop.weight));
                        string finalLoc="t";
                        int cost=0;
                        if(currLoc.at(0)=='s'){
                            cost=abs(9-currLocY)+abs(1-currLocX)+2;
                        }else if(currLoc.at(0)=='b'){
                            cost=abs(5-currLocY)+abs(24-currLocX)+2;
                        }
                        newNode->cost_g=cost;
                        newNode->operation = new Move(mContainer, finalLoc,cost);
                        newNode->shipState[openSpot].description=n->containerToDrop.description;
                        newNode->shipState[openSpot].weight=n->containerToDrop.weight;
                        newNode->containersToUnload.erase(newNode->containersToUnload.begin()+i);
                        newNode->containerToDrop.arrayLoc=-1;
                        newNode->containerToDrop.weight="nothing picked up";
                        newNode->containerToDrop.description="nothing picked up";
                        newNode->containerToDrop.location="nothing picked up";
                        newNode->parent=n;
                        newNodes.push_back(newNode);
                    }
                    break;
                }
            }
        }

    }
    return newNodes;
}

bool Search2::isGoalState(Node* n){
    //if goal state, otherwise false
    if(n->containersToLoad.empty() && n->containersToUnload.empty()){
        for(int i=96; i<132;i++){ //nothing in buffer or 9th row
            if(n->shipState[i].description!="UNUSED"){
                return false;
            }
        }
    }else{
        return false;
    }
    return true;
}

vector<Move*> Search2::trace(Node* n){
    //trace and convert to move objects
    vector<Move*> moves;
    Node* currNode = n;

    while(currNode->parent!=NULL){
        if(currNode->operation!=NULL){
            moves.push_back(currNode->operation);
        }
        currNode=currNode->parent;
    }
    reverse(moves.begin(), moves.end());

    //for testing
    for(int i=0;i<moves.size();i++){
        cout<<"\nMove "<<moves[i]->get_container()->get_description()<<" at "<<moves[i]->get_container()->get_location()<< " to "<<moves[i]->get_final_location();
    }


    return moves;
}

int Search2::getHeuristic(Node* n, int slotsNum){
    //for each containr to be unloaded
    //+2 + how many cols away from 01(portal to truck)
    //for each container to be loaded
    //+2 + how many cols away from first
    return 0;
}

Node* Search2::getGoalNode(Node* root, int totalCols, int slotsNum){
    vector<Node*> queue;
    queue.push_back(root);
    vector<Node*> alreadyExpanded;
    int minPos;
    Node* currNode;
    vector<Node*> expandedNodeOptions;
    while(!queue.empty()){
        minPos = 0;
        for(int k=0;k<queue.size();k++){
            queue[k]->heuristic_h = getHeuristic(queue[k], slotsNum);
            queue[k]->totalCost_f = queue[k]->heuristic_h+ queue[k]->cost_g;
            if(queue[k]->totalCost_f < queue[minPos]->totalCost_f){
                minPos=k;
            }
        }
        currNode=queue[minPos];
        alreadyExpanded.push_back(currNode);
        queue.erase(queue.begin()+minPos);

        if(isGoalState(currNode)){
            cout<<"\ngoal node found!\n";
            return currNode;
        }else{
            expandedNodeOptions=expand(currNode, totalCols, slotsNum, alreadyExpanded);
            cout<<"\nexapnded nodes:\n";
            for(int i=0; i<expandedNodeOptions.size();i++){
                bool alreadyExists=false;
                for(int j=0; j<alreadyExpanded.size(); j++){
                    if(isSameNode(expandedNodeOptions[i], alreadyExpanded[j],slotsNum)){ //prune previous nodes
                        if(expandedNodeOptions[i]->totalCost_f>= alreadyExpanded[j]->totalCost_f){
                            alreadyExists=true;
                            break;
                        }
                    }
                }
                if(!alreadyExists){
                    queue.push_back(expandedNodeOptions[i]);
                }
            }
        }
    }

    currNode->notValid=true; //no goal state found
    cout<<"\nno goal state found\n";
    return currNode;
}

vector<Move*> Search2::getMovesList(vector<Container *> containersToLoad, vector<Container *> containersToUnload){
    vector<Move*> moves;
    int totalCols = 36;
    int slotsNum =204;
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
    for(int l=0; l<108;l++){
        slotsArray[l+i].description="UNUSED";
        slotsArray[l+i].weight="00000";
    }
    //hard coded because easier for me to reference the values
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
    slotsArray[108].location="01,01";
    slotsArray[109].location="01,02";
    slotsArray[110].location="01,03";
    slotsArray[111].location="01,04";
    slotsArray[112].location="01,05";
    slotsArray[113].location="01,06";
    slotsArray[114].location="01,07";
    slotsArray[115].location="01,08";
    slotsArray[116].location="01,09";
    slotsArray[117].location="01,10";
    slotsArray[118].location="01,11";
    slotsArray[119].location="01,12";
    slotsArray[120].location="01,13";
    slotsArray[121].location="01,14";
    slotsArray[122].location="01,15";
    slotsArray[123].location="01,16";
    slotsArray[124].location="01,17";
    slotsArray[125].location="01,18";
    slotsArray[126].location="01,19";
    slotsArray[127].location="01,20";
    slotsArray[128].location="01,21";
    slotsArray[129].location="01,22";
    slotsArray[130].location="01,23";
    slotsArray[131].location="01,24";
    slotsArray[132].location="02,01";
    slotsArray[133].location="02,02";
    slotsArray[134].location="02,03";
    slotsArray[135].location="02,04";
    slotsArray[136].location="02,05";
    slotsArray[137].location="02,06";
    slotsArray[138].location="02,07";
    slotsArray[139].location="02,08";
    slotsArray[140].location="02,09";
    slotsArray[141].location="02,10";
    slotsArray[142].location="02,11";
    slotsArray[143].location="02,12";
    slotsArray[144].location="02,13";
    slotsArray[145].location="02,14";
    slotsArray[146].location="02,15";
    slotsArray[147].location="02,16";
    slotsArray[148].location="02,17";
    slotsArray[149].location="02,18";
    slotsArray[150].location="02,19";
    slotsArray[151].location="02,20";
    slotsArray[152].location="02,21";
    slotsArray[153].location="02,22";
    slotsArray[154].location="02,23";
    slotsArray[155].location="02,24";
    slotsArray[156].location="03,01";
    slotsArray[157].location="03,02";
    slotsArray[158].location="03,03";
    slotsArray[159].location="03,04";
    slotsArray[160].location="03,05";
    slotsArray[161].location="03,06";
    slotsArray[162].location="03,07";
    slotsArray[163].location="03,08";
    slotsArray[164].location="03,09";
    slotsArray[165].location="03,10";
    slotsArray[166].location="03,11";
    slotsArray[167].location="03,12";
    slotsArray[168].location="03,13";
    slotsArray[169].location="03,14";
    slotsArray[170].location="03,15";
    slotsArray[171].location="03,16";
    slotsArray[172].location="03,17";
    slotsArray[173].location="03,18";
    slotsArray[174].location="03,19";
    slotsArray[175].location="03,20";
    slotsArray[176].location="03,21";
    slotsArray[177].location="03,22";
    slotsArray[178].location="03,23";
    slotsArray[179].location="03,24";
    slotsArray[180].location="04,01";
    slotsArray[181].location="04,02";
    slotsArray[182].location="04,03";
    slotsArray[183].location="04,04";
    slotsArray[184].location="04,05";
    slotsArray[185].location="04,06";
    slotsArray[186].location="04,07";
    slotsArray[187].location="04,08";
    slotsArray[188].location="04,09";
    slotsArray[189].location="04,10";
    slotsArray[190].location="04,11";
    slotsArray[191].location="04,12";
    slotsArray[192].location="04,13";
    slotsArray[193].location="04,14";
    slotsArray[194].location="04,15";
    slotsArray[195].location="04,16";
    slotsArray[196].location="04,17";
    slotsArray[197].location="04,18";
    slotsArray[198].location="04,19";
    slotsArray[199].location="04,20";
    slotsArray[200].location="04,21";
    slotsArray[201].location="04,22";
    slotsArray[202].location="04,23";
    slotsArray[203].location="04,24";

    Node* root=new Node();
    for(int j=0; j<slotsNum;j++){
        root->shipState[j]=slotsArray[j];
    }
    root->containerToDrop.arrayLoc=-1;
    root->containerToDrop.description="nothing picked up";
    root->containerToDrop.location="nothing picked up";
    root->containerToDrop.weight="nothing picked up";
    root->parent=NULL;
    if(!containersToLoad.empty()){
        root->containersToLoad=containersToLoad;
    }
    if(!containersToUnload.empty()){
        root->containersToUnload=containersToUnload;
    }


    Node* goal = getGoalNode(root, totalCols, slotsNum);
    //cout<<"got goal\n";
    if(!goal->notValid){
        cout<<"got goal\n";
        moves=trace(goal);
        printState(goal,slotsNum);
    }else{
        cout<<"no goal node found\n";
    }


    return moves;
}
