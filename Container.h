#ifndef CONTAINER_H
#define CONTAINER_H
using namespace std;
#include <string>
#include <iostream>
class Container {

private:
    string location; // suggested format: truck - "t", ship - "s 00,00", buffer - "b 00,00"
    string description;
    int weight; //set to negative 1 if no weight provided
public:
    Container();
    Container(string, string, int);
    string get_location();
    string get_description();
    int get_weight();
    void set_weight(int);
};
#endif // CONTAINER_H
