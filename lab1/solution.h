#ifndef SOLUTION_H
#define SOLUTION_H

#include <string>
#include "Graph.h"
#include <fstream>
#include <iostream>
#include <unordered_set>
#include <sstream>
#include <set>

using namespace std;

class Solution{
    public:
        void read_benchmark(Graph &graph, string benchmark_name);
        void my_partition_algorithm(Graph &graph, unordered_set<int>&X, unordered_set<int>&Y);
};

#endif