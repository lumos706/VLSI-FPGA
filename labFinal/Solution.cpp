#include "Solution.h"
#include "FPGA.h"
#include "Design.h"
#include "Net.h"
#include "RRNode.h"
#include <iostream>
#include <queue>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <set>
#include <map>
#include <omp.h>

bool rrnode_less(RRNode* a, RRNode* b) {
    if (a->getX() != b->getX()) return a->getX() < b->getX();
    if (a->getY() != b->getY()) return a->getY() < b->getY();
    return a->getIdx() < b->getIdx();
}

bool routeAlgorithm(Net* net, std::set<RRNode*>& connected, RRNode* sink, std::unordered_map<RRNode*, int>& congestionMap) {
    using NodePair = std::pair<int, RRNode*>;
    std::priority_queue<NodePair, std::vector<NodePair>, std::greater<NodePair>> pq;
    std::unordered_map<RRNode*, int> costMap;
    std::unordered_map<RRNode*, RRNode*> parentMap;

    for (RRNode* start : connected) {
        pq.push({0, start});
        costMap[start] = 0;
    }

    RRNode* found = nullptr;

    while (!pq.empty()) {
        auto [curCost, current] = pq.top();
        pq.pop();

        if (current == sink) {
            found = current;
            break;
        }

        std::vector<RRNode*> neighbors(current->getConnections().begin(), current->getConnections().end());
        std::sort(neighbors.begin(), neighbors.end(), rrnode_less);

        for (RRNode* neighbor : neighbors) {
            if (neighbor->getNet() != nullptr && neighbor->getNet() != net)
                continue;

            int newCost = costMap[current] + 1 + congestionMap[neighbor];
            if (!costMap.count(neighbor) || newCost < costMap[neighbor]) {
                costMap[neighbor] = newCost;
                parentMap[neighbor] = current;
                pq.push({newCost, neighbor});
            }
        }
    }

    if (!found) {
        return false;
    }

    // 回溯路径，并加锁修改共享结构
    std::vector<RRNode*> path;
    for (RRNode* node = sink; connected.count(node) == 0; node = parentMap[node]) {
        path.push_back(node);
    }

    bool valid = true;
    #pragma omp critical
    {
        for (RRNode* node : path) {
            if (node->getNet() != nullptr && node->getNet() != net) {
                valid = false;
                break;
            }
        }

        if (valid) {
            for (RRNode* node : path) {
                if (node->getNet() == nullptr) {
                    node->setNet(*net);
                }
                net->addRRToPath(*node);
                congestionMap[node]++;
                connected.insert(node);
            }
        }
    }

    return valid;
}

void MyRouter::routeDesign(FPGA &fpga, Design &design) {
    const std::vector<Net*> &allNets = design.getNets();
    std::vector<Net*> nets(allNets.begin(), allNets.end());

    std::sort(nets.begin(), nets.end(), [](Net* a, Net* b) {
        int ax = a->getSource().getX(), ay = a->getSource().getY();
        int bx = b->getSource().getX(), by = b->getSource().getY();
        int aBox = 0, bBox = 0;
        for (RRNode* s : a->getSinks()) {
            aBox += abs(ax - s->getX()) + abs(ay - s->getY());
        }
        for (RRNode* s : b->getSinks()) {
            bBox += abs(bx - s->getX()) + abs(by - s->getY());
        }
        return aBox > bBox;
    });

    std::unordered_map<RRNode*, int> congestionMap;

    #pragma omp parallel for schedule(dynamic)
    for (long long i = 0; i < static_cast<long long>(nets.size()); ++i) {
        Net* net = nets[i];
        RRNode &source = net->getSource();
        std::set<RRNode*> sinks = net->getSinks();
        std::set<RRNode*> connected;

        #pragma omp critical
        {
            if (source.getNet() == nullptr) {
                source.setNet(*net);
            }
            congestionMap[&source]++;
        }

        net->addRRToPath(source);
        connected.insert(&source);

        for (RRNode* sink : sinks) {
            if (sink->getNet() == net) continue;

            if (!routeAlgorithm(net, connected, sink, congestionMap)) {
                #pragma omp critical
                std::cerr << "Failed to route net " << net->getIdx() << " to one of its sinks." << std::endl;
                break; // 若任意一个 sink 失败，当前 net 就退出
            }
        }

        net->finalizeRouting();
    }
}

 

// bool routeAlgorithm(Net *net, RRNode &startNode, RRNode &endNode) {
//     using NodePtr = RRNode*;

//     constexpr int MAX_RETRIES = 50;
//     for (int retry = 0; retry < MAX_RETRIES; ++retry) {
//         std::queue<NodePtr> nodeQueue;
//         std::unordered_map<NodePtr, NodePtr> parentTracker;
//         nodeQueue.push(&startNode);
//         parentTracker[&startNode] = nullptr;

//         while (!nodeQueue.empty()) {
//             NodePtr currentNode = nodeQueue.front();
//             nodeQueue.pop();

//             if (currentNode == &endNode) break;

//             for (NodePtr adjacentNode : currentNode->getConnections()) {
//                 if ((adjacentNode->getNet() == nullptr || adjacentNode->getNet() == net) && !parentTracker.count(adjacentNode)) {
//                     nodeQueue.push(adjacentNode);
//                     parentTracker[adjacentNode] = currentNode;
//                 }
//             }
//         }

//         if (!parentTracker.count(&endNode)) return false;

//         std::vector<NodePtr> pathNodes;
//         for (NodePtr node = &endNode; node; node = parentTracker[node]) pathNodes.push_back(node);

//         bool pathValid = false;
//         #pragma omp critical
//         {
//             bool hasConflict = false;
//             for (NodePtr node : pathNodes) {
//                 if (node->getNet() != nullptr && node->getNet() != net) {
//                     hasConflict = true;
//                     break;
//                 }
//             }

//             if (!hasConflict) {
//                 for (NodePtr node : pathNodes) {
//                     if (node->getNet() == nullptr) node->setNet(*net);
//                     net->addRRToPath(*node);
//                 }
//                 pathValid = true;
//             }
//         }

//         if (pathValid) return true;
//     }

//     return false;
// }

// void MyRouter::routeDesign(FPGA &fpga, Design &design) {
//     const std::vector<Net*> &netList = design.getNets();

//     #pragma omp parallel for schedule(dynamic)
//     for (long long idx = 0; idx < static_cast<long long>(netList.size()); ++idx) {
//         Net *currentNet = netList[idx];
//         RRNode &sourceNode = currentNet->getSource();

//         #pragma omp critical
//         {
//             if (sourceNode.getNet() == nullptr) {
//                 sourceNode.setNet(*currentNet);
//             }
//         }

//         for (RRNode *targetNode : currentNet->getSinks()) {
//             if (targetNode->getNet() == currentNet) {
//                 continue;
//             }
//             if (!routeAlgorithm(currentNet, sourceNode, *targetNode)) {
//                 #pragma omp critical
//                 std::cerr << "Failed to route net " << currentNet->getIdx() << std::endl;
//             }
//         }
//     }
// }