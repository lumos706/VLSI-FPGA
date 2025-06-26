# 实验3：FPGA布线算法复现指南

## 实验简介

本实验实现了一个基于基本迷宫算法的FPGA布线算法（Windows系统），探索每个数据集的**最小布线通道宽度**，并输出对应的 routing segments 数量。在 `Solution.cpp` 实现了核心布线算法，通过实验探索每个数据集的最小可行通道宽度。

---

## 目录结构

```
lab3/
├── circuits/           # 输入数据集，tiny、small_dense、...、huge 等
│   ├── tiny
│   ├── small_dense
│   └── ...
├── output/             # 输出目录，存放 tiny_routing.txt 等
│   ├── tiny_routing.txt
│   └── ...
├── main.cpp
├── Design.cpp/.h
├── FPGA.cpp/.h
├── FpgaTile.cpp/.h
├── Net.cpp/.h
├── RRNode.cpp/.h
├── Solution.cpp/.h
├── 22336216_陶宇卓_第三次作业.pdf
└── README.md
```

---

## 编译与运行方法

### 1. 手动编译

在 `lab3/` 目录下：

```bash
g++ -std=c++17 -o main main.cpp Design.cpp FPGA.cpp FpgaTile.cpp Net.cpp RRNode.cpp Solution.cpp
```

### 2. 运行

以 `tiny` 为例，指定通道宽度（如12）：

```bash
./main ./circuits/tiny 12
```

（需要手动多次尝试不同宽度，找到每个数据集的最小可行通道宽度）

---

## 输入输出文件格式

### 输入文件（如 tiny）

- 第一行：单个整数 $n$，表示 $n \times n$ logic block 网格。
- 之后每行：`XS YS PS XD1 YD1 PD1 ...`，表示线网连接。
- 以 `-1 -1 -1 -1 -1 -1` 结尾。

例：

```
10
1 2 4 2 3 2
0 0 4 1 2 3
0 0 4 1 2 3 4 4 1
-1 -1 -1 -1 -1 -1
```

### 输出文件（如 tiny_routing.txt）

两行，第一行是布线通道宽度，第二行是 routing segments 数量：

```
12
48
```

---

## 复现实验结果流程

1. **准备数据集**  
   下载 [circuits.zip](http://172.18.233.211:5244) 并解压到 `circuits/` 目录。

2. **编译项目**  
   参照上方编译方法，生成 `main` 可执行文件。

3. **探索最小可行通道宽度**  
   对每个数据集，尝试不同宽度 $W$，运行：

   ```bash
   ./main ./circuits/<dataset_name> <W>
   ```

   如果布线成功且所有 net 均成功连通，记录该 $W$ 为当前可行宽度，依次递减 $W$ 直到不可行，再回到最小可行宽度。

   可自行编写循环脚本自动测试。

4. **收集与整理结果**  
   输出文件命名为 `<dataset_name>_routing.txt`，内容为最小可行通道宽度和对应 routing segments 数量。

   将所有数据集的实验结果整理为表格，便于实验报告撰写。

---

## 常见问题

- **布线失败？**  
  若输出提示布线失败，说明当前 $W$ 不可行，需尝试更大值。
- **输出格式不对？**  
  输出文件需严格为两行：第一行为通道宽度，第二行为 routing segments 数量。
- **如何判断布线是否成功？**  
  通常所有 net 均连通且无冲突即可视为成功，或参考输出提示。

---

## 参考

- [FPGA布线问题维基百科](https://en.wikipedia.org/wiki/FPGA_routing)
- 实验配套代码和注释
- FPGA结构与Wilton拓扑讲义

---