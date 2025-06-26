#include <iostream>
#include <list>
#include <set>
#include "Design.h"
#include "Net.h"
#include "RRNode.h"

using namespace std;

Design::Design() {}

Design::~Design() {
    for(auto net : nets) {
        delete net;
    }
}

void Design::addNet(Net &net) {     // 给design添加net
    if(net.getSinks().size() == 0) {
        cout << "Adding net to design with no sinks" << endl;
        exit(-1);
    }

    set<RRNode *> nodes;

    for(auto existing_net : nets) {
        nodes.insert(&(existing_net->getSource()));
        for(auto dest : existing_net->getSinks()) {
            nodes.insert(dest);
        }
    }

    if(nodes.find(&(net.getSource())) != nodes.end()) {
        cout << "Adding net to design with conflicting source pin " << net.getSource() << endl;
        exit(-1);
    }

    for(auto sink : net.getSinks()) {
        if(nodes.find(sink) != nodes.end()) {
            cout << "Adding net to design with conflicting sink pin " << *sink << endl;
            exit(-1);
        }
    }

    nets.push_back(&net);
}

bool Design::verifyRouting() {
    // Check that each net can route successfully
    for(auto net : nets) {
        bool success = net->verifyRouting();
        if(!success) {
            return false;
        }
    }
    return true;
}