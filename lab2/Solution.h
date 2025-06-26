#pragma once
#ifndef SOLUTION_H
#define SOLUTION_H

#include <string>
#include "Global.h"
#include "Object.h"

class Solution {
    public:
        Solution() {};
        ~Solution() {};
        // 你需要在这里实现你的布局算法
        void myplacementAlgorithm(std::string i_file_name);
        void initializePlacement();
        // 恢复最优解的正确代码
        void restoreBestSolution(const std::map<int, std::pair<int, int>>& bestSolution);
    };

int readBenchMarkFile(std::string i_file_name);

int outputSolution(std::string i_file_name);
// 报告当前布局的布线长度，使用HPWL线长预估模型
int reportWireLength();
// 打印当前布局的布线长度
int printWireLength();
// 报告当前布局是否合法
int reportValid();

#endif