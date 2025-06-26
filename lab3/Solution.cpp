#include "Solution.h"
#include "FPGA.h"
#include "Design.h"
#include "Net.h"
#include "RRNode.h"
#include <iostream>
#include <queue>
#include <unordered_map>
#include <algorithm>
#include <vector>
#include <tuple>


bool routeAlgorithm(Net *net, RRNode &source, RRNode &sink) {
    std::queue<RRNode*> queue;
    std::unordered_map<RRNode*, RRNode*> parentMap; // 用于记录路径 
    queue.push(&source);
    parentMap[&source] = nullptr; // 起点没有父节点

    while(!queue.empty()) {
        RRNode *current = queue.front();
        queue.pop();

        // 检查是否到达目标
        if (current == &sink) {
            break; // 找到目标节点
        }

        // 遍历当前节点的连接
        for (RRNode *neighbor : current->getConnections()) {
            if ((neighbor->getNet() == nullptr || neighbor->getNet() == net) && !parentMap.count(neighbor)) {
                queue.push(neighbor);
                parentMap[neighbor] = current; // 记录父节点
            }
        }
    }
    if (!parentMap.count(&sink)) {
        std::cerr << "No path found from source to sink." << std::endl;
        return false; // 没有找到路径
    }
    // 回溯路径并连接RRNode
    for (RRNode *node = &sink; node != nullptr; node = parentMap[node]) {
        if (node->getNet() == nullptr) {
            node->setNet(*net); // 设置net
        }
    }
    return true; // 返回布线是否成功
}

// 顺序全局布线算法
void MyRouter::routeDesign(FPGA &fpga, Design &design) {
    const std::vector<Net*> &nets = design.getNets();
    for (Net *net : nets) {
        RRNode &source = net->getSource();
        if (source.getNet() == nullptr) {
            source.setNet(*net);
        }

        for (RRNode *sink : net->getSinks()) {
            if (sink->getNet() == net) {
                continue; // 已经连接到该net
            }
            // 需要自己实现布线算法 BFS
            if (!routeAlgorithm(net, source, *sink)) {
                std::cerr << "Failed to route net " << net->getIdx() << std::endl;
                return; // 布线失败，退出
            }

        }
    }
}