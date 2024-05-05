import matplotlib.pyplot as plt
import pandas as pd
import numpy as np
from matplotlib.patches import Patch
import matplotlib.lines as mlines
from csv_process import write_csv_from_txt, process_csv

test_names = ["cache_size"]
for test_name in test_names:
    write_csv_from_txt(test_name)
    process_csv(test_name)

cache_size = pd.read_csv("lsm_join/csv_result/cache_size.csv")

style = "tab10"
plt.set_cmap(style)
cmap = plt.get_cmap(style)
colors = cmap.colors
darkening_factor = 0.5
colors = [
    (r * darkening_factor, g * darkening_factor, b * darkening_factor)
    for r, g, b in colors
]

label_settings = {
    "CEager-INLJ": {"color": colors[0], "marker": "o"},
    "CLazy-INLJ": {"color": colors[1], "marker": "s"},
    "2CComp-ISJ": {"color": colors[2], "marker": "d"},
}

# 手动设置的参数
bar_width = 0.01  # 条形图的宽度
group_gap = 0.02   # 组之间的间隔

# 准备绘图
fig, (ax) = plt.subplots(1, figsize=(4, 3))

cache_size["cache_size_MB"] = cache_size["cache_size"] / (2**20)

unique_cache_size = sorted(cache_size["cache_size_MB"].unique())
unique_labels = sorted(cache_size["label"].unique())

# 计算每个组的起始位置
group_width = len(unique_labels) * bar_width
start_pos = np.arange(len(unique_cache_size)) * (group_width + group_gap)

# 设置x轴的刻度和标签
x_ticks = [i * (len(unique_labels) * bar_width + group_gap) + (len(unique_labels) - 2) * bar_width for i in range(len(unique_cache_size))]

for i, cs in enumerate(unique_cache_size):
    for j, label in enumerate(unique_labels): 
        sum_join_time = cache_size[(cache_size["label"] == label) & (cache_size["cache_size_MB"] == cs)]["sum_join_time"].values
        
        marker = label_settings[label]["marker"]
        color = label_settings[label]["color"]

        x_pos = i * (len(unique_labels) * bar_width + group_gap) + j * bar_width
        
        ax.bar(x_pos, sum_join_time, width=bar_width, color=color, fill=False, hatch='///', edgecolor=color, linewidth=0.5)    


# 新的代码来绘制曲线
ax2 = ax.twinx()  # 创建第二个y轴

for i, b in enumerate(unique_cache_size):
    for j, label in enumerate(unique_labels):
        ch = cache_size[(cache_size["label"] == label)]["cache_hit_rate"].values

        marker = label_settings[label]["marker"]
        color = label_settings[label]["color"]

        # 绘制条形图
        ax2.plot(
            x_ticks,
            ch * 100,
            marker=marker,
            fillstyle="none",
            linewidth=1.5,
            markeredgewidth=1.5,
            markersize=5,
            color=color,
        )  # 绘制曲线

ax.set_xticks(x_ticks)
ax.set_xticklabels(["0M", "16M", "32M", "64M"])

# set y max
# ax2.set_ylim(-20, 100)
# ax.set_ylim(-10, 1000)

font_size = 9
ax.set_xlabel("cache size", fontsize=font_size, fontweight="bold")
ax.set_ylabel("Join Latency (s)", fontsize=font_size, fontweight="bold")
ax2.set_ylabel("Cache Hit Rate (%)", fontsize=font_size, fontweight="bold")

legend_handles = []

legend_handles2 = []

for label in unique_labels:
    legend_handles.append(
        Patch(color=label_settings[label]["color"], hatch='/////', fill=False, label=label)
    )
    legend_handles2.append(
        mlines.Line2D(
        [],
        [],
        color=label_settings[label]["color"],
        marker=label_settings[label]["marker"],
        linestyle="None",
        markersize=4,
        fillstyle="none",
        label=label,
    ),
    )
    
fig.text(0.4, 1.08, 'Cache Hit Rate', ha='center', va='center', fontsize=6)
fig.legend(
    handles=legend_handles2,
    fontsize=6,
    ncol=1,
    bbox_to_anchor=(0.5, 1.08),
    frameon=False,
)

fig.text(0.6, 1.08, 'Join Latency', ha='center', va='center', fontsize=6)
fig.legend(
    handles=legend_handles,
    fontsize=6,
    ncol=1,
    bbox_to_anchor=(0.7, 1.08),
    frameon=False,
)

plt.tight_layout()
plt.savefig("lsm_join/lsm_plot/7_cachesize.pdf", bbox_inches="tight", pad_inches=0.02)
plt.close()
