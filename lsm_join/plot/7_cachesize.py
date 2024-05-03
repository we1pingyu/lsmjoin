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

# 准备绘图
fig, (ax) = plt.subplots(1, figsize=(4, 3))

cache_size["cache_size_MB"] = cache_size["cache_size"] / (2**20)

unique_cache_size = sorted(cache_size["cache_size_MB"].unique())
unique_labels = sorted(cache_size["label"].unique())

for i, label in enumerate(unique_labels):
    sum_join_time = cache_size[(cache_size["label"] == label)]["sum_join_time"].values
    if "CComp" in label:
        marker = "o"
    else:
        marker = "s"

    ax.plot(
        unique_cache_size,
        sum_join_time,
        marker=marker,
        fillstyle="none",
        linewidth=0.5,
        markersize=4,
        color="black",
    )  # 绘制曲线


# 新的代码来绘制曲线
ax2 = ax.twinx()  # 创建第二个y轴

for i, b in enumerate(unique_cache_size):
    for j, label in enumerate(unique_labels):
        # 获取对应M值和标签的sum_join_time
        # sum_join_time = bpk[(bpk['bpk'] == b) & (bpk['label'] == label) ]['sum_join_time'].values
        # cache_hit_rate = buffer_size[(buffer_size['M_MB'] == M) & (buffer_size['label'] == label) & (buffer_size['theory'] == theory)]['cache_hit_rate'].values
        fp = cache_size[(cache_size["label"] == label)]["cache_hit_rate"].values

        if "CComp" in label:
            marker = "o"
        else:
            marker = "s"

        color = "black"
        # 绘制条形图
        ax2.plot(
            unique_cache_size,
            fp * 100,
            marker=marker,
            fillstyle="none",
            linewidth=0.5,
            markersize=4,
            color="black",
            linestyle="--",
        )  # 绘制曲线

ax.set_xticks(unique_cache_size)
ax.set_xticklabels(["0M", "16M", "32M", "64M"])

# set y max
# ax2.set_ylim(-20, 100)
# ax.set_ylim(-10, 1000)

font_size = 9
ax.set_xlabel("cache size", fontsize=font_size)
ax.set_ylabel("Join Latency (s)", fontsize=font_size)
ax2.set_ylabel("Cache Hit Rate (%)", fontsize=font_size)

legend_handles = [
    mlines.Line2D([], [], color="black", label="Join Latency", linewidth=0.5),
    mlines.Line2D(
        [], [], color="black", linestyle="--", label="Cache Hit Rate", linewidth=0.5
    ),
    mlines.Line2D(
        [],
        [],
        color="black",
        marker="o",
        linestyle="None",
        markersize=4,
        fillstyle="none",
        label="2CComp-ISJ",
    ),
    mlines.Line2D(
        [],
        [],
        color="black",
        marker="s",
        linestyle="None",
        markersize=4,
        fillstyle="none",
        label="P-INLJ",
    ),
]

fig.legend(
    handles=legend_handles,
    fontsize=6,
    ncol=2,
    bbox_to_anchor=(0.82, 0.93),
    frameon=False,
)

plt.tight_layout()
plt.savefig("lsm_join/lsm_plot/7_cachesize.pdf", bbox_inches="tight", pad_inches=0.02)
plt.close()
