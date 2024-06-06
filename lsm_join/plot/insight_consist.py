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
test_names = ["insight_consist"]
for test_name in test_names:
    write_csv_from_txt(test_name)
    process_csv(test_name)

B = pd.read_csv("lsm_join/csv_result/insight_consist.csv")


# 创建数据框的数组
dfs = [
    {"df": B, "title": "B"},
]

# 设置图的大小和子图布局
fig, axes = plt.subplots(1, 2, figsize=(6, 3), sharey=True)
# print(axes)
# if axes is not list:
#     axes = [axes]

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
    "CLazy-INLJ": {"color": colors[0], "marker": "s"},
    # "1CLazy-ISJ": {"color": colors[1], "marker": "d"},
    "Lazy-INLJ": {"color": colors[1], "marker": "d"},
    # "1Lazy-ISJ": {"color": colors[3], "marker": "d"},
    "CLazy-INLJn": {"color": colors[2], "marker": "o"},
    # "1CLazy-ISJn": {"color": colors[5], "marker": "d"},
}

loop_values = [1, 16]
df = dfs[0]["df"]
df["B"] = df["B"].apply(lambda x: str(int(4096 / x)))
# print(df["B"])
for index, row in df.iterrows():
    if row["noncovering"] == 1:
        df.at[index, "label"] += "n"

for ax, loop in zip(axes, loop_values):
    attribute = "B"
    fillstyle = "none"
    title = attribute

    for label, group in df.groupby("label"):
        group = df[(df["label"] == label) & (df["num_loop"] == loop)]
        # print(label, group["noncovering"])
        color = label_settings[label]["color"]
        marker = label_settings[label]["marker"]
        # print(group["B"])
        ax.plot(
            group[attribute],
            group["sum_join_time"] + group["sum_index_build_time"],
            marker=marker,
            fillstyle=fillstyle,
            color=color,
            linewidth=edgewidth,
            markeredgewidth=edgewidth,
            markersize=markersize,
            linestyle="-" if not label.endswith("n") else "--",
        )

    ax.set_xlabel("Entry Size (byte)", fontweight="bold", fontsize=fontsize)
    # ax.set_xticks([32, 512, 1024, 2048, 4096])
axes[0].set_ylabel("System Latency (s)", fontweight="bold", fontsize=fontsize)

legend_handles2 = []
for label, setting in label_settings.items():
    if label.endswith("n"):
        label = label[:-1] + "(cannot cover all queries)"
    legend_handles2.append(
        mlines.Line2D(
            [],
            [],
            color=setting["color"],
            marker=setting["marker"],
            linestyle="-" if not len(label) > 20 else "--",
            markersize=markersize,
            fillstyle="none",
            label=label,
        )
    )

fig.legend(
    handles=legend_handles2,
    bbox_to_anchor=(1, 1.1),
    # loc="best",
    ncol=3,
    fontsize=fontsize - 2,
    columnspacing=0.8,
    handletextpad=0.2,
    edgecolor="black",
)
fig.text(0.3, 0.8, "Numer of Joins = 1", ha="center", fontsize=fontsize)
fig.text(0.75, 0.8, "Numer of Joins = 16", ha="center", fontsize=fontsize)
# plt.yscale('log')

plt.subplots_adjust(wspace=0.01, hspace=0.1)
plt.tight_layout()
plt.savefig("lsm_join/plot/insight_consist.pdf", bbox_inches="tight", pad_inches=0.02)
plt.close()
