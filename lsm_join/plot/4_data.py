import matplotlib.pyplot as plt
import pandas as pd
import numpy as np
from matplotlib.patches import Patch
from csv_process import write_csv_from_txt, process_csv
from matplotlib.ticker import MaxNLocator
import sci_palettes
import matplotlib as mpl

sci_palettes.register_cmap()

test_names = ["num_loop", "dataset_size", "dataratio", "c", "k", "skewness"]

for test_name in test_names:
    write_csv_from_txt(test_name)
    process_csv(test_name)

num_loop = pd.read_csv("lsm_join/csv_result/num_loop.csv")
dataset_size = pd.read_csv("lsm_join/csv_result/dataset_size.csv")
data_ratio = pd.read_csv("lsm_join/csv_result/dataratio.csv")
c_r = pd.read_csv("lsm_join/csv_result/c.csv")
k_r = pd.read_csv("lsm_join/csv_result/k.csv")
skewness = pd.read_csv("lsm_join/csv_result/skewness.csv")

tests = [[num_loop, dataset_size, data_ratio], [c_r, k_r, skewness]]
attributes = [["num_loop", "r_tuples", "dataratio"], ["c_r", "k_r", "k_s"]]
titles = [["Loops", "Dataset Size", "Data Ratio"], ["c", "k", "Skewness"]]

# 手动设置的参数
bar_width = 0.01  # 条形图的宽度
group_gap = 0.02  # 组之间的间隔
edgewidth = 0.9
mpl.rcParams["hatch.linewidth"] = edgewidth

colors = ["#808080", "#ffa600", "#9AC9DB", "#003f5c", "#bc5090", "#800000"]
style = "tab10"
plt.set_cmap(style)
cmap = plt.get_cmap(style)
colors = cmap.colors
darkening_factor = 0.5
colors = [
    (r * darkening_factor, g * darkening_factor, b * darkening_factor)
    for r, g, b in colors
]
hatches = ["//", "\\\\", "xx", "..", "**", "++"]

style_dict = {
    "2CLazy-ISJ": {"color": colors[0], "hatch": hatches[0]},
    "2Eager-ISJ": {"color": colors[1], "hatch": hatches[1]},
    "CLazy-INLJ": {"color": colors[2], "hatch": hatches[2]},
    "Comp-INLJ": {"color": colors[3], "hatch": hatches[3]},
    "Grace-HJ": {"color": colors[4], "hatch": hatches[4]},
    "P-INLJ": {"color": colors[7], "hatch": hatches[5]},
}

# 准备绘图
fig, axss = plt.subplots(2, 3, figsize=(12, 5))

dataset = ["Unif", "User", "Unif", "Unif", "Unif", "Skew"]
alphabet = "abcdef"
count = 0

for axs, test, attribute, title in zip(axss, tests, attributes, titles):
    for t, ax, attr, ti in zip(test, axs, attribute, title):
        if attr == "r_tuples":
            # to Million, convert to int
            t[attr] = t[attr] // 1000000

        unique_attributes = sorted(t[attr].unique())
        unique_labels = sorted(t["label"].unique())

        # 计算每个组的起始位置
        group_width = len(unique_labels) * bar_width
        start_pos = np.arange(len(unique_attributes)) * (group_width + group_gap)

        # 设置x轴的刻度和标签
        x_ticks = [
            i * (len(unique_labels) * bar_width + group_gap)
            + (len(unique_labels) - 3.5) * bar_width
            for i in range(len(unique_attributes))
        ]

        for i, at in enumerate(unique_attributes):
            for j, label in enumerate(unique_labels):
                sum_join_time = t[(t["label"] == label) & (t[attr] == at)][
                    "sum_join_time"
                ].values
                index_build_time = t[(t["label"] == label) & (t[attr] == at)][
                    "sum_index_build_time"
                ].values

                # 计算条形图的位置
                x_pos = i * (len(unique_labels) * bar_width + group_gap) + j * bar_width

                color = style_dict[label]["color"]
                hatch = style_dict[label]["hatch"]

                # 绘制条形图
                ax.bar(
                    x_pos,
                    index_build_time,
                    width=bar_width,
                    color=color,
                    edgecolor=color,
                    fill=False,
                    linewidth=edgewidth,
                    bottom=sum_join_time,
                    hatch=hatch,
                )
                ax.bar(
                    x_pos,
                    sum_join_time,
                    width=bar_width,
                    color=color,
                    edgecolor="black",
                    linewidth=edgewidth,
                    hatch=hatch,
                )

        ax.set_xticks(x_ticks)
        if attr == "r_tuples":
            ax.set_xticklabels([f"{at}M" for at in unique_attributes])
        else:
            ax.set_xticklabels(unique_attributes)
        font_size = 9
        ax.set_xlabel(
            f"({alphabet[count]}) " + ti + f" (on {dataset[count]})",
            fontsize=font_size,
            fontweight="bold",
        )
        # if ti == "k":
        #     ax.set_xlabel(
        #         f"({alphabet[count]}) " + "skewness" + f" (on {dataset[count]})",
        #         fontsize=font_size,
        #     )
        count += 1
        # only set y label for the first plot
        if ti == "Loops" or ti == "c":
            ax.set_ylabel("System Latency (s)", fontsize=font_size, fontweight="bold")


legend_handles = [
    Patch(
        edgecolor="black",
        linewidth=edgewidth,
        label=label,
        facecolor=style_dict[label]["color"],
        hatch=style_dict[label]["hatch"],
    )
    for label in unique_labels
]

legend_handles2 = [
    Patch(
        facecolor="grey",
        linewidth=edgewidth,
        label="Join",
        hatch="//",
        edgecolor="black",
    ),
    Patch(
        color="black",
        linewidth=edgewidth,
        label="Index build",
        fill=False,
        hatch=hatches[0],
    ),
]

fig.legend(handles=legend_handles, fontsize=8, ncol=6, bbox_to_anchor=(0.70, 1.05))
fig.legend(handles=legend_handles2, fontsize=8, ncol=2, bbox_to_anchor=(0.88, 1.05))

plt.tight_layout()
plt.savefig("lsm_join/lsm_plot/4_data.pdf", bbox_inches="tight", pad_inches=0.02)
plt.close()
