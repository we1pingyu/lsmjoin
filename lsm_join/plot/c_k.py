import matplotlib.pyplot as plt
import pandas as pd
import matplotlib.lines as mlines
import numpy as np
from csv_process import write_csv_from_txt, process_csv
import sci_palettes
from scipy.spatial.distance import pdist, squareform
import matplotlib.ticker as ticker

edgewidth = 1.5
markersize = 5
sci_palettes.register_cmap()
test_names = ["c_k"]
for test_name in test_names:
    write_csv_from_txt(test_name)
    process_csv(test_name)

c_k = pd.read_csv("lsm_join/csv_result/c_k.csv")


# 创建数据框的数组
dfs = [
    {"df": c_k, "title": ["c_r", "k_r"]},
]

# 设置图的大小和子图布局
fig, axes = plt.subplots(2, 6, figsize=(21, 6))
axes = axes.flatten()

colors = ["#3E8D27", "#A22025", "#1432F5"]
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
    "2Eager-ISJ": {"color": colors[1], "marker": "d"},
    "2CLazy-ISJ": {"color": colors[2], "marker": "H"},
    "Comp-INLJ": {"color": colors[3], "marker": "s"},
    "Grace-HJ": {"color": colors[4], "marker": "^"},
    "P-INLJ": {"color": colors[4], "marker": "^"},
}

global_min = np.inf
global_max = -np.inf
for df_info in dfs:
    df = df_info["df"]
    global_min_join = min(global_min, df["sum_join_time"].min())
    global_max_join = max(global_max, df["sum_join_time"].max())
    global_min_index = min(global_min, df["sum_index_build_time"].min())
    global_max_index = max(global_max, df["sum_index_build_time"].max())

i = 0
for df_info in dfs:
    df = df_info["df"]
    attributes = df_info["title"]
    title = attributes

    for label, group in df.groupby("label"):
        if label not in label_settings:
            continue
        pivot_table = group.pivot_table(
            index="k_r",
            columns="c_r",
            values="sum_join_time",
            aggfunc="mean",
        )
        c = axes[i].pcolormesh(
            pivot_table.columns,
            pivot_table.index,
            pivot_table,
            shading="auto",
            cmap="cividis_r",
            # vmin=global_min_join,
            # vmax=global_max_join,
        )
        axes[i].set_xlabel("c_r")
        axes[i].set_ylabel("k_r")
        color_bar = fig.colorbar(c, ax=axes[i])
        color_bar.set_label("Join (s)")
        color_bar.locator = ticker.MaxNLocator(nbins=5, integer=True)
        color_bar.update_ticks()
        i += 1

        pivot_table = group.pivot_table(
            index="k_r", columns="c_r", values="sum_index_build_time", aggfunc="mean"
        )
        c = axes[i].pcolormesh(
            pivot_table.columns,
            pivot_table.index,
            pivot_table,
            shading="auto",
            cmap="cividis_r",
            # vmin=global_min_index,
            # vmax=global_max_index,
        )
        axes[i].set_xlabel("c_r")
        axes[i].set_ylabel("k_r")
        color_bar = fig.colorbar(c, ax=axes[i])
        color_bar.set_label("Index Build (s)")
        color_bar.locator = ticker.MaxNLocator(nbins=5, integer=True)
        if label == "P-INLJ" or label == "Grace-HJ":
            color_bar.set_ticks([0])
        color_bar.update_ticks()
        y = axes[i].get_position().y0
        if i >= 6:
            y = axes[i].get_position().y0 - 0.1
        fig.text(
            i % 6 / 6 - 0.02,
            y,
            label,
            fontsize=12,
        )
        i += 1


# plt.yscale('log')

# plt.subplots_adjust(wspace=0.01, hspace=0.1, bottom=0.4)
plt.tight_layout()
plt.subplots_adjust(hspace=0.4)
plt.savefig("lsm_join/plot/c_k.pdf", bbox_inches="tight", pad_inches=0.02)
plt.close()


test_names = ["c_skewness"]
for test_name in test_names:
    write_csv_from_txt(test_name)
    process_csv(test_name)

c_k = pd.read_csv("lsm_join/csv_result/c_skewness.csv")


# 创建数据框的数组
dfs = [
    {"df": c_k, "title": ["c_r", "k_s"]},
]

# 设置图的大小和子图布局
fig, axes = plt.subplots(2, 6, figsize=(21, 6))
axes = axes.flatten()

colors = ["#3E8D27", "#A22025", "#1432F5"]
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
    "2Eager-ISJ": {"color": colors[1], "marker": "d"},
    "2CLazy-ISJ": {"color": colors[2], "marker": "H"},
    "Comp-INLJ": {"color": colors[3], "marker": "s"},
    "Grace-HJ": {"color": colors[4], "marker": "^"},
    "P-INLJ": {"color": colors[4], "marker": "^"},
}

global_min = np.inf
global_max = -np.inf
for df_info in dfs:
    df = df_info["df"]
    global_min_join = min(global_min, df["sum_join_time"].min())
    global_max_join = max(global_max, df["sum_join_time"].max())
    global_min_index = min(global_min, df["sum_index_build_time"].min())
    global_max_index = max(global_max, df["sum_index_build_time"].max())

i = 0
for df_info in dfs:
    df = df_info["df"]
    attributes = df_info["title"]
    title = attributes

    for label, group in df.groupby("label"):
        if label not in label_settings:
            continue
        pivot_table = group.pivot_table(
            index="k_s",
            columns="c_r",
            values="sum_join_time",
            aggfunc="mean",
        )
        c = axes[i].pcolormesh(
            pivot_table.columns,
            pivot_table.index,
            pivot_table,
            shading="auto",
            cmap="cividis_r",
            # vmin=global_min_join,
            # vmax=global_max_join,
        )
        axes[i].set_xlabel("c_r")
        axes[i].set_ylabel("skewness")
        color_bar = fig.colorbar(c, ax=axes[i])
        color_bar.set_label("Join (s)")
        color_bar.locator = ticker.MaxNLocator(nbins=4, integer=True)
        color_bar.update_ticks()
        i += 1

        pivot_table = group.pivot_table(
            index="k_s", columns="c_r", values="sum_index_build_time", aggfunc="mean"
        )
        c = axes[i].pcolormesh(
            pivot_table.columns,
            pivot_table.index,
            pivot_table,
            shading="auto",
            cmap="cividis_r",
            # vmin=global_min_index,
            # vmax=global_max_index,
        )
        axes[i].set_xlabel("c_r")
        axes[i].set_ylabel("skewness")
        color_bar = fig.colorbar(c, ax=axes[i])
        color_bar.set_label("Index Build (s)")
        color_bar.locator = ticker.MaxNLocator(nbins=5, integer=True)
        if label == "P-INLJ" or label == "Grace-HJ":
            color_bar.set_ticks([0])
        color_bar.update_ticks()
        y = axes[i].get_position().y0
        if i >= 6:
            y = axes[i].get_position().y0 - 0.1
        fig.text(
            i % 6 / 6 - 0.02,
            y,
            label,
            fontsize=12,
        )
        i += 1


# plt.yscale('log')

# plt.subplots_adjust(wspace=0.01, hspace=0.1, bottom=0.4)
plt.tight_layout()
plt.subplots_adjust(hspace=0.4)
plt.savefig("lsm_join/plot/c_skewness.pdf", bbox_inches="tight", pad_inches=0.02)
plt.close()


test_names = ["loops_size"]
for test_name in test_names:
    write_csv_from_txt(test_name)
    process_csv(test_name)

c_k = pd.read_csv("lsm_join/csv_result/loops_size.csv")


# 创建数据框的数组
dfs = [
    {"df": c_k, "title": ["num_loop", "s_tuples"]},
]

# 设置图的大小和子图布局
fig, axes = plt.subplots(2, 6, figsize=(21, 6))
axes = axes.flatten()

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
    "2Eager-ISJ": {"color": colors[1], "marker": "d"},
    "2CLazy-ISJ": {"color": colors[2], "marker": "H"},
    "Comp-INLJ": {"color": colors[3], "marker": "s"},
    "Grace-HJ": {"color": colors[4], "marker": "^"},
    "P-INLJ": {"color": colors[4], "marker": "^"},
}

global_min = np.inf
global_max = -np.inf
for df_info in dfs:
    df = df_info["df"]
    global_min_join = min(global_min, df["sum_join_time"].min())
    global_max_join = max(global_max, df["sum_join_time"].max())
    global_min_index = min(global_min, df["sum_index_build_time"].min())
    global_max_index = max(global_max, df["sum_index_build_time"].max())

i = 0
for df_info in dfs:
    df = df_info["df"]
    attributes = df_info["title"]
    title = attributes

    for label, group in df.groupby("label"):
        if label not in label_settings:
            continue
        pivot_table = group.pivot_table(
            index="s_tuples",
            columns="num_loops",
            values="sum_join_time",
            aggfunc="mean",
        )
        c = axes[i].pcolormesh(
            pivot_table.columns,
            pivot_table.index,
            pivot_table,
            shading="auto",
            cmap="cividis_r",
            # vmin=global_min_join,
            # vmax=global_max_join,
        )
        axes[i].set_xlabel("c_r")
        axes[i].set_ylabel("skewness")
        color_bar = fig.colorbar(c, ax=axes[i])
        color_bar.set_label("Join (s)")
        color_bar.locator = ticker.MaxNLocator(nbins=4, integer=True)
        color_bar.update_ticks()
        i += 1

        pivot_table = group.pivot_table(
            index="s_tuples", columns="num_loops", values="sum_index_build_time", aggfunc="mean"
        )
        c = axes[i].pcolormesh(
            pivot_table.columns,
            pivot_table.index,
            pivot_table,
            shading="auto",
            cmap="cividis_r",
            # vmin=global_min_index,
            # vmax=global_max_index,
        )
        axes[i].set_xlabel("c_r")
        axes[i].set_ylabel("skewness")
        color_bar = fig.colorbar(c, ax=axes[i])
        color_bar.set_label("Index Build (s)")
        color_bar.locator = ticker.MaxNLocator(nbins=5, integer=True)
        if label == "P-INLJ" or label == "Grace-HJ":
            color_bar.set_ticks([0])
        color_bar.update_ticks()
        y = axes[i].get_position().y0
        if i >= 6:
            y = axes[i].get_position().y0 - 0.1
        fig.text(
            i % 6 / 6 - 0.02,
            y,
            label,
            fontsize=12,
        )
        i += 1

plt.tight_layout()
plt.subplots_adjust(hspace=0.4)
plt.savefig("lsm_join/plot/c_skewness.pdf", bbox_inches="tight", pad_inches=0.02)
plt.close()
