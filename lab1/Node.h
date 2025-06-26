#ifndef NODE_H
#define NODE_H

#include <vector>
#include "Net.h"
using namespace std;

class Net;

class Node{
    public:
        Node(int index);
        virtual ~Node();
        int get_index() { return this->index; }
        void add_net(Net *net);
        vector<Net *> &get_nets() { return this->nets; } 
    private:
        int index;
        vector<Net *> nets;

};

#endif