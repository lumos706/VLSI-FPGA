#ifndef EVALUATE_H
#define EVALUATE_H
#include <unordered_set>
#include "Graph.h"

int calculate_cut(Graph graph, std::unordered_set<int> X, std::unordered_set<int> Y);

int evaluate(Graph graph, string partition_name);

#endif