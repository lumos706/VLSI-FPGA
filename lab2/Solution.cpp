#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>
#include "Solution.h"
#include "Object.h"
#include <queue>
#include <cmath>
#include <unordered_set>

#include <vector>
#include <map>  // 稀疏矩阵求解器

void Solution::initializePlacement() {
    // 初始化未布局模块集合
    std::unordered_set<int> unplacedModules;
    for (const auto& [id, inst] : glb_inst_map) {
        if (!inst->isFixed()) {
            unplacedModules.insert(id);
        }
    }

    // 初始化布局
    while (!unplacedModules.empty()) {
        // 1. 选择连接密度最大的模块
        int selectedModule = -1;
        int maxConnectionDensity = -1;

        for (int moduleId : unplacedModules) {
            int connectionDensity = 0;
            Instance* inst = glb_inst_map[moduleId];
            for (Net* net : inst->getNets()) {
                for (Instance* connectedInst : net->getInsts()) {
                    if (unplacedModules.find(connectedInst->getInstId()) == unplacedModules.end()) {
                        connectionDensity++;
                    }
                }
            }
            if (connectionDensity > maxConnectionDensity) {
                maxConnectionDensity = connectionDensity;
                selectedModule = moduleId;
            }
        }

        // 2. 选择代价最小的位置
        Instance* selectedInst = glb_inst_map[selectedModule];
        std::pair<int, int> bestPosition = {-1, -1};
        int minCost = std::numeric_limits<int>::max();

        for (int x = 0; x < glb_fpga.getSizeX(); ++x) {
            for (int y = 0; y < glb_fpga.getSizeY(); ++y) {
                if (glb_fpga.getBlock(x, y)->getInstsCount() == 0) { // 位置为空
                    // 尝试放置模块
                    glb_fpga.addInst(x, y, selectedInst);
                    selectedInst->setPosition(x, y);

                    // 计算代价（目标函数）
                    int cost = reportWireLength();

                    // 更新最优位置
                    if (cost < minCost) {
                        minCost = cost;
                        bestPosition = {x, y};
                    }

                    // 恢复原状态
                    glb_fpga.clearInst(x, y);
                    selectedInst->setPosition(-1, -1);
                }
            }
        }

        // 3. 将模块放置在最优位置
        glb_fpga.addInst(bestPosition.first, bestPosition.second, selectedInst);
        selectedInst->setPosition(bestPosition.first, bestPosition.second);

        // 从未布局集合中移除
        unplacedModules.erase(selectedModule);
    }

    std::printf("[INFO] Initial placement completed.\n");
}

void Solution::myplacementAlgorithm(std::string i_file_name) {
    printWireLength();
    // 初始化参数
    double T = 1000.0; // 初始温度
    double alpha = 0.98; // 降温速率
    int maxIter = 1000; // 每个温度下的最大迭代次数
    double T_min = 1e-5; // 最低温度

    // 初始化随机数种子
    std::srand(std::time(nullptr));

    // 计算初始总线长
    int currentWireLength = reportWireLength();
    int bestWireLength = currentWireLength;

    // 保存当前最优解
    std::map<int, std::pair<int, int>> bestSolution;

    while (T > T_min) {
        std::printf("[INFO] Current temperature: %.2f\n", T);
        for (int iter = 0; iter < maxIter; ++iter) {
            bool doSwap = (std::rand() % 2 == 0);
            if (doSwap) {
                // 执行交换操作
                Instance* inst1 = nullptr;
                Instance* inst2 = nullptr;
        
                // 随机选择两个未固定的模块
                do {
                    int instId1 = std::rand() % glb_inst_map.size();
                    int instId2 = std::rand() % glb_inst_map.size();
                    inst1 = glb_inst_map[instId1];
                    inst2 = glb_inst_map[instId2];
                } while (inst1->isFixed() || inst2->isFixed() || inst1 == inst2);
        
                // 保存当前模块的位置
                auto pos1 = inst1->getPosition();
                auto pos2 = inst2->getPosition();
        
                // 交换位置
                glb_fpga.clearInst(pos1.first, pos1.second);
                glb_fpga.clearInst(pos2.first, pos2.second);
        
                glb_fpga.addInst(pos1.first, pos1.second, inst2);
                glb_fpga.addInst(pos2.first, pos2.second, inst1);
        
                inst1->setPosition(pos2.first, pos2.second);
                inst2->setPosition(pos1.first, pos1.second);
        
                // 计算新的总线长
                int newWireLength = reportWireLength();
                int delta = newWireLength - currentWireLength;
        
                // 判断是否接受新解
                if (delta < 0 || (std::exp(-delta / T) > (double)std::rand() / RAND_MAX)) {
                    currentWireLength = newWireLength;
                    if (currentWireLength < bestWireLength) {
                        bestWireLength = currentWireLength;
                        bestSolution.clear();
                        for (const auto& [id, inst] : glb_inst_map) {
                            bestSolution[id] = inst->getPosition();
                        }
                        std::printf("[INFO] Found better solution with wirelength: %d\n", bestWireLength);
                    }
                } else {
                    // 恢复原位置
                    glb_fpga.clearInst(pos1.first, pos1.second);
                    glb_fpga.clearInst(pos2.first, pos2.second);
        
                    glb_fpga.addInst(pos1.first, pos1.second, inst1);
                    glb_fpga.addInst(pos2.first, pos2.second, inst2);
        
                    inst1->setPosition(pos1.first, pos1.second);
                    inst2->setPosition(pos2.first, pos2.second);
                }
            } else {
                // 随机选择一个模块
                Instance* inst = nullptr;
                int attempts = 0;
                do {
                    int instId = std::rand() % glb_inst_map.size();
                    inst = glb_inst_map[instId];
                    attempts++;
                    if (attempts > glb_inst_map.size()) {
                        std::printf("[ERROR] All instances are fixed. Cannot proceed.\n");
                        return; // 或者跳出当前温度的迭代
                    }
                } while (inst->isFixed());
        
                // 保存当前模块的位置
                auto oldPos = inst->getPosition();
        
                // 随机生成新位置，确保位置为空
                int newX, newY;
                do {
                    newX = std::rand() % glb_fpga.getSizeX();
                    newY = std::rand() % glb_fpga.getSizeY();
                } while (glb_fpga.getBlock(newX, newY)->getInstsCount() != 0);
        
                // 尝试移动模块
                // std::printf("[INFO] Clearing instance %d at (%d, %d)\n", inst->getInstId(), oldPos.first, oldPos.second);
                glb_fpga.clearInst(oldPos.first, oldPos.second);
        
                //std::printf("[DEBUG] Adding instance %d to (%d, %d)\n", inst->getInstId(), newX, newY);
                glb_fpga.addInst(newX, newY, inst);

                inst->setPosition(newX, newY);
        
                // 计算新的总线长
                int newWireLength = reportWireLength();
                int delta = newWireLength - currentWireLength;
        
                // 判断是否接受新解
                if (delta < 0 || (std::exp(-delta / T) > (double)std::rand() / RAND_MAX)) {
                    currentWireLength = newWireLength;
                    if (currentWireLength < bestWireLength) {
                        bestWireLength = currentWireLength;
                        bestSolution.clear();
                        for (const auto& [id, inst] : glb_inst_map) {
                            bestSolution[id] = inst->getPosition();
                        }
                        std::printf("[INFO] Found better solution with wirelength: %d\n", bestWireLength);
                        // outputSolution(i_file_name);
                    }
                } else {
                    // 恢复原位置
                    glb_fpga.clearInst(newX, newY);
                    glb_fpga.addInst(oldPos.first, oldPos.second, inst);
                    inst->setPosition(oldPos.first, oldPos.second);
                    //std::printf("[INFO] Rejected move for instance %d from (%d, %d) to (%d, %d)\n", inst->getInstId(), oldPos.first, oldPos.second, newX, newY);
                }
            }
            
        }
        // 降温
        T *= alpha;
        if (T < T_min) {
            //std::printf("[DEBUG] Temperature dropped below minimum threshold: %.2f\n", T);
            break;
        }
    }
    restoreBestSolution(bestSolution); // 恢复最优解到全局状态
}

// 恢复最优解的正确代码
void Solution::restoreBestSolution(const std::map<int, std::pair<int, int>>& bestSolution) {
    // 清空所有Block中的实例
    for (int x = 0; x < glb_fpga.getSizeX(); ++x) {
        for (int y = 0; y < glb_fpga.getSizeY(); ++y) {
            glb_fpga.clearInst(x, y);
        }
    }

    // 按最优解重新布局
    for (const auto& [id, pos] : bestSolution) {
        Instance* inst = glb_inst_map[id];
        if(!inst->isFixed()) inst->setPosition(pos.first, pos.second);
        glb_fpga.addInst(pos.first, pos.second, inst);
    }
}

int readBenchMarkFile(std::string i_file_name){

    std::fstream f;
    f.open(i_file_name, std::ios::in);

    if (!f.is_open()){
        std::printf("file %s open failed", i_file_name.c_str());
        return -1;
    }

    std::string line;
    while (std::getline(f, line)){
        if (line.empty())
            break;
        
        std::istringstream iss(line);
        std::string temp;
        std::vector<std::string> row;
        while (iss >> temp){
            row.push_back(temp);
        }
        if (row.size() == 2){
            int l_size_x = std::stoi(row[0]);
            int l_size_y = std::stoi(row[1]);
            glb_fpga.setSize(l_size_x, l_size_y);
            glb_fpga.initialize();
        }else if (row.size() == 3){
            int l_inst_id, l_x, l_y;
            l_inst_id = std::stoi(row[0]);
            l_x = std::stoi(row[1]);
            l_y = std::stoi(row[2]);
            Instance* inst = new Instance(l_x, l_y, l_inst_id, true);
            glb_inst_map[l_inst_id] = inst;
            glb_fpga.addInst(l_x, l_y, inst);
        }else{
            std::printf("something wrong when try to parser: %s", line.c_str());
            return -1;
        }
    }
    while (std::getline(f, line)){
        if (line.empty())
            continue;
        
        std::istringstream iss(line);
        std::string temp;
        std::vector<std::string> row;
        while (iss >> temp){
            row.push_back(temp);
        }
        int l_inst_id, l_net_id;
        l_inst_id = std::stoi(row[0]);
        Instance* l_inst_point = nullptr;
        if (glb_inst_map.find(l_inst_id) == glb_inst_map.end()){
            l_inst_point = new Instance();
            l_inst_point->setInstId(l_inst_id);
            glb_inst_map[l_inst_id] = l_inst_point;
        }else{
            l_inst_point = glb_inst_map[l_inst_id];
        }
        
        for (size_t i = 1; i < row.size(); i++){
            l_net_id = std::stoi(row[i]);
            Net* lo_net_point = nullptr;
            if (glb_net_map.find(l_net_id) == glb_net_map.end()){
                lo_net_point = new Net;
                lo_net_point->setNetId(l_net_id);
                glb_net_map[l_net_id] = lo_net_point;
            }else{
                lo_net_point = glb_net_map[l_net_id];
            }
            l_inst_point->addNet(lo_net_point);
            lo_net_point->addInst(l_inst_point);
        }
    }
    f.close();
    std::printf("Number of instances: %d\n", (int)glb_inst_map.size());
    return 0;
}

int outputSolution(std::string i_file_name){
    std::fstream f;
    f.open(i_file_name, std::ios::out);
    if (!f.is_open()){
        std::printf("unable to open file %s\n", i_file_name.c_str());
        return -1;
    }
    for (size_t i = 0; i < glb_inst_map.size(); i++){
        Instance* lo_inst_p = glb_inst_map[i];
        std::pair<int, int> lo_pos = lo_inst_p->getPosition();
        f << std::setw(5) << std::left << lo_inst_p->getInstId() \
            << std::setw(5) << std::left << lo_pos.first \
            << std::setw(5) << std::left << lo_pos.second << std::endl;
    }
    f.close();
    std::printf("output solution to %s\n", i_file_name.c_str());
    return 0;
}

int reportWireLength(){
    int l_wirelength = 0;
    for (auto lo_net : glb_net_map){
        l_wirelength += lo_net.second->evalHPWL();
    }
    //std::cout << "Wirelength: " << std::setw(5) << std::right << l_wirelength << std::endl;
    return l_wirelength;
}

int printWireLength(){
    int l_wirelength = 0;
    for (auto lo_net : glb_net_map){
        l_wirelength += lo_net.second->evalHPWL();
    }
    std::cout << "Wirelength: " << std::setw(5) << std::right << l_wirelength << std::endl;
    return l_wirelength;
}

int reportValid(){
    // 检查布局是否合法
    // 1. 检查每个inst的布局位置是否和Block包含的inst一致
    // 2. 检查每个Block是否存在inst重复出现的情况

    int l_error_count = 0;
    // 先检查每个inst的布局位置是否和Block包含的inst一致
    for (auto lo_inst : glb_inst_map){
        Instance* lo_inst_p = lo_inst.second;
        std::pair<int, int> lo_inst_pos = lo_inst_p->getPosition();
        Block* lo_block_p = glb_fpga.getBlock(lo_inst_pos.first, lo_inst_pos.second);
        if (lo_block_p == nullptr){
            std::printf("[ERROR] inst %d is not placed (%d, %d)\n", lo_inst_p->getInstId(), lo_inst_pos.first, lo_inst_pos.second);
            l_error_count++;
            continue;
        }
        if (lo_block_p->getInsts()[0] != lo_inst_p){
            std::printf("[ERROR] inst %d is not in block (%d, %d)\n", lo_inst_p->getInstId(), lo_inst_pos.first, lo_inst_pos.second);
            l_error_count++;
        }
    }
    // 再从block一侧检查是否存在inst重复出现的情况
    std::set<Instance*> lo_inst_attend;
    for (int i = 0; i < glb_fpga.getSizeX(); i++){
        for (int j = 0; j < glb_fpga.getSizeY(); j++){
            Block* lo_block_p = glb_fpga.getBlock(i, j);
            if (lo_block_p == nullptr)
                continue;
            for (auto lo_inst : lo_block_p->getInsts()){
                if (lo_inst_attend.find(lo_inst) != lo_inst_attend.end()){
                    std::printf("[ERROR] inst %d is repeated in block (%d, %d)\n", lo_inst->getInstId(), i, j);
                    l_error_count++; 
                } 
                lo_inst_attend.insert(lo_inst);
            }
        } 
    }
    return l_error_count;
}