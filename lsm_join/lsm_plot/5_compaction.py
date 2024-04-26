import matplotlib.pyplot as plt
import pandas as pd
import matplotlib.lines as mlines
from csv_process import write_csv_from_txt, process_csv

test_names = ["T", "T_t", "K", "buffer_size", "buffer_size_t"]
for test_name in test_names:
    write_csv_from_txt(test_name)
    process_csv(test_name)

T = pd.read_csv("lsm_join/lsm_res/T.csv")
T_t = pd.read_csv("lsm_join/lsm_res/T_t.csv")
K = pd.read_csv("lsm_join/lsm_res/K.csv")
buffer_size = pd.read_csv("lsm_join/lsm_res/buffer_size.csv")
buffer_size_t = pd.read_csv("lsm_join/lsm_res/buffer_size_t.csv")

# 创建数据框的数组
dfs = [
    {"df": T, "title": "T"},
    {"df": T_t, "title": "T_t"},
    {"df": K, "title": "K"},
    {"df": buffer_size, "title": "buffer_size"},
    {"df": buffer_size_t, "title": "buffer_size_t"},
]

# 设置图的大小和子图布局
fig, axes = plt.subplots(1, 5, figsize=(16, 3), sharey=True)  # 一行两列

colors = ["#3E8D27", "#A22025", "#1432F5"]

label_settings = {
    "CComp-INLJ": {"color": colors[0], "marker": "o"},
    "CEager-INLJ": {"color": colors[2], "marker": "o"},
    "2Comp-ISJ": {"color": colors[1], "marker": "o"},
    "2CLazy-ISJ": {"color": colors[1], "marker": "s"},
    "1CLazy-ISJ": {"color": colors[0], "marker": "s"},
}


for i, (ax, df_info) in enumerate(zip(axes, dfs)):
    df = df_info["df"]
    attribute = df_info["title"]
    fillstyle = "none"
    title = attribute
    if attribute == "T_t":
        attribute = "T"
        title = "T (Theoretical)"
    if title == "buffer_size":
        df["M_MB"] = df["M"] / (2**20)
        attribute = "M_MB"
        title = "buffer size"
    if title == "buffer_size_t":
        df["M_MB"] = df["M"] / (2**20)
        attribute = "M_MB"
        title = "buffer size (Theoretical)"

    for label, group in df.groupby("label"):
        color = label_settings[label]["color"]
        marker = label_settings[label]["marker"]

        ax.plot(
            group[attribute],
            group["sum_join_time"],
            marker=marker,
            fillstyle=fillstyle,
            color=color,
            linewidth=1.5,
            markeredgewidth=1.5,
            markersize=5,
        )
        ax.plot(
            group[attribute],
            group["sum_index_build_time"],
            linestyle="--",
            marker=marker,
            fillstyle=fillstyle,
            color=color,
            linewidth=1.5,
            markeredgewidth=1.5,
            markersize=5,
        )

    if i == 0:
        ax.set_ylabel("System Latency (s)")
    ax.set_xlabel(title)
    if attribute == 'K':
        ax.set_xticks([2, 3, 4, 5])
    elif attribute == 'T':
        ax.set_xticks([2, 4, 10, 40])
    elif attribute == 'M_MB':
        ax.set_xticks([4, 16, 32, 64])
        ax.set_xticklabels(["4M", "16M", "32M", "64M"])


# 创建图例
legend_handles = [
    mlines.Line2D(
        [], [], color="black", linestyle="-", linewidth=0.5, label="Join Time"
    ),
    mlines.Line2D(
        [], [], color="black", linestyle="--", linewidth=0.5, label="Index Build Time"
    ),
]

for label, setting in label_settings.items():
    legend_handles.append(
        mlines.Line2D(
            [],
            [],
            color=setting["color"],
            marker=setting["marker"],
            linestyle="None",
            markersize=4,
            fillstyle="none",
            label=label,
        )
    )

fig.legend(handles=legend_handles, bbox_to_anchor=(0.76, 1.04), ncol=7, fontsize=8)

# plt.yscale('log')

plt.subplots_adjust(wspace=0.01, hspace=0.1)
plt.tight_layout()
plt.savefig("lsm_join/lsm_plot/5_compaction.pdf", bbox_inches="tight", pad_inches=0.02)
plt.close()
