#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <cmath>
#include <string>
#include <filesystem>
#include <unordered_set>
#include <algorithm>
#include "Net.h"
#include "Node.h"
#include "Graph.h"
#include "evaluate.h"
#include "solution.h"

using namespace std;


void evaluate_all_benchmarks(const string& benchmark_dir, const string& output_dir) {
    Solution solution;

    for (int i = 1; i <= 18; ++i) {
        // 构造文件名
        stringstream benchmark_file, output_file;
        benchmark_file << benchmark_dir << "/ibm" << setw(2) << setfill('0') << i << ".hgr";
        output_file << output_dir << "/ibm" << setw(2) << setfill('0') << i << "_partition.txt";

        // 初始化图
        Graph graph;
        solution.read_benchmark(graph, benchmark_file.str());

        // 输出图信息
        cout << "Processing: " << benchmark_file.str() << endl;
        cout << "Num nodes: " << graph.get_node_num() << endl;
        cout << "Num nets: " << graph.get_net_num() << endl;

        // 调用 evaluate 并输出割代价
        int cut_cost = evaluate(graph, output_file.str());
        cout << "Cut cost for " << output_file.str() << ": " << cut_cost << endl;
    }
}

int main(int argc, char **argv) {

    Solution solution;

    if(argc != 2) {
        cout << "Usage: ./main benchmark_file" << endl;
        exit(-1);
    }
    string benchmark_name = argv[1];
    Graph graph;

    solution.read_benchmark(graph, benchmark_name);    

    cout << "Num nodes: " << graph.get_node_num() << endl;
    cout << "Num nets: " << graph.get_net_num() << endl;

    // TODO: 
    // 1. finish your partition algorithm
    // 2. output your partition result to a file
    // 3. evaluate your partition result

    unordered_set<int> X, Y;

    solution.my_partition_algorithm(graph, X, Y);
    // 输出划分结果到文件
    string output_file = "C:/Users/10559/Desktop/Study/Three/VLSI-FPGA/lab1/output/ibm17_partition.txt";
    ofstream outfile(output_file);
    if (!outfile.is_open()) {
        cerr << "Failed to open output file: " << output_file << endl;
    }
    int node_num = graph.get_node_num();
    for (int node_id = 1; node_id <= node_num; ++node_id) {
        outfile << (X.count(node_id) ? 0 : 1) << endl;
    }
    outfile.close();
    // 评价划分结果
    cout << "Cut cost: " << evaluate(graph, output_file) << endl;
    cout << "Cut cost: " << calculate_cut(graph, X, Y) << endl;

    
    return 0;
}

// int main() {
//     string benchmark_dir = "C:/Users/10559/Desktop/Study/Three/VLSI-FPGA/lab1/benchmark";
//     string output_dir = "C:/Users/10559/Desktop/Study/Three/VLSI-FPGA/lab1/output";

//     evaluate_all_benchmarks(benchmark_dir, output_dir);

//     return 0;
// }