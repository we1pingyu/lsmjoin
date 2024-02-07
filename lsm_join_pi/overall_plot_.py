import matplotlib.pyplot as plt
import numpy as np
import re

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

fb_ratios = ["20M:1", "10M:1", "2M:1"]
user_ratios = ["16M:1", "8M:1", "1.6M:1"]


def plot_data(datasets, titles, filename, colors):
    num_datasets = 6
    fig, axs = plt.subplots(3, num_datasets, figsize=(4 * num_datasets, 12))
    fig.legend(
        [
            plt.Rectangle((0, 0), 1, 1, color="black"),
            plt.Rectangle((0, 0), 1, 1, color="black", fill=False, hatch="///"),
        ],
        ["Join", "Index build"],
        # loc="lower right",
        bbox_to_anchor=(0.995, 0.96),
        ncol=1,
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
        for i, (label, color) in enumerate(zip(data.keys(), colors)):
            try:
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
            except:
                break
        ax.set_ylim(0, max_y_val)
        # ax.set_xlabel("Updates:Joins Ratios")
        if index == 0:
            ax.set_ylabel("System Latency (s)", fontsize=16)
        else:
            ax.set_yticklabels([])
        if index == num_datasets - 1:
            ax.legend(handles, labels, bbox_to_anchor=(1.0, 0.7), loc="upper left")
        ax.set_xticks([])
        ax.set_title(title, fontsize=16)
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
            print(label)
            print(data)
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
            ax.set_ylabel("System Latency (s)", fontsize=16)
        else:
            ax.set_yticklabels([])
        if index == num_datasets - 1:
            ax.legend(handles, labels, bbox_to_anchor=(1.0, 1), loc="upper left")
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
            ax.set_ylabel("System Latency (s)", fontsize=16)
        else:
            ax.set_yticklabels([])
        if index == num_datasets - 1:
            ax.legend(handles, labels, bbox_to_anchor=(1.0, 1), loc="upper left")
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
    with open(file_path, "r") as file:
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
    with open(file_path, "r") as file:
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
    with open(file_path, "r") as file:
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
even_data = extract_and_organize_data("even.txt")
skew_data = extract_and_organize_data("skew.txt")

user_data_5nlj = extract_and_organize_data_5("user_id_5nlj.txt")
movie_data_5nlj = extract_and_organize_data_5("movie_id_5nlj.txt")
osm_data_5nlj = extract_and_organize_data_5("osm_cellids_800M_uint64_5nlj.txt")
fb_data_5nlj = extract_and_organize_data_5("fb_200M_uint64_5nlj.txt")
even_data_5nlj = extract_and_organize_data_5("even_5nlj.txt")
skew_data_5nlj = extract_and_organize_data_5("skew_5nlj.txt")

# user_data_5sj = extract_and_organize_data_5_sj("user_id_5sj.txt")
# movie_data_5sj = extract_and_organize_data_5_sj("movie_id_5sj.txt")
# osm_data_5sj = extract_and_organize_data_5_sj("osm_cellids_800M_uint64_5sj.txt")
# fb_data_5sj = extract_and_organize_data_5_sj("fb_200M_uint64_5sj.txt")
# even_data_5sj = extract_and_organize_data_5_sj("even_5sj.txt")
# skew_data_5sj = extract_and_organize_data_5_sj("skew_5sj.txt")
user_data_5sj = extract_and_organize_data("user_id.txt")
movie_data_5sj = extract_and_organize_data("movie_id.txt")
osm_data_5sj = extract_and_organize_data("osm_cellids_800M_uint64.txt")
fb_data_5sj = extract_and_organize_data("fb_200M_uint64.txt")
even_data_5sj = extract_and_organize_data("even.txt")
skew_data_5sj = extract_and_organize_data("skew.txt")

datasets = [
    user_data,
    movie_data,
    osm_data,
    fb_data,
    even_data,
    skew_data,
    user_data_5nlj,
    movie_data_5nlj,
    osm_data_5nlj,
    fb_data_5nlj,
    even_data_5nlj,
    skew_data_5nlj,
    user_data_5sj,
    movie_data_5sj,
    osm_data_5sj,
    fb_data_5sj,
    even_data_5sj,
    skew_data_5sj,
]
titles = ["user", "movie", "osm", "fb", "even", "skew"]
plot_data(datasets, titles, "overall_.pdf", colors)