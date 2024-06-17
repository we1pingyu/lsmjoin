import matplotlib.pyplot as plt
import pandas as pd
import numpy as np
from matplotlib.patches import Patch
import matplotlib.lines as mlines
from csv_process import write_csv_from_txt, process_csv

markersize = 5
linewidth = 1.2
fontsize = 12
groups = 4

test_names = ["cache_size"]
for test_name in test_names:
    write_csv_from_txt(test_name)
    process_csv(test_name)

cache_size = pd.read_csv("lsm_join/csv_result/cache_size.csv")

style = "tab10"
plt.set_cmap(style)
cmap = plt.get_cmap(style)
colors = cmap.colors
darkening_factor = 0.7
colors = [
    (r * darkening_factor, g * darkening_factor, b * darkening_factor)
    for r, g, b in colors
]


# 手动设置的参数
bar_width = 0.01  # 条形图的宽度
group_gap = 0.02  # 组之间的间隔

fig, axes = plt.subplots(1, 2, figsize=(8, 3))  # 一行两列
test_names = ["bpk"]
for test_name in test_names:
    write_csv_from_txt(test_name)
    process_csv(test_name)
bpk = pd.read_csv("lsm_join/csv_result/bpk.csv")
ax2 = axes[0]
label_settings = {
    "NL-NS/S-EI": {"color": colors[0], "marker": "o", "hatch": "//"},
    "NL-NS/S-LI": {"color": colors[1], "marker": "s", "hatch": "\\\\"},
    "NL-NS/S-CI": {"color": colors[2], "marker": "H", "hatch": "xx"},
    "NL-NS/V-LI": {"color": colors[3], "marker": "^", "hatch": ".."},
    # "SJ-SS/S-CI": {"color": colors[4], "marker": "d", "hatch": "++"},
    # "SJ-SS/V-EI": {"color": colors[5], "marker": "+", "hatch": "--"},
}
unique_bpk = sorted(bpk["bpk"].unique())
unique_labels = sorted(bpk["label"].unique())
group_width = len(unique_labels) * bar_width
start_pos = np.arange(len(unique_bpk)) * (group_width + group_gap)

x_ticks = [
    i * (len(unique_labels) * bar_width + group_gap)
    + (len(unique_labels) - 3.5) * bar_width
    for i in range(len(unique_bpk))
]

for i, b in enumerate(unique_bpk):
    for j, label in enumerate(unique_labels):
        if label not in label_settings:
            continue
        sum_join_time = bpk[(bpk["bpk"] == b) & (bpk["label"] == label)][
            "sum_join_time"
        ].values
        color = label_settings[label]["color"]
        hatch = label_settings[label]["hatch"]
        x_pos = i * (len(unique_labels) * bar_width + group_gap) + j * bar_width
        ax2.bar(
            x_pos,
            sum_join_time,
            width=bar_width,
            color=color,
            fill=False,
            hatch=hatch,
            edgecolor=color,
            linewidth=linewidth,
        )

ax3 = ax2.twinx()

for i, label in enumerate(unique_labels):
    if label not in label_settings:
        continue
    false_positive_rate = (
        bpk[(bpk["label"] == label)]["false_positive_rate"].values * 100
    )

    marker = label_settings[label]["marker"]
    color = label_settings[label]["color"]

    ax3.plot(
        x_ticks,
        false_positive_rate,
        marker=marker,
        fillstyle="none",
        linewidth=linewidth,
        markeredgewidth=linewidth,
        markersize=markersize,
        linestyle="-" if "EI" in label else "--",
        color=color,
    )

ax2.set_xticks(x_ticks)
ax2.set_xticklabels(unique_bpk)

ax2.set_xlabel("Bits per Key", fontsize=fontsize, fontweight="bold")
ax2.set_ylabel("Join Latency (s)", fontsize=fontsize, fontweight="bold", labelpad=-1)
ax3.set_ylabel("False Positive Rate (%)", fontsize=fontsize, fontweight="bold")

label_settings = {
    # "NL-NS/S-EI": {"color": colors[0], "marker": "o", hatch: "//"},
    "NL-NS/S-LI": {"color": colors[1], "marker": "s", "hatch": "\\\\"},
    "NL-NS/S-CI": {"color": colors[2], "marker": "H", "hatch": "xx"},
    "NL-NS/V-LI": {"color": colors[3], "marker": "^", "hatch": ".."},
    "SJ-SS/S-CI": {"color": colors[4], "marker": "d", "hatch": "++"},
    "SJ-SS/V-EI": {"color": colors[5], "marker": "+", "hatch": "--"},
}
ax = axes[1]
cache_size["cache_size_MB"] = cache_size["cache_size"] / (2**20)
unique_cache_size = sorted(cache_size["cache_size_MB"].unique())
unique_labels = sorted(cache_size["label"].unique())
x_ticks = [
    i * (len(unique_labels) * bar_width + group_gap)
    + (len(unique_labels) - 2.5) * bar_width
    for i in range(len(unique_cache_size))
]

for i, cs in enumerate(unique_cache_size):
    for j, label in enumerate(unique_labels):
        if label not in label_settings:
            continue
        sum_join_time = cache_size[
            (cache_size["label"] == label) & (cache_size["cache_size_MB"] == cs)
        ]["sum_join_time"].values

        marker = label_settings[label]["marker"]
        color = label_settings[label]["color"]
        hatch = label_settings[label]["hatch"]
        x_pos = i * (len(unique_labels) * bar_width + group_gap) + j * bar_width

        ax.bar(
            x_pos,
            sum_join_time,
            width=bar_width,
            color=color,
            fill=False,
            hatch=hatch,
            edgecolor=color,
            linewidth=linewidth,
        )
ax1 = ax.twinx()
for i, b in enumerate(unique_cache_size):
    for j, label in enumerate(unique_labels):
        if label not in label_settings:
            continue
        ch = cache_size[(cache_size["label"] == label)]["cache_hit_rate"].values
        marker = label_settings[label]["marker"]
        color = label_settings[label]["color"]
        ax1.plot(
            x_ticks,
            ch * 100,
            marker=marker,
            fillstyle="none",
            linewidth=linewidth,
            linestyle="--",
            markeredgewidth=linewidth,
            markersize=markersize,
            color=color,
        )  # 绘制曲线

ax.set_xticks(x_ticks)
ax.set_xticklabels(["0M", "16M", "32M", "64M"])

ax.set_xlabel("Cache Size", fontsize=fontsize, fontweight="bold")
ax.set_ylabel("Join Latency (s)", fontsize=fontsize, fontweight="bold")
ax1.set_ylabel("Cache Hit Rate (%)", fontsize=fontsize, fontweight="bold", labelpad=-1)


label_settings = {
    "NL-NS/S-EI": {"color": colors[0], "marker": "o", "hatch": "//"},
    "NL-NS/S-LI": {"color": colors[1], "marker": "s", "hatch": "\\\\"},
    "NL-NS/S-CI": {"color": colors[2], "marker": "H", "hatch": "xx"},
    "NL-NS/V-LI": {"color": colors[3], "marker": "^", "hatch": ".."},
    "SJ-SS/S-CI": {"color": colors[4], "marker": "d", "hatch": "++"},
    "SJ-SS/V-EI": {"color": colors[5], "marker": "+", "hatch": "--"},
}
legend_handles = []
legend_handles2 = []
i = 0
for label in label_settings:
    legend_handles.append(
        Patch(
            color=label_settings[label]["color"],
            hatch=label_settings[label]["hatch"],
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
    fontsize=fontsize - 2,
    ncol=3,
    bbox_to_anchor=(0.98, 1.2),
    title="Rate (%)",
    columnspacing=0.8,
    title_fontsize=fontsize - 2,
    edgecolor="black",
    handletextpad=0.5,
    # frameon=False,
)

# fig.text(0.6, 1.08, "Latency", ha="center", va="center", fontsize=6)
fig.legend(
    handles=legend_handles,
    fontsize=fontsize - 2,
    ncol=3,
    bbox_to_anchor=(0.49, 1.2),
    title="Latency (s)",
    columnspacing=0.8,
    title_fontsize=fontsize - 2,
    handletextpad=0.5,
    edgecolor="black",
    # frameon=False,
)
plt.tight_layout()
plt.savefig("lsm_join/plot/cache_bpk.pdf", bbox_inches="tight", pad_inches=0.02)
plt.close()
