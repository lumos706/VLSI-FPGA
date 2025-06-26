# 实验2：FPGA布局算法复现指南

## 实验简介

本实验实现了一个基于模拟退火的FPGA布局算法（Windows系统），目标是输出每个模块的坐标，使整个芯片设计的总布线长度最小化。在`Solution.cpp`中实现了核心布局算法，并输出每个模块的最终放置坐标。

---

## 目录结构

```
lab2/
├── benchmark/            # 输入数据集，包含 small.txt, med1.txt ... huge.txt
│   ├── small.txt
│   ├── med1.txt
│   └── ...
├── output/               # 输出文件目录，存放 small_placement.txt, med1_placement.txt 等
│   ├── small_placement.txt
│   └── ...
├── main.cpp
├── Solution.cpp
├── Solution.h
├── Arch.cpp
├── Arch.h
├── Object.cpp
├── Object.h
├── Global.cpp
├── Global.h
├── 22336216_陶宇卓_第二次作业.pdf
└── README.md
```

---

## 编译与运行方法

### 1. 手动编译

在 `lab2/` 目录下运行：

```bash
g++ -std=c++17 -o main main.cpp Arch.cpp Global.cpp Object.cpp Solution.cpp
```

### 2. 运行

以 small.txt 为例：

```bash
./main ./benchmark/small.txt ./output/small_placement.txt
```

---

## 输入输出文件格式

### 输入文件（如 small.txt）

- **第一行**：FPGA资源宽度（x维度）和高度（y维度）
- **接下来的若干行**：每行一个固定模块，格式为 `模块编号 x坐标 y坐标`
- **空行**
- **接下来的若干行**：每行一个模块的网表信息，格式为 `模块编号 net1 net2 ...`

### 输出文件（如 small_placement.txt）

每行一个模块的最终放置坐标，格式为：

```
模块编号 x坐标 y坐标
```

---

## 复现实验结果流程

1. **准备数据集**  
   下载 [lab2_generate_benchmark.zip](https://github.com/Customized-Computing/VLSI-FPGA/releases/download/lab2/lab2_generate_benchmark.zip) 解压到 `benchmark/` 目录下。

2. **编译项目**  
   参照上方编译方法，生成可执行文件 `main`。

3. **运行所有数据集**  

4. **检查输出文件**  
   输出文件应位于 `output/` 目录，每行对应一个模块的编号和坐标。

5. **评估布线长度**  
   参考助教提供的评估脚本或代码，统计每个数据集的总布线长度，将结果整理成表格用于实验报告。

---

## 常见问题

- **输出文件格式错误？**  
  确保每行格式为 `模块编号 x坐标 y坐标`，行数应等于模块总数。
- **部分模块坐标为(-1,-1)？**  
  初始化未布局模块时坐标为(-1,-1)。算法需确保所有模块都布局在合法区域。
- **固定模块不允许移动？**  
  固定模块的坐标必须按照输入文件指定值输出，算法禁止改变其位置。
- **代码报错或输出异常？**  
  核查 `Instance` 和 `Block` 的联动实现，确保布局变动后数据结构一致。

---

## 参考

- 课程网站 [链接](https://customized-computing.github.io/VLSI-FPGA/#/lab2/lab2_problem)
- 助教提供的初始代码结构与注释
- [FPGA Layout 问题简介（英文）](https://en.wikipedia.org/wiki/Placement_(EDA))

---