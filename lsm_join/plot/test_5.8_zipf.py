import matplotlib.pyplot as plt
import pandas as pd
import matplotlib.lines as mlines
from csv_process import write_csv_from_txt, process_csv
import sci_palettes
from scipy.spatial.distance import pdist, squareform
import matplotlib as mpl
import numpy as np
from matplotlib.patches import Rectangle
from matplotlib.path import Path
import matplotlib.patches as patches
import matplotlib.transforms as transforms
from matplotlib.lines import Line2D

mpl.rcParams["font.family"] = "Times New Roman"
mpl.use("Agg")
fontsize = 14
edgewidth = 1.5
markersize = 7
sci_palettes.register_cmap()
test_names = ["5.8_zipf"]
for test_name in test_names:
    write_csv_from_txt(test_name)
    process_csv(test_name)


df3 = pd.read_csv("lsm_join/csv_result/5.8_zipf.csv")

fig, ax = plt.subplots(1, 2, figsize=(8, 3.5))
ax = ax.flatten()
# 创建数据框的数组
dfs = [
    {"df": df3, "title": "zipf"},
]

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
    "NL-NS/V-CI": {"color": colors[3], "marker": "+", "hatch": ".."},
    "NL-NS/S-EI": {"color": colors[1], "marker": "d", "hatch": "\\\\"},
    "NL-NS/V-EI": {"color": colors[4], "marker": "^", "hatch": "++"},
    "NL-NS/S-LI": {"color": colors[2], "marker": "s", "hatch": "xx"},
    "NL-NS/V-LI": {"color": colors[5], "marker": "H", "hatch": "oo"},
}

t = 0
for n, df in enumerate(dfs):
    df = dfs[n]["df"]
    df["k_s"] = df["k_s"].apply(lambda x: str(x))
    df["c_s"] = df["c_s"].apply(lambda x: str(int(x)))
    attribute = "k_s"
    if n == 1:
        attribute = "c_s"
    fillstyle = "none"
    title = attribute
    bar_width = 0.2  # Width of each bar, adjust as needed
    x_offset = np.arange(df[attribute].nunique()) * 1.5
    print(x_offset)
    ax1 = ax[n]
    ax2 = ax[n + 1]
    for i, (label, group) in enumerate(df.groupby("label")):
        # Calculate offset positions for each group
        positions = x_offset + (i * bar_width)
        color = label_settings[label]["color"]
        hatch = label_settings[label]["hatch"]
        marker = label_settings[label]["marker"]
        values = np.clip(group["sum_index_build_time"], 0, 200)
        ax1.bar(
            positions,
            values,
            width=bar_width,
            color=color,
            edgecolor="black",
            hatch=hatch,
            label=label,
            linewidth=edgewidth,
        )
        for j, (original, clipped) in enumerate(
            zip(group["sum_index_build_time"], values)
        ):
            if original > 200:
                x1 = positions[j] - bar_width / 2
                x2 = positions[j] + bar_width / 2
                y1 = 180  # 截断线的位置
                y2 = 185
                line = Line2D([x1, x2], [y1, y2], linewidth=5, color="white")
                ax1.add_line(line)
                line = Line2D(
                    [x1 - 0.04, x2 + 0.04],
                    [y1 + 3, y2 + 4],
                    linewidth=1.5,
                    color="black",
                )
                ax1.add_line(line)
                line = Line2D(
                    [x1 - 0.04, x2 + 0.04],
                    [y1 - 5, y2 - 3],
                    linewidth=1.5,
                    color="black",
                )
                ax1.add_line(line)
                ax1.text(
                    positions[j],
                    205,
                    f"{int(original)}",
                    ha="center",
                    va="center",
                    fontsize=fontsize - 2,
                    fontweight="bold",
                )

        values = np.clip(group["sum_join_time"], 0, 500)
        ax2.bar(
            positions,
            values,
            width=bar_width,
            color=color,
            edgecolor="black",
            hatch=hatch,
            label=label,
            linewidth=edgewidth,
        )
        for j, (original, clipped) in enumerate(zip(group["sum_join_time"], values)):
            if original > 500 and label == "NL-NS/V-EI":
                x1 = positions[j] - bar_width * 2
                x2 = positions[j] + bar_width * 2
                y1 = 450  # 截断线的位置
                y2 = 465
                line = Line2D([x1, x2], [y1, y2], linewidth=8, color="white")
                ax2.add_line(line)
                line = Line2D(
                    [x1, x2],
                    [y1 + 8, y2 + 9],
                    linewidth=2,
                    color="black",
                )
                ax2.add_line(line)
                line = Line2D(
                    [x1, x2],
                    [y1 - 9, y2 - 8],
                    linewidth=2,
                    color="black",
                )
                ax2.add_line(line)
            if original > 500:
                if t == 0:
                    offest = -0.1
                if t == 1:
                    offest = 0.0
                if t == 2:
                    offest = 0.1
                ax2.text(
                    positions[j] + offest,
                    540,
                    f"{int(original)}",
                    ha="center",
                    va="center",
                    fontsize=fontsize - 2,
                    fontweight="bold",
                    rotation=90,
                )
                t += 1
        ax1.legend(
            ncols=3,
            edgecolor="black",
            fontsize=fontsize - 2,
            bbox_to_anchor=(1, 1.3),
            loc="upper center",
        )
    if n == 0:
        ax1.set_ylim(0, 202)
        ax2.set_ylim(0, 502)
        ax1.spines["right"].set_visible(False)
        ax1.spines["top"].set_visible(False)
        ax2.spines["right"].set_visible(False)
        ax2.spines["top"].set_visible(False)
        ax2.set_ylabel("Joining(s)", fontweight="bold", fontsize=fontsize)
        ax1.set_ylabel("Index Building(s)", fontweight="bold", fontsize=fontsize)
        # ax.set_xlabel("Entry Size (byte)", fontweight="bold", fontsize=fontsize)
        # ax.set_xticks([1, 2, 4, 8, 16, 32])
    if n == 0:
        label = ax2.set_xlabel(
            "Skewness",
            fontweight="bold",
            fontsize=fontsize,
            # x=-0.001,
        )
        ax1.set_xlabel("Skewness", fontweight="bold", fontsize=fontsize)
        # t = ax2.text(
        #     0.9,
        #     -0.215,
        #     "Zipf",
        #     transform=ax2.transAxes,
        #     style="italic",
        #     weight="bold",
        #     fontsize=fontsize,
        #     verticalalignment="top",
        #     horizontalalignment="right",
        # )
    x, y = label.get_position()
    label.set_position((x - 0.07, y))

    x_offset = bar_width * 2.5
    if n == 0:
        ax1.set_xticks(
            [
                0 + x_offset,
                1.5 + x_offset,
                3 + x_offset,
                4.5 + x_offset,
            ],
            [0.1, 0.3, 0.5, 0.7],
        )
        ax2.set_xticks(
            [
                0 + x_offset,
                1.5 + x_offset,
                3 + x_offset,
                4.5 + x_offset,
            ],
            [0.1, 0.3, 0.5, 0.7],
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

plt.subplots_adjust(wspace=0.15, hspace=0)
# plt.tight_layout()
plt.savefig("lsm_join/plot/test_5.8_zipf.pdf", bbox_inches="tight", pad_inches=0.02)
plt.close()
