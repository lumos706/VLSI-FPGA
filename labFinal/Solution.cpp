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

// 优化算法
/**
 * rrnode_less
 * 功能：比较两个 RRNode 节点的优先级，用于排序。
 * 参数：
 * - a: 第一个 RRNode 节点。
 * - b: 第二个 RRNode 节点。
 * 返回值：
 * - 如果 a 的优先级高于 b，则返回 true；否则返回 false。
 * 逻辑：
 * 1. 优先比较 X 坐标，X 坐标小的优先。
 * 2. 如果 X 坐标相同，则比较 Y 坐标，Y 坐标小的优先。
 * 3. 如果 X 和 Y 坐标都相同，则比较节点索引，索引小的优先。
 */
bool rrnode_less(RRNode* a, RRNode* b) {
    if (a->getX() != b->getX()) return a->getX() < b->getX();
    if (a->getY() != b->getY()) return a->getY() < b->getY();
    return a->getIdx() < b->getIdx();
}

/**
 * routeAlgorithm
 * 功能：实现单个网络的布线算法，使用优先队列和回溯路径找到从已连接节点到目标节点的最优路径。
 * 参数：
 * - net: 当前需要布线的网络对象。
 * - connected: 已连接的 RRNode 节点集合。
 * - sink: 当前网络的目标节点。
 * - congestionMap: 节点拥塞映射，用于记录每个节点的拥塞程度。
 * 返回值：
 * - 布线成功返回 true，失败返回 false。
 * 逻辑：
 * 1. 初始化优先队列和相关映射（成本映射、父节点映射）。
 * 2. 从已连接节点开始，逐步扩展到目标节点。
 * 3. 如果找到目标节点，回溯路径并更新网络和拥塞信息。
 * 4. 如果路径无效（存在冲突），返回布线失败。
 */
bool routeAlgorithm(Net* net, std::set<RRNode*>& connected, RRNode* sink, std::unordered_map<RRNode*, int>& congestionMap) {
    using NodePair = std::pair<int, RRNode*>; // 定义优先队列中的节点对
    std::priority_queue<NodePair, std::vector<NodePair>, std::greater<NodePair>> pq; // 优先队列，按成本升序排列
    std::unordered_map<RRNode*, int> costMap; // 节点成本映射
    std::unordered_map<RRNode*, RRNode*> parentMap; // 节点父节点映射

    // 初始化优先队列和成本映射
    for (RRNode* start : connected) {
        pq.push({0, start});
        costMap[start] = 0;
    }

    RRNode* found = nullptr; // 用于记录找到的目标节点

    // 使用优先队列进行路径搜索
    while (!pq.empty()) {
        auto [curCost, current] = pq.top(); // 获取当前节点及其成本
        pq.pop();

        if (current == sink) { // 如果找到目标节点，退出循环
            found = current;
            break;
        }

        // 获取当前节点的所有邻居节点，并按优先级排序
        std::vector<RRNode*> neighbors(current->getConnections().begin(), current->getConnections().end());
        std::sort(neighbors.begin(), neighbors.end(), rrnode_less);

        for (RRNode* neighbor : neighbors) {
            // 检查邻居节点是否可用（未被其他网络占用或属于当前网络）
            if (neighbor->getNet() != nullptr && neighbor->getNet() != net)
                continue;

            int newCost = costMap[current] + 1 + congestionMap[neighbor]; // 计算新成本
            if (!costMap.count(neighbor) || newCost < costMap[neighbor]) {
                costMap[neighbor] = newCost; // 更新成本映射
                parentMap[neighbor] = current; // 更新父节点映射
                pq.push({newCost, neighbor}); // 将邻居节点加入优先队列
            }
        }
    }

    // 如果未找到目标节点，返回布线失败
    if (!found) {
        return false;
    }

    // 回溯路径，并加锁修改共享结构
    std::vector<RRNode*> path; // 用于存储回溯路径
    for (RRNode* node = sink; connected.count(node) == 0; node = parentMap[node]) {
        path.push_back(node);
    }

    bool valid = true; // 用于记录路径是否有效
    #pragma omp critical
    {
        // 检查路径是否存在冲突
        for (RRNode* node : path) {
            if (node->getNet() != nullptr && node->getNet() != net) {
                valid = false;
                break;
            }
        }

        // 如果路径有效，更新网络和拥塞信息
        if (valid) {
            for (RRNode* node : path) {
                if (node->getNet() == nullptr) {
                    node->setNet(*net); // 设置网络到当前节点
                }
                net->addRRToPath(*node); // 将节点加入网络路径
                congestionMap[node]++; // 更新拥塞信息
                connected.insert(node); // 将节点加入已连接集合
            }
        }
    }

    return valid; // 返回路径是否有效
}

/**
 * MyRouter::routeDesign
 * 功能：实现全局布线算法，为设计中的所有网络进行布线。
 * 参数：
 * - fpga: 当前 FPGA 对象，包含 RRNode 图。
 * - design: 当前设计对象，包含所有网络信息。
 * 逻辑：
 * 1. 获取设计中的所有网络，并按曼哈顿距离总和降序排序。
 * 2. 使用 OpenMP 并行处理每个网络的布线。
 * 3. 对每个网络，先设置源节点的网络信息。
 * 4. 对网络的每个目标节点，调用 routeAlgorithm 进行布线。
 * 5. 如果某个网络布线失败，输出错误信息并退出。
 */
void MyRouter::routeDesign(FPGA &fpga, Design &design) {
    const std::vector<Net*> &allNets = design.getNets(); // 获取设计中的所有网络
    std::vector<Net*> nets(allNets.begin(), allNets.end()); // 将网络复制到可排序的容器中

    // 按曼哈顿距离总和降序排序网络
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
        return aBox > bBox; // 曼哈顿距离大的优先
    });

    std::unordered_map<RRNode*, int> congestionMap; // 节点拥塞映射

    // 使用 OpenMP 并行处理每个网络
    #pragma omp parallel for schedule(dynamic)
    for (long long i = 0; i < static_cast<long long>(nets.size()); ++i) {
        Net* net = nets[i];
        RRNode &source = net->getSource(); // 获取网络的源节点
        std::set<RRNode*> sinks = net->getSinks(); // 获取网络的目标节点集合
        std::set<RRNode*> connected; // 已连接的节点集合

        #pragma omp critical
        {
            if (source.getNet() == nullptr) {
                source.setNet(*net); // 设置源节点的网络信息
            }
            congestionMap[&source]++; // 更新源节点的拥塞信息
        }

        net->addRRToPath(source); // 将源节点加入网络路径
        connected.insert(&source); // 将源节点加入已连接集合

        for (RRNode* sink : sinks) {
            if (sink->getNet() == net) continue; // 如果目标节点已连接到当前网络，跳过

            // 调用 routeAlgorithm 进行布线
            if (!routeAlgorithm(net, connected, sink, congestionMap)) {
                #pragma omp critical
                std::cerr << "Failed to route net " << net->getIdx() << " to one of its sinks." << std::endl;
                break; // 若任意一个目标节点布线失败，当前网络退出
            }
        }

        net->finalizeRouting(); // 完成网络布线
    }
}
 
// 基线算法(lab3算法，不再注释)
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