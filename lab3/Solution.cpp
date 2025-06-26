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

/**
 * routeAlgorithm
 * 功能：实现单个网络的布线算法，使用广度优先搜索 (BFS) 在 RRNode 图中找到从源节点到目标节点的路径。
 * 参数：
 * - net: 指向当前需要布线的网络对象。
 * - source: 当前网络的源节点。
 * - sink: 当前网络的目标节点。
 * 返回值：
 * - 布线成功返回 true，失败返回 false。
 * 逻辑：
 * 1. 初始化 BFS 队列和父节点映射，用于记录路径。
 * 2. 从源节点开始，逐层遍历连接的 RRNode。
 * 3. 如果找到目标节点，回溯路径并将网络设置到路径上的节点。
 * 4. 如果无法找到路径，返回布线失败。
 */
bool routeAlgorithm(Net *net, RRNode &source, RRNode &sink) {
    std::queue<RRNode *> queue;                       // BFS 队列，用于存储待访问的节点
    std::unordered_map<RRNode *, RRNode *> parentMap; // 父节点映射，用于记录路径
    queue.push(&source);                              // 将源节点加入队列
    parentMap[&source] = nullptr; // 起点没有父节点

    while (!queue.empty())
    {
        RRNode *current = queue.front(); // 获取队列中的当前节点
        queue.pop();                     // 从队列中移除当前节点

        // 检查是否到达目标节点
        if (current == &sink) {
            break; // 找到目标节点，退出循环
        }

        // 遍历当前节点的所有连接节点
        for (RRNode *neighbor : current->getConnections()) {
            // 检查邻居节点是否可用（未被其他网络占用或属于当前网络）
            if ((neighbor->getNet() == nullptr || neighbor->getNet() == net) && !parentMap.count(neighbor)) {
                queue.push(neighbor);          // 将邻居节点加入队列
                parentMap[neighbor] = current; // 记录邻居节点的父节点
            }
        }
    }

    // 如果目标节点未被访问，说明没有找到路径
    if (!parentMap.count(&sink)) {
        std::cerr << "No path found from source to sink." << std::endl;
        return false; // 返回布线失败
    }

    // 回溯路径并设置网络到路径上的节点
    for (RRNode *node = &sink; node != nullptr; node = parentMap[node]) {
        if (node->getNet() == nullptr) {
            node->setNet(*net); // 设置网络到当前节点
        }
    }
    return true; // 返回布线成功
}

/**
 * MyRouter::routeDesign
 * 功能：实现顺序全局布线算法，为设计中的所有网络进行布线。
 * 参数：
 * - fpga: 当前 FPGA 对象，包含 RRNode 图。
 * - design: 当前设计对象，包含所有网络信息。
 * 逻辑：
 * 1. 获取设计中的所有网络。
 * 2. 对每个网络，先设置源节点的网络信息。
 * 3. 对网络的每个目标节点，调用 routeAlgorithm 进行布线。
 * 4. 如果某个网络布线失败，输出错误信息并退出。
 */
void MyRouter::routeDesign(FPGA &fpga, Design &design) {
    const std::vector<Net *> &nets = design.getNets(); // 获取设计中的所有网络
    for (Net *net : nets) {
        RRNode &source = net->getSource(); // 获取当前网络的源节点
        if (source.getNet() == nullptr) {
            source.setNet(*net); // 设置源节点的网络信息
        }

        for (RRNode *sink : net->getSinks())
        { // 遍历当前网络的所有目标节点
            if (sink->getNet() == net) {
                continue; // 如果目标节点已经连接到当前网络，跳过
            }

            // 调用 routeAlgorithm 进行布线
            if (!routeAlgorithm(net, source, *sink)) {
                std::cerr << "Failed to route net " << net->getIdx() << std::endl;
                return; // 如果布线失败，输出错误信息并退出
            }
        }
    }
}