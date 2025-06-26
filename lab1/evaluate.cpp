#include <set>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <omp.h>
#include "evaluate.h"

using namespace std;

#define BIPARTITION_RATIO 0.02

int calculate_cut(Graph graph, unordered_set<int> X, unordered_set<int> Y) {
    int size_X = X.size();
    int size_Y = Y.size();
    int size = size_X + size_Y;
    if(!(((size_X * 1.0 / size >= 0.5-BIPARTITION_RATIO) && (size_X * 1.0 / size <= 0.5+BIPARTITION_RATIO)) \
        && ((size_Y * 1.0 / size >= 0.5-BIPARTITION_RATIO) && (size_Y * 1.0 / size <= 0.5+BIPARTITION_RATIO)))) {
        cerr << "The size of partition X and Y doesn't satisfy the requirement!" << endl;
        return -1;
    }

    int cut = 0;
    vector<Net *> nets = graph.get_nets();
    // Openmp parallelization
    #pragma omp parallel for num_threads(12) default(none) shared(X, Y, nets) reduction(+:cut)
    for (size_t i = 0; i < nets.size(); ++i) {
        const auto& net = nets[i];
        bool in_X = false, in_Y = false;

        // 遍历边的所有节点
        for (const auto& node : net->get_nodes()) {
            int index = node->get_index();
            if (X.count(index)) {
                in_X = true;
            }
            if (Y.count(index)) {
                in_Y = true;
            }
            // 如果节点分别属于两个划分，增加割代价并提前退出
            if (in_X && in_Y) {
                cut++;
                break;
            }
        }
    }

    return cut;
}


int evaluate(Graph graph, string partition_name) {
    unordered_set<int> X;
    unordered_set<int> Y;
    string line;
    ifstream partition_file(partition_name);
    int i = 1;
    while(getline(partition_file, line)) {
        istringstream iss(line);
        int partition;
        iss >> partition;
        if(partition == 0){
            X.insert(i);
        }
        else if(partition == 1){
            Y.insert(i);
        }else{
            break;
        }
        i++;
    }
    return calculate_cut(graph, X, Y);
}