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
    
    num_combinations = 4  # 指定每行显示的大图数量
    num_figures = 3  # 指定每个大图包含的小图数量
    # 动态计算行数，确保所有数据集都能被显示
    num_rows = len(datasets) // (num_combinations * num_figures)
    
    bar_group_width = 0.05  # 每组bar的宽度
    group_padding = 0.05   # 组间距
    individual_padding = 0.  # 组内各bar间距
    
    fig, axs = plt.subplots(
        num_rows, num_combinations, figsize=(24, num_rows * 4)
    )
    
    axs = axs.flatten()
    
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
    
    for row in range(num_rows):
        max_y_val = 0
        handles, labels = [], []
        for comb_index in range(num_combinations):
            if comb_index == 2:
                max_y_val = 0
            global_index = row * num_combinations + comb_index
            start_index = global_index * num_figures
            end_index = start_index + num_figures
            selected_datasets = datasets[start_index:end_index]
            selected_titles = titles[start_index:end_index]
            ax = axs[global_index]
            for data in selected_datasets:
                for label in data.keys():
                    total_vals = [sum(data[label][j]) for j in range(len(data[label]))]
                    max_y_val = max(max_y_val, max(total_vals))
            num_bars = len(selected_datasets)
            x_base = [0]
            for i, (data, title) in enumerate(zip(selected_datasets, selected_titles)):
                    num_labels = len(data)
                    x_offsets = np.arange(len(data)) * (bar_group_width + individual_padding)  # 计算每个bar的偏移量
                    x_base.append(x_base[-1] + num_labels * (bar_group_width + individual_padding) + group_padding)  # 计算每组数据的基础x坐标，包括组间距
                    for j, (label, values) in enumerate(data.items()):
                        join_vals, build_vals = values[0]
                        join_bar = ax.bar(x_base[i] + x_offsets[j], join_vals, bar_group_width, color=color_dict[label], label=f"{label} Join Time")
                        ax.bar(x_base[i] + x_offsets[j], build_vals, bar_group_width, bottom=join_vals, edgecolor=color_dict[label], fill=False, hatch="///", label=f"{label} Index Build Time")
                        if (i == 0 or i == 2) and comb_index == 0:
                            handles.append(join_bar)
                            labels.append(label)
            # x_ticks in the middle of the group
            for i in range(len(x_base) - 1):
                x_base[i] = (x_base[i] + x_base[i + 1] - group_padding) / 2
            ax.set_xticks(x_base[0:-1])
            ax.set_xticklabels(selected_titles, fontsize=10)
            if comb_index % 2 == 0:  
                ax.set_title("Table 4")
            else :
                ax.set_title("Table 5")
            if comb_index == 0 or comb_index == 2:  # 第一个大图添加Y轴标签
                ax.set_ylabel("System Latency (s)", fontsize=16)
            else:
                ax.set_yticks([])
                                
            ax.set_ylim(0, max_y_val)
            if row == 0 and comb_index == 3:
                ax.legend(handles, labels, bbox_to_anchor=(1.0, 0.7), loc="upper left")

    plt.tight_layout()
    plt.subplots_adjust(top=0.8)  # 调整 top 参数以在图表上方留出更多空白
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
plot_data(tests, titles, "ablation.pdf", colors)
