# FPGA Routing 大作业

## 项目简介

本项目旨在探索 FPGA Routing 算法的性能优化与并行加速方法。通过引入 **拥塞感知代价函数**、**失败重试机制**、**并行策略**等改进，对比分析不同算法在多个数据集下的 **最小可行通道宽度**、**Routing Segment 数量**、**运行时间**等关键指标。

实验包括三组对比方案：

1. **Baseline**：原始串行路由器，采用基础 Pathfinder 策略；
2. **Optimized**：改进后的串行算法，引入代价优化与失败自动重试机制；
3. **Width+30%**：在方案 2 基础上增加布线宽度 +30%，评估性能余裕对路由成功率和速度的影响。

项目支持自动化测试、数据收集与图表绘制，便于系统性地评估算法效果。

---

## 项目目录结构

```
labFinal/
├── circuits/           # 测试用 FPGA 电路数据集
│   ├── tiny
│   ├── small_dense
│   └── ...
├── results/
│   ├── width.py        # 探索最小通道宽度
│   ├── monitor.py      # 运行主程序并记录数据
│   ├── results.csv         # 结果
│   └── ...
├── plots/ 
│   ├── plot.py         # 绘制结果图表
│   ├── speedup_plot.png 
│   ├── segments_vs_width_log.png
│   └── memory_usage_plot.png
├── main.cpp
├── Design.cpp/.h
├── FPGA.cpp/.h
├── FpgaTile.cpp/.h
├── Net.cpp/.h
├── RRNode.cpp/.h
├── Solution.cpp/.h
├── Congestion-Aware Parallel Routing for FPGAs-Achieving Superlinear Speedups and Resource Efficiency.pdf    # 大作业论文
└── README.md           # 当前文档
```

---

## 编译方法

### Step 1. 编译主程序

进入 `src/` 目录（或项目根目录），运行：

```bash
g++ -std=c++17 -fopenmp -o main.exe main.cpp Design.cpp FPGA.cpp FpgaTile.cpp Net.cpp RRNode.cpp Solution.cpp
```

---

## 复现实验流程

### Step 2. 选择算法版本并再次编译

进入 `src/Solution.cpp`，将对应版本的算法（Baseline 或 Optimized）代码块解除注释，屏蔽其他版本。切换后需重新编译。

---

### Step 3. 探测最小可行通道宽度

进入results目录并运行：

```bash
python width.py
```

- 该脚本将自动遍历 `circuits/` 中所有数据集，找到最小可行的布线通道宽度。
- 每次运行会自动调用 `main` 并分析其返回状态。
- 结果将保存至 `results/min_width_results.csv`。

---

### Step 4. 采集完整实验数据

- 修改 `results/monitor.py` 中的参数以适应不同的需求

- 进入results目录并运行数据采集：

```bash
python monitor.py
```

- 该脚本会读取最小通道宽度，自动多次运行主程序，采集稳定的性能数据（包括运行时间、segment 数量等），并保存为 CSV 文件。
- 你需要为每组实验设计保存不同的文件名（如 results.csv, results2.csv, results3.csv）。

---

### Step 5. 绘制对比图表

确保所有实验完成后，进入plots目录并执行：

```bash
python plot.py
```

脚本会从三个 CSV 文件中读取数据并绘制如下图表，结果图像将保存在 `./plots/` 目录中：

- memory_usage_plot.png
- segments_vs_width_log.png
- speedup_plot.png

---

## 输出文件格式说明

每组 CSV 文件字段：

| 字段名               | 含义                     |
|----------------------|--------------------------|
| test_case            | 测试数据集名称           |
| threads              | 并发线程数（或串行为1）  |
| time_sec             | 路由所需时间（单位：秒） |
| acceleration_ratio   | 加速比（与 baseline 比较）|
| segments_used        | 使用的 routing segment 数量|
| peak_memory          | 路由过程峰值内存（KB/MB）   |

示例：

```csv
test_case,threads,time_sec,acceleration_ratio,segments_used,peak_memory
tiny,1,0.1607,1.0,63,36.00 KB
tiny,2,0.076,2.11,63,140.00 KB
tiny,4,0.0958,1.68,63,104.00 KB
tiny,8,0.0842,1.91,63,104.00 KB
...
```

---

## 常见问题

**Q1. 如何确认布线是否成功？**  
程序内部已有判断逻辑（所有 nets 连通，且无非法 segment 重用）。若失败会在 stdout 中打印 "Error: Routing not complete"，且 monitor.py 会记录失败状态。

**Q2. 路由算法失败后会自动重试吗？**  
是的，在优化版本中内置了自适应重试机制，只要布线未完成就会自动 rerun。

**Q3. 为什么某些数据集始终失败？**  
可能当前布线宽度太小，可在 monitor.py 中手动调整起始宽度或设置 fallback 宽度。

---

## 参考资料

- [FPGA Routing – Wikipedia](https://en.wikipedia.org/wiki/FPGA_routing)
- PathFinder Routing Algorithm and congestion cost 相关论文
- [本实验课程讲义与配套代码注释](https://customized-computing.github.io/VLSI-FPGA/#/advanced/routing_problem_advanced)

---

## 实验报告与贡献说明

详细实验分析与对比图表见论文：

```
Congestion-Aware Parallel Routing for FPGAs_ Achieving Superlinear Speedups and Resource Efficiency.pdf
```

核心代码与实验设计均由本人独立完成,如需复现实验，请严格按照上述流程依次执行并记录中间结果，以保证可比性与数据一致性。如遇环境配置、路径错误等问题可根据报错信息联系开发者。