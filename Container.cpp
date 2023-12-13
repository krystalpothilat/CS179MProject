#include "Container.h"

Container::Container() {
    location = "";
    description = "";
    weight = -1;
}

Container::Container(string l) {
    location = l;
    description = "NAN";
    weight = -1;
}
Container::Container(string l, string d, int w) {
    location = l;
    description = d;
    weight = w;
}

string Container::get_location() {
    return location;
}

string Container::get_description() {
    return description;
}

int Container::get_weight() {
    return weight;
}

void Container::set_weight(int w) {
    weight = w;
}
