import matplotlib.pyplot as plt
import pandas as pd
import numpy as np
from matplotlib.patches import Patch
import matplotlib.lines as mlines
from csv_process import write_csv_from_txt, process_csv

markersize = 5
linewidth = 1.5
font_size = 12


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
    "CComp-INLJ": {"color": colors[4], "marker": "H"},
}

# 手动设置的参数
bar_width = 0.01  # 条形图的宽度
group_gap = 0.02  # 组之间的间隔

# 准备绘图
# fig, (ax) = plt.subplots(1, figsize=(4, 3))
fig, axes = plt.subplots(1, 2, figsize=(8, 3))  # 一行两列
ax = axes[0]

cache_size["cache_size_MB"] = cache_size["cache_size"] / (2**20)

unique_cache_size = sorted(cache_size["cache_size_MB"].unique())
unique_labels = sorted(cache_size["label"].unique())
hatches = ["//", "\\\\", "xx", ".."]

# 计算每个组的起始位置
group_width = len(unique_labels) * bar_width
start_pos = np.arange(len(unique_cache_size)) * (group_width + group_gap)

# 设置x轴的刻度和标签
x_ticks = [
    i * (len(unique_labels) * bar_width + group_gap)
    + (len(unique_labels) - 2) * bar_width
    for i in range(len(unique_cache_size))
]

for i, cs in enumerate(unique_cache_size):
    for j, label in enumerate(unique_labels):
        sum_join_time = cache_size[
            (cache_size["label"] == label) & (cache_size["cache_size_MB"] == cs)
        ]["sum_join_time"].values

        marker = label_settings[label]["marker"]
        color = label_settings[label]["color"]

        x_pos = i * (len(unique_labels) * bar_width + group_gap) + j * bar_width

        ax.bar(
            x_pos,
            sum_join_time,
            width=bar_width,
            color=color,
            fill=False,
            hatch=hatches[j],
            edgecolor=color,
            linewidth=linewidth,
        )


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
            linewidth=linewidth,
            linestyle="-" if "Eager" in label else "--",
            markeredgewidth=linewidth,
            markersize=markersize,
            color=color,
        )  # 绘制曲线

ax.set_xticks(x_ticks)
ax.set_xticklabels(["0M", "16M", "32M", "64M"])

ax.set_xlabel("Cache Size", fontsize=font_size, fontweight="bold")
ax.set_ylabel("Join Latency (s)", fontsize=font_size, fontweight="bold")
ax2.set_ylabel("Cache Hit Rate (%)", fontsize=font_size, fontweight="bold")

legend_handles = []

legend_handles2 = []
i = 0
for label in label_settings:
    legend_handles.append(
        Patch(
            color=label_settings[label]["color"],
            hatch=hatches[i],
            fill=False,
            label=label,
            linewidth=linewidth,
        )
    )
    i += 1
    legend_handles2.append(
        mlines.Line2D(
            [],
            [],
            color=label_settings[label]["color"],
            marker=label_settings[label]["marker"],
            markersize=markersize,
            fillstyle="none",
            linewidth=linewidth,
            label=label,
            linestyle="--",
        ),
    )

# fig.text(0.4, 1.08, "Cache Hit Rate", ha="center", va="center", fontsize=6)
fig.legend(
    handles=legend_handles2,
    fontsize=font_size - 1,
    ncol=2,
    bbox_to_anchor=(0.46, 1.23),
    title="Rate (%)",
    # columnspacing=0.6,
    title_fontsize=font_size - 1,
    edgecolor="black",
    # handletextpad=0.5,
    # frameon=False,
)

# fig.text(0.6, 1.08, "Latency", ha="center", va="center", fontsize=6)
fig.legend(
    handles=legend_handles,
    fontsize=font_size - 1,
    ncol=2,
    bbox_to_anchor=(0.95, 1.23),
    title="Latency (s)",
    # columnspacing=0.6,
    title_fontsize=font_size - 1,
    # handletextpad=0.5,
    edgecolor="black",
    # frameon=False,
)

test_names = ["bpk"]
for test_name in test_names:
    write_csv_from_txt(test_name)
    process_csv(test_name)

bpk = pd.read_csv("lsm_join/csv_result/bpk.csv")

style = "tab10"
plt.set_cmap(style)
cmap = plt.get_cmap(style)
colors = cmap.colors
darkening_factor = 0.5
colors = [
    (r * darkening_factor, g * darkening_factor, b * darkening_factor)
    for r, g, b in colors
]


# 准备绘图
# fig, (ax) = plt.subplots(figsize=(4, 3))
ax = axes[1]

bar_width = 0.01
group_gap = 0.02

unique_bpk = sorted(bpk["bpk"].unique())
unique_labels = sorted(bpk["label"].unique())
group_width = len(unique_labels) * bar_width
start_pos = np.arange(len(unique_bpk)) * (group_width + group_gap)
x_ticks = [
    i * (len(unique_labels) * bar_width + group_gap)
    + (len(unique_labels) - 2) * bar_width
    for i in range(len(unique_bpk))
]

for i, label in enumerate(unique_labels):
    false_positive_rate = (
        bpk[(bpk["label"] == label)]["false_positive_rate"].values * 100
    )

    marker = label_settings[label]["marker"]
    color = label_settings[label]["color"]

    ax.plot(
        x_ticks,
        false_positive_rate,
        marker=marker,
        fillstyle="none",
        linewidth=linewidth,
        markeredgewidth=linewidth,
        markersize=markersize,
        linestyle="-" if "Eager" in label else "--",
        color=color,
    )

ax2 = ax.twinx()

for i, b in enumerate(unique_bpk):
    for j, label in enumerate(unique_labels):
        sum_join_time = bpk[(bpk["bpk"] == b) & (bpk["label"] == label)][
            "sum_join_time"
        ].values
        color = label_settings[label]["color"]
        x_pos = i * (len(unique_labels) * bar_width + group_gap) + j * bar_width
        ax2.bar(
            x_pos,
            sum_join_time,
            width=bar_width,
            color=color,
            fill=False,
            hatch=hatches[j],
            edgecolor=color,
            linewidth=linewidth,
        )

ax2.set_xticks(x_ticks)
ax2.set_xticklabels(unique_bpk)

ax.set_xlabel("Bits per Key", fontsize=font_size, fontweight="bold")
ax.set_ylabel("Join Latency (s)", fontsize=font_size, fontweight="bold")
ax2.set_ylabel("False Positive Rate (%)", fontsize=font_size, fontweight="bold")
plt.tight_layout()
plt.savefig("lsm_join/plot/cache_bpk.pdf", bbox_inches="tight", pad_inches=0.02)
plt.close()
