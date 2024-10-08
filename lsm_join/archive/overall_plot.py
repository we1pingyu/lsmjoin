import matplotlib.pyplot as plt
import numpy as np
import re
import matplotlib as mpl
import sci_palettes
mpl.rcParams["font.family"] = "Times New Roman"
mpl.use("Agg")
sci_palettes.register_cmap()
colors = [
    "#a59344",
    "#7b463b",
    "#00b89f",
    "#191970",
    "#3d1c02",
    "#456789",
    "#05a3ad",
    "#4c4f56",
    "#4d3c2d",
    "#166461",
    "#9bafad",
    "#990066",
    "#5ba8ff",
    "##002c2b",
]
style = "tab10"
plt.set_cmap(style)
cmap = plt.get_cmap(style)
colors = cmap.colors
darkening_factor = 0.5
colors = [
    (r * darkening_factor, g * darkening_factor, b * darkening_factor)
    for r, g, b in colors
]

legend_fontsize = 14


def plot_data(datasets, titles, filename, colors):
    for data in datasets:
        print(data)
    num_datasets = 6
    fig, axs = plt.subplots(3, num_datasets, figsize=(4 * num_datasets, 12))
    fig.legend(
        [
            plt.Rectangle((0, 0), 1, 1, color="black"),
            plt.Rectangle((0, 0), 1, 1, color="black", fill=False, hatch="///"),
        ],
        ["Join", "Index build"],
        # loc="lower right",
        bbox_to_anchor=(0.99, 0.99),
        ncol=1,
        fontsize=legend_fontsize,
    )
    max_y_val = 0
    for data in datasets[:num_datasets]:
        try:
            for label in data.keys():
                total_vals = [
                    data[label][j][0] + data[label][j][1]
                    for j in range(len(data[label]))
                ]
                max_y_val = max(max_y_val, max(total_vals))
        except:
            continue
    axs = axs.flatten()
    for index, (ax, data, title) in enumerate(zip(axs, datasets, titles)):
        num_ratios = 1
        x = np.arange(num_ratios)
        width = 0.05
        handles, labels = [], []
        bax = brokenaxes(ylims=((0, 120), (200, 250)), hspace=.05)  # Setup broken y-axis
        for i, (label, color) in enumerate(zip(data.keys(), colors)):
            try:
                join_vals = data[label][0][0]
                build_vals = data[label][0][1]
                join_bar = bax.bar(
                    x - width / 2 + i * width,
                    join_vals,
                    width,
                    edgecolor=color,
                    color=color,
                    label=f"{label} Join Time",
                )
                bax.bar(
                    x - width / 2 + i * width,
                    build_vals,
                    width,
                    edgecolor=color,
                    fill=False,
                    hatch="///",
                    bottom=join_vals,
                    label=f"{label} Index Build Time",
                )
                handles.append(join_bar)
                labels.append(label)
            except:
                break
        ax.set_ylim(0, max_y_val)
        # ax.set_xlabel("Updates:Joins Ratios")
        if index == 0:
            ax.set_ylabel("System Latency (s)", fontsize=18)
        else:
            ax.set_yticklabels([])
        if index == num_datasets - 1:
            ax.legend(
                handles,
                labels,
                bbox_to_anchor=(1.0, 0.8),
                loc="upper left",
                fontsize=legend_fontsize,
            )
        ax.set_xticks([])
        ax.set_title(title, fontsize=18)
    max_y_val = 0
    for data in datasets[num_datasets : 2 * num_datasets]:
        try:
            for label in data.keys():
                total_vals = [
                    data[label][j][0] + data[label][j][1]
                    for j in range(len(data[label]))
                ]
                max_y_val = max(max_y_val, max(total_vals))
        except:
            continue
    for index, (ax, data) in enumerate(
        zip(
            axs[num_datasets : 2 * num_datasets],
            datasets[num_datasets : 2 * num_datasets],
        )
    ):
        num_ratios = 1
        x = np.arange(num_ratios)
        width = 0.05
        handles, labels = [], []
        for i, (label, color) in enumerate(zip(data.keys(), colors)):
            join_vals = data[label][0][0]
            build_vals = data[label][0][1]
            join_bar = ax.bar(
                x - width / 2 + i * width,
                join_vals,
                width,
                edgecolor=color,
                color=color,
                label=f"{label} Join Time",
            )
            ax.bar(
                x - width / 2 + i * width,
                build_vals,
                width,
                edgecolor=color,
                fill=False,
                hatch="///",
                bottom=join_vals,
                label=f"{label} Index Build Time",
            )
            handles.append(join_bar)
            labels.append(label)
        ax.set_ylim(0, max_y_val)
        # ax.set_xlabel("Updates:Joins Ratios")
        if index == 0:
            ax.set_ylabel("System Latency (s)", fontsize=18)
        else:
            ax.set_yticklabels([])
        if index == num_datasets - 1:
            ax.legend(
                handles,
                labels,
                bbox_to_anchor=(1.0, 1),
                loc="upper left",
                fontsize=legend_fontsize,
            )
        ax.set_xticks([])
    max_y_val = 0
    for data in datasets[2 * num_datasets : 3 * num_datasets]:
        try:
            for label in data.keys():
                total_vals = [
                    data[label][j][0] + data[label][j][1]
                    for j in range(len(data[label]))
                ]
                max_y_val = max(max_y_val, max(total_vals))
        except:
            continue
    for index, (ax, data) in enumerate(
        zip(
            axs[2 * num_datasets : 3 * num_datasets],
            datasets[2 * num_datasets : 3 * num_datasets],
        )
    ):
        num_ratios = 1
        x = np.arange(num_ratios)
        width = 0.05
        handles, labels = [], []
        for i, (label, color) in enumerate(zip(data.keys(), colors)):
            join_vals = data[label][0][0]
            build_vals = data[label][0][1]
            join_bar = ax.bar(
                x - width / 2 + i * width,
                join_vals,
                width,
                edgecolor=color,
                color=color,
                label=f"{label} Join Time",
            )
            ax.bar(
                x - width / 2 + i * width,
                build_vals,
                width,
                edgecolor=color,
                fill=False,
                hatch="///",
                bottom=join_vals,
                label=f"{label} Index Build Time",
            )
            handles.append(join_bar)
            labels.append(label)
        ax.set_ylim(0, max_y_val)
        # ax.set_xlabel("Updates:Joins Ratios")
        if index == 0:
            ax.set_ylabel("System Latency (s)", fontsize=18)
        else:
            ax.set_yticklabels([])
        if index == num_datasets - 1:
            ax.legend(
                handles,
                labels,
                bbox_to_anchor=(1.0, 1),
                loc="upper left",
                fontsize=legend_fontsize,
            )
        ax.set_xticks([])

    plt.tight_layout()
    plt.savefig(filename, bbox_inches="tight", pad_inches=0.02)
    plt.close()


def extract_and_organize_data(file_path):
    user_data = {
        "P-INLJ": [],
        "P-ISJ": [],
        "P-Eager-ISJ": [],
        "P-Lazy-ISJ": [],
        "P-Comp-ISJ": [],
        "P-CEager-ISJ": [],
        "P-CLazy-ISJ": [],
        "P-CComp-ISJ": [],
        "Grace-HJ": [],
    }

    times_list = []
    with open("lsm_join/" + file_path, "r") as file:
        for line in file:
            join_time_match = re.search(r"sum_join_time=([\d\.]+)", line)
            index_build_time_match = re.search(r"sum_index_build_time=([\d\.]+)", line)

            if join_time_match and index_build_time_match:
                join_time = float(join_time_match.group(1))
                index_build_time = float(index_build_time_match.group(1))
                times_list.append((join_time, index_build_time))

    keys = list(user_data.keys())
    for i, times in enumerate(times_list):
        key_index = i % len(keys)
        user_data[keys[key_index]].append(times)
        if i >= len(keys):
            break
    return user_data


def extract_and_organize_data_5(file_path):
    user_data = {
        "Eager-INLJ": [],
        "Lazy-INLJ": [],
        "Comp-INLJ": [],
        "CEager-INLJ": [],
        "CLazy-INLJ": [],
        "CComp-INLJ": [],
        "Grace-HJ": [],
    }

    times_list = []
    with open("lsm_join/" + file_path, "r") as file:
        for line in file:
            join_time_match = re.search(r"sum_join_time=([\d\.]+)", line)
            index_build_time_match = re.search(r"sum_index_build_time=([\d\.]+)", line)

            if join_time_match and index_build_time_match:
                join_time = float(join_time_match.group(1))
                index_build_time = float(index_build_time_match.group(1))
                times_list.append((join_time, index_build_time))

    keys = list(user_data.keys())
    for i, times in enumerate(times_list):
        key_index = i % len(keys)
        user_data[keys[key_index]].append(times)
        if i >= len(keys):
            break
    return user_data


def extract_and_organize_data_5_sj(file_path):
    user_data = {
        "NISJ": [],
        "1Eager-ISJ": [],
        "1Lazy-ISJ": [],
        "1Comp-ISJ": [],
        "2Eager-ISJ": [],
        "2Lazy-ISJ": [],
        "2Comp-ISJ": [],
        "1CEager-ISJ": [],
        "1CLazy-ISJ": [],
        "1CComp-ISJ": [],
        "2CEager-ISJ": [],
        "2CLazy-ISJ": [],
        "2CComp-ISJ": [],
    }

    times_list = []
    with open("lsm_join/" + file_path, "r") as file:
        for line in file:
            join_time_match = re.search(r"sum_join_time=([\d\.]+)", line)
            index_build_time_match = re.search(r"sum_index_build_time=([\d\.]+)", line)

            if join_time_match and index_build_time_match:
                join_time = float(join_time_match.group(1))
                index_build_time = float(index_build_time_match.group(1))
                times_list.append((join_time, index_build_time))

    keys = list(user_data.keys())
    for i, times in enumerate(times_list):
        key_index = i % len(keys)
        user_data[keys[key_index]].append(times)
        if i >= len(keys):
            break
    return user_data


user_data = extract_and_organize_data("user_id.txt")
movie_data = extract_and_organize_data("movie_id.txt")
osm_data = extract_and_organize_data("osm_cellids_800M_uint64.txt")
fb_data = extract_and_organize_data("fb_200M_uint64.txt")
unif_data = extract_and_organize_data("unif.txt")
skew_data = extract_and_organize_data("skew.txt")

user_data_5nlj = extract_and_organize_data_5("user_id_5nlj.txt")
movie_data_5nlj = extract_and_organize_data_5("movie_id_5nlj.txt")
osm_data_5nlj = extract_and_organize_data_5("osm_cellids_800M_uint64_5nlj.txt")
fb_data_5nlj = extract_and_organize_data_5("fb_200M_uint64_5nlj.txt")
unif_data_5nlj = extract_and_organize_data_5("unif_5nlj.txt")
skew_data_5nlj = extract_and_organize_data_5("skew_5nlj.txt")

user_data_5sj = extract_and_organize_data_5_sj("user_id_5sj.txt")
movie_data_5sj = extract_and_organize_data_5_sj("movie_id_5sj.txt")
osm_data_5sj = extract_and_organize_data_5_sj("osm_cellids_800M_uint64_5sj.txt")
fb_data_5sj = extract_and_organize_data_5_sj("fb_200M_uint64_5sj.txt")
unif_data_5sj = extract_and_organize_data_5_sj("unif_5sj.txt")
skew_data_5sj = extract_and_organize_data_5_sj("skew_5sj.txt")

datasets = [
    user_data,
    movie_data,
    osm_data,
    fb_data,
    unif_data,
    skew_data,
    user_data_5nlj,
    movie_data_5nlj,
    osm_data_5nlj,
    fb_data_5nlj,
    unif_data_5nlj,
    skew_data_5nlj,
    user_data_5sj,
    movie_data_5sj,
    osm_data_5sj,
    fb_data_5sj,
    unif_data_5sj,
    skew_data_5sj,
]
titles = ["User", "Movie", "OSM", "Face", "Unif", "Skew"]
plot_data(datasets, titles, "overall.pdf", colors)
