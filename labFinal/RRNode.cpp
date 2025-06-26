#include <algorithm>
#include <assert.h>
#include <ostream>

#include "RRNode.h"

using namespace std;

RRNode::RRNode(rrType type, int x, int y, int idx): type(type), x(x), y(y), idx(idx), net(nullptr) {

}

RRNode::~RRNode(){}

void RRNode::setNet(Net &net) {
    this->net = &net;
}

bool RRNode::isConnected(RRNode &node) {
    return find(connections.begin(), connections.end(), &node) != connections.end();
}

void RRNode::connect(RRNode &node) {
    assert(!isConnected(node));
    assert(!node.isConnected(*this));

    connections.push_back(&node);
    node.connections.push_back(this);
}