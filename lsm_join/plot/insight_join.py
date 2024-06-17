import matplotlib.pyplot as plt
import pandas as pd
import matplotlib.lines as mlines
from csv_process import write_csv_from_txt, process_csv
import sci_palettes
from scipy.spatial.distance import pdist, squareform
import matplotlib as mpl

mpl.rcParams["font.family"] = "Times New Roman"
mpl.use("Agg")
fontsize = 14
edgewidth = 1.5
markersize = 7
sci_palettes.register_cmap()
test_names = ["insight_join"]
for test_name in test_names:
    write_csv_from_txt(test_name)
    process_csv(test_name)

B = pd.read_csv("lsm_join/csv_result/insight_join.csv")


# 创建数据框的数组
dfs = [
    {"df": B, "title": "B"},
]

# 设置图的大小和子图布局
fig, axes = plt.subplots(1, 1, figsize=(3.5, 3), sharey=True)
if axes is not list:
    axes = [axes]

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
    "NL-NS/S-LI": {"color": colors[0], "marker": "o"},
    "SJ-NS/S-LI": {"color": colors[1], "marker": "d"},
    "HJ-N": {"color": colors[2], "marker": "^"},
}

k_s_values = [10]
df = dfs[0]["df"]
df["B"] = df["B"].apply(lambda x: str(int(4096 / x)))
for ax, k_s in zip(axes, k_s_values):
    attribute = "B"
    fillstyle = "none"
    title = attribute

    for label, group in df.groupby("label"):
        group = df[(df["label"] == label) & (df["bpk"] == k_s)]
        color = label_settings[label]["color"]
        marker = label_settings[label]["marker"]
        if "INLJ" in label:
            print(group["sum_join_time"])
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

    ax.set_ylabel("Join Latency (s)", fontweight="bold", fontsize=fontsize)
    # ax.set_xlabel("Entry Size (byte)", fontweight="bold", fontsize=fontsize)
    # ax.set_xticks([1, 2, 4, 8, 16, 32])
    label = ax.set_xlabel(
        "Entry Size (byte) of ", fontweight="bold", fontsize=fontsize
    )  # 设置常规部分
    x, y = label.get_position()
    label.set_position((x - 0.07, y))
    t = ax.text(
        0.9,
        -0.125,
        "Face",
        transform=ax.transAxes,
        style="italic",
        weight="bold",
        fontsize=fontsize,
        verticalalignment="top",
        horizontalalignment="right",
    )
    # ax.set_xticks([32, 512, 1024, 2048, 4096])


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
    bbox_to_anchor=(0.98, 1.1),
    # loc="best",
    ncol=3,
    fontsize=fontsize - 2,
    edgecolor="black",
    columnspacing=0.6,
    handletextpad=0.2,
    handlelength=1.5,
)

# plt.yscale('log')

plt.subplots_adjust(wspace=0.01, hspace=0.1)
plt.tight_layout()
plt.savefig("lsm_join/plot/insight_join.pdf", bbox_inches="tight", pad_inches=0.02)
plt.close()
