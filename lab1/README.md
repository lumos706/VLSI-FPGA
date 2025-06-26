# 实验1：图划分算法复现指南

## 实验简介

本实验实现一个基于模拟退火的二分图划分算法（Windows系统），目标是将图的顶点集合分为两个子集 $V_1$ 和 $V_2$，使得割代价最小，并且每个子集的大小接近一半（允许2%偏差）。数据集来自 ISPD 基准集。

---

## 目录结构

```
lab1/
├── benchmark/           # 存放输入数据集（ibm01.hgr ~ ibm18.hgr）
│   ├── ibm01.hgr
│   └── ...
├── output/              # 存放划分结果（ibm01_partition.txt ~ ibm18_partition.txt）
│   ├── ibm01_partition.txt
│   └── ...
├── UBfactor2/           # 存放每个数据集的最优解
│   ├── ibm01.hgr.k.2.UBfactor.2.eig_vecs.2.num_cycles.2.h_threshold.300.solver_iters.80.spec_iters.2.best_solns.5.seed,0
│   └── ...
├── main.cpp
├── solution.cpp
├── solution.h
├── Graph.cpp
├── Graph.h
├── Node.cpp
├── Node.h
├── Net.cpp
├── Net.h
├── evaluate.cpp
├── distance.cpp
├── 22336216_陶宇卓_第一次作业.pdf
└── README.md            
```

---

## 编译方法

在 `lab1/` 目录下，运行如下命令编译主程序：

```bash
g++ -o main main.cpp solution.cpp Graph.cpp Net.cpp Node.cpp evaluate.cpp
```

如需使用欧式距离脚本：

```bash
g++ -o distance distance.cpp
```

---

## 运行方法

以 `ibm01.hgr` 为例：

```bash
./main benchmark/ibm01.hgr
```

程序将自动输出划分结果至 `output/ibm01_partition.txt`，文件命名格式为 `数据集前缀名_partition.txt`。

对所有数据集批量运行（可自己编写 bat 脚本）：

```bash
for file in benchmark/*.hgr; do
    ./main "$file"
done
```

---

## 输入、输出文件格式

### 输入文件（如 `ibm01.hgr`）

- 第一行：`边数 顶点数`
- 每一行：一条超边所包含的顶点编号（从 1 开始）

例：

```
14111 12752
1 2 3
2 5 8 9
...
```

### 输出文件（如 `ibm01_partition.txt`）

- 每行一个顶点的划分结果，0 或 1
- 第 $i$ 行表示编号 $i$ 的顶点所属分区（0 或 1）

例：

```
0
1
1
0
...
```

---

## 结果复现流程

1. **准备数据集**  
   下载 [ISPD_benchmark.zip] 解压至 `lab1/benchmark/`，确保每个数据集文件名为 `ibmXX.hgr`。

2. **编译代码**  
   参考上文编译说明，确保生成 `main` 可执行文件。

3. **运行算法**  
   按上述方法运行所有数据集，划分结果输出至 `output/`。

4. **评估划分质量**  
   使用 `evaluate.cpp` 提供的 `calculate_cut` 或 `evaluate` 函数，统计每个划分的割代价。

5. **与最优解对比**  
   参考 `UBfactor2/` 目录下的对应文件（如 `ibm01_ub.txt`），比较你的划分割代价与最优解的距离。

6. **结果整理**  
   将所有数据集的割代价、最优解及误差整理为表格，写入实验报告。

---

## 常见问题

- **输出文件行数不一致？**  
  检查输出文件行数是否等于输入文件顶点数。
- **命名不规范？**  
  输出必须命名为 `数据集名_partition.txt`，如 `ibm01_partition.txt`。
- **如何自测割代价？**  
  运行 `evaluate.cpp` 里的函数，或用实验提供的 checker 工具。

---

## 参考

- 初始代码结构说明见项目注释
- ISPD benchmark 详见 [ISPD Benchmark 官网](https://www2.eecs.berkeley.edu/Research/Projects/CS/eda/benchmarks/ispd98.html)

---