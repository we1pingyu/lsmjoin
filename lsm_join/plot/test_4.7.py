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
test_names = ["5.9_user", "5.9_wiki"]
for test_name in test_names:
    write_csv_from_txt(test_name)
    process_csv(test_name)

df1 = pd.read_csv("lsm_join/csv_result/5.9_user.csv")
df2 = pd.read_csv("lsm_join/csv_result/5.9_wiki.csv")

fig, ax = plt.subplots(1, 2, figsize=(9, 3))

ax = ax.flatten()

# 创建数据框的数组
dfs = [
    {"df": df1, "title": "movie"},
    {"df": df2, "title": "wiki"},
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

for n, df in enumerate(dfs):
    df = dfs[n]["df"]
    df = df[df["num_loop"] != 12]
    # df["num_loop"] = df["num_loop"].astype(str)
    df = df.sort_values(by="num_loop")
    attribute = "num_loop"
    fillstyle = "none"
    title = attribute
    bar_width = 0.3  # Width of each bar, adjust as needed
    x_offset = np.arange(df[attribute].nunique()) * 2
    # ax1 = ax[n]
    ax2 = ax[n]
    for i, (label, group) in enumerate(df.groupby("label")):
        if label not in label_settings:
            continue
        # Calculate offset positions for each group
        positions = x_offset + (i * bar_width)
        color = label_settings[label]["color"]
        hatch = label_settings[label]["hatch"]
        # Bar plot for the group
        # ax2.bar(
        #     positions,
        #     group["sum_index_build_time"],
        #     bottom=group["sum_join_time"],
        #     width=bar_width,
        #     color=color,
        #     edgecolor="black",
        #     # hatch=hatch,
        #     label=label,
        #     linewidth=edgewidth,
        # )
        try:
            bars = ax2.bar(
                positions,
                group["sum_join_time"],
                width=bar_width,
                color=color,
                edgecolor="black",
                hatch=hatch,
                label=label,
                linewidth=edgewidth,
            )
            if "SJ-PS" in label:
                i = 0
                for bar, g in zip(bars, group["sum_index_build_time"]):
                    if i == 0:
                        ax2.text(
                            bar.get_x() + bar.get_width() / 2 + 0.05,
                            bar.get_height() + 2,
                            "+{} s index building".format(round(g)),
                            ha="center",
                            va="bottom",
                            rotation=90,
                            fontsize=fontsize - 5,
                            weight="bold",
                        )
                        i += 1
                    else:
                        ax2.text(
                            bar.get_x() + bar.get_width() / 2 + 0.05,
                            bar.get_height() + 2,
                            "+{} s".format(round(g)),
                            ha="center",
                            va="bottom",
                            rotation=90,
                            fontsize=fontsize - 5,
                            weight="bold",
                        )
        except:
            continue
        if n == 0:
            ax2.set_ylabel("Joining(s)", fontweight="bold", fontsize=fontsize + 2)
        # ax1.set_ylabel("Index Building(s)", fontweight="bold", fontsize=fontsize - 1)
        # ax.set_xlabel("Entry Size (byte)", fontweight="bold", fontsize=fontsize)
        # ax.set_xticks([1, 2, 4, 8, 16, 32])
        label = ax2.set_xlabel(
            "Number of Joins on", fontweight="bold", fontsize=fontsize + 2
        )  # 设置常规部分
        x, y = label.get_position()
        label.set_position((x - 0.07, y))
    if n == 0:
        t = ax2.text(
            0.9,
            -0.133,
            "User",
            transform=ax2.transAxes,
            style="italic",
            weight="bold",
            fontsize=fontsize + 2,
            verticalalignment="top",
            horizontalalignment="right",
        )
    else:
        t = ax2.text(
            0.9,
            -0.133,
            "Wiki",
            transform=ax2.transAxes,
            style="italic",
            weight="bold",
            fontsize=fontsize + 2,
            verticalalignment="top",
            horizontalalignment="right",
        )
    x_offset = bar_width * 2.5
    ax2.set_xticks(
        [
            0 + x_offset,
            2 + x_offset,
            4 + x_offset,
            6 + x_offset,
            8 + x_offset,
        ],
        [2, 4, 8, 16, 32],
        fontsize=fontsize - 2,
    )
    ax2.tick_params(axis="y", labelsize=fontsize - 2)
ax2.legend(
    ncols=3, edgecolor="black", fontsize=fontsize - 1, bbox_to_anchor=(0.7, 1.35)
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

plt.subplots_adjust(wspace=0.2, hspace=0)
# plt.tight_layout()
plt.savefig("lsm_join/plot/test_5.9.pdf", bbox_inches="tight", pad_inches=0.02)
plt.close()
