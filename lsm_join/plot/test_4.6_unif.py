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
fontsize = 15
sci_palettes.register_cmap()
test_names = ["5.8_unif_ck"]
for test_name in test_names:
    write_csv_from_txt(test_name)
    process_csv(test_name)

alpha = "abcdefghijklmnopqrstuvwxyz"

c_k = pd.read_csv("lsm_join/csv_result/5.8_unif_ck.csv")

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
    "NL-NS/V-CI": {"color": colors[4], "marker": "^"},
    "NL-NS/V-EI": {"color": colors[4], "marker": "^"},
    "NL-NS/V-LI": {"color": colors[4], "marker": "^"},
    "NL-NS/S-CI": {"color": colors[4], "marker": "^"},
    "NL-NS/S-EI": {"color": colors[3], "marker": "s"},
    "NL-NS/S-LI": {"color": colors[1], "marker": "d"},
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
        if i < 6:
            gs_nested = gridspec.GridSpecFromSubplotSpec(
                1, 2, subplot_spec=gs_main[0, i : i + 2], wspace=0.1, hspace=0
            )
            ax = fig.add_subplot(gs_nested[0, 0])
        else:
            gs_nested = gridspec.GridSpecFromSubplotSpec(
                1,
                2,
                subplot_spec=gs_main[1, i - 6 : i - 6 + 2],
                wspace=0.1,
                hspace=0,
            )
            ax = fig.add_subplot(gs_nested[0, 0])
        # print(group)
        pivot_table = group.pivot_table(
            index="k_s",
            columns="c_s",
            values="sum_join_time",
            aggfunc="mean",
        )
        c = ax.pcolormesh(
            pivot_table.columns,
            pivot_table.index,
            pivot_table,
            shading="auto",
            cmap=cmap1 if label not in ["NL-P", "HJ-N"] else cmap1,
        )
        ax.set_xlabel(r"$c_r,c_s$", fontsize=fontsize, labelpad=0)
        ax.set_ylabel(r"$d_r,d_s$", fontsize=fontsize, labelpad=0)
        ax.tick_params(axis="both", which="both", length=0, labelsize=fontsize)
        ax.yaxis.set_major_formatter(ticker.FormatStrFormatter("%d"))
        color_bar = fig.colorbar(c, ax=ax)
        # color_bar.set_label("Join (s)")
        color_bar.locator = ticker.MaxNLocator(nbins=5, integer=True)
        color_bar.ax.tick_params(axis="y", which="both", length=0, labelsize=fontsize)

        color_bar.update_ticks()
        i += 1

        pivot_table = group.pivot_table(
            index="k_s",
            columns="c_s",
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

        ax.set_xlabel(r"$c_r,c_s$", fontsize=fontsize, labelpad=0)
        # axes[i].set_ylabel("k_r")
        ax.tick_params(axis="both", which="both", length=0, labelsize=fontsize)
        # axes[i].yaxis.set_major_formatter(ticker.FormatStrFormatter("%d"))
        ax.set_yticklabels([])
        color_bar = fig.colorbar(c, ax=ax)
        # color_bar.set_label("Index Build (s)")
        color_bar.locator = ticker.MaxNLocator(nbins=5, integer=True)
        color_bar.ax.tick_params(axis="y", which="both", length=0, labelsize=fontsize)

        if label == "NL-P" or label == "HJ-N":
            color_bar.set_ticks([0])
        color_bar.update_ticks()
        x = ax.get_position().x0
        y = ax.get_position().y0
        fig.text(
            x - 0.04,
            y - 0.13,
            f"({alpha[j]}) {label}",
            fontsize=fontsize + 1,
            fontweight="bold",
        )
        j += 1
        i += 1

cbar_ax = fig.add_axes([0.25, 0.9, 0.1, 0.04])  # [left, bottom, width, height]
norm = mcolors.Normalize(vmin=0, vmax=1)
sm = plt.cm.ScalarMappable(cmap=cmap1, norm=norm)
sm.set_array([])
cbar = fig.colorbar(sm, cax=cbar_ax, orientation="horizontal")
cbar.ax.tick_params(axis="both", which="both", length=0)
cbar.set_ticks([])
# cbar.set_label("Index Build Latency (s)")
fig.text(
    cbar_ax.get_position().x1 + 0.08,
    cbar_ax.get_position().y0-0.002,
    "Join Latency (s)",
    va="bottom",
    ha="right",
    fontsize=fontsize+1,
)

cbar_ax = fig.add_axes([0.45, 0.9, 0.1, 0.04])  # [left, bottom, width, height]
norm = mcolors.Normalize(vmin=0, vmax=1)
sm = plt.cm.ScalarMappable(cmap=cmap2, norm=norm)
sm.set_array([])
cbar = fig.colorbar(sm, cax=cbar_ax, orientation="horizontal")
cbar.ax.tick_params(axis="both", which="both", length=0)
cbar.set_ticks([])
fig.text(
    cbar_ax.get_position().x1 + 0.11,
    cbar_ax.get_position().y0-0.002,
    "Index Build Latency (s)",
    va="bottom",
    ha="right",
    fontsize=fontsize+1,
)
plt.savefig("lsm_join/plot/test_5.8_unif.pdf", bbox_inches="tight", pad_inches=0.02)
plt.close()
