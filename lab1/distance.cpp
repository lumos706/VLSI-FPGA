#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>
#include <filesystem>
#include <unordered_map>

namespace fs = std::filesystem;

// 计算两个向量之间的欧几里得距离
double calculate_distance(const std::vector<double>& vec1, const std::vector<double>& vec2) {
    if (vec1.size() != vec2.size()) {
        throw std::invalid_argument("Vectors must be of the same size");
    }
    double sum = 0.0;
    for (size_t i = 0; i < vec1.size(); ++i) {
        sum += std::pow(vec1[i] - vec2[i], 2);
    }
    return std::sqrt(sum);
}

// 从文件中读取数据到向量
std::vector<double> read_file_to_vector(const std::string& filepath) {
    std::vector<double> data;
    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + filepath);
    }
    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        double value;
        while (iss >> value) {
            data.push_back(value);
        }
    }
    file.close();
    return data;
}

int main() {
    std::string benchmark_path = "C:/Users/10559/Desktop/Study/Three/VLSI-FPGA/lab1/result";
    std::string ubfactor_path = "C:/Users/10559/Desktop/Study/Three/VLSI-FPGA/lab1/UBfactor_2";
    //C:/Users/10559/Desktop/Study/Three/VLSI-FPGA/lab1/UBfactor_2/ibm01.k.2.UBfactor.2.eig_vecs.2.num_cycles.2.h_threshold.300.solver_iters.80.spec_iters.2.best_solns.5.seed.0
    //                                                             ibm01.hgr.k.2.UBfactor.2.eig_vecs.2.num_cycles.2.h_threshold.300.solver_iters.80.spec_iters.2.best_solns.5.seed.0
    // 遍历 benchmark 目录下的所有 *_partition.txt 文件
    for (const auto& entry : fs::directory_iterator(benchmark_path)) {
        if (entry.path().extension() == ".txt" && entry.path().string().find("_partition") != std::string::npos) {
            std::string partition_file = entry.path().filename().string();
            std::string base_name = partition_file.substr(0, partition_file.find("_partition.txt"));
            std::string ubfactor_file = base_name + ".hgr.k.2.UBfactor.2.eig_vecs.2.num_cycles.2.h_threshold.300.solver_iters.80.spec_iters.2.best_solns.5.seed.0";

            std::string partition_filepath = benchmark_path + "/" + partition_file;
            std::string ubfactor_filepath = ubfactor_path + "/" + ubfactor_file;
            // std::cout<< "Partition file: " << partition_filepath << std::endl;
            // std::cout<< "UBfactor file: " << ubfactor_filepath << std::endl;
            // 检查 UBfactor 文件是否存在
            if (!fs::exists(ubfactor_filepath)) {
                std::cerr << "UBfactor file not found: " << ubfactor_filepath << std::endl;
                continue;
            }

            try {
                // 读取文件内容并计算距离
                std::vector<double> partition_data = read_file_to_vector(partition_filepath);
                std::vector<double> ubfactor_data = read_file_to_vector(ubfactor_filepath);
                double distance = calculate_distance(partition_data, ubfactor_data);

                std::cout << "Distance between " << partition_file << " and " << ubfactor_file << ": " << distance << std::endl;
            } catch (const std::exception& e) {
                std::cerr << "Error processing files: " << e.what() << std::endl;
            }
        }
    }

    return 0;
}
