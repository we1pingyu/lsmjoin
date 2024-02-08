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

color_dict = {}

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
    for leng in range(len(datasets) // 6):
        max_y_val = 0
        for data in datasets[num_tests * leng:num_tests * (leng + 1)]:
            for label in data.keys():
                total_vals = [
                    data[label][j][0] + data[label][j][1] for j in range(len(data[label]))
                ]
                max_y_val = max(max_y_val, max(total_vals))
        axs = axs.flatten()
        handles, labels = [], []
        for index, (ax, data, title) in enumerate(zip(axs[num_tests * leng:num_tests * (leng + 1)], datasets[num_tests * leng:num_tests * (leng + 1)], titles[num_tests * leng:num_tests * (leng + 1)])):
            num_ratios = 1
            x = np.arange(num_ratios)
            width = 0.05
            for i, label in enumerate(data.keys()):
                try:
                    join_vals = data[label][0][0]
                    build_vals = data[label][0][1]
                    join_bar = ax.bar(
                        x - width / 2 + i * width,
                        join_vals,
                        width,
                        edgecolor=color_dict[label],
                        color=color_dict[label],
                        label=f"{label} Join Time",
                    )
                    ax.bar(
                        x - width / 2 + i * width,
                        build_vals,
                        width,
                        edgecolor=color_dict[label],
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
            if leng == 0 and index == num_tests - 1:
                ax.legend(handles, labels, bbox_to_anchor=(1.0, 0.7), loc="upper left")
            ax.set_xticks([])
            ax.set_title(title)

    plt.tight_layout()
    plt.savefig(filename, bbox_inches="tight", pad_inches=0.02)
    plt.close()

import matplotlib.pyplot as plt
import numpy as np

def plot_combined_data(datasets, titles, filename, color_dict):
    num_combinations = 4  # 指定每行显示的大图数量
    num_figures = 3  # 指定每个大图包含的小图数量

    # 动态计算行数，确保所有数据集都能被显示
    num_rows = len(datasets) // (num_combinations * num_figures)
    # if len(datasets) % (num_combinations * num_figures) != 0:
    #     num_rows += 1  # 如果不能整除，增加一行以容纳剩余的数据集

    bar_group_width = 0.04  # 每组bar的宽度
    group_padding = 0.2   # 组间距
    individual_padding = 0  # 组内各bar间距
    
    # 调整figsize参数以适应图表的实际需要
    fig, axs = plt.subplots(num_rows, num_combinations, figsize=(20, 4 * num_rows), squeeze=False)
    axs = axs.flatten()  # 将axs展平以便于使用一维索引

    for row in range(num_rows):
        for comb_index in range(num_combinations):
            global_index = row * num_combinations + comb_index
            start_index = global_index * num_figures
            end_index = start_index + num_figures
            selected_datasets = datasets[start_index:end_index]
            selected_titles = titles[start_index:end_index]

            if global_index < len(axs):  # 检查索引是否在axs范围内
                ax = axs[global_index]

                # 计算最大Y值以统一Y轴
                max_y_val = 0
                for data in selected_datasets:
                    for label in data.keys():
                        total_vals = [sum(data[label][j]) for j in range(len(data[label]))]
                        max_y_val = max(max_y_val, max(total_vals))
                ax.set_ylim(0, max_y_val)
                
                num_bars = len(selected_datasets)
                x_base = np.arange(num_bars) * (bar_group_width + group_padding)  # 基础x坐标，为每组数据分配空间
                
                for i, (data, title) in enumerate(zip(selected_datasets, selected_titles)):
                    x_offsets = np.arange(len(data)) * (bar_group_width + individual_padding)  # 计算每个bar的偏移量
                    for j, (label, values) in enumerate(data.items()):
                        join_vals, build_vals = values[0]
                        ax.bar(x_base[i] + x_offsets[j], join_vals, bar_group_width, color=color_dict[label], label=f"{label} Join Time" if row == 0 and comb_index == 0 and i == 0 else "")
                        ax.bar(x_base[i] + x_offsets[j], build_vals, bar_group_width, bottom=join_vals, edgecolor=color_dict[label], fill=False, hatch="///", label=f"{label} Index Build Time" if row == 0 and comb_index == 0 and i == 0 else "")
                
                # 设置图例和标题
                if row == 0 and comb_index == 0:  # 在第一个大图中添加图例
                    ax.legend(loc="upper right")
                ax.set_xticks(x_base + bar_group_width / 2)
                ax.set_xticklabels(selected_titles, rotation=45, ha="right")
                if comb_index == 0:  # 第一个大图添加Y轴标签
                    ax.set_ylabel("System Latency (s)", fontsize=16)

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
    
    # make color dict
    if color_dict == {}:
        for key in user_data.keys():
            color_dict[key] = colors.pop(0)
    
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


path = "./lsm_join_pi/"
# test_names = [
#     "use_cache",
#     "uniform",
#     "B",
#     "page_size",
#     "num_loop",
#     "k",
#     "dataset_size",
#     "dataratio",
#     "c",
#     "buffer_size",
# ]
test_names = ["use_cache", "uniform", "page_size", "num_loop"]


tests = []
titles = []


for name in test_names:
    test, title = extract_and_organize_data(path + "test_7_" + name + ".txt", name)
    tests.extend(test)
    titles.extend(title)
# plot_data(tests, titles, "ablation.pdf", colors)
plot_combined_data(tests, titles, "ablation_1.pdf", color_dict)
