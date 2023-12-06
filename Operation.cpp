#include "Operation.h"
#include "QtCore/qdebug.h"

Operation::Operation() {
    username = "";
    note = "";
    load_or_balance = ' ';
    manifest_path = "";

}
void Operation::set_username(string name) {
    username = name;
    cout<<"Name Set: "<<name<<endl;
}

void Operation::set_note(string n) {
    note = n;
    cout<<"Note Set: "<<note<<endl;
}

void Operation::set_load_or_balance(char a) {
    load_or_balance = a;
    cout<<"Load or Balance set: "<<a<<endl;
}

void Operation::set_manifest_path(string path) {
    manifest_path = path;
    cout<<"Path set: "<<path<<endl;
}

string Operation::get_manifest_path() {
    //make sure new manifest has been saved to new location before returning the path
    //DEMO PURPOSES ONLY: MANIFEST SAVED TO SAME LOCATION
    return manifest_path;
}

vector<Container*> Operation::get_containers() {
    QFile file(QString::fromStdString(manifest_path));
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        qDebug() << "Error oppening file: " << file.errorString();
        return {};
    }
    QTextStream in(&file);

    QRegularExpression regex("\\[([^,]+),([^\\]]+)\\], \\{([^\\}]+)\\}, (.+)");
    QRegularExpressionMatch match;
    while (!in.atEnd()){
        QString line = in.readLine();
        match = regex.match(line);
        string location = "s " + match.captured(1).toStdString() + "," + match.captured(2).toStdString();
        int weight = match.captured(3).toInt();
        string desc = match.captured(4).toStdString();
        if(desc != "NAN" && desc != "UNUSED"){
            containers.push_back(new Container(location, desc, weight));
        }
    }
    file.close();
    return containers;
}
//Move(Container*, string, unsigned int);
vector<Move*> Operation::get_moves() {
    //Calculate moves given all the data before returning moves

    //DEMO PURPOSES ONLY!!! BEGIN
    Move* move1 = new Move(containers.at(0),"b 01,01",7);
    moves.push_back(move1);
    Move* move2 = new Move(containers.at(1),"t",8);
    moves.push_back(move2);
    Move* move3 = new Move(containers.at(2),"s 01,02",5);
    moves.push_back(move3);
    //DEMO PURPOSES ONLY!!! END

    return moves;
}

void Operation::set_unload(vector<Container*> u) {
    //do something with containers to be unloaded
    for(unsigned long long i=0;i<u.size();i++){
        cout<<"Container containing: "<<u.at(i)->get_description()<<" weighing "<<u.at(i)->get_weight()<<" kilos located at: "<<u.at(i)->get_location()<< "is to be unloaded."<<endl;
    }
}

void Operation::set_load(vector<Container*> l) {
    //do something with container to be loaded
    for(unsigned long long i=0;i<l.size();i++){
        cout<<"Container containing: "<<l.at(i)->get_description()<<" is to be loaded."<<endl;
    }
}

void Operation::move_complete(unsigned long long a) {
    cout<<"Move "<<a+1<<" has been completed"<<endl;
    //update log and manfiest that move at index a of the vector moves is complete
}

void Operation::reset() {
    username = "";
    note = "";
    load_or_balance = ' ';
    manifest_path = "";
    for (Container* ptr : containers) {
        delete ptr;
    }
    containers.clear();
    for (Move* ptr : moves) {
        delete ptr;
    }
    moves.clear();
}
