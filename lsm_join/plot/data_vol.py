import matplotlib.pyplot as plt
import pandas as pd
import matplotlib.lines as mlines
from csv_process import write_csv_from_txt, process_csv
import sci_palettes
import numpy as np
from scipy.spatial.distance import pdist, squareform

fontsize = 12
edgewidth = 1.5
markersize = 5
sci_palettes.register_cmap()
test_names = ["num_loop1", "num_loop2"]
for test_name in test_names:
    write_csv_from_txt(test_name)
    process_csv(test_name)

num_loop1 = pd.read_csv("lsm_join/csv_result/num_loop1.csv")
num_loop2 = pd.read_csv("lsm_join/csv_result/num_loop2.csv")

# 创建数据框的数组
dfs = [{"df": num_loop1, "title": "num_loop"}, {"df": num_loop2, "title": "num_loop"}]
attribute = "num_loop"
# 设置图的大小和子图布局
fig, axes = plt.subplots(1, 4, figsize=(15, 3))

# colors = ["#3E8D27", "#A22025", "#1432F5"]
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
    "2Eager-ISJ": {"color": colors[0], "marker": "d"},
    "2CLazy-ISJ": {"color": colors[1], "marker": "H"},
    "Comp-INLJ": {"color": colors[2], "marker": "s"},
    "Grace-HJ": {"color": colors[3], "marker": "^"},
    "P-INLJ": {"color": colors[4], "marker": "p"},
}


def cumulative_sum(lst):
    result = []
    current_sum = 0
    for number in lst:
        current_sum += number
        result.append(current_sum)
    return result


def sort_label(col, df):
    label_means = {}
    for label, group in df.groupby("label"):
        join_time_list = ",".join(map(str, group[col]))
        join_time_list = [float(x) for x in join_time_list.strip("[]").split(",")]
        print(join_time_list)
        label_means[label] = np.mean(join_time_list)

    sorted_labels = sorted(label_means, key=lambda x: label_means[x], reverse=True)
    return sorted_labels


def subplot(sorted_labels, col, ax, df):
    for label in sorted_labels:
        group = df[df["label"] == label]
        color = label_settings[label]["color"]
        marker = label_settings[label]["marker"]
        join_time_list = ",".join(map(str, group[col]))
        join_time_list = [float(x) for x in join_time_list.strip("[]").split(",")]
        join_time_list = cumulative_sum(join_time_list)
        xs = [i + 1 for i in range(group[attribute].values[0])]
        ax.plot(
            xs,
            join_time_list,
            marker=marker,
            fillstyle="none",
            color=color,
            linewidth=edgewidth,
            markeredgewidth=edgewidth,
            label=label,
            markersize=markersize,
        )
        factor = 2
        light_color = (
            min(1, color[0] * factor),
            min(1, color[1] * factor),
            min(1, color[2] * factor),
        )
        ax.fill_between(xs, 0, join_time_list, color=light_color)
        ax.plot(
            [xs[-1], xs[-1]], [0, join_time_list[-1]], color=color, linewidth=edgewidth
        )
        ax.plot(
            [xs[0], xs[0]], [0, join_time_list[0]], color=color, linewidth=edgewidth
        )


sorted_labels = sort_label("join_time_list", dfs[0]["df"])
subplot(sorted_labels, "join_time_list", axes[0], dfs[0]["df"])
axes[0].set_ylabel("Cumulative Latency (s)", fontweight="bold", fontsize=fontsize)
axes[0].set_xlabel("Number of Updated Partitions", fontsize=fontsize - 2)
axes[0].text(
    0.5,
    -0.3,
    "Join Latency on Unif",
    transform=axes[0].transAxes,
    ha="center",
    va="top",
    fontweight="bold",
    fontsize=fontsize,
)


sorted_labels = sort_label("index_build_time_list", dfs[0]["df"])
subplot(sorted_labels, "index_build_time_list", axes[1], dfs[0]["df"])
axes[1].set_xlabel("Number of Updated Partitions", fontsize=fontsize)
axes[1].text(
    0.5,
    -0.3,
    "Index Build Latency on Unif",
    transform=axes[1].transAxes,
    ha="center",
    va="top",
    fontweight="bold",
    fontsize=fontsize,
)

sorted_labels = sort_label("join_time_list", dfs[1]["df"])
subplot(sorted_labels, "join_time_list", axes[2], dfs[1]["df"])
axes[2].set_xlabel("Number of Updated Partitions", fontsize=fontsize)
axes[2].text(
    0.5,
    -0.3,
    "Join Latency on Skew",
    transform=axes[2].transAxes,
    ha="center",
    va="top",
    fontweight="bold",
    fontsize=fontsize,
)

sorted_labels = sort_label("index_build_time_list", dfs[1]["df"])
subplot(sorted_labels, "index_build_time_list", axes[3], dfs[1]["df"])
axes[3].set_xlabel("Number of Updated Partitions", fontsize=fontsize)
axes[3].text(
    0.5,
    -0.3,
    "Index Build Latency on Skew",
    transform=axes[3].transAxes,
    ha="center",
    va="top",
    fontweight="bold",
    fontsize=fontsize,
)

legend_handles2 = []
for label, setting in label_settings.items():
    legend_handles2.append(
        mlines.Line2D(
            [],
            [],
            color=setting["color"],
            marker=setting["marker"],
            linestyle="-",
            markersize=markersize,
            fillstyle="none",
            label=label,
        )
    )

fig.legend(
    handles=legend_handles2,
    bbox_to_anchor=(0.75, 1.1),
    ncol=7,
    fontsize=fontsize - 1,
    edgecolor="black",
)


plt.subplots_adjust(wspace=0.01, hspace=0.1)
plt.tight_layout()
plt.savefig("lsm_join/plot/data_vol.pdf", bbox_inches="tight", pad_inches=0.02)
plt.close()
