#ifndef OPERATION_H
#define OPERATION_H
#include "Move.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QRegularExpression>
#include <QString>

class Operation {

private:
    string username;
    string note;
    char load_or_balance; // set to l means load operation, set to b means balance
    string manifest_path; //file location of the manifest
    vector<Container*> containers; //vector of container pointers
    vector<Container*> NAN_containers;
    vector<Move*> moves; //vector of move pointers to be completed in order index 0 = first move
    QString current_container_loc;
    QString current_goal_loc;
    vector<Container *> containersToLoad;
    vector<Container *> containersToUnload;
public:
    Operation();

    void set_username(string);

    void set_note(string);

    void set_load_or_balance(char);

    void set_manifest_path(string);

    string get_manifest_path();

    vector<Container*> get_NAN_containers();

    vector<Container*> get_containers();

    void set_current_container(QString);

    QString get_current_container();

    void set_goal_loc(QString);

    QString get_goal_loc();

    vector<Move*> get_moves();

    void set_unload(vector<Container*>);

    void set_load(vector<Container*>);

    void move_complete(unsigned long long);

    void reset();

};
#endif // OPERATION_H
