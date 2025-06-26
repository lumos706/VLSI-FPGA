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

// 随机数引擎
std::default_random_engine gen((unsigned int)time(NULL));
std::uniform_real_distribution<double> rand01(0, 1);

void FM_local_optimize(Graph& graph, std::unordered_set<int>& X, std::unordered_set<int>& Y) {
    const auto& nodes = graph.get_nodes();
    size_t total_nodes = nodes.size();
    size_t min_size = static_cast<size_t>(ceil(total_nodes * 0.48));

    // --- 增益计算函数（与FM算法一致） ---
    auto calculate_gain = [&](int node_id, 
                             const std::unordered_set<int>& from_partition, 
                             const std::unordered_set<int>& to_partition) {
        int external_cost = 0, internal_cost = 0;
        for (const auto& net : graph.get_node(node_id)->get_nets()) {
            bool has_from = false, has_to = false;
            for (const auto& neighbor : net->get_nodes()) {
                int neighbor_id = neighbor->get_index();
                if (from_partition.count(neighbor_id)) has_from = true;
                if (to_partition.count(neighbor_id)) has_to = true;
            }
            if (has_from && has_to) external_cost++;
            else if (has_from) internal_cost++;
        }
        return external_cost - internal_cost;
    };

    // --- 初始化增益表 ---
    std::unordered_map<int, int> gain;
    for (const auto& node : nodes) {
        int node_id = node->get_index();
        gain[node_id] = (X.count(node_id)) ? 
            calculate_gain(node_id, X, Y) : 
            calculate_gain(node_id, Y, X);
    }

    // --- FM单轮优化循环 ---
    std::unordered_set<int> locked;
    bool improved;
    do {
        improved = false;
        int best_node = -1;
        int max_gain = std::numeric_limits<int>::min();
        bool move_to_X = false;

        // 查找最大增益节点
        for (const auto& [node_id, g] : gain) {
            if (locked.count(node_id)) continue;

            if (X.count(node_id) && X.size() > min_size) {
                if (g > max_gain) {
                    max_gain = g;
                    best_node = node_id;
                    move_to_X = false; // X→Y
                }
            } else if (Y.count(node_id) && Y.size() > min_size) {
                if (g > max_gain) {
                    max_gain = g;
                    best_node = node_id;
                    move_to_X = true; // Y→X
                }
            }
        }

        if (best_node == -1 || max_gain <= 0) break;

        // 移动节点并锁定
        if (move_to_X) {
            Y.erase(best_node);
            X.insert(best_node);
        } else {
            X.erase(best_node);
            Y.insert(best_node);
        }
        locked.insert(best_node);
        improved = true;

        // 更新邻居节点的增益
        for (const auto& net : graph.get_node(best_node)->get_nets()) {
            for (const auto& neighbor : net->get_nodes()) {
                int neighbor_id = neighbor->get_index();
                if (locked.count(neighbor_id)) continue;

                gain[neighbor_id] = (X.count(neighbor_id)) ? 
                    calculate_gain(neighbor_id, X, Y) : 
                    calculate_gain(neighbor_id, Y, X);
            }
        }

    } while (improved);
}

void Solution::my_partition_algorithm(Graph& graph, unordered_set<int>& X, unordered_set<int>& Y) {
    // 读取节点并确定分区大小限制
    // 记录时间
    auto start_time = std::chrono::high_resolution_clock::now();
    const auto& nodes = graph.get_nodes();
    size_t total_nodes = nodes.size();
    std::uniform_int_distribution<> rand_index(0, static_cast<int>(total_nodes) - 1);
    size_t min_size = static_cast<size_t>(ceil(total_nodes * 0.48));
    size_t max_size = static_cast<size_t>(floor(total_nodes * 0.52));
    unordered_set<int> best_X, best_Y;
    
    //基于度数贪心策略
    vector<pair<int, int>> node_degrees;
    for (const auto& node : nodes) {
        node_degrees.emplace_back(node->get_index(), node->get_nets().size());
    }
    sort(node_degrees.begin(), node_degrees.end(), 
        [](const auto& a, const auto& b) { return a.second > b.second; });

    X.clear(); Y.clear();
    for (const auto& [node_id, _] : node_degrees) {
        if (X.size() < min_size || (X.size() < max_size && Y.size() >= max_size)) {
            X.insert(node_id);
        } else {
            Y.insert(node_id);
        }
    }
    int best_cost = INT_MAX;
    for(int i = 0; i < 1; i++){
        cout << "--" << i + 1 << "( " << best_cost << " )--" << endl;
        // 计算初始开销
        int current_cost = calculate_cut(graph, X, Y);
        best_cost = min(current_cost, best_cost);
        // 模拟退火参数
        double T = 10 * current_cost; // 初始温度
        cout << "Initial Temperature: " << T << endl;
        double cooling_rate = 0.98;
        double min_T = 1e-6; // 最小温度
        int max_iterations = 500; // 最大迭代次数
        int iteration = 0;

        while (T > min_T && iteration < max_iterations) {
            iteration++;
            // 随机选一个节点
            int idx = rand_index(gen);
            int node_id = nodes[idx]->get_index();
            //int node_id = select_high_gain_node(graph, X, Y); // 基于FM增益选择节点

            bool inX = X.count(node_id) > 0;
            // 尝试移动到对方分区，需保证不违反分区大小限制
            if (inX && X.size() > min_size) {
                // 从X移到Y
                X.erase(node_id); 
                Y.insert(node_id);
                // 若违反大小限制，就撤回
                if (Y.size() > max_size) {
                    Y.erase(node_id);
                    X.insert(node_id);
                    T *= cooling_rate;
                    continue;
                }
            } else if (!inX && Y.size() > min_size) {
                // 从Y移到X
                Y.erase(node_id); 
                X.insert(node_id);
                // 若违反大小限制，就撤回
                if (X.size() > max_size) {
                    X.erase(node_id);
                    Y.insert(node_id);
                    T *= cooling_rate;
                    continue;
                }
            } else {
                // 如果无法移动则跳过
                T *= cooling_rate;
                continue;
            }

            // 计算新开销
            int new_cost = calculate_cut(graph, X, Y);
            int delta = new_cost - current_cost;

            // Metropolis准则决定是否接受
            if (delta >= 0) {
                double prob = exp(-delta / T), rand_val = rand01(gen);
                if (rand_val < prob) {
                    // 接受
                    current_cost = new_cost; 
                    // cout << iteration << " Tem: "<< T << "-- (SA-BAD)Current cost: " << current_cost << endl;
                } else {
                    // 撤回移动
                    //if(iteration%100==0) cout << iteration << " Tem: "<< T << "-- Rollback: " << current_cost << endl;
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
                if(current_cost < best_cost) {
                    best_cost = current_cost;
                    best_X = X;
                    best_Y = Y;
                    //cout << iteration << " Tem: "<< T << "-- (SA-UP-BEST)Current cost: " << current_cost << endl;
                }
                //else cout << iteration << " Tem: "<< T << "-- (SA-UP)Current cost: " << current_cost << endl;
            }
            // 在每25次迭代时进行FM局部优化
            if (iteration % 25 == 0) {
                FM_local_optimize(graph, X, Y);
                int new_cost = calculate_cut(graph, X, Y);
                if (new_cost < current_cost) {
                    current_cost = new_cost;
                    if(new_cost < best_cost) {
                        best_cost = new_cost;
                        best_X = X;
                        best_Y = Y;
                        //cout << iteration << " Tem: "<< T << "-- (FM-UP-BEST)Current cost: " << current_cost << endl;
                    }
                    //else cout << iteration << " Tem: "<< T << "-- (FM-UP)Current cost: " << current_cost << endl;
                }
            }
            T *= cooling_rate;
        }
        X = best_X;
        Y = best_Y;
    }
    X = best_X;
    Y = best_Y;

    // 记录结束时间
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
    cout << "Total time: " << duration / 1000.0 << " seconds" << endl;
}

void Solution::read_benchmark(Graph &graph, string benchmark_name) {
    ifstream file(benchmark_name);

    if(!file.is_open()) {
        cerr << "Failed to open the file!" << benchmark_name << endl;
        exit(-1);
    }

    int edge_num, node_num;
    string line;
    getline(file >> ws, line);
    istringstream iss(line);
    iss >> edge_num;
    iss >> node_num;

    
    for(int i = 0; i < edge_num; i++) {
        getline(file, line);
        istringstream iss(line);
        int node_id;
        
        Net *net = graph.add_net(i);

        while(iss >> node_id) {
            Node *node = graph.get_or_create_node(node_id);
            node->add_net(net);
            net->add_node(node);
        }
        
    }

    file.close();
}