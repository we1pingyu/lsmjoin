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
test_names = ["entry_size"]
for test_name in test_names:
    write_csv_from_txt(test_name)
    process_csv(test_name)

B = pd.read_csv("lsm_join/csv_result/entry_size.csv")


# 创建数据框的数组
dfs = [
    {"df": B, "title": "B"},
]

# 设置图的大小和子图布局
fig, axes = plt.subplots(1, 1, figsize=(5, 3), sharey=True)
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
    "CLazy-INLJ": {"color": colors[0], "marker": "o"},
    "1CLazy-ISJ": {"color": colors[1], "marker": "d"},
    "Grace-HJ": {"color": colors[2], "marker": "^"},
}

k_s_values = [4]
df = dfs[0]["df"]
df["B"] = 4096 / df["B"]
for ax, k_s in zip(axes, k_s_values):
    attribute = "B"
    fillstyle = "none"
    title = attribute

    for label, group in df.groupby("label"):
        group = df[(df["label"] == label) & (df["k_s"] == k_s)]
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

    ax.set_ylabel("Join Latency (s)", fontweight="bold", fontsize=fontsize)
    ax.set_xlabel("Entry Size (byte)", fontweight="bold", fontsize=fontsize)
    ax.set_xticks([32, 512, 1024, 2048, 4096])


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
    bbox_to_anchor=(0.45, 0.95),
    # loc="best",
    ncol=1,
    fontsize=fontsize - 1,
    edgecolor="black",
)

# plt.yscale('log')

plt.subplots_adjust(wspace=0.01, hspace=0.1)
plt.tight_layout()
plt.savefig("lsm_join/plot/entry_size.pdf", bbox_inches="tight", pad_inches=0.02)
plt.close()
