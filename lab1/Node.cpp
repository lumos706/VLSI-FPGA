#include "Node.h"

Node::Node(int index):index(index) {}

Node::~Node(){}

void Node::add_net(Net *net) {
    this->nets.push_back(net);
}