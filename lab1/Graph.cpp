#include "Graph.h"
#include "Net.h"
#include "Node.h"

Node *Graph::get_or_create_node(int index) {
    for(auto node : nodes) {
        if(node->get_index() == index)  return node;
    }
    Node *node = new Node(index);
    nodes.push_back(node);
    node_map[index] = node;
    return node;
}

Net *Graph::add_net(int index) {
    Net *net = new Net(index);
    this->nets.push_back(net);
    net_map[index] = net;
    return net;
}