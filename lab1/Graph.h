#ifndef DESIGN_H
#define DESIGN_H

#include "Net.h"
#include "Node.h"
#include <map>

class Graph{
    public:
        Graph(){}
        virtual ~Graph(){}
        vector<Node *> &get_nodes() { return this->nodes; }
        vector<Net *> &get_nets() { return this->nets; }
        int get_node_num() { return this->nodes.size(); }
        int get_net_num() { return this->nets.size(); }
        Node *get_node(int index) { return this->node_map[index]; }
        Net *get_net(int index) { return this->net_map[index]; }
        Node *get_or_create_node(int index);
        Net *add_net(int index);

    private:
        vector<Node *> nodes;
        vector<Net *> nets;
        map<int, Node*> node_map;
        map<int, Net*> net_map;
};

#endif