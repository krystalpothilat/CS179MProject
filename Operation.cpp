#include "Operation.h"
#include "QtCore/qdebug.h"
#include "Search.h"
#include "Search2.h"

Operation::Operation()
{
    username = "";
    note = "";
    load_or_balance = ' ';
    manifest_path = "";
}
void Operation::set_username(string name)
{
    username = name;
    cout << "Name Set: " << name << endl;
    //update log
}

void Operation::set_note(string n)
{
    note = n;
    cout << "Note Set: " << note << endl;
}

void Operation::set_load_or_balance(char a)
{
    load_or_balance = a;
    cout << "Load or Balance set: " << a << endl;
}

void Operation::set_manifest_path(string path)
{
    manifest_path = path;
    cout << "Manifest Path set: " << path << endl;
}

string Operation::get_manifest_path()
{
    return manifest_path;
}

QStringList Operation::get_manifestlines(){
    return manifestlines;
}

void Operation::set_manifest_line(int i, QString line){
    cout << "setting manifest line" << endl;
    manifestlines[i] = line;
    cout << "done setting manifest line" << endl;
}

vector<Container*> Operation::get_NAN_containers(){
    return NAN_containers;
}

vector<Container*> Operation::get_containers()
{
    QFile file(QString::fromStdString(manifest_path));
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Error oppening file: " << file.errorString();
        return {};
    }
    QTextStream in(&file);

    QRegularExpression regex("\\[([^,]+),([^\\]]+)\\], \\{([^\\}]+)\\}, (.+)");
    QRegularExpressionMatch match;
    while (!(in.atEnd())){
        QString line = in.readLine();
        manifestlines.append(line);
        match = regex.match(line);
        string location = "s " + match.captured(1).toStdString() + ","
                          + match.captured(2).toStdString();
        int weight = match.captured(3).toInt();
        string desc = match.captured(4).toStdString();
        if(desc == "NAN"){ //NAN container
            NAN_containers.push_back(new Container(location));
        } else if (desc != "UNUSED"){ //non NAN or UNUSED
            containers.push_back(new Container(location, desc, weight));
        }

    }
    file.close();
    return containers;
}

void Operation::set_current_container(QString s){
    current_container_loc = s;
}

QString Operation::get_current_container(){
    return current_container_loc;
}

void Operation::set_goal_loc(QString s){
    current_goal_loc = s;
};

QString Operation::get_goal_loc(){
    return current_goal_loc;
}

//Move(Container*, string, unsigned int);
vector<Move *> Operation::get_moves()
{
    //Calculate moves given all the data before returning moves

    if(load_or_balance=='b'){ //balance
        Search balanceOp;
        balanceOp.manifestPath=get_manifest_path();
        moves= balanceOp.getMovesList();
    }else{ // load/unload
        Search2 load;
        load.manifestPath=get_manifest_path();
        moves= load.getMovesList(containersToLoad,containersToUnload);
    }



    //DEMO PURPOSES ONLY!!! BEGIN
    //Container* container1 = new Container("s 01,02","Cat",99);
    //Container* container2 = new Container("s 01,03","Dog",100);
    //Container* container3 = new Container("s 01,07","Rats",-1);
    //Move *move1 = new Move(container1, "b 01,01", 7);
    //moves.push_back(move1);
    //Move *move2 = new Move(container2, "t", 8);
    //moves.push_back(move2);
    //Move *move3 = new Move(container3, "s 01,05", 5);
    //moves.push_back(move3);
    //DEMO PURPOSES ONLY!!! END

    return moves;
}

void Operation::set_unload(vector<Container *> u)
{
    //do something with containers to be unloaded
    if(!u.empty()){
        for (unsigned long long i = 0; i < u.size(); i++) {
            cout << "Container containing: " << u.at(i)->get_description() << " weighing "
                 << u.at(i)->get_weight() << " kilos located at: " << u.at(i)->get_location()
                 << " is to be unloaded." << endl;
            containersToUnload.push_back(u[i]);
        }
    }
}

void Operation::set_load(vector<Container *> l)
{
    //do something with container to be loaded
    if(!l.empty()){
        for (unsigned long long i = 0; i < l.size(); i++) {
            cout << "Container containing: " << l.at(i)->get_description() << " is to be loaded."
                 << endl;
            containersToLoad.push_back(l[i]);
        }
    }
}

void Operation::move_complete(unsigned long long a)
{
    cout << "Move " << a + 1 << " has been completed" << endl;
    //update log and manfiest that move at index a of the vector moves is complete
}


void Operation::reset()
{
    username = "";
    note = "";
    load_or_balance = ' ';
    manifest_path = "";
    for (Container *ptr : containers) {
        delete ptr;
    }
    containers.clear();
    for (Move *ptr : moves) {
        delete ptr;
    }
    moves.clear();
}
