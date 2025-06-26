#include "Net.h"

Net::Net(int index):index(index) {}

Net::~Net() {}

void Net::add_node(Node *node) {
    this->nodes.push_back(node);
}