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
fontsize = 13
edgewidth = 1.5
markersize = 7
sci_palettes.register_cmap()
test_names = ["5.7_movie", "5.7_face"]
for test_name in test_names:
    write_csv_from_txt(test_name)
    process_csv(test_name)

df1 = pd.read_csv("lsm_join/csv_result/5.7_movie.csv")
df2 = pd.read_csv("lsm_join/csv_result/5.7_face.csv")

fig, ax = plt.subplots(2, 2, figsize=(6, 3.5))
ax = ax.flatten()
# 创建数据框的数组
dfs = [
    {"df": df1, "title": "unif_k"},
    {"df": df2, "title": "unif_c"},
]

style = "tab10"
plt.set_cmap(style)
cmap = plt.get_cmap(style)
colors = cmap.colors
darkening_factor = 0.9
colors = [(r * darkening_factor, g * darkening_factor, b * darkening_factor) for r, g, b in colors]

label_settings = {
    "NL-NS/S-CI": {"color": colors[0], "marker": "o", "hatch": "//"},
    "NL-NS/S-EI": {"color": colors[1], "marker": "d", "hatch": "\\\\"},
    "NL-NS/S-LI": {"color": colors[2], "marker": "s", "hatch": "xx"},
    "NL-NS/V-CI": {"color": colors[3], "marker": "+", "hatch": ".."},
    "NL-NS/V-EI": {"color": colors[4], "marker": "^", "hatch": "++"},
    "NL-NS/V-LI": {"color": colors[5], "marker": "H", "hatch": "oo"},
}


for n, df in enumerate(dfs):
    df = dfs[n]["df"]
    df["B"] = df["B"].apply(lambda x: str(int(4096 / x)))
    attribute = "B"
    fillstyle = "none"
    title = attribute
    bar_width = 0.2  # Width of each bar, adjust as needed
    x_offset = np.arange(df[attribute].nunique()) * 1.5
    print(x_offset)
    ax1 = ax[n]
    ax2 = ax[n + 2]
    # for i, (label, group) in enumerate(df.groupby("label")):
    for i, label in enumerate(label_settings):
        group = df[df["label"] == label]
        # Calculate offset positions for each group
        positions = x_offset + (i * bar_width)
        color = label_settings[label]["color"]
        hatch = label_settings[label]["hatch"]
        marker = label_settings[label]["marker"]

        label = label.replace("NL", "INLJ").replace("EI", "Eager").replace("LI", "Lazy").replace("CI", "Comp")
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

    if n == 0:
        ax2.set_ylabel("Joining(s)", fontweight="bold", fontsize=fontsize)
        ax1.set_ylabel("Index Building(s)", fontweight="bold", fontsize=fontsize)
        # ax.set_xlabel("Entry Size (byte)", fontweight="bold", fontsize=fontsize)
        # ax.set_xticks([1, 2, 4, 8, 16, 32])
    if n == 0:
        label = ax2.set_xlabel(
            "(a) Entry Size (byte) of ",
            fontweight="bold",
            fontsize=fontsize,
        )
        x, y = label.get_position()
        label.set_position((x - 0.06, y))
        t = ax2.text(
            1.01,
            -0.21,
            "Movie",
            transform=ax2.transAxes,
            style="italic",
            weight="bold",
            fontsize=fontsize,
            verticalalignment="top",
            horizontalalignment="right",
        )
    if n == 1:
        label = ax2.set_xlabel(
            "(b) Entry Size (byte) of ",
            fontweight="bold",
            fontsize=fontsize,
        )
        x, y = label.get_position()
        label.set_position((x - 0.06, y))
        t = ax2.text(
            0.96,
            -0.21,
            "Face",
            transform=ax2.transAxes,
            style="italic",
            weight="bold",
            fontsize=fontsize,
            verticalalignment="top",
            horizontalalignment="right",
        )
    if n == 2:
        label = ax2.set_xlabel(
            r"(c)Skewness ($\theta_s,\theta_r$) of ",
            fontweight="bold",
            fontsize=fontsize,
            # x=-0.001,
        )
        t = ax2.text(
            0.85,
            -0.215,
            "Zipf",
            transform=ax2.transAxes,
            style="italic",
            weight="bold",
            fontsize=fontsize,
            verticalalignment="top",
            horizontalalignment="right",
        )
    x, y = label.get_position()
    label.set_position((x - 0.07, y))

    x_offset = bar_width * 2.5
    ax2.set_xticks(
        [
            0 + x_offset,
            1.5 + x_offset,
            3 + x_offset,
            4.5 + x_offset,
            6 + x_offset,
        ],
        [32, 128, 512, 1024, 2048],
    )
ax1.legend(
    ncol=3,
    edgecolor="black",
    fontsize=fontsize - 2,
    columnspacing=1,
    bbox_to_anchor=(1, 1.45),
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

plt.subplots_adjust(wspace=0.18, hspace=0)
# plt.tight_layout()
plt.savefig("lsm_join/plot/test_4.5.pdf", bbox_inches="tight", pad_inches=0.02)
plt.close()
