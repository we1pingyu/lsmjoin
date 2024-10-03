import matplotlib.pyplot as plt
import pandas as pd
import matplotlib.lines as mlines
from csv_process import write_csv_from_txt, process_csv
import sci_palettes
from scipy.spatial.distance import pdist, squareform
import matplotlib as mpl

# plt.rcParams["text.usetex"] = True
# plt.rcParams["text.latex.preamble"] = r"\usepackage{bm}"

mpl.rcParams["font.family"] = "Times New Roman"
mpl.use("Agg")
fontsize = 15
edgewidth = 1.5
markersize = 7
sci_palettes.register_cmap()
test_names = ["concurrent_face"]
for test_name in test_names:
    write_csv_from_txt(test_name)
    process_csv(test_name)

workload = pd.read_csv("lsm_join/csv_result/concurrent_face.csv")


# print(workload)

# 创建数据框的数组
dfs = [
    {"df": workload, "title": "workload"},
]

# 设置图的大小和子图布局
fig, axes = plt.subplots(1, 1, figsize=(3.5, 2))  # 一行两列

# colors = ["#3E8D27", "#A22025", "#1432F5"]
style = "tab10"
plt.set_cmap(style)
cmap = plt.get_cmap(style)
colors = cmap.colors
darkening_factor = 0.5
colors = [(r * darkening_factor, g * darkening_factor, b * darkening_factor) for r, g, b in colors]

label_settings = {
    "NL-P": {"color": colors[0], "marker": "o"},
    "SJ-P": {"color": colors[1], "marker": "d"},
    "SJ-PS/S-CI": {"color": colors[2], "marker": "s"},
    "HJ-P": {"color": colors[3], "marker": "^"},
}

ax = axes
df = dfs[0]["df"]
attribute = "concurrent_threads"
fillstyle = "none"
title = attribute
df[attribute] = df[attribute].apply(lambda x: str(x))

for label, group in df.groupby("label"):
    # print(label)
    color = label_settings[label]["color"]
    marker = label_settings[label]["marker"]
    if "SJ-PS" in label:
        label = "SJ-PS"
    if "NL-P" in label:
        label = "INLJ-P"
    ax.plot(
        group[attribute],
        group["sum_join_time"] + group["sum_index_build_time"],
        marker=marker,
        fillstyle=fillstyle,
        color=color,
        linewidth=edgewidth,
        markeredgewidth=edgewidth,
        markersize=markersize,
    )
ax.tick_params(axis="both", which="major", labelsize=fontsize - 2)
ax.set_ylabel("System Latency (s)", fontweight="bold", fontsize=fontsize)
# ax.set_xticks([1, 2, 4, 8, 16, 32])
label = ax.set_xlabel("Threads with Face", fontweight="bold", fontsize=fontsize + 1, labelpad=-1)  # 设置常规部分
x, y = label.get_position()
label.set_position((x - 0.15, y))
# t = ax.text(
#     0.76,
#     -0.2,
#     "Face",
#     transform=ax.transAxes,
#     style="italic",
#     weight="bold",
#     fontsize=fontsize + 1,
#     verticalalignment="top",
#     horizontalalignment="right",
# )

legend_handles2 = []
for label, setting in label_settings.items():
    if "SJ-PS" in label:
        label = "SJ-PS"
    if "NL-P" in label:
        label = "INLJ-P"
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
    bbox_to_anchor=(0.93, 1.05),
    ncol=4,
    fontsize=fontsize - 2,
    edgecolor="black",
    columnspacing=0.2,
    handletextpad=0.1,
    borderpad=0.1,
)
for spine in ax.spines.values():
    spine.set_color("blue")
# plt.yscale('log')

plt.subplots_adjust(wspace=0.2)
# plt.tight_layout()
plt.savefig("lsm_join/plot/test_concurrent.pdf", bbox_inches="tight", pad_inches=0.02)
