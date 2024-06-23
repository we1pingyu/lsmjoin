import matplotlib.pyplot as plt
import pandas as pd
from csv_process import write_overall_csv, process_csv
import sci_palettes
from matplotlib.patches import Patch
from matplotlib.ticker import MaxNLocator
import matplotlib as mpl
import matplotlib.gridspec as gridspec
import numpy as np
from brokenaxes import brokenaxes

mpl.rcParams["font.family"] = "Times New Roman"
mpl.use("Agg")
sci_palettes.register_cmap()

test_name = "overall"
write_overall_csv()
process_csv(test_name)
# read df
df = pd.read_csv("lsm_join/csv_result/overall.csv")
# pair info
fontsize = 16
pairs1 = [
    ["NL-P"],
    ["SJ-P"],
    ["SJ-PS/V-EI", "SJ-PS/V-LI", "SJ-PS/V-CI"],
    ["SJ-PS/S-EI", "SJ-PS/S-LI", "SJ-PS/S-CI"],
    ["HJ-P"],
]
x_labels1 = ["NL-P", "SJ-P", "SJ-PS/V", "SJ-PS/S", "HJ-P"]

pairs2 = [
    ["NL-NS/V-EI", "NL-NS/V-LI", "NL-NS/V-CI"],
    ["NL-NS/S-EI", "NL-NS/S-LI", "NL-NS/S-CI"],
    ["HJ-N"],
    ["SJ-N"],
]
x_labels2 = ["NL-NS/V", "NL-NS/S", "HJ-N", "SJ-N"]

pairs3 = [
    ["SJ-NS/V-EI", "SJ-NS/V-LI", "SJ-NS/V-CI"],
    ["SJ-NS/S-EI", "SJ-NS/S-LI", "SJ-NS/S-CI"],
    ["SJ-SS/V-EI", "SJ-SS/V-LI", "SJ-SS/V-CI"],
    ["SJ-SS/S-EI", "SJ-SS/S-LI", "SJ-SS/S-CI"],
]
x_labels3 = ["SJ-NS/V", "SJ-NS/S", "SJ-SS/V", "SJ-SS/S"]

all_pairs = [pairs1, pairs2, pairs3]
all_x_lables = [x_labels1, x_labels2, x_labels3]

# 确定条形图的宽度和间隔
bar_width = 0.1  # 条形图的宽度
group_gap = 0.1  # 不同组之间的间隔
edgewidth = 1.2
mpl.rcParams["hatch.linewidth"] = edgewidth

# 颜色设置
colors = ["#003f5c", "#bc5090", "#ffa600", "#5F8670"]
style = "tab10"
plt.set_cmap(style)
cmap = plt.get_cmap(style)
colors = cmap.colors
darkening_factor = 0.5
colors = [
    (r * darkening_factor, g * darkening_factor, b * darkening_factor)
    for r, g, b in colors
]
hatches = ["//", "\\\\", "xx", ".."]

legend_patches = [
    Patch(
        facecolor=colors[0],
        label="Eager Index",
        edgecolor="black",
        linewidth=edgewidth,
        hatch=hatches[0],
    ),
    Patch(
        facecolor=colors[1],
        label="Lazy Index",
        edgecolor="black",
        linewidth=edgewidth,
        hatch=hatches[1],
    ),
    Patch(
        facecolor=colors[2],
        label="Composite Index",
        edgecolor="black",
        linewidth=edgewidth,
        hatch=hatches[2],
    ),
    Patch(
        facecolor=colors[3],
        label="Non-Index",
        edgecolor="black",
        linewidth=edgewidth,
        hatch=hatches[3],
    ),
]

# 分两行显示图表
datasets = df["dataset"].unique()
rows = 3
cols = len(datasets)  # 确保有足够的列来容纳所有的数据集


# plt.style.use('aaas')
# 初始化绘图，设置为两行
fig = plt.figure(figsize=(4 * cols, 4 * rows))
gs = gridspec.GridSpec(rows, cols, figure=fig)
# 存储每一行的最大y值
max_y_values = [0] * rows

for row, pairs, x_labels in zip(range(rows), all_pairs, all_x_lables):
    # 计算每个group的起始位置
    group_start_positions = [0]  # 第一个group从位置0开始

    for pair in pairs[:-1]:  # 跳过最后一个pair
        next_position = group_start_positions[-1] + len(pair) * bar_width + group_gap
        group_start_positions.append(next_position)

    for i, dataset in enumerate(datasets):
        col = i % cols
        # current_ax = (
        #     axes[row, col] if rows > 1 else axes[col]
        # )  # 当只有一行时直接索引 axes
        current_ax = fig.add_subplot(gs[row, col])
        # if row == 1:
        #     current_ax = brokenaxes(
        #         ylims=((0, 100), (400, 500)), subplot_spec=current_ax
        #     )
        positions = []
        x_sticks = []
        group_positions = []
        for j, pair in enumerate(pairs):
            start_position = group_start_positions[j]

            for k, label in enumerate(pair):

                position = start_position + k * bar_width

                join_time = (
                    df[(df["dataset"] == dataset) & (df["label"] == label)][
                        "sum_join_time"
                    ].values[0]
                    if label in df[df["dataset"] == dataset]["label"].values
                    else 0
                )
                index_build_time = (
                    df[(df["dataset"] == dataset) & (df["label"] == label)][
                        "sum_index_build_time"
                    ].values[0]
                    if label in df[df["dataset"] == dataset]["label"].values
                    else 0
                )

                color = colors[3]
                hatch = hatches[3]
                if "EI" in label:
                    color = colors[0]
                    hatch = hatches[0]
                elif "LI" in label:
                    color = colors[1]
                    hatch = hatches[1]
                elif "CI" in label:
                    color = colors[2]
                    hatch = hatches[2]

                current_ax.bar(
                    position,
                    index_build_time,
                    width=bar_width,
                    edgecolor=color,
                    # color=color,
                    fill=False,
                    hatch=hatch,
                    bottom=join_time,
                    linewidth=edgewidth,
                )
                current_ax.bar(
                    position,
                    join_time,
                    width=bar_width,
                    color=color,
                    edgecolor="black",
                    hatch=hatch,
                    linewidth=edgewidth,
                )
                positions.append(position)
                x_sticks.append(label)
                # current_ax.set_xticks(position, label)

            # 将当前组的中心位置添加到列表中
            group_positions.append(
                start_position + (len(pair) * bar_width) / 2 - bar_width / 2
            )

        # 设置 x 轴标签和标题
        current_ax.set_xticks(group_positions)
        current_ax.xaxis.set_tick_params(length=0)
        # current_ax.set_xticklabels(["" for _ in x_labels])
        # current_ax.set_xticklabels(x_sticks, rotation=60, fontsize=13)
        current_ax.set_xticklabels(x_labels, fontsize=fontsize - 2)
        current_ax.tick_params(axis="x", which="major", pad=5)
        current_ax.set_xlabel("")
        current_ax.set_ylabel(
            "System Latency (s)" if col == 0 else "",
            fontsize=fontsize + 1,
            fontweight="bold",
        )
        current_ax.yaxis.set_tick_params(labelsize=fontsize - 1)
        if dataset == "Skew":
            dataset = "Zipf"
        if row == 0:
            current_ax.set_title(dataset, fontsize=fontsize, fontweight="bold")
        max_y_values[row] = max(max_y_values[row], current_ax.get_ylim()[1])

# 设置统一的y轴范围，并隐藏非首图的y轴标记
for i in range(rows * cols):
    ax = fig.axes[i]
    row = i // cols
    ax.set_ylim(0, max_y_values[row])
    ax.yaxis.set_major_locator(MaxNLocator(5))
    if i % cols != 0:  # 如果不是每行的第一个图表
        ax.set_yticklabels([])

fig.legend(
    handles=legend_patches,
    bbox_to_anchor=(0.6, 0.98),
    ncol=4,
    fontsize=fontsize + 1,
    edgecolor="black",
)
legend_handles2 = [
    Patch(
        facecolor="grey",
        linewidth=edgewidth,
        label="Join",
        hatch="//",
        edgecolor="black",
    ),
    Patch(
        color="black", linewidth=edgewidth, label="Index build", fill=False, hatch="//"
    ),
]
fig.legend(
    handles=legend_handles2,
    fontsize=fontsize + 1,
    ncol=2,
    bbox_to_anchor=(0.8, 0.98),
    edgecolor="black",
)
# 调整布局
plt.subplots_adjust(top=0.9, wspace=0.03, hspace=0.1)
# plt.tight_layout()
plt.savefig("lsm_join/plot/overall.pdf", bbox_inches="tight", pad_inches=0.02)
plt.close()
