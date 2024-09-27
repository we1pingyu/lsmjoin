import matplotlib.pyplot as plt
import pandas as pd
import matplotlib.lines as mlines
from csv_process import write_csv_from_txt, process_csv
import sci_palettes
from scipy.spatial.distance import pdist, squareform
import matplotlib as mpl
import numpy as np
from matplotlib.lines import Line2D

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
df1 = pd.DataFrame(
    {
        "label": [
            "NL-P",
            "HJ-P",
            "SJ-P",
            "SJ-PS/S-CI",
            "SJ-SS/S-EI",
            "SJ-SS/V-LI",
            "NL-P",
            "HJ-P",
            "SJ-P",
            "SJ-PS/S-CI",
            "SJ-SS/S-EI",
            "SJ-SS/V-LI",
            "NL-P",
            "HJ-P",
            "SJ-P",
            "SJ-PS/S-CI",
            "SJ-SS/S-EI",
            "SJ-SS/V-LI",
            "NL-P",
            "HJ-P",
            "SJ-P",
            "SJ-PS/S-CI",
            "SJ-SS/S-EI",
            "SJ-SS/V-LI",
            "NL-P",
            "HJ-P",
            "SJ-P",
            "SJ-PS/S-CI",
            "SJ-SS/S-EI",
            "SJ-SS/V-LI",
            "NL-NS/S-CI",
            "NL-NS/V-CI",
            "NL-NS/S-EI",
            "NL-NS/V-EI",
            "NL-NS/S-LI",
            "NL-NS/V-LI",
            "NL-NS/S-CI",
            "NL-NS/V-CI",
            "NL-NS/S-EI",
            "NL-NS/V-EI",
            "NL-NS/S-LI",
            "NL-NS/V-LI",
            "NL-NS/S-CI",
            "NL-NS/V-CI",
            "NL-NS/S-EI",
            "NL-NS/V-EI",
            "NL-NS/S-LI",
            "NL-NS/V-LI",
            "NL-NS/S-CI",
            "NL-NS/V-CI",
            "NL-NS/S-EI",
            "NL-NS/V-EI",
            "NL-NS/S-LI",
            "NL-NS/V-LI",
            "NL-NS/S-CI",
            "NL-NS/V-CI",
            "NL-NS/S-EI",
            "NL-NS/V-EI",
            "NL-NS/S-LI",
            "NL-NS/V-LI",
        ],
        "B": [
            32,
            32,
            32,
            32,
            32,
            32,
            128,
            128,
            128,
            128,
            128,
            128,
            512,
            512,
            512,
            512,
            512,
            512,
            1024,
            1024,
            1024,
            1024,
            1024,
            1024,
            2048,
            2048,
            2048,
            2048,
            2048,
            2048,
            32,
            32,
            32,
            32,
            32,
            32,
            128,
            128,
            128,
            128,
            128,
            128,
            512,
            512,
            512,
            512,
            512,
            512,
            1024,
            1024,
            1024,
            1024,
            1024,
            1024,
            2048,
            2048,
            2048,
            2048,
            2048,
            2048,
        ],
        # "c_s": [0.7, 0.7, 0.7, 0.7, 0.7, 0.7],
        "sum_index_build_time": [
            0,  # 1
            0,
            0,
            43,
            149,
            40,
            0,  # 2
            0,
            0,
            59,
            333,
            43,
            0,  # 3
            0,
            0,
            180,
            1019,
            125,
            0,  # 4
            0,
            0,
            313,
            2308,
            274,
            0,  # 5
            0,
            0,
            775,
            5953,
            717,
            40,  # 6
            20,
            73,
            40,
            40,
            19,
            53,  # 7
            21,
            113,
            41,
            53,
            20,
            85,  # 8
            42,
            294,
            60,
            153,
            41,
            143,  # 9
            70,
            665,
            104,
            247,
            87,
            331,  # 10
            165,
            1600,
            217,
            662,
            201,
        ],
        "sum_join_time": [
            12,  # 1
            19,
            7,
            1,
            1,
            62,
            20,  # 2
            20,
            12,
            2,
            3,
            79,
            30,  # 3
            30,
            35,
            5,
            8,
            111,
            44,  # 4
            78,
            83,
            9,
            16,
            162,
            52,  # 5
            159,
            177,
            18,
            32,
            353,
            251,  # 6
            297,
            33,
            308,
            38,
            320,
            67,  # 7
            370,
            46,
            253,
            62,
            270,
            96,  # 8
            470,
            62,
            394,
            82,
            410,
            156,  # 9
            602,
            100,
            556,
            154,
            561,
            301,  # 10
            890,
            163,
            810,
            278,
            832,
        ],
    }
)
df2 = pd.DataFrame(
    {
        "label": [
            "NL-P",
            "HJ-P",
            "SJ-P",
            "SJ-PS/S-CI",
            "SJ-SS/S-EI",
            "SJ-SS/V-LI",
            "NL-P",
            "HJ-P",
            "SJ-P",
            "SJ-PS/S-CI",
            "SJ-SS/S-EI",
            "SJ-SS/V-LI",
            "NL-P",
            "HJ-P",
            "SJ-P",
            "SJ-PS/S-CI",
            "SJ-SS/S-EI",
            "SJ-SS/V-LI",
            "NL-P",
            "HJ-P",
            "SJ-P",
            "SJ-PS/S-CI",
            "SJ-SS/S-EI",
            "SJ-SS/V-LI",
            "NL-NS/S-CI",
            "NL-NS/V-CI",
            "NL-NS/S-EI",
            "NL-NS/V-EI",
            "NL-NS/S-LI",
            "NL-NS/V-LI",
            "NL-NS/S-CI",
            "NL-NS/V-CI",
            "NL-NS/S-EI",
            "NL-NS/V-EI",
            "NL-NS/S-LI",
            "NL-NS/V-LI",
            "NL-NS/S-CI",
            "NL-NS/V-CI",
            "NL-NS/S-EI",
            "NL-NS/V-EI",
            "NL-NS/S-LI",
            "NL-NS/V-LI",
            "NL-NS/S-CI",
            "NL-NS/V-CI",
            "NL-NS/S-EI",
            "NL-NS/V-EI",
            "NL-NS/S-LI",
            "NL-NS/V-LI",
        ],
        "k_s": [
            0.1,
            0.1,
            0.1,
            0.1,
            0.1,
            0.1,
            0.3,
            0.3,
            0.3,
            0.3,
            0.3,
            0.3,
            0.5,
            0.5,
            0.5,
            0.5,
            0.5,
            0.5,
            0.7,
            0.7,
            0.7,
            0.7,
            0.7,
            0.7,
            0.1,
            0.1,
            0.1,
            0.1,
            0.1,
            0.1,
            0.3,
            0.3,
            0.3,
            0.3,
            0.3,
            0.3,
            0.5,
            0.5,
            0.5,
            0.5,
            0.5,
            0.5,
            0.7,
            0.7,
            0.7,
            0.7,
            0.7,
            0.7,
        ],
        # "c_s": [0.7, 0.7, 0.7, 0.7, 0.7, 0.7],
        "sum_index_build_time": [
            0,
            0,
            0,
            49,
            169,
            45,
            0,
            0,
            0,
            50,
            171,
            44,
            0,
            0,
            0,
            50,
            176,
            44,
            0,
            0,
            0,
            52,
            289,
            42,
            48,
            22,
            90,
            47,
            48,
            23,
            47,
            22,
            92,
            47,
            47,
            22,
            48,
            22,
            91,
            48,
            48,
            23,
            49,
            22,
            274,
            95,
            47,
            21,
        ],
        "sum_join_time": [
            21,
            25,
            9,
            49,
            2,
            71,
            21,
            25,
            10,
            2,
            2,
            72,
            21,
            25,
            9,
            2,
            2,
            71,
            19,
            24,
            9,
            2,
            2,
            73,
            47,
            136,
            33,
            86,
            35,
            91,
            49,
            141,
            35,
            90,
            36,
            98,
            53,
            203,
            36,
            165,
            40,
            172,
            235,
            4049,
            125,
            4420,
            172,
            4490,
        ],
    }
)

fig, ax = plt.subplots(2, 2, figsize=(12, 3.5))
ax = ax.flatten()
# 创建数据框的数组
dfs = [
    {"df": df1, "title": "unif_k"},
    {"df": df2, "title": "unif_c"},
]

style = "tab20"
plt.set_cmap(style)
cmap = plt.get_cmap(style)
colors = cmap.colors
darkening_factor = 0.9
colors = [(r * darkening_factor, g * darkening_factor, b * darkening_factor) for r, g, b in colors]

label_settings = {
    "NL-P": {"color": colors[0], "marker": "o", "hatch": "//"},
    "HJ-P": {"color": colors[1], "marker": "d", "hatch": "\\\\"},
    "SJ-P": {"color": colors[2], "marker": "s", "hatch": "xx"},
    "SJ-PS/S-CI": {"color": colors[3], "marker": "+", "hatch": ".."},
    "SJ-SS/S-EI": {"color": colors[4], "marker": "^", "hatch": "++"},
    "SJ-SS/V-LI": {"color": colors[5], "marker": "H", "hatch": "oo"},
    "NL-NS/S-CI": {"color": colors[7], "marker": "o", "hatch": "//"},
    "NL-NS/S-EI": {"color": colors[8], "marker": "d", "hatch": "\\\\"},
    "NL-NS/S-LI": {"color": colors[9], "marker": "s", "hatch": "xx"},
    "NL-NS/V-CI": {"color": colors[10], "marker": "+", "hatch": ".."},
    "NL-NS/V-EI": {"color": colors[11], "marker": "^", "hatch": "++"},
    "NL-NS/V-LI": {"color": colors[12], "marker": "H", "hatch": "oo"},
}

z = 0

for n, df in enumerate(dfs):
    df = dfs[n]["df"]
    if n == 0:
        # df["B"] = df["B"].apply(lambda x: str(int(4096 / x)))
        attribute = "B"
    if n == 1:
        attribute = "k_s"
    fillstyle = "none"
    title = attribute
    bar_width = 0.2  # Width of each bar, adjust as needed
    x_offset = np.arange(df[attribute].nunique()) * 3
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
        if len(group["sum_index_build_time"]) == 0:
            continue
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
        values = np.clip(group["sum_index_build_time"], 0, 1600)
        for j, (original, clipped) in enumerate(zip(group["sum_index_build_time"], values)):
            if original > 1600:
                x1 = positions[j] - bar_width / 2
                x2 = positions[j] + bar_width / 2
                y1 = 1500  # 截断线的位置
                y2 = 1550
                line = Line2D([x1, x2], [y1, y2], linewidth=4.5, color="white")
                ax1.add_line(line)
                line = Line2D(
                    [x1 - 0.1, x2 + 0.1],
                    [y1 + 30, y2 + 50],
                    linewidth=1.5,
                    color="black",
                )
                ax1.add_line(line)
                line = Line2D(
                    [x1 - 0.1, x2 + 0.1],
                    [y1 - 50, y2 - 30],
                    linewidth=1.5,
                    color="black",
                )
                ax1.add_line(line)
                ax1.text(
                    positions[j] - 0.4,
                    1450,
                    f"{int(original)}",
                    ha="center",
                    va="center",
                    rotation=90,
                    fontsize=fontsize - 2,
                    fontweight="bold",
                )
        for j, (original, clipped) in enumerate(zip(group["sum_join_time"], values)):
            if original > 4000 and label == "NL-NS/V-EI":
                x1 = positions[j] - bar_width * 2
                x2 = positions[j] + bar_width * 2
                y1 = 850  # 截断线的位置
                y2 = 885
                line = Line2D([x1, x2], [y1, y2], linewidth=5, color="white")
                ax2.add_line(line)
                line = Line2D(
                    [x1, x2],
                    [y1 + 20, y2 + 30],
                    linewidth=2,
                    color="black",
                )
                ax2.add_line(line)
                line = Line2D(
                    [x1, x2],
                    [y1 - 30, y2 - 20],
                    linewidth=2,
                    color="black",
                )
                ax2.add_line(line)
            if original > 4000:
                if z == 0:
                    offest = -0.6
                if z == 1:
                    offest = -0.5
                if z == 2:
                    offest = 0.5
                ax2.text(
                    positions[j] + offest,
                    850,
                    f"{int(original)}",
                    ha="center",
                    va="center",
                    fontsize=fontsize - 2,
                    fontweight="bold",
                    rotation=90,
                )
                z += 1

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
            0.68,
            -0.208,
            "Movie",
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
                0.6 + x_offset,
                3.6 + x_offset,
                6.6 + x_offset,
                9.6 + x_offset,
                12.6 + x_offset,
            ],
            [32, 128, 512, 1024, 2048],
        )
    if n == 1:
        label = ax2.set_xlabel(
            "(b) Skewness of",
            fontweight="bold",
            fontsize=fontsize,
        )
        x, y = label.get_position()
        label.set_position((x - 0.06, y))
        t = ax2.text(
            0.59,
            -0.208,
            "Zipf",
            transform=ax2.transAxes,
            style="italic",
            weight="bold",
            fontsize=fontsize,
            verticalalignment="top",
            horizontalalignment="right",
        )
        # if n == 2:
        #     label = ax2.set_xlabel(
        #         r"(c)Skewness ($\theta_s,\theta_r$) of ",
        #         fontweight="bold",
        #         fontsize=fontsize,
        #         # x=-0.001,
        #     )
        #     t = ax2.text(
        #         0.9,
        #         -0.215,
        #         "Zipf",
        #         transform=ax2.transAxes,
        #         style="italic",
        #         weight="bold",
        #         fontsize=fontsize,
        #         verticalalignment="top",
        #         horizontalalignment="right",
        #     )
        x, y = label.get_position()
        label.set_position((x - 0.07, y))

        x_offset = bar_width * 5
        ax2.set_xticks(
            [
                0.1 + x_offset,
                3.1 + x_offset,
                6.1 + x_offset,
                9.1 + x_offset,
            ],
            [0.1, 0.3, 0.5, 0.7],
        )
    if n == 0:
        ax1.set_ylim(0, 1700)
        ax2.set_ylim(0, 850)
    if n == 1:
        ax1.set_ylim(0, 300)
        ax2.set_ylim(0, 1000)
    for spine in ax1.spines.values():
        spine.set_edgecolor("blue")
    for spine in ax2.spines.values():
        spine.set_edgecolor("blue")
ax1.legend(
    ncol=6,
    edgecolor="black",
    fontsize=fontsize - 2,
    bbox_to_anchor=(0.85, 1.5),
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

plt.subplots_adjust(wspace=0.1, hspace=0)
# plt.tight_layout()
plt.savefig("lsm_join/plot/test_pebble.pdf", bbox_inches="tight", pad_inches=0.02)
plt.close()
