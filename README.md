# VLSI-FPGA 项目实验

本仓库包含中山大学“VLSI（超大规模集成电路）设计导论”课程的全部实验（lab1~lab3）及大作业，覆盖图划分、布局、布线及大作业等核心内容。各实验/项目均有独立 README，支持复现所有结果。

---

## 目录结构

```
.
├── lab1/            # 实验1：图划分算法
│   └── README.md
├── lab2/            # 实验2：FPGA布局算法
│   └── README.md
├── lab3/            # 实验3：FPGA布线算法
│   └── README.md
├── project/         # 大作业：FPGA Routing加速与优化
│   └── README.md
└── README.md        # 本总览文档
```

---

## 实验1：图划分算法（lab1）

- **内容**：实现二分图划分算法，使割边代价最小，满足分区均衡约束。
- **数据集**：ISPD基准集（ibm01~ibm18）。
- **实现要点**：在提供的 C++ 框架（Graph/Node/Net 类）基础上，完成 solution.cpp 算法逻辑，输出分区结果文件。
- **复现指南**：参见 `lab1/README.md`，含编译、运行、输入输出格式、结果评估等流程。

---

## 实验2：FPGA布局算法（lab2）

- **内容**：设计FPGA模块布局算法，输出模块坐标，最小化总布线长度。
- **数据集**：small, med, lg, xl, huge 等多规模基准。
- **实现要点**：在 Solution.cpp 内实现核心布局算法，支持固定模块约束及可移动模块自动布局。
- **复现指南**：见 `lab2/README.md`，含详细编译、批量测试、输出格式和评估方法。

---

## 实验3：FPGA布线算法（lab3）

- **内容**：实现FPGA布线算法，探索最小可行通道宽度，统计 routing segments 数量。
- **数据集**：tiny, small_dense, large_dense, huge 等。
- **实现要点**：在 Solution.cpp/MyRouter 类实现布线逻辑，支持自动多轮测试，输出最小通道宽度与 segments。
- **复现指南**：见 `lab3/README.md`，含自动化参数扫描、输出说明、批量评测等。

---

## 大作业：FPGA Routing 加速与优化（labFinal）

- **内容**：基于实验三，系统性探索路由算法的性能优化与并行加速，包括拥塞感知代价、失败自适应重试、并行策略等。
- **功能**：支持三组对比实验（Baseline、Optimized、Width+30%），自动化批量测试、性能数据采集与图表绘制。
- **评测指标**：最小可行通道宽度、segments数、运行时间、加速比、内存峰值等。
- **复现流程**：见 `project/README.md`，涵盖源码切换、自动化脚本、结果采集与绘图全流程。
- **分析与结论**：见课程论文Congestion-Aware Parallel Routing for FPGAs-Achieving Superlinear Speedups and Resource Efficiency.pdf。

---

## 快速上手

1. **进入各实验文件夹，参考对应 README 完成环境准备、编译、运行和结果复现。**
2. **如需整体评测/对比，可结合 project/ 下自动化脚本批量运行与统计。**
3. **所有实验均配套数据集、输出样例与详细流程，便于自测与扩展。**

---

## 参考

- ISPD Benchmark、课程提供数据集
- [课程网站](https://customized-computing.github.io/VLSI-FPGA/#/)
- 相关论文

**作者：Lumos706**  
对你有帮助就点个star吧

---