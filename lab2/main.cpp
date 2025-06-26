#include <sstream>
#include <queue>
#include <cmath>
#include <unordered_set>
#include <vector>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include <chrono>
#include "Global.h"
#include "Solution.h"

int main(int argv, char** argc){
    std::string output_dir = "./output";

    // 检查output文件夹是否存在
    if (!std::filesystem::exists(output_dir)) {
        // 如果不存在，创建文件夹
        std::filesystem::create_directories(output_dir);
        std::cout << "Output directory created." << std::endl;
    } else {
        std::cout << "Output directory already exists." << std::endl;
    }
    if (argv != 3){
        std::printf("usage: ./main benchmark.txt output.txt\n");
        return -2;
    }
    std::string l_input_file_name(argc[1]);
    std::string l_output_file_name(argc[2]);
    int result = 0;
    auto start = std::chrono::high_resolution_clock::now();
    result = readBenchMarkFile(l_input_file_name);
    if (result != 0){
        std::printf("read benchmark file failed\n");
        return -1;
    }
    Solution solve = Solution();
    // ========================================================
    // You should finish placement algorithm in Solution class, and call your algorithm here. 
    // Finally, don't forget to output you solution into file.
    // ========================================================
    std::printf("[INFO] Initial wirelength before placement:\n");
    glb_fpga.reportFPGA();
    //计算时间
    
    solve.initializePlacement();
    solve.myplacementAlgorithm(l_output_file_name);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "Elapsed time: " << elapsed.count() << " seconds" << std::endl;
    result = outputSolution(l_output_file_name);
    if (result != 0){
        std::printf("output solution failed\n");
        return -1;
    }


    glb_fpga.reportFPGA();
    printWireLength();
    result = reportValid();
    for (auto lo_inst : glb_inst_map)
        delete lo_inst.second;
    for (auto lo_net : glb_net_map)
        delete lo_net.second;
    glb_inst_map.clear();
    glb_net_map.clear();
    std::printf("[INFO] Program exited with: %d errors\n", result);
    return result == 0 ? 0 : -1;
}