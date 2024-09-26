import matplotlib.pyplot as plt
import pandas as pd
import matplotlib.lines as mlines
from csv_process import write_csv_from_txt, process_csv
import sci_palettes
from scipy.spatial.distance import pdist, squareform
import matplotlib as mpl

mpl.rcParams["font.family"] = "Times New Roman"
mpl.use("Agg")
fontsize = 12
edgewidth = 1.5
markersize = 5
sci_palettes.register_cmap()
test_names = ["T", "buffer_size"]
for test_name in test_names:
    write_csv_from_txt(test_name)
    process_csv(test_name)

T = pd.read_csv("lsm_join/csv_result/T.csv")
# T_t = pd.read_csv("lsm_join/csv_result/T_t.csv")
# K = pd.read_csv("lsm_join/csv_result/K.csv")
buffer_size = pd.read_csv("lsm_join/csv_result/buffer_size.csv")
# buffer_size_t = pd.read_csv("lsm_join/csv_result/buffer_size_t.csv")

# 创建数据框的数组
dfs = [
    {"df": T, "title": "T"},
    # {"df": T_t, "title": "T_t"},
    # {"df": K, "title": "K"},
    {"df": buffer_size, "title": "buffer_size"},
    # {"df": buffer_size_t, "title": "buffer_size_t"},
]

# 设置图的大小和子图布局
fig, axes = plt.subplots(2, 2, figsize=(6, 4))  # 一行两列
axes = axes.flatten()
plt.subplots_adjust(hspace=1)

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
    "NL-NS/S-CI": {"color": colors[0], "marker": "o"},
    "NL-NS/S-EI": {"color": colors[1], "marker": "d"},
    "NL-NS/V-LI": {"color": colors[2], "marker": "^"},
    "SJ-SS/V-CI": {"color": colors[3], "marker": "H"},
    "SJ-NS/S-LI": {"color": colors[4], "marker": "s"},
}
for df in dfs:
    if df["title"] == "T":
        df["df"]["T"] = df["df"]["T"].apply(lambda x: str(x))
    if df["title"] == "buffer_size":
        df["df"]["M"] = df["df"]["M"].apply(lambda x: str(int(x / (2**20))))

j = 0
for i, (ax, df_info) in enumerate(zip(axes, dfs)):
    df = df_info["df"]
    attribute = df_info["title"]
    fillstyle = "none"
    title = attribute
    if attribute == "T":
        attribute = "T"
        title = "Size Ratio"
    if title == "buffer_size":
        df["M_MB"] = df["M"]
        attribute = "M_MB"
        title = "Buffer Size (MB)"
    if title == "buffer_size_t":
        df["M_MB"] = df["M"] / (2**20)
        attribute = "M_MB"
        title = "Buffer Size (Theoretical)"

    ax = axes[j]
    for label, group in df.groupby("label"):
        color = label_settings[label]["color"]
        marker = label_settings[label]["marker"]

        ax.plot(
            group[attribute],
            group["sum_join_time"],
            marker=marker,
            fillstyle=fillstyle,
            color=color,
            linewidth=edgewidth,
            markeredgewidth=edgewidth,
            markersize=markersize,
        )
        ax.tick_params(axis="both", labelsize=fontsize - 2)

    ax = axes[j + 2]
    j += 1
    for label, group in df.groupby("label"):
        color = label_settings[label]["color"]
        marker = label_settings[label]["marker"]

        ax.plot(
            group[attribute],
            group["sum_index_build_time"],
            linestyle="--",
            marker=marker,
            fillstyle=fillstyle,
            color=color,
            linewidth=edgewidth,
            markeredgewidth=edgewidth,
            markersize=markersize,
        )
        ax.tick_params(axis="both", labelsize=fontsize - 2)
    # if i == 0:
    #     ax.set_ylabel("Join Latency (s)", fontweight="bold", fontsize=fontsize)
    ax.set_xlabel(title, fontweight="bold", fontsize=fontsize)
    # if attribute == "K":
    #     ax.set_xticks([2, 3, 4, 5])
    # elif attribute == "T":
    #     ax.set_xticks([2, 4, 10, 40])
    # elif attribute == "M_MB":
    #     ax.set_xticks([4, 16, 32, 64])
    #     ax.set_xticklabels(["4M", "16M", "32M", "64M"])
axes[0].set_ylabel("Join Latency (s)", fontweight="bold", fontsize=fontsize)
axes[2].set_ylabel("Index Build Latency (s)", fontweight="bold", fontsize=fontsize)

# 创建图例
legend_handles1 = [
    mlines.Line2D(
        [], [], color="black", linestyle="-", linewidth=edgewidth, label="Join"
    ),
    mlines.Line2D(
        [], [], color="black", linestyle="--", linewidth=edgewidth, label="Index Build"
    ),
]

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
    bbox_to_anchor=(0.85, 1.04),
    ncol=3,
    fontsize=fontsize - 1,
    edgecolor="black",
)
# fig.legend(
#     handles=legend_handles1,
#     bbox_to_anchor=(0.85, 1.1),
#     ncol=2,
#     fontsize=fontsize - 2,
#     edgecolor="black",
# )

# plt.yscale('log')

plt.subplots_adjust(wspace=0.2, hspace=0.2)
# plt.tight_layout()
plt.savefig("lsm_join/plot/lsm_structure.pdf", bbox_inches="tight", pad_inches=0.02)
plt.close()
