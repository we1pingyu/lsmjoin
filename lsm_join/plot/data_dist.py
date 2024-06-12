import matplotlib.pyplot as plt
import matplotlib as mpl
import pandas as pd
import matplotlib.lines as mlines
import numpy as np
from csv_process import write_csv_from_txt, process_csv
import sci_palettes
from scipy.spatial.distance import pdist, squareform
import matplotlib.ticker as ticker
import matplotlib.gridspec as gridspec
import matplotlib.colors as mcolors

mpl.rcParams["font.family"] = "Times New Roman"
mpl.use("Agg")
# plt.rcParams['text.usetex'] = True

edgewidth = 1.5
markersize = 5
fontsize = 14
sci_palettes.register_cmap()
test_names = ["c_k"]
for test_name in test_names:
    write_csv_from_txt(test_name)
    process_csv(test_name)

alpha = "abcdefghijklmnopqrstuvwxyz"

c_k = pd.read_csv("lsm_join/csv_result/c_k.csv")

dfs = [
    {"df": c_k, "title": ["c_r", "k_r"]},
]
# fig, axes = plt.subplots(3, 7, figsize=(21, 7), constrained_layout=True)
# axes = axes.flatten()
fig = plt.figure(figsize=(21, 5))
# gs = gridspec.GridSpec(3, 7)
gs_main = gridspec.GridSpec(
    2, 7, figure=fig, wspace=0.5, hspace=0.6, width_ratios=[1, 1, 1, 1, 1, 1, 1.2]
)


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
    "P-Comp-ISJ": {"color": colors[4], "marker": "^"},
    "P-Eager-ISJ": {"color": colors[4], "marker": "^"},
    "P-CLazy-ISJ": {"color": colors[4], "marker": "^"},
    "P-INLJ": {"color": colors[4], "marker": "^"},
    "CLazy-INLJ": {"color": colors[3], "marker": "s"},
    "2Eager-ISJ": {"color": colors[1], "marker": "d"},
    "2CComp-ISJ": {"color": colors[2], "marker": "H"},
    "HJ": {"color": colors[4], "marker": "^"},
}
cmap1 = "crest"
cmap2 = "copper_r"
i = 0
j = 0
for df_info in dfs:
    df = df_info["df"]
    attributes = df_info["title"]
    title = attributes

    for label in label_settings:
        group = df[df["label"] == label]
        if label != "P-INLJ" and label != "HJ":
            if i < 7:
                gs_nested = gridspec.GridSpecFromSubplotSpec(
                    1, 2, subplot_spec=gs_main[0, i : i + 2], wspace=0.1, hspace=0
                )
                ax = fig.add_subplot(gs_nested[0, 0])
            else:
                gs_nested = gridspec.GridSpecFromSubplotSpec(
                    1,
                    2,
                    subplot_spec=gs_main[1, i - 7 : i - 7 + 2],
                    wspace=0.1,
                    hspace=0,
                )
                ax = fig.add_subplot(gs_nested[0, 0])
        else:
            if i < 7:
                gs_nested = gridspec.GridSpecFromSubplotSpec(
                    1, 1, subplot_spec=gs_main[0, i : i + 1], hspace=0.0
                )
                ax = fig.add_subplot(gs_nested[0, 0])
            else:
                gs_nested = gridspec.GridSpecFromSubplotSpec(
                    1, 1, subplot_spec=gs_main[1, i - 7 : i - 7 + 1], hspace=0.1
                )
                ax = fig.add_subplot(gs_nested[0, 0])
        # print(group)
        pivot_table = group.pivot_table(
            index="k_r",
            columns="c_r",
            values="sum_join_time",
            aggfunc="mean",
        )
        c = ax.pcolormesh(
            pivot_table.columns,
            pivot_table.index,
            pivot_table,
            shading="auto",
            cmap=cmap1 if label not in ["P-INLJ", "HJ"] else cmap2,
        )
        ax.set_xlabel(r"$c_r$", fontsize=fontsize - 2, labelpad=0)
        ax.set_ylabel(r"$k_r$", fontsize=fontsize - 2, labelpad=0)
        ax.tick_params(axis="both", which="both", length=0, labelsize=fontsize - 2)
        ax.yaxis.set_major_formatter(ticker.FormatStrFormatter("%d"))
        color_bar = fig.colorbar(c, ax=ax)
        # color_bar.set_label("Join (s)")
        color_bar.locator = ticker.MaxNLocator(nbins=5, integer=True)
        color_bar.ax.tick_params(axis="y", which="both", length=0)

        color_bar.update_ticks()
        i += 1

        if label != "P-INLJ" and label != "HJ":
            pivot_table = group.pivot_table(
                index="k_r",
                columns="c_r",
                values="sum_index_build_time",
                aggfunc="mean",
            )
            ax = fig.add_subplot(gs_nested[0, 1])
            c = ax.pcolormesh(
                pivot_table.columns,
                pivot_table.index,
                pivot_table,
                shading="auto",
                cmap=cmap2,
            )
            ax.set_xlabel(r"$c_r$", fontsize=fontsize - 2, labelpad=0)
            # axes[i].set_ylabel("k_r")
            ax.tick_params(axis="both", which="both", length=0, labelsize=fontsize - 2)
            # axes[i].yaxis.set_major_formatter(ticker.FormatStrFormatter("%d"))
            ax.set_yticklabels([])
            color_bar = fig.colorbar(c, ax=ax)
            # color_bar.set_label("Index Build (s)")
            color_bar.locator = ticker.MaxNLocator(nbins=5, integer=True)
            if label == "P-Comp-ISJ":
                color_bar.locator = ticker.MaxNLocator(nbins=2, integer=True)
            color_bar.ax.tick_params(axis="y", which="both", length=0)

            if label == "P-INLJ" or label == "HJ":
                color_bar.set_ticks([0])
            color_bar.update_ticks()
            x = ax.get_position().x0
            y = ax.get_position().y0
            fig.text(
                x - 0.04,
                y - 0.11,
                f"({alpha[j]}) {label}",
                fontsize=fontsize,
                fontweight="bold",
            )
            j += 1
            i += 1
        else:
            x = ax.get_position().x0
            y = ax.get_position().y0
            fig.text(
                x + 0.01,
                y - 0.11,
                f"({alpha[j]}) {label}",
                fontsize=fontsize,
                fontweight="bold",
            )
            j += 1
cbar_ax = fig.add_axes([0.3, 0.9, 0.1, 0.04])  # [left, bottom, width, height]
norm = mcolors.Normalize(vmin=0, vmax=1)
sm = plt.cm.ScalarMappable(cmap=cmap1, norm=norm)
sm.set_array([])
cbar = fig.colorbar(sm, cax=cbar_ax, orientation="horizontal")
cbar.ax.tick_params(axis="both", which="both", length=0)
cbar.set_ticks([])
# cbar.set_label("Index Build Latency (s)")
fig.text(
    cbar_ax.get_position().x1 + 0.07,
    cbar_ax.get_position().y0,
    "Join Latency (s)",
    va="bottom",
    ha="right",
    fontsize=fontsize,
)

cbar_ax = fig.add_axes([0.52, 0.9, 0.1, 0.04])  # [left, bottom, width, height]
norm = mcolors.Normalize(vmin=0, vmax=1)
sm = plt.cm.ScalarMappable(cmap=cmap2, norm=norm)
sm.set_array([])
cbar = fig.colorbar(sm, cax=cbar_ax, orientation="horizontal")
cbar.ax.tick_params(axis="both", which="both", length=0)
cbar.set_ticks([])
fig.text(
    cbar_ax.get_position().x1 + 0.1,
    cbar_ax.get_position().y0,
    "Index Build Latency (s)",
    va="bottom",
    ha="right",
    fontsize=fontsize,
)
plt.savefig("lsm_join/plot/data_unif.pdf", bbox_inches="tight", pad_inches=0.02)
plt.close()


fig, axes = plt.subplots(3, 2, figsize=(8, 6), sharex=True)
axes = axes.ravel(order="F")
fig.subplots_adjust(hspace=0.08)

test_names = ["c_skewness"]
for test_name in test_names:
    write_csv_from_txt(test_name)
    process_csv(test_name)

c_k = pd.read_csv("lsm_join/csv_result/c_skewness.csv")
dfs = [
    {"df": c_k, "title": ["c_r", "k_s"]},
]
label_settings = {
    "P-INLJ": {"color": colors[0], "marker": "^"},
    "Comp-INLJ": {"color": colors[1], "marker": "d"},
    "CLazy-INLJ": {"color": colors[2], "marker": "H"},
    "2CLazy-ISJ": {"color": colors[3], "marker": "o"},
    "2Eager-ISJ": {"color": colors[4], "marker": "s"},
    "HJ": {"color": colors[5], "marker": "x"},
}
for df_info in dfs:
    df = df_info["df"]
    attributes = df_info["title"]
    title = attributes
    c_r_values = [1, 5, 10]
    for ax, c_r_value in zip(axes[:3], c_r_values):
        for label in label_settings:
            group = df[(df["label"] == label) & (df["c_r"] == c_r_value)]
            ax.plot(
                group["k_s"],
                group["sum_join_time"],
                marker=label_settings[label]["marker"],
                color=label_settings[label]["color"],
                fillstyle="none",
                linestyle="-",
                linewidth=edgewidth,
                markeredgewidth=edgewidth,
                markersize=markersize,
            )
            ax.text(
                0.5,
                0.7,
                r"$c_r={}$".format(c_r_value),
                fontsize=fontsize - 2,
                ha="center",
                va="center",
                transform=ax.transAxes,
            )
    axes[2].set_xlabel("Join Time vs. Skewness", fontsize=fontsize, fontweight="bold")
    for ax, c_r_value in zip(axes[3:], c_r_values):
        for label in label_settings:
            group = df[(df["label"] == label) & (df["c_r"] == c_r_value)]
            ax.plot(
                group["k_s"],
                group["sum_index_build_time"],
                marker=label_settings[label]["marker"],
                color=label_settings[label]["color"],
                fillstyle="none",
                linestyle="--",
                linewidth=edgewidth,
                markeredgewidth=edgewidth,
                markersize=markersize,
            )
            ax.text(
                0.5,
                0.7,
                r"$c_r={}$".format(c_r_value),
                fontsize=fontsize - 2,
                ha="center",
                va="center",
                transform=ax.transAxes,
            )
    axes[-1].set_xlabel(
        "Index Build Time vs. Skewness", fontsize=fontsize, fontweight="bold"
    )
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
            linestyle="None",
            markersize=markersize,
            fillstyle="none",
            label=label,
        )
    )

fig.legend(
    handles=legend_handles2,
    bbox_to_anchor=(0.65, 0.98),
    ncol=3,
    fontsize=fontsize - 2,
    edgecolor="black",
)
fig.legend(
    handles=legend_handles1,
    bbox_to_anchor=(0.88, 0.98),
    ncol=1,
    fontsize=fontsize - 2,
    edgecolor="black",
)
plt.savefig("lsm_join/plot/data_skew.pdf", bbox_inches="tight", pad_inches=0.02)
plt.close()
