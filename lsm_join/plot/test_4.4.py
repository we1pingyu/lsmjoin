import matplotlib.pyplot as plt
import pandas as pd
import matplotlib.lines as mlines
from csv_process import write_csv_from_txt, process_csv
import sci_palettes
from scipy.spatial.distance import pdist, squareform
import matplotlib as mpl
import numpy as np

mpl.rcParams["font.family"] = "Times New Roman"
mpl.use("Agg")
fontsize = 15
edgewidth = 1.5
markersize = 7
sci_palettes.register_cmap()
test_names = ["5.5"]
for test_name in test_names:
    write_csv_from_txt(test_name)
    process_csv(test_name)

B = pd.read_csv("lsm_join/csv_result/5.5.csv")

fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(3.5, 2.8), sharex=True)

# 创建数据框的数组
dfs = [
    {"df": B, "title": "B"},
]

# 设置图的大小和子图布局
# fig, axes = plt.subplots(1, 1, figsize=(3.5, 3), sharey=True)
# if axes is not list:
#     axes = [axes]

# colors = ["#3E8D27", "#A22025", "#1432F5"]
style = "tab10"
plt.set_cmap(style)
cmap = plt.get_cmap(style)
colors = cmap.colors
darkening_factor = 0.9
colors = [
    (r * darkening_factor, g * darkening_factor, b * darkening_factor)
    for r, g, b in colors
]

label_settings = {
    "NL-NS/S-CI": {"color": colors[0], "marker": "o", "hatch": "//"},
    "SJ-NS/S-CI": {"color": colors[1], "marker": "d", "hatch": "\\\\"},
    "SJ-SS/S-CI": {"color": colors[2], "marker": "s", "hatch": "xx"},
    "HJ-N": {"color": colors[4], "marker": "^", "hatch": ".."},
}

k_s_values = [10]
df = dfs[0]["df"]
df["eps_s"] = df["eps_s"].apply(lambda x: str(1 - x))
attribute = "eps_s"
fillstyle = "none"
title = attribute
bar_width = 0.2  # Width of each bar, adjust as needed
x_offset = np.arange(df[attribute].nunique())

for i, (label, group) in enumerate(df.groupby("label")):
    # Calculate offset positions for each group
    positions = x_offset + (i * bar_width)
    color = label_settings[label]["color"]
    hatch = label_settings[label]["hatch"]
    # Bar plot for the group
    ax1.bar(
        positions,
        group["sum_index_build_time"],
        width=bar_width,
        color=color,
        edgecolor="black",
        hatch=hatch,
        label=label,
        linewidth=edgewidth,
    )
    ax2.bar(
        positions,
        group["sum_join_time"],
        width=bar_width,
        color=color,
        edgecolor="black",
        hatch=hatch,
        label=label,
        linewidth=edgewidth,
    )

    ax2.set_ylabel("Joining(s)", fontweight="bold", fontsize=fontsize - 1)
    ax1.set_ylabel("Index Building(s)", fontweight="bold", fontsize=fontsize - 1)
    ax1.tick_params(axis="both", which="major", labelsize=fontsize - 2)
    ax2.tick_params(axis="both", which="major", labelsize=fontsize - 2)
    # ax.set_xlabel("Entry Size (byte)", fontweight="bold", fontsize=fontsize)
    # ax.set_xticks([1, 2, 4, 8, 16, 32])
label = ax2.set_xlabel(
    r"Matching rate ($\epsilon_s$) of ", fontweight="bold", fontsize=fontsize + 1
)  # 设置常规部分
x, y = label.get_position()
label.set_position((x - 0.12, y))
t = ax2.text(
    0.92,
    -0.28,
    "Unif",
    transform=ax2.transAxes,
    style="italic",
    weight="bold",
    fontsize=fontsize + 1,
    verticalalignment="top",
    horizontalalignment="right",
)
x_offset = bar_width * 1.5
ax2.set_xticks(
    [
        0 + x_offset,
        1 + x_offset,
        2 + x_offset,
        3 + x_offset,
        4 + x_offset,
    ],
    [1.0, 0.8, 0.6, 0.4, 0.2],
)
ax1.set_yticks([0, 20, 40])
ax2.set_yticks([0, 20, 40])
ax1.legend(
    ncols=2,
    edgecolor="black",
    fontsize=fontsize - 1,
    bbox_to_anchor=(0.45, 1.6),
    loc="upper center",
    borderpad=0.2,
    columnspacing=0.6,
    handletextpad=0.2,
    labelspacing=0.1,  # 添加这个参数来减小行间距
)

# legend_handles2 = []
# for label, setting in label_settings.items():
#     legend_handles2.append(
#         mlines.Line2D(
#             [],
#             [],
#             color=setting["color"],
#             marker=setting["marker"],
#             linestyle="-",
#             markersize=markersize,
#             fillstyle="none",
#             label=label,
#         )
#     )

# fig.legend(
#     handles=legend_handles2,
#     bbox_to_anchor=(0.98, 1.1),
#     # loc="best",
#     ncol=3,
#     fontsize=fontsize - 2,
#     edgecolor="black",
#     columnspacing=0.6,
#     handletextpad=0.2,
#     handlelength=1.5,
# )

# plt.yscale('log')

plt.subplots_adjust(wspace=0.01, hspace=0)
# plt.tight_layout()
plt.savefig(
    "lsm_join/plot/test_5.5.pdf", bbox_inches="tight", pad_inches=0.02, cmap="gray"
)
plt.close()
