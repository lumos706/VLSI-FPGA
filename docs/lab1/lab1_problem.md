# 实验1

## 简介

在本次实验中，你将设计一个图划分算法，输出两个划分$V_1$和$V_2$，并且$V_1$和$V_2$之间的割代价尽可能小。对于每个划分$V_i$，需要满足：
$$
\frac{1}{2}-\epsilon \le \frac{|V_i|}{|V|} \le \frac{1}{2}+\epsilon
$$
其中，$\epsilon=2\%$。$|V|$表示图顶点的数量。

* 实现你的算法，但**暂时不提交**代码。（大作业时统一收三次实验代码）
* **提交**实验报告说明你是如何做的。实验报告应该包含以下内容：
  * 你的算法逻辑和实现思路。**（不要超过2页，挑重点的说）**
  * 你的算法运行结果表格，报告每一个数据集的割代价。同时报告你的解相比最优解的距离。
  * 实验总结。说明你的方法存在的优点和缺点，下一步的改进方向是什么？

## 代码说明

为了降低同学们非核心算法的代码量，助教为各位同学提供了一个[初始代码](https://github.com/Customized-Computing/VLSI-FPGA/tree/main/lab1)，你只需要实现一个图划分算法。

初始代码包括了Graph，Node和Net的基本类。你需要在`solution.cpp`内部实现你的算法，然后在`main.cpp`中调用你的算法。

* `main.cpp`：主可执行文件，需要一个命令行参数——电路文件的路径。
* `solution.cpp/.h`：表示解决方法的类，里面包含一个`read_benchmark`函数用于读取电路文件和构造网表。需要在这实现图划分算法。
* `Graph.cpp/.h`：表示图的类，包含图中顶点的集合和边的集合。
* `Node.cpp/.h`：表示顶点的类，其中有一个顶点连接的边的列表。
* `Net.cpp/.h`：表示边的类，其中有一个边连接的顶点的列表。
* `evaluate.cpp`：包含一个`calculate_cut`函数，用于计算两个划分的割。一个`evaluate`函数，用于评价划分结果。
> 助教们本来想写成一个单独的checker程序，用来评价同学们划分结果的好坏。后来觉得普通作业没有必要验证同学们的结果，于是就留下evaluate的实现，而没有完整的checker程序。

下面是如何访问Node列表和Net列表的示例：

访问图中的顶点信息示例

```cpp
vector<Node *> nodes = graph.get_nodes();
for(const auto &node : nodes) {
    cout << node->get_index() << ": ";
	for(const auto &net : node->get_nets()) {
        cout << net->get_index() << " ";
    }
    cout << endl;
}
```

访问图中边的信息示例

```cpp
vector<Net *> nets = graph.get_nets();
for(const auto &net : nets) {
    cout << net->get_index() << ": ";
	for(const auto &node : net->get_nodes()) {
        cout << node->get_index() << " ";
    }
    cout << endl;
}
```



### 测试集

下载连接：[校内链接](http://172.18.233.211:5244/VLSI%E8%AF%BE%E4%BB%B6/dataset)和[校外链接](https://github.com/Customized-Computing/VLSI-FPGA/releases/tag/lab1)

`ISPD_benchmark.zip`数据集中电路的大小如下表所示：


| Benchmark | 顶点数量  | 边数量  |
| --------- | ------ | ------ |
| IBM01     | 12752  | 14111  |
| IBM02     | 19601  | 19584  |
| IBM03     | 23136  | 27401  |
| IBM04     | 27507  | 31970  |
| IBM05     | 29347  | 28446  |
| IBM06     | 32498  | 34826  |
| IBM07     | 45926  | 48117  |
| IBM08     | 51309  | 50513  |
| IBM09     | 53395  | 60902  |
| IBM10     | 69429  | 75196  |
| IBM11     | 70558  | 81454  |
| IBM12     | 71076  | 77240  |
| IBM13     | 84199  | 99666  |
| IBM14     | 147605 | 152772 |
| IBM15     | 161570 | 186608 |
| IBM16     | 183484 | 190048 |
| IBM17     | 185495 | 189581 |
| IBM18     | 210613 | 201920 |

### 程序运行方法

1. 手动编译运行：

```bash
g++ -o main main.cpp solution.cpp Graph.cpp Net.cpp Node.cpp
./main benchmark_file
```

2. 使用`makefile`脚本编译运行（需要Linux环境）：
> 助教们推荐同学们学习如何编写`makefile`脚本，这对于日后工程开发非常有用。
> 
> cmakefile也是一个非常好用的脚本，但是助教们不推荐使用，对于这样一个简单的程序来说，cmake的学习成本有些过于高昂了。如果同学们学有余力，可以尝试把这个项目改造成一个cmake项目。
```bash
make all
./main benchmark_file
```

3. 兼容Vscode的`F5运行并调试`功能：

我们需要创建两个配置文件告诉Vscode如何执行项目的Debug工作。
1. 创建一个文件夹`.vscode`和两个文件`launch.json`和`tasks.json`。
2. `launch.json`填写以下内容：
> 注意`args`中的ibm01.hgr路径是否和你实际存放位置一致。
> 助教们本地测试设置的Vscode的工作路径是`~/VLSI-FPGA/lab1`，如果你的路径不是这个，请自行修正。

```json
{
    // Use IntelliSense to learn about possible attributes.
    // Hover to view descriptions of existing attributes.
    // For more information, visit: https://go.microsoft.com/fwlink/?linkid=830387
    "version": "0.2.0",
        "configurations": [
        {
            "name": "g++ - Build and debug active file",
            "type": "cppdbg",
            "request": "launch",
            "program": "${cwd}/main",
            "args": [
                "${cwd}/benchmark/ibm01.hgr"
            ],
            "stopAtEntry": false,
            "cwd": "${fileDirname}",
            "environment": [],
            "externalConsole": false,
            "MIMode": "gdb",
            "setupCommands": [
                {
                    "description": "Enable pretty-printing for gdb",
                    "text": "-enable-pretty-printing",
                    "ignoreFailures": true
                }
            ],
            "preLaunchTask": "build",
            "miDebuggerPath": "/usr/bin/gdb"
        }
    ]
}
```
3. `tasks.json`填写以下内容：

```json
{
    "version": "2.0.0",
    "tasks": [
        {
            "label": "make",
            "type": "shell",
            "command": "make",
            "args": [
                "all"
            ],
            "options": {
                "cwd": "${cwd}"
            }
        },
        {
            "label": "build",
            "dependsOn": ["make"]
        }
    ]
}
```

## 输入文件格式

> 如果你对于助教们写的数据读取模块性能不够满意，或者不支持你的奇思妙想，你可以自己写一个数据读取模块。

本章介绍的内容仅面向需要自行编写数据读取模块的同学。

第一行包括两个数字，第一个数字代表边的数量，第二个数字代表顶点的数量，<mark>顶点的编号从1开始</mark>。

下面每一行代表一条超边连接的顶点，一行可能会有多个数字：

```
edge_num node_num
node1 node2
node1 node2 ... noden
```


## 输出文件格式

要求图划分算法的输出是一个文件，命名格式为`benchmark_name_partition.txt`，例如`ibm01_partition.txt`。

每一行代表顶点划分的类，0代表划分到X，1代表划分到Y，格式如下：

```
0
1
1
0
...
```

## 如何提交

实验报告的命名格式为**学号\_姓名\_第一次作业.pdf**。

发送到助教邮箱：wangzh523@mail2.sysu.edu.cn<br>
<mark>截止日期：2025年4月24日23时59分</mark><br>
后期会统计已经收到作业的同学，并发在群里。如果上传作业后有更新，请命名为V2、V3……，并在助教统计作业上交情况时注意是否收到最新版本。

## 附件

以下给出这个数据集当前的最优解结果，同学们可以据此判断自己的算法距离最优解有多远。来自[TILOS-AI-Institute](https://github.com/TILOS-AI-Institute/HypergraphPartitioning/tree/main)。

|   Testcase   | Statistics |              |    Cutsize    |              |              |               |
|:------------:|:----------:|:------------:|:-------------:|:------------:|:------------:|:-------------:|
|              | # Vertices | # Hyperedges | ε  = 1 | ε = 2 | ε = 5 | ε = 10 |
|     IBM01    |    12752   |     14111    |      [203](https://github.com/TILOS-AI-Institute/HypergraphPartitioning/blob/main/solutions_2way/ISPD_benchmark_solutions/hMetis_SpecPart/UBfactor_1/ibm01.hgr.k.2.UBfactor.1.eig_vecs.2.num_cycles.2.h_threshold.300.solver_iters.80.spec_iters.2.best_solns.5.seed.0)      |      [200](https://github.com/TILOS-AI-Institute/HypergraphPartitioning/blob/main/solutions_2way/ISPD_benchmark_solutions/hMetis_SpecPart/UBfactor_2/ibm01.hgr.k.2.UBfactor.2.eig_vecs.2.num_cycles.2.h_threshold.300.solver_iters.80.spec_iters.2.best_solns.5.seed.0)     |      [180](https://github.com/TILOS-AI-Institute/HypergraphPartitioning/blob/main/solutions_2way/ISPD_benchmark_solutions/hMetis_SpecPart/UBfactor_5/ibm01.hgr.k.2.UBfactor.5.eig_vecs.2.num_cycles.2.h_threshold.300.solver_iters.80.spec_iters.2.best_solns.5.seed.0)     |      [166](https://github.com/TILOS-AI-Institute/HypergraphPartitioning/blob/main/solutions_2way/ISPD_benchmark_solutions/hMetis_SpecPart/UBfactor_10/ibm01.hgr.k.2.UBfactor.10.eig_vecs.2.num_cycles.2.h_threshold.300.solver_iters.80.spec_iters.2.best_solns.5.seed.0)      |
|     IBM02    |    19601   |     19584    |      [341](https://github.com/TILOS-AI-Institute/HypergraphPartitioning/blob/main/solutions_2way/ISPD_benchmark_solutions/hMetis_SpecPart/UBfactor_1/ibm02.hgr.k.2.UBfactor.1.eig_vecs.2.num_cycles.2.h_threshold.300.solver_iters.80.spec_iters.2.best_solns.5.seed.0)      |      [307](https://github.com/TILOS-AI-Institute/HypergraphPartitioning/blob/main/solutions_2way/ISPD_benchmark_solutions/hMetis_SpecPart/UBfactor_2/ibm02.hgr.k.2.UBfactor.2.eig_vecs.2.num_cycles.2.h_threshold.300.solver_iters.80.spec_iters.2.best_solns.5.seed.0)     |      [262](https://github.com/TILOS-AI-Institute/HypergraphPartitioning/blob/main/solutions_2way/ISPD_benchmark_solutions/hMetis_SpecPart/UBfactor_5/ibm02.hgr.k.2.UBfactor.5.eig_vecs.2.num_cycles.2.h_threshold.300.solver_iters.80.spec_iters.2.best_solns.5.seed.0)     |      [262](https://github.com/TILOS-AI-Institute/HypergraphPartitioning/blob/main/solutions_2way/ISPD_benchmark_solutions/hMetis_SpecPart/UBfactor_10/ibm02.hgr.k.2.UBfactor.10.eig_vecs.2.num_cycles.2.h_threshold.300.solver_iters.80.spec_iters.2.best_solns.5.seed.0)      |
|     IBM03    |    23136   |     27401    |      [954](https://github.com/TILOS-AI-Institute/HypergraphPartitioning/blob/main/solutions_2way/ISPD_benchmark_solutions/hMetis_SpecPart/UBfactor_1/ibm03.hgr.k.2.UBfactor.1.eig_vecs.2.num_cycles.2.h_threshold.300.solver_iters.80.spec_iters.2.best_solns.5.seed.0)      |      [951](https://github.com/TILOS-AI-Institute/HypergraphPartitioning/blob/main/solutions_2way/ISPD_benchmark_solutions/hMetis_SpecPart/UBfactor_2/ibm03.hgr.k.2.UBfactor.2.eig_vecs.2.num_cycles.2.h_threshold.300.solver_iters.80.spec_iters.2.best_solns.5.seed.0)     |      [950](https://github.com/TILOS-AI-Institute/HypergraphPartitioning/blob/main/solutions_2way/ISPD_benchmark_solutions/hMetis_SpecPart/UBfactor_5/ibm03.hgr.k.2.UBfactor.5.eig_vecs.2.num_cycles.2.h_threshold.300.solver_iters.80.spec_iters.2.best_solns.5.seed.0)     |      [950](https://github.com/TILOS-AI-Institute/HypergraphPartitioning/blob/main/solutions_2way/ISPD_benchmark_solutions/hMetis_SpecPart/UBfactor_10/ibm03.hgr.k.2.UBfactor.10.eig_vecs.2.num_cycles.2.h_threshold.300.solver_iters.80.spec_iters.2.best_solns.5.seed.0)      |
|     IBM04    |    27507   |     31970    |      [580](https://github.com/TILOS-AI-Institute/HypergraphPartitioning/blob/main/solutions_2way/ISPD_benchmark_solutions/hMetis_SpecPart/UBfactor_1/ibm04.hgr.k.2.UBfactor.1.eig_vecs.2.num_cycles.2.h_threshold.300.solver_iters.80.spec_iters.2.best_solns.5.seed.0)      |      [573](https://github.com/TILOS-AI-Institute/HypergraphPartitioning/blob/main/solutions_2way/ISPD_benchmark_solutions/hMetis_SpecPart/UBfactor_2/ibm04.hgr.k.2.UBfactor.2.eig_vecs.2.num_cycles.2.h_threshold.300.solver_iters.80.spec_iters.2.best_solns.5.seed.0)     |      [514](https://github.com/TILOS-AI-Institute/HypergraphPartitioning/blob/main/solutions_2way/ISPD_benchmark_solutions/hMetis_SpecPart/UBfactor_5/ibm04.hgr.k.2.UBfactor.5.eig_vecs.2.num_cycles.2.h_threshold.300.solver_iters.80.spec_iters.2.best_solns.5.seed.0)     |      [388](https://github.com/TILOS-AI-Institute/HypergraphPartitioning/blob/main/solutions_2way/ISPD_benchmark_solutions/hMetis_SpecPart/UBfactor_10/ibm04.hgr.k.2.UBfactor.10.eig_vecs.2.num_cycles.2.h_threshold.300.solver_iters.80.spec_iters.2.best_solns.5.seed.0)      |
|     IBM05    |    29347   |     28446    |      [1719](https://github.com/TILOS-AI-Institute/HypergraphPartitioning/blob/main/solutions_2way/ISPD_benchmark_solutions/hMetis_SpecPart/UBfactor_1/ibm05.hgr.k.2.UBfactor.1.eig_vecs.2.num_cycles.2.h_threshold.300.solver_iters.80.spec_iters.2.best_solns.5.seed.0)     |     [1706](https://github.com/TILOS-AI-Institute/HypergraphPartitioning/blob/main/solutions_2way/ISPD_benchmark_solutions/hMetis_SpecPart/UBfactor_2/ibm05.hgr.k.2.UBfactor.2.eig_vecs.2.num_cycles.2.h_threshold.300.solver_iters.80.spec_iters.2.best_solns.5.seed.0)     |     [1697](https://github.com/TILOS-AI-Institute/HypergraphPartitioning/blob/main/solutions_2way/ISPD_benchmark_solutions/hMetis_SpecPart/UBfactor_5/ibm05.hgr.k.2.UBfactor.5.eig_vecs.2.num_cycles.2.h_threshold.300.solver_iters.80.spec_iters.2.best_solns.5.seed.0)     |      [1645](https://github.com/TILOS-AI-Institute/HypergraphPartitioning/blob/main/solutions_2way/ISPD_benchmark_solutions/hMetis_SpecPart/UBfactor_10/ibm05.hgr.k.2.UBfactor.10.eig_vecs.2.num_cycles.2.h_threshold.300.solver_iters.80.spec_iters.2.best_solns.5.seed.0)     |
|     IBM06    |    32498   |     34826    |      [976](https://github.com/TILOS-AI-Institute/HypergraphPartitioning/blob/main/solutions_2way/ISPD_benchmark_solutions/hMetis_SpecPart/UBfactor_1/ibm06.hgr.k.2.UBfactor.1.eig_vecs.2.num_cycles.2.h_threshold.300.solver_iters.80.spec_iters.2.best_solns.5.seed.0)      |      [962](https://github.com/TILOS-AI-Institute/HypergraphPartitioning/blob/main/solutions_2way/ISPD_benchmark_solutions/hMetis_SpecPart/UBfactor_2/ibm06.hgr.k.2.UBfactor.2.eig_vecs.2.num_cycles.2.h_threshold.300.solver_iters.80.spec_iters.2.best_solns.5.seed.0)     |      [871](https://github.com/TILOS-AI-Institute/HypergraphPartitioning/blob/main/solutions_2way/ISPD_benchmark_solutions/hMetis_SpecPart/UBfactor_5/ibm06.hgr.k.2.UBfactor.5.eig_vecs.2.num_cycles.2.h_threshold.300.solver_iters.80.spec_iters.2.best_solns.5.seed.0)     |      [728](https://github.com/TILOS-AI-Institute/HypergraphPartitioning/blob/main/solutions_2way/ISPD_benchmark_solutions/hMetis_SpecPart/UBfactor_10/ibm06.hgr.k.2.UBfactor.10.eig_vecs.2.num_cycles.2.h_threshold.300.solver_iters.80.spec_iters.2.best_solns.5.seed.0)      |
|     IBM07    |    45926   |     48117    |      [910](https://github.com/TILOS-AI-Institute/HypergraphPartitioning/blob/main/solutions_2way/ISPD_benchmark_solutions/hMetis_SpecPart/UBfactor_1/ibm07.hgr.k.2.UBfactor.1.eig_vecs.2.num_cycles.2.h_threshold.300.solver_iters.80.spec_iters.2.best_solns.5.seed.0)      |      [878](https://github.com/TILOS-AI-Institute/HypergraphPartitioning/blob/main/solutions_2way/ISPD_benchmark_solutions/hMetis_SpecPart/UBfactor_2/ibm07.hgr.k.2.UBfactor.2.eig_vecs.2.num_cycles.2.h_threshold.300.solver_iters.80.spec_iters.2.best_solns.5.seed.0)     |      [818](https://github.com/TILOS-AI-Institute/HypergraphPartitioning/blob/main/solutions_2way/ISPD_benchmark_solutions/hMetis_SpecPart/UBfactor_5/ibm07.hgr.k.2.UBfactor.5.eig_vecs.2.num_cycles.2.h_threshold.300.solver_iters.80.spec_iters.2.best_solns.5.seed.0)     |      [764](https://github.com/TILOS-AI-Institute/HypergraphPartitioning/blob/main/solutions_2way/ISPD_benchmark_solutions/hMetis_SpecPart/UBfactor_10/ibm07.hgr.k.2.UBfactor.10.eig_vecs.2.num_cycles.2.h_threshold.300.solver_iters.80.spec_iters.2.best_solns.5.seed.0)      |
|     IBM08    |    51309   |     50513    |      [1140](https://github.com/TILOS-AI-Institute/HypergraphPartitioning/blob/main/solutions_2way/ISPD_benchmark_solutions/hMetis_SpecPart/UBfactor_1/ibm08.hgr.k.2.UBfactor.1.eig_vecs.2.num_cycles.2.h_threshold.300.solver_iters.80.spec_iters.2.best_solns.5.seed.0)     |     [1140](https://github.com/TILOS-AI-Institute/HypergraphPartitioning/blob/main/solutions_2way/ISPD_benchmark_solutions/hMetis_SpecPart/UBfactor_2/ibm08.hgr.k.2.UBfactor.2.eig_vecs.2.num_cycles.2.h_threshold.300.solver_iters.80.spec_iters.2.best_solns.5.seed.0)     |     [1140](https://github.com/TILOS-AI-Institute/HypergraphPartitioning/blob/main/solutions_2way/ISPD_benchmark_solutions/hMetis_SpecPart/UBfactor_5/ibm08.hgr.k.2.UBfactor.5.eig_vecs.2.num_cycles.2.h_threshold.300.solver_iters.80.spec_iters.2.best_solns.5.seed.0)     |      [1120](https://github.com/TILOS-AI-Institute/HypergraphPartitioning/blob/main/solutions_2way/ISPD_benchmark_solutions/hMetis_SpecPart/UBfactor_10/ibm08.hgr.k.2.UBfactor.10.eig_vecs.2.num_cycles.2.h_threshold.300.solver_iters.80.spec_iters.2.best_solns.5.seed.0)     |
|     IBM09    |    53395   |     60902    |      [625](https://github.com/TILOS-AI-Institute/HypergraphPartitioning/blob/main/solutions_2way/ISPD_benchmark_solutions/hMetis_SpecPart/UBfactor_1/ibm09.hgr.k.2.UBfactor.1.eig_vecs.2.num_cycles.2.h_threshold.300.solver_iters.80.spec_iters.2.best_solns.5.seed.0)      |      [620](https://github.com/TILOS-AI-Institute/HypergraphPartitioning/blob/main/solutions_2way/ISPD_benchmark_solutions/hMetis_SpecPart/UBfactor_2/ibm09.hgr.k.2.UBfactor.2.eig_vecs.2.num_cycles.2.h_threshold.300.solver_iters.80.spec_iters.2.best_solns.5.seed.0)     |      [620](https://github.com/TILOS-AI-Institute/HypergraphPartitioning/blob/main/solutions_2way/ISPD_benchmark_solutions/hMetis_SpecPart/UBfactor_5/ibm09.hgr.k.2.UBfactor.5.eig_vecs.2.num_cycles.2.h_threshold.300.solver_iters.80.spec_iters.2.best_solns.5.seed.0)     |      [519](https://github.com/TILOS-AI-Institute/HypergraphPartitioning/blob/main/solutions_2way/ISPD_benchmark_solutions/hMetis_SpecPart/UBfactor_10/ibm09.hgr.k.2.UBfactor.10.eig_vecs.2.num_cycles.2.h_threshold.300.solver_iters.80.spec_iters.2.best_solns.5.seed.0)      |
|     IBM10    |    69429   |     75196    |      [1285](https://github.com/TILOS-AI-Institute/HypergraphPartitioning/blob/main/solutions_2way/ISPD_benchmark_solutions/hMetis_SpecPart/UBfactor_1/ibm10.hgr.k.2.UBfactor.1.eig_vecs.2.num_cycles.2.h_threshold.300.solver_iters.80.spec_iters.2.best_solns.5.seed.0)     |     [1253](https://github.com/TILOS-AI-Institute/HypergraphPartitioning/blob/main/solutions_2way/ISPD_benchmark_solutions/hMetis_SpecPart/UBfactor_2/ibm10.hgr.k.2.UBfactor.2.eig_vecs.2.num_cycles.2.h_threshold.300.solver_iters.80.spec_iters.2.best_solns.5.seed.0)     |     [1244](https://github.com/TILOS-AI-Institute/HypergraphPartitioning/blob/main/solutions_2way/ISPD_benchmark_solutions/hMetis_SpecPart/UBfactor_5/ibm10.hgr.k.2.UBfactor.5.eig_vecs.2.num_cycles.2.h_threshold.300.solver_iters.80.spec_iters.2.best_solns.5.seed.0)     |      [1244](https://github.com/TILOS-AI-Institute/HypergraphPartitioning/blob/main/solutions_2way/ISPD_benchmark_solutions/hMetis_SpecPart/UBfactor_10/ibm10.hgr.k.2.UBfactor.10.eig_vecs.2.num_cycles.2.h_threshold.300.solver_iters.80.spec_iters.2.best_solns.5.seed.0)     |
|     IBM11    |    70558   |     81454    |      [1065](https://github.com/TILOS-AI-Institute/HypergraphPartitioning/blob/main/solutions_2way/ISPD_benchmark_solutions/hMetis_SpecPart/UBfactor_1/ibm11.hgr.k.2.UBfactor.1.eig_vecs.2.num_cycles.2.h_threshold.300.solver_iters.80.spec_iters.2.best_solns.5.seed.0)     |     [1051](https://github.com/TILOS-AI-Institute/HypergraphPartitioning/blob/main/solutions_2way/ISPD_benchmark_solutions/hMetis_SpecPart/UBfactor_2/ibm11.hgr.k.2.UBfactor.2.eig_vecs.2.num_cycles.2.h_threshold.300.solver_iters.80.spec_iters.2.best_solns.5.seed.0)     |      [951](https://github.com/TILOS-AI-Institute/HypergraphPartitioning/blob/main/solutions_2way/ISPD_benchmark_solutions/hMetis_SpecPart/UBfactor_5/ibm11.hgr.k.2.UBfactor.5.eig_vecs.2.num_cycles.2.h_threshold.300.solver_iters.80.spec_iters.2.best_solns.5.seed.0)     |      [763](https://github.com/TILOS-AI-Institute/HypergraphPartitioning/blob/main/solutions_2way/ISPD_benchmark_solutions/hMetis_SpecPart/UBfactor_10/ibm11.hgr.k.2.UBfactor.10.eig_vecs.2.num_cycles.2.h_threshold.300.solver_iters.80.spec_iters.2.best_solns.5.seed.0)      |
|     IBM12    |    71076   |     77240    |      [1934](https://github.com/TILOS-AI-Institute/HypergraphPartitioning/blob/main/solutions_2way/ISPD_benchmark_solutions/hMetis_SpecPart/UBfactor_1/ibm12.hgr.k.2.UBfactor.1.eig_vecs.2.num_cycles.2.h_threshold.300.solver_iters.80.spec_iters.2.best_solns.5.seed.0)     |     [1919](https://github.com/TILOS-AI-Institute/HypergraphPartitioning/blob/main/solutions_2way/ISPD_benchmark_solutions/hMetis_SpecPart/UBfactor_2/ibm12.hgr.k.2.UBfactor.2.eig_vecs.2.num_cycles.2.h_threshold.300.solver_iters.80.spec_iters.2.best_solns.5.seed.0)     |     [1871](https://github.com/TILOS-AI-Institute/HypergraphPartitioning/blob/main/solutions_2way/ISPD_benchmark_solutions/hMetis_SpecPart/UBfactor_5/ibm12.hgr.k.2.UBfactor.5.eig_vecs.2.num_cycles.2.h_threshold.300.solver_iters.80.spec_iters.2.best_solns.5.seed.0)     |      [1841](https://github.com/TILOS-AI-Institute/HypergraphPartitioning/blob/main/solutions_2way/ISPD_benchmark_solutions/hMetis_SpecPart/UBfactor_10/ibm12.hgr.k.2.UBfactor.10.eig_vecs.2.num_cycles.2.h_threshold.300.solver_iters.80.spec_iters.2.best_solns.5.seed.0)     |
|     IBM13    |    84199   |     99666    |      [837](https://github.com/TILOS-AI-Institute/HypergraphPartitioning/blob/main/solutions_2way/ISPD_benchmark_solutions/hMetis_SpecPart/UBfactor_1/ibm13.hgr.k.2.UBfactor.1.eig_vecs.2.num_cycles.2.h_threshold.300.solver_iters.80.spec_iters.2.best_solns.5.seed.0)      |      [831](https://github.com/TILOS-AI-Institute/HypergraphPartitioning/blob/main/solutions_2way/ISPD_benchmark_solutions/hMetis_SpecPart/UBfactor_2/ibm13.hgr.k.2.UBfactor.2.eig_vecs.2.num_cycles.2.h_threshold.300.solver_iters.80.spec_iters.2.best_solns.5.seed.0)     |      [831](https://github.com/TILOS-AI-Institute/HypergraphPartitioning/blob/main/solutions_2way/ISPD_benchmark_solutions/hMetis_SpecPart/UBfactor_5/ibm13.hgr.k.2.UBfactor.5.eig_vecs.2.num_cycles.2.h_threshold.300.solver_iters.80.spec_iters.2.best_solns.5.seed.0)     |      [655](https://github.com/TILOS-AI-Institute/HypergraphPartitioning/blob/main/solutions_2way/ISPD_benchmark_solutions/hMetis_SpecPart/UBfactor_10/ibm13.hgr.k.2.UBfactor.10.eig_vecs.2.num_cycles.2.h_threshold.300.solver_iters.80.spec_iters.2.best_solns.5.seed.0)      |
|     IBM14    |   147605   |    152772    |      [1842](https://github.com/TILOS-AI-Institute/HypergraphPartitioning/blob/main/solutions_2way/ISPD_benchmark_solutions/hMetis_SpecPart/UBfactor_1/ibm14.hgr.k.2.UBfactor.1.eig_vecs.2.num_cycles.2.h_threshold.300.solver_iters.80.spec_iters.2.best_solns.5.seed.0)     |     [1842](https://github.com/TILOS-AI-Institute/HypergraphPartitioning/blob/main/solutions_2way/ISPD_benchmark_solutions/hMetis_SpecPart/UBfactor_2/ibm14.hgr.k.2.UBfactor.2.eig_vecs.2.num_cycles.2.h_threshold.300.solver_iters.80.spec_iters.2.best_solns.5.seed.0)     |     [1794](https://github.com/TILOS-AI-Institute/HypergraphPartitioning/blob/main/solutions_2way/ISPD_benchmark_solutions/hMetis_SpecPart/UBfactor_5/ibm14.hgr.k.2.UBfactor.5.eig_vecs.2.num_cycles.2.h_threshold.300.solver_iters.80.spec_iters.2.best_solns.5.seed.0)     |      [1509](https://github.com/TILOS-AI-Institute/HypergraphPartitioning/blob/main/solutions_2way/ISPD_benchmark_solutions/hMetis_SpecPart/UBfactor_10/ibm14.hgr.k.2.UBfactor.10.eig_vecs.2.num_cycles.2.h_threshold.300.solver_iters.80.spec_iters.2.best_solns.5.seed.0)     |
|     IBM15    |   161570   |    186608    |      [2743](https://github.com/TILOS-AI-Institute/HypergraphPartitioning/blob/main/solutions_2way/ISPD_benchmark_solutions/hMetis_SpecPart/UBfactor_1/ibm15.hgr.k.2.UBfactor.1.eig_vecs.2.num_cycles.2.h_threshold.300.solver_iters.80.spec_iters.2.best_solns.5.seed.0)     |     [2730](https://github.com/TILOS-AI-Institute/HypergraphPartitioning/blob/main/solutions/ISPD_weight_benchmark_solutions/hMetis_SpecPart/UBfactor_2/ibm15.hgr.k.2.UBfactor.2.eig_vecs.2.num_cycles.2.h_threshold.300.solver_iters.80.spec_iters.2.best_solns.5.seed.0)     |     [2530](https://github.com/TILOS-AI-Institute/HypergraphPartitioning/blob/main/solutions_2way/ISPD_benchmark_solutions/hMetis_SpecPart/UBfactor_5/ibm15.hgr.k.2.UBfactor.5.eig_vecs.2.num_cycles.2.h_threshold.300.solver_iters.80.spec_iters.2.best_solns.5.seed.0)     |      [2135](https://github.com/TILOS-AI-Institute/HypergraphPartitioning/blob/main/solutions_2way/ISPD_benchmark_solutions/hMetis_SpecPart/UBfactor_10/ibm15.hgr.k.2.UBfactor.10.eig_vecs.2.num_cycles.2.h_threshold.300.solver_iters.80.spec_iters.2.best_solns.5.seed.0)     |
|     IBM16    |   183484   |    190048    |      [1975](https://github.com/TILOS-AI-Institute/HypergraphPartitioning/blob/main/solutions_2way/ISPD_benchmark_solutions/hMetis_SpecPart/UBfactor_1/ibm16.hgr.k.2.UBfactor.1.eig_vecs.2.num_cycles.2.h_threshold.300.solver_iters.80.spec_iters.2.best_solns.5.seed.0)     |     [1827](https://github.com/TILOS-AI-Institute/HypergraphPartitioning/blob/main/solutions_2way/ISPD_benchmark_solutions/hMetis_SpecPart/UBfactor_2/ibm15.hgr.k.2.UBfactor.2.eig_vecs.2.num_cycles.2.h_threshold.300.solver_iters.80.spec_iters.2.best_solns.5.seed.0)     |     [1695](https://github.com/TILOS-AI-Institute/HypergraphPartitioning/blob/main/solutions_2way/ISPD_benchmark_solutions/hMetis_SpecPart/UBfactor_5/ibm16.hgr.k.2.UBfactor.5.eig_vecs.2.num_cycles.2.h_threshold.300.solver_iters.80.spec_iters.2.best_solns.5.seed.0)     |      [1619](https://github.com/TILOS-AI-Institute/HypergraphPartitioning/blob/main/solutions_2way/ISPD_benchmark_solutions/hMetis_SpecPart/UBfactor_10/ibm16.hgr.k.2.UBfactor.10.eig_vecs.2.num_cycles.2.h_threshold.300.solver_iters.80.spec_iters.2.best_solns.5.seed.0)     |
|     IBM17    |   185495   |    189581    |      [2314](https://github.com/TILOS-AI-Institute/HypergraphPartitioning/blob/main/solutions_2way/ISPD_benchmark_solutions/hMetis_SpecPart/UBfactor_1/ibm17.hgr.k.2.UBfactor.1.eig_vecs.2.num_cycles.2.h_threshold.300.solver_iters.80.spec_iters.2.best_solns.5.seed.0)     |     [2270](https://github.com/TILOS-AI-Institute/HypergraphPartitioning/blob/main/solutions_2way/ISPD_benchmark_solutions/hMetis_SpecPart/UBfactor_2/ibm17.hgr.k.2.UBfactor.2.eig_vecs.2.num_cycles.2.h_threshold.300.solver_iters.80.spec_iters.2.best_solns.5.seed.0)     |     [2186](https://github.com/TILOS-AI-Institute/HypergraphPartitioning/blob/main/solutions_2way/ISPD_benchmark_solutions/hMetis_SpecPart/UBfactor_5/ibm17.hgr.k.2.UBfactor.5.eig_vecs.2.num_cycles.2.h_threshold.300.solver_iters.80.spec_iters.2.best_solns.5.seed.0)     |      [1989](https://github.com/TILOS-AI-Institute/HypergraphPartitioning/blob/main/solutions_2way/ISPD_benchmark_solutions/hMetis_SpecPart/UBfactor_10/ibm17.hgr.k.2.UBfactor.10.eig_vecs.2.num_cycles.2.h_threshold.300.solver_iters.80.spec_iters.2.best_solns.5.seed.0)     |
|     IBM18    |   210613   |    201920    |      [1521](https://github.com/TILOS-AI-Institute/HypergraphPartitioning/blob/main/solutions_2way/ISPD_benchmark_solutions/hMetis_SpecPart/UBfactor_1/ibm18.hgr.k.2.UBfactor.1.eig_vecs.2.num_cycles.2.h_threshold.300.solver_iters.80.spec_iters.2.best_solns.5.seed.0)     |     [1521](https://github.com/TILOS-AI-Institute/HypergraphPartitioning/blob/main/solutions_2way/ISPD_benchmark_solutions/hMetis_SpecPart/UBfactor_2/ibm18.hgr.k.2.UBfactor.2.eig_vecs.2.num_cycles.2.h_threshold.300.solver_iters.80.spec_iters.2.best_solns.5.seed.0)     |     [1521](https://github.com/TILOS-AI-Institute/HypergraphPartitioning/blob/main/solutions_2way/ISPD_benchmark_solutions/hMetis_SpecPart/UBfactor_5/ibm18.hgr.k.2.UBfactor.5.eig_vecs.2.num_cycles.2.h_threshold.300.solver_iters.80.spec_iters.2.best_solns.5.seed.0)     |      [1520](https://github.com/TILOS-AI-Institute/HypergraphPartitioning/blob/main/solutions_2way/ISPD_benchmark_solutions/hMetis_SpecPart/UBfactor_10/ibm18.hgr.k.2.UBfactor.10.eig_vecs.2.num_cycles.2.h_threshold.300.solver_iters.80.spec_iters.2.best_solns.5.seed.0)     |
