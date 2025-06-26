#include "solution.h"
#include "evaluate.h"
#include <cmath>
#include <queue>
#include <cstdlib>
#include <ctime>
#include <list>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>
#include <climits>
#include <time.h>
#include <vector>
#include <random>
#include <chrono>

// 初始化随机数引擎，用于模拟退火算法中的随机选择
std::default_random_engine gen((unsigned int)time(NULL));
std::uniform_real_distribution<double> rand01(0, 1); // 生成 [0, 1] 的随机数，用于概率计算

/**
 * FM_local_optimize
 * 功能：基于 Fiduccia-Mattheyses (FM) 算法进行局部优化，减少划分后的割代价。
 * 参数：
 * - graph: 图对象，包含节点和网络信息。
 * - X: 分区 X，存储节点 ID。
 * - Y: 分区 Y，存储节点 ID。
 */
void FM_local_optimize(Graph& graph, std::unordered_set<int>& X, std::unordered_set<int>& Y) {
    const auto &nodes = graph.get_nodes();                           // 获取所有节点
    size_t total_nodes = nodes.size();                               // 总节点数
    size_t min_size = static_cast<size_t>(ceil(total_nodes * 0.48)); // 分区最小大小限制

    // --- 增益计算函数 ---
    // 功能：计算节点的增益值，增益值 = 外部代价 - 内部代价
    auto calculate_gain = [&](int node_id,
                              const std::unordered_set<int> &from_partition,
                              const std::unordered_set<int> &to_partition)
    {
        int external_cost = 0, internal_cost = 0; // 外部代价和内部代价
        for (const auto &net : graph.get_node(node_id)->get_nets())
        {                                          // 遍历节点连接的网络
            bool has_from = false, has_to = false; // 标记网络是否连接到两个分区
            for (const auto &neighbor : net->get_nodes())
            { // 遍历网络中的所有节点
                int neighbor_id = neighbor->get_index();
                if (from_partition.count(neighbor_id))
                    has_from = true;
                if (to_partition.count(neighbor_id))
                    has_to = true;
            }
            if (has_from && has_to)
                external_cost++; // 网络连接到两个分区，增加外部代价
            else if (has_from)
                internal_cost++; // 网络仅连接到当前分区，增加内部代价
        }
        return external_cost - internal_cost; // 返回增益值
    };

    // --- 初始化增益表 ---
    std::unordered_map<int, int> gain; // 存储每个节点的增益值
    for (const auto& node : nodes) {
        int node_id = node->get_index();
        gain[node_id] = (X.count(node_id)) ? calculate_gain(node_id, X, Y) : calculate_gain(node_id, Y, X); // 根据节点所属分区计算增益值
    }

    // --- FM单轮优化循环 ---
    std::unordered_set<int> locked; // 存储已锁定的节点，避免重复移动
    bool improved;                  // 标记是否有优化发生
    do {
        improved = false;
        int best_node = -1;                             // 当前增益最大的节点
        int max_gain = std::numeric_limits<int>::min(); // 当前最大增益值
        bool move_to_X = false;                         // 标记节点移动方向

        // 查找最大增益节点
        for (const auto& [node_id, g] : gain) {
            if (locked.count(node_id))
                continue; // 跳过已锁定节点

            if (X.count(node_id) && X.size() > min_size)
            { // 节点在分区 X 且满足大小限制
                if (g > max_gain)
                {
                    max_gain = g;
                    best_node = node_id;
                    move_to_X = false; // X→Y
                }
            }
            else if (Y.count(node_id) && Y.size() > min_size)
            { // 节点在分区 Y 且满足大小限制
                if (g > max_gain) {
                    max_gain = g;
                    best_node = node_id;
                    move_to_X = true; // Y→X
                }
            }
        }

        if (best_node == -1 || max_gain <= 0)
            break; // 如果没有可移动节点或增益值不大于 0，则退出循环

        // 移动节点并锁定
        if (move_to_X) {
            Y.erase(best_node);
            X.insert(best_node);
        } else {
            X.erase(best_node);
            Y.insert(best_node);
        }
        locked.insert(best_node); // 锁定节点
        improved = true;

        // 更新邻居节点的增益
        for (const auto& net : graph.get_node(best_node)->get_nets()) {
            for (const auto& neighbor : net->get_nodes()) {
                int neighbor_id = neighbor->get_index();
                if (locked.count(neighbor_id))
                    continue; // 跳过已锁定节点

                gain[neighbor_id] = (X.count(neighbor_id)) ? calculate_gain(neighbor_id, X, Y) : calculate_gain(neighbor_id, Y, X); // 更新增益值
            }
        }

    } while (improved); // 如果有优化发生，则继续循环
}

/**
 * my_partition_algorithm
 * 功能：综合使用贪心策略、模拟退火和 FM 局部优化进行图划分。
 * 参数：
 * - graph: 图对象，包含节点和网络信息。
 * - X: 分区 X，存储节点 ID。
 * - Y: 分区 Y，存储节点 ID。
 */
void Solution::my_partition_algorithm(Graph& graph, unordered_set<int>& X, unordered_set<int>& Y) {
    auto start_time = std::chrono::high_resolution_clock::now();                      // 记录开始时间
    const auto &nodes = graph.get_nodes();                                            // 获取所有节点
    size_t total_nodes = nodes.size();                                                // 总节点数
    std::uniform_int_distribution<> rand_index(0, static_cast<int>(total_nodes) - 1); // 随机选择节点
    size_t min_size = static_cast<size_t>(ceil(total_nodes * 0.48));                  // 分区最小大小限制
    size_t max_size = static_cast<size_t>(floor(total_nodes * 0.52));                 // 分区最大大小限制
    unordered_set<int> best_X, best_Y;                                                // 存储最佳分区结果

    // --- 基于度数的贪心策略 ---
    vector<pair<int, int>> node_degrees; // 存储节点的度数（连接的网络数）
    for (const auto& node : nodes) {
        node_degrees.emplace_back(node->get_index(), node->get_nets().size());
    }
    sort(node_degrees.begin(), node_degrees.end(),
         [](const auto &a, const auto &b)
         { return a.second > b.second; }); // 按度数降序排列

    X.clear();
    Y.clear(); // 清空分区
    for (const auto& [node_id, _] : node_degrees) {
        if (X.size() < min_size || (X.size() < max_size && Y.size() >= max_size)) {
            X.insert(node_id); // 分配到分区 X
        } else {
            Y.insert(node_id); // 分配到分区 Y
        }
    }

    // --- 模拟退火优化 ---
    int best_cost = INT_MAX; // 初始化最佳开销
    for (int i = 0; i < 1; i++)
    {                                                  // 退火循环
        int current_cost = calculate_cut(graph, X, Y); // 计算初始开销
        best_cost = min(current_cost, best_cost);      // 更新最佳开销
        double T = 10 * current_cost; // 初始温度
        double cooling_rate = 0.98;   // 冷却率
        double min_T = 1e-6; // 最小温度
        int max_iterations = 500; // 最大迭代次数
        int iteration = 0;

        while (T > min_T && iteration < max_iterations) {
            iteration++;
            int idx = rand_index(gen); // 随机选择节点
            int node_id = nodes[idx]->get_index();
            bool inX = X.count(node_id) > 0;

            // 尝试移动节点
            if (inX && X.size() > min_size)
            {
                X.erase(node_id);
                Y.insert(node_id);
                if (Y.size() > max_size)
                { // 如果违反大小限制，则撤回
                    Y.erase(node_id);
                    X.insert(node_id);
                    T *= cooling_rate;
                    continue;
                }
            }
            else if (!inX && Y.size() > min_size)
            {
                Y.erase(node_id);
                X.insert(node_id);
                if (X.size() > max_size)
                { // 如果违反大小限制，则撤回
                    X.erase(node_id);
                    Y.insert(node_id);
                    T *= cooling_rate;
                    continue;
                }
            }
            else
            {
                T *= cooling_rate;
                continue;
            }

            int new_cost = calculate_cut(graph, X, Y); // 计算新开销
            int delta = new_cost - current_cost;

            // Metropolis准则决定是否接受
            if (delta >= 0) {
                double prob = exp(-delta / T), rand_val = rand01(gen);
                if (rand_val < prob) {
                    current_cost = new_cost; // 接受
                }
                else
                {
                    if (inX) {
                        Y.erase(node_id);
                        X.insert(node_id);
                    } else {
                        X.erase(node_id);
                        Y.insert(node_id);
                    }
                }
            } else {
                current_cost = new_cost;
                if (current_cost < best_cost)
                {
                    best_cost = current_cost;
                    best_X = X;
                    best_Y = Y;
                }
            }

            // 每 25 次迭代进行 FM 局部优化
            if (iteration % 25 == 0) {
                FM_local_optimize(graph, X, Y);
                int new_cost = calculate_cut(graph, X, Y);
                if (new_cost < current_cost) {
                    current_cost = new_cost;
                    if (new_cost < best_cost)
                    {
                        best_cost = new_cost;
                        best_X = X;
                        best_Y = Y;
                    }
                }
            }
            T *= cooling_rate; // 降温
        }
        X = best_X;
        Y = best_Y;
    }
    X = best_X;
    Y = best_Y;

    auto end_time = std::chrono::high_resolution_clock::now(); // 记录结束时间
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
    cout << "Total time: " << duration / 1000.0 << " seconds" << endl; // 输出总时间
}

/**
 * read_benchmark
 * 功能：读取基准测试文件，构建图对象。
 * 参数：
 * - graph: 图对象，用于存储节点和网络信息。
 * - benchmark_name: 基准测试文件路径。
 */
void Solution::read_benchmark(Graph &graph, string benchmark_name) {
    ifstream file(benchmark_name); // 打开文件
    if (!file.is_open())
    {
        cerr << "Failed to open the file!" << benchmark_name << endl;
        exit(-1); // 文件打开失败，退出程序
    }

    int edge_num, node_num; // 网络数和节点数
    string line;
    getline(file >> ws, line); // 读取第一行
    istringstream iss(line);
    iss >> edge_num;
    iss >> node_num;

    for (int i = 0; i < edge_num; i++)
    { // 遍历所有网络
        getline(file, line);
        istringstream iss(line);
        int node_id;

        Net *net = graph.add_net(i); // 添加网络到图

        while (iss >> node_id)
        {                                                   // 遍历网络中的节点
            Node *node = graph.get_or_create_node(node_id); // 获取或创建节点
            node->add_net(net);                             // 将网络添加到节点
            net->add_node(node);                            // 将节点添加到网络
        }
    }

    file.close(); // 关闭文件
}