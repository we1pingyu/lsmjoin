import matplotlib as mpl

mpl.use("Agg")
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
    num_tests = 6
    fig, axs = plt.subplots(
        len(datasets) // 6, num_tests, figsize=(4 * num_tests, len(datasets) // 6 * 4)
    )
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
    for data in datasets[:num_tests]:
        for label in data.keys():
            total_vals = [
                data[label][j][0] + data[label][j][1] for j in range(len(data[label]))
            ]
            max_y_val = max(max_y_val, max(total_vals))
    axs = axs.flatten()
    handles, labels = [], []
    for index, (ax, data, title) in enumerate(zip(axs, datasets, titles)):
        num_ratios = 1
        x = np.arange(num_ratios)
        width = 0.05
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
                if index == 0 or index == 3:
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
        if index == num_tests - 1:
            ax.legend(handles, labels, bbox_to_anchor=(1.0, 0.7), loc="upper left")
        ax.set_xticks([])
        ax.set_title(title)
    # max_y_val = 0
    # for data in datasets[num_tests : 2 * num_tests]:
    #     try:
    #         for label in data.keys():
    #             total_vals = [
    #                 data[label][j][0] + data[label][j][1]
    #                 for j in range(len(data[label]))
    #             ]
    #             max_y_val = max(max_y_val, max(total_vals))
    #     except:
    #         continue
    # for index, (ax, data) in enumerate(
    #     zip(
    #         axs[num_tests : 2 * num_tests],
    #         datasets[num_tests : 2 * num_tests],
    #     )
    # ):
    #     num_ratios = 1
    #     x = np.arange(num_ratios)
    #     width = 0.05
    #     handles, labels = [], []
    #     for i, (label, color) in enumerate(zip(data.keys(), colors)):
    #         print(label)
    #         print(data)
    #         join_vals = data[label][0][0]
    #         build_vals = data[label][0][1]
    #         join_bar = ax.bar(
    #             x - width / 2 + i * width,
    #             join_vals,
    #             width,
    #             edgecolor=color,
    #             color=color,
    #             label=f"{label} Join Time",
    #         )
    #         ax.bar(
    #             x - width / 2 + i * width,
    #             build_vals,
    #             width,
    #             edgecolor=color,
    #             fill=False,
    #             hatch="///",
    #             bottom=join_vals,
    #             label=f"{label} Index Build Time",
    #         )
    #         handles.append(join_bar)
    #         labels.append(label)
    #     ax.set_ylim(0, max_y_val)
    #     # ax.set_xlabel("Updates:Joins Ratios")
    #     if index == 0:
    #         ax.set_ylabel("System Latency (s)")
    #     else:
    #         ax.set_yticklabels([])
    #     if index == num_tests - 1:
    #         ax.legend(handles, labels, bbox_to_anchor=(1.0, 1), loc="upper left")
    #     ax.set_xticks([])
    # max_y_val = 0
    # for data in datasets[2 * num_tests : 3 * num_tests]:
    #     try:
    #         for label in data.keys():
    #             total_vals = [
    #                 data[label][j][0] + data[label][j][1]
    #                 for j in range(len(data[label]))
    #             ]
    #             max_y_val = max(max_y_val, max(total_vals))
    #     except:
    #         continue
    # for index, (ax, data) in enumerate(
    #     zip(
    #         axs[2 * num_tests : 3 * num_tests],
    #         datasets[2 * num_tests : 3 * num_tests],
    #     )
    # ):
    #     num_ratios = 1
    #     x = np.arange(num_ratios)
    #     width = 0.05
    #     handles, labels = [], []
    #     for i, (label, color) in enumerate(zip(data.keys(), colors)):
    #         join_vals = data[label][0][0]
    #         build_vals = data[label][0][1]
    #         join_bar = ax.bar(
    #             x - width / 2 + i * width,
    #             join_vals,
    #             width,
    #             edgecolor=color,
    #             color=color,
    #             label=f"{label} Join Time",
    #         )
    #         ax.bar(
    #             x - width / 2 + i * width,
    #             build_vals,
    #             width,
    #             edgecolor=color,
    #             fill=False,
    #             hatch="///",
    #             bottom=join_vals,
    #             label=f"{label} Index Build Time",
    #         )
    #         handles.append(join_bar)
    #         labels.append(label)
    #     ax.set_ylim(0, max_y_val)
    #     # ax.set_xlabel("Updates:Joins Ratios")
    #     if index == 0:
    #         ax.set_ylabel("System Latency (s)")
    #     else:
    #         ax.set_yticklabels([])
    #     if index == num_tests - 1:
    #         ax.legend(handles, labels, bbox_to_anchor=(1.0, 1), loc="upper left")
    #     ax.set_xticks([])

    plt.tight_layout()
    plt.savefig(filename, bbox_inches="tight", pad_inches=0.02)
    plt.close()


def extract_and_organize_data(file_path, attribute):
    user_data = {
        "P-INLJ": [],
        "P-ISJ": [],
        "P-Comp-ISJ": [],
        "P-CComp-ISJ": [],
        "P-Grace-HJ": [],
        "Lazy-INLJ": [],
        "CLazy-INLJ": [],
        "NISJ": [],
        "1Lazy-ISJ": [],
        "2Lazy-ISJ": [],
        "2CComp-ISJ": [],
        "1CLazy-ISJ": [],
        "Grace-HJ": [],
    }
    Table4 = ["P-INLJ", "P-ISJ", "P-Comp-ISJ", "P-CComp-ISJ", "P-Grace-HJ"]

    times_list = []
    with open(file_path, "r") as file:
        for line in file:
            join_time_match = re.search(r"sum_join_time=([\d\.]+)", line)
            index_build_time_match = re.search(r"sum_index_build_time=([\d\.]+)", line)
            attribute_match = re.search(attribute + r"=(\w+)", line)

            if join_time_match and index_build_time_match:
                join_time = float(join_time_match.group(1))
                index_build_time = float(index_build_time_match.group(1))
                if not attribute_match:
                    attribute_val = 0
                else:
                    attribute_val = int(attribute_match.group(1))
                times_list.append((join_time, index_build_time, attribute_val))

    keys = list(user_data.keys())
    for i, times in enumerate(times_list):
        key_index = i % len(keys)
        user_data[keys[key_index]].append(times)

    attribute_values = []
    input_data = user_data
    # get the first value
    for key in input_data.keys():
        vals = input_data[key]
        for val in vals:
            attribute_values.append(val[2])
        break
    # only the first 3 values
    attribute_values = attribute_values[0:3]
    # attributes: [1, 2, 4, 8]
    res = []
    res2 = []
    titles = []
    titles2 = []
    for at in attribute_values:
        temp = {}
        temp2 = {}
        for key in input_data.keys():
            if key in Table4:
                temp[key] = [val[0:2] for val in input_data[key] if val[2] == at]
            else:
                temp2[key] = [val[0:2] for val in input_data[key] if val[2] == at]
        res.append(temp)
        res2.append(temp2)
        titles.append(f"{attribute}_{at}")
        titles2.append(f"{attribute}_{at}")
    res.extend(res2)
    titles.extend(titles2)
    return res, titles


path = "./"
test_names = [
    "use_cache",
    "uniform",
    "B",
    "page_size",
    "num_loop",
    "k",
    "dataset_size",
    "dataratio",
    "c",
    "buffer_size",
]
# test_names = ["use_cache", "uniform", "page_size", "num_loop"]


tests = []
titles = []


for name in test_names:
    test, title = extract_and_organize_data(path + "test_7_" + name + ".txt", name)
    tests.extend(test)
    titles.extend(title)
plot_data(tests, titles, "ablation.pdf", colors)
