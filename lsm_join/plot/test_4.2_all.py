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
test_names = ["4.2_movie_all", "4.2_face_all"]
for test_name in test_names:
    write_csv_from_txt(test_name)
    process_csv(test_name)

df1 = pd.read_csv("lsm_join/csv_result/4.2_movie_all.csv")
df2 = pd.read_csv("lsm_join/csv_result/4.2_face_all.csv")

fig, ax = plt.subplots(1, 2, figsize=(12, 3))

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
colors = [(r * darkening_factor, g * darkening_factor, b * darkening_factor) for r, g, b in colors]

label_settings = {
    "NL-P": {"color": colors[0], "marker": "o", "hatch": "//"},
    "SJ-P": {"color": colors[1], "marker": "d", "hatch": "\\\\"},
    "SJ-PS/S-CI": {"color": colors[2], "marker": "s", "hatch": "xx"},
    "HJ-P": {"color": colors[3], "marker": "^", "hatch": ".."}, 
    "NL-NS/S-CI": {"color": colors[4], "marker": "o", "hatch": "//"},
    "SJ-N": {"color": colors[5], "marker": "d", "hatch": "\\\\"},
    "SJ-NS/S-CI": {"color": colors[6], "marker": "s", "hatch": "xx"},
    "HJ-N": {"color": colors[7], "marker": "^", "hatch": ".."},
}


for n, df in enumerate(dfs):
    df = dfs[n]["df"]
    df["B"] = df["B"].apply(lambda x: str(int(4096 / x)))
    df = df[df["B"] != "4096"]
    attribute = "B"
    fillstyle = "none"
    title = attribute
    bar_width = 0.1  # Width of each bar, adjust as needed
    x_offset = np.arange(df[attribute].nunique())
    # ax1 = ax[n]
    ax2 = ax[n]
    print(len(df.groupby("label")))
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
        if "SJ-PS" in label:
            label = "SJ-PS"
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
            # if "SJ-PS" in label:
            #     i = 0
            #     for bar, g in zip(bars, group["sum_index_build_time"]):
            #         if i == 0:
            #             ax2.text(
            #                 bar.get_x() + bar.get_width() / 2 + 0.15,
            #                 bar.get_height() + 2,
            #                 "+{} s index building".format(round(g)),
            #                 ha="center",
            #                 va="bottom",
            #                 rotation=90,
            #                 fontsize=fontsize - 3,
            #                 weight="bold",
            #             )
            #             i += 1
            #         else:
            #             ax2.text(
            #                 bar.get_x() + bar.get_width() / 2 + 0.15,
            #                 bar.get_height() + 2,
            #                 "+{} s".format(round(g)),
            #                 ha="center",
            #                 va="bottom",
            #                 rotation=90,
            #                 fontsize=fontsize - 3,
            #                 weight="bold",
            #             )
        except:
            continue
        if n == 0:
            ax2.set_ylabel("Joining(s)", fontweight="bold", fontsize=fontsize + 1)
        # ax1.set_ylabel("Index Building(s)", fontweight="bold", fontsize=fontsize - 1)
        # ax.set_xlabel("Entry Size (byte)", fontweight="bold", fontsize=fontsize)
        # ax.set_xticks([1, 2, 4, 8, 16, 32])
        label = ax2.set_xlabel("Entry Size(byte) of ", fontweight="bold", fontsize=fontsize + 1)  # 设置常规部分
        x, y = label.get_position()
        label.set_position((x - 0.14, y))
    if n == 0:
        t = ax2.text(
            1.07,
            -0.125,
            "Movie",
            transform=ax2.transAxes,
            style="italic",
            weight="bold",
            fontsize=fontsize + 1,
            verticalalignment="top",
            horizontalalignment="right",
        )
    else:
        t = ax2.text(
            1.01,
            -0.125,
            "Face",
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
            5 + x_offset,
        ],
        [32, 128, 512, 1024, 2048, 4096],
        fontsize=fontsize - 4,
    )
    ax2.tick_params(axis="y", labelsize=fontsize - 4)
    ax2.set_ylim(0, 220)
ax2.legend(
    ncols=4,
    edgecolor="black",
    fontsize=fontsize,
    bbox_to_anchor=(1.05, 1.21),
    columnspacing=1.4,
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
plt.savefig("lsm_join/plot/test_4.2_all.pdf", bbox_inches="tight", pad_inches=0.02)
plt.close()