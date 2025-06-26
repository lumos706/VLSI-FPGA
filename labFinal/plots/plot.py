import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns

# IEEE风格设置
plt.rcParams.update({
    'font.family': 'serif',
    'font.size': 7,
    'axes.titlesize': 7,  # 调整标题字体大小
    'axes.labelsize': 7,
    'legend.fontsize': 7,
    'xtick.labelsize': 6,
    'ytick.labelsize': 7,
    'figure.figsize': (3.25, 2.5),  # 单栏图
})

# 读取数据
df_base = pd.read_csv("results.csv")
df_impr = pd.read_csv("results2.csv")
df_plus30 = pd.read_csv("results3.csv")

df_base['Algorithm'] = 'Baseline'
df_impr['Algorithm'] = 'Improved'
df_plus30['Algorithm'] = 'Improved+30%W'

# 合并原算法和改进算法结果（Wmin版本）
df_all = pd.concat([df_base, df_impr], ignore_index=True)

# 转换 Memory 单位（KB->MB）
def convert_memory(mem_str):
    if isinstance(mem_str, str):
        if 'KB' in mem_str:
            return float(mem_str.replace('KB', '').strip()) / 1024
        elif 'MB' in mem_str:
            return float(mem_str.replace('MB', '').strip())
    return float(mem_str)

df_all['peak_memory_MB'] = df_all['peak_memory'].apply(convert_memory)
df_plus30['peak_memory_MB'] = df_plus30['peak_memory'].apply(convert_memory)

# 设定8个数据集顺序，保证绘图时顺序固定
dataset_order = ["tiny", "small_dense", "med_sparse", "med_dense",
                 "lg_sparse", "large_dense", "xl", "huge"]

# -------------------
# 图1：Speedup vs Threads (按数据集分颜色、算法分样式)
# -------------------
plt.figure()
sns.lineplot(data=df_all, x="threads", y="acceleration_ratio",
             hue="test_case", style="Algorithm",
             markers=True, dashes=False,
             palette="tab10",  # 10色调色盘够用
             hue_order=dataset_order)
plt.xlabel("Threads")
plt.ylabel("Speedup")
plt.title("Speedup vs Threads by Dataset and Algorithm", loc='center')  # 标题居中
plt.grid(True, linestyle='--', alpha=0.3)
plt.legend(bbox_to_anchor=(1.05, 1), loc='upper left', borderaxespad=0)
plt.tight_layout()
plt.savefig("speedup_plot.png", dpi=300)

# -------------------
# 图2：Memory vs Threads
# -------------------
plt.figure()
sns.lineplot(data=df_all, x="threads", y="peak_memory_MB",
             hue="test_case", style="Algorithm",
             markers=True, dashes=False,
             palette="tab10",
             hue_order=dataset_order)
plt.xlabel("Threads")
plt.ylabel("Peak Memory (MB)")
plt.title("Memory Usage vs Threads by Dataset / Algorithm", loc='center')  # 标题居中
plt.grid(True, linestyle='--', alpha=0.3)
plt.legend(bbox_to_anchor=(1.05, 1), loc='upper left', borderaxespad=0)
plt.tight_layout()
plt.savefig("memory_usage_plot.png", dpi=300)

# -------------------
# 图3：Segments vs Width (改进算法单线程Wmin & Wmin+30%)
# -------------------
# 筛选改进算法单线程数据和Wmin+30%数据
seg_base = df_impr[(df_impr["threads"] == 1) & (df_impr["test_case"].isin(df_plus30["test_case"]))]
seg_base = seg_base[["test_case", "segments_used"]].rename(columns={"segments_used": "Wmin"})
seg_30 = df_plus30[["test_case", "segments_used"]].rename(columns={"segments_used": "Wmin+30%"})

df_segments = pd.merge(seg_base, seg_30, on="test_case")
df_segments = pd.melt(df_segments, id_vars=["test_case"], value_vars=["Wmin", "Wmin+30%"],
                      var_name="Width", value_name="Segments")

# 设定顺序
df_segments['test_case'] = pd.Categorical(df_segments['test_case'], categories=dataset_order, ordered=True)

# 需要显示的测试集列表
selected_datasets = ["small_dense", "med_dense", "large_dense", "xl", "huge"]

# 过滤数据，只保留这5个测试集的数据
df_segments_filtered = df_segments[df_segments["test_case"].isin(selected_datasets)].copy()

# 设置类别顺序（顺序固定）
df_segments_filtered['test_case'] = pd.Categorical(df_segments_filtered['test_case'], categories=selected_datasets, ordered=True)

plt.figure()
sns.barplot(data=df_segments_filtered, x="test_case", y="Segments", hue="Width",
            palette="pastel", order=selected_datasets)

plt.xticks(rotation=45)  # 适当旋转，避免标签拥挤
plt.yscale('log')
plt.xlabel("Test Case")
plt.ylabel("Segments Used (log scale)")
plt.title("Segments Used under Wmin and Wmin+30% Widths", loc='center')
plt.grid(True, linestyle='--', alpha=0.3)
plt.legend(title=None, bbox_to_anchor=(1.05, 1), loc='upper left')
plt.tight_layout()
plt.savefig("segments_vs_width_log.png", dpi=300)