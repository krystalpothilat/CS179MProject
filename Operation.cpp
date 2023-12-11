#include "Operation.h"
#include "QtCore/qdebug.h"
#include "Search.h"

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
    //update log
}

void Operation::set_load_or_balance(char a)
{
    load_or_balance = a;
    cout << "Load or Balance set: " << a << endl;
}

void Operation::set_manifest_path(string path)
{
    manifest_path = path;
    cout << "Path set: " << path << endl;
    //update log stating manifest was opened
}

string Operation::get_manifest_path()
{
    //make sure new manifest has been saved to new location before returning the path
    //DEMO PURPOSES ONLY: MANIFEST SAVED TO SAME LOCATION
    return manifest_path;
}

vector<Container *> Operation::get_containers()
{
    //make sure all containers have been gotten before returning containers
    //DEMO PURPOSES ONLY!!! BEGIN
    QFile file(QString::fromStdString(manifest_path));
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Error oppening file: " << file.errorString();
        return {};
    }
    QTextStream in(&file);

    QRegularExpression regex("\\[([^,]+),([^\\]]+)\\], \\{([^\\}]+)\\}, (.+)");
    QRegularExpressionMatch match;
    while (!in.atEnd()) {
        QString line = in.readLine();
        match = regex.match(line);
        string location = "s " + match.captured(1).toStdString() + ","
                          + match.captured(2).toStdString();
        int weight = match.captured(3).toInt();
        string desc = match.captured(4).toStdString();
        if (desc != "NAN" && desc != "UNUSED") {
            containers.push_back(new Container(location, desc, weight));
        }
    }
    file.close();
    // Container* container1 = new Container("s 01,02","Cat",99);
    // containers.push_back(container1);
    // Container* container2 = new Container("s 01,03","Dog",100);
    // containers.push_back(container2);
    // Container* container3 = new Container("s 01,07","Rats",-1);
    // containers.push_back(container3);
    //DEMO PURPOSES ONLY!! END
    return containers;
}
//Move(Container*, string, unsigned int);
vector<Move *> Operation::get_moves()
{
    //Calculate moves given all the data before returning moves

    //Malina testing
    Search t;
    t.manifestPath=get_manifest_path();
    //cout<<"\nSeach path: "<< t.manifestPath;
    //t.getMovesList();
    string test="00900";
    int w=stoi(test);
    cout<<"converted: "<<w;


    //DEMO PURPOSES ONLY!!! BEGIN
    Container* container1 = new Container("s 01,02","Cat",99);
    Container* container2 = new Container("s 01,03","Dog",100);
    Container* container3 = new Container("s 01,07","Rats",-1);
    Move *move1 = new Move(container1, "b 01,01", 7);
    moves.push_back(move1);
    Move *move2 = new Move(container2, "t", 8);
    moves.push_back(move2);
    Move *move3 = new Move(container3, "s 01,05", 5);
    moves.push_back(move3);
    //DEMO PURPOSES ONLY!!! END

    return moves;
}

void Operation::set_unload(vector<Container *> u)
{
    //do something with containers to be unloaded
    for (unsigned long long i = 0; i < u.size(); i++) {
        cout << "Container containing: " << u.at(i)->get_description() << " weighing "
             << u.at(i)->get_weight() << " kilos located at: " << u.at(i)->get_location()
             << " is to be unloaded." << endl;
    }
}

void Operation::set_load(vector<Container *> l)
{
    //do something with container to be loaded
    for (unsigned long long i = 0; i < l.size(); i++) {
        cout << "Container containing: " << l.at(i)->get_description() << " is to be loaded."
             << endl;
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
