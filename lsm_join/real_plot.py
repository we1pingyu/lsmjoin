import matplotlib.pyplot as plt
import numpy as np
import re

# 标签和数据
labels = [
    "P-INLJ",
    "P-ISJ",
    "P-Eager-ISJ",
    "P-Lazy-ISJ",
    "P-Comp-ISJ",
    "P-CEager-ISJ",
    "P-CLazy-ISJ",
    "P-CComp-ISJ",
    "Grace-HJ",
]
fb_data = {
    "P-INLJ": [(61, 0), (144, 0), (469, 0)],
    "P-ISJ": [(33, 0), (84, 0), (282, 0)],
    "P-Eager-ISJ": [(19, 59), (78, 59), (255, 58)],
    "P-Lazy-ISJ": [(20, 38), (82, 36), (258, 36)],
    "P-Comp-ISJ": [(19, 37), (80, 36), (258, 36)],
    "P-CEager-ISJ": [(8, 86), (19, 85), (65, 85)],
    "P-CLazy-ISJ": [(8, 66), (21, 65), (70, 68)],
    "P-CComp-ISJ": [(8, 67), (20, 67), (66, 64)],
    "Grace-HJ": [(27, 0), (92, 0), (340, 0)],
}
osm_data = {
    "P-INLJ": [(270, 0), (652, 0), (2236, 0)],
    "P-ISJ": [(126, 0), (319, 0), (1094, 0)],
    "P-Eager-ISJ": [(89, 307), (340, 304), (1091, 308)],
    "P-Lazy-ISJ": [(93, 147), (347, 150), (1134, 149)],
    "P-Comp-ISJ": [(92, 148), (346, 145), (1129, 148)],
    "P-CEager-ISJ": [(25, 429), (66, 431), (231, 439)],
    "P-CLazy-ISJ": [(27, 290), (73, 290), (264, 296)],
    "P-CComp-ISJ": [(26, 283), (69, 283), (235, 283)],
    "Grace-HJ": [(67, 0), (186, 0), (674, 0)],
}
user_data = {
    "P-INLJ": [(270, 0), (652, 0), (2236, 0)],
    "P-ISJ": [(126, 0), (319, 0), (1094, 0)],
    "P-Eager-ISJ": [(89, 307), (340, 304), (1091, 308)],
    "P-Lazy-ISJ": [(93, 147), (347, 150), (1134, 149)],
    "P-Comp-ISJ": [(92, 148), (346, 145), (1129, 148)],
    "P-CEager-ISJ": [(25, 429), (66, 431), (231, 439)],
    "P-CLazy-ISJ": [(27, 290), (73, 290), (264, 296)],
    "P-CComp-ISJ": [(26, 283), (69, 283), (235, 283)],
    "Grace-HJ": [(67, 0), (186, 0), (674, 0)],
}
movie_data = {
    "P-INLJ": [(63, 0), (128, 0), (368, 0)],
    "P-ISJ": [(32, 0), (82, 0), (275, 0)],
    "P-Eager-ISJ": [(18, 67), (79, 63), (250, 57)],
    "P-Lazy-ISJ": [(18, 37), (78, 36), (250, 35)],
    "P-Comp-ISJ": [(19, 36), (78, 36), (249, 36)],
    "P-CEager-ISJ": [(7, 114), (18, 145), (60, 146)],
    "P-CLazy-ISJ": [(7, 80), (19, 111), (63, 119)],
    "P-CComp-ISJ": [(7, 65), (18, 64), (61, 63)],
    "Grace-HJ": [(26, 0), (91, 0), (338, 0)],
}
# 手动设置的颜色
colors = [
    "#a59344",
    "#7b463b",
    "#191970",
    "#3d1c02",
    "#456789",
    "#4c4f56",
    "#4d3c2d",
    "#166461",
    "#9bafad",
]

fb_ratios = ["20M:1", "10M:1", "2M:1"]
user_ratios = ["16M:1", "8M:1", "1.6M:1"]


def plot_data(data, ratios, title, filename):
    num_ratios = len(ratios)
    x = np.arange(num_ratios)
    width = 0.1
    fig, ax = plt.subplots(figsize=(8, 4))

    for i, (label, color) in enumerate(zip(labels, colors)):
        join_vals = [data[label][j][0] for j in range(num_ratios)]
        build_vals = [data[label][j][1] for j in range(num_ratios)]

        ax.bar(
            x - width / 2 + i * width,
            join_vals,
            width,
            color=color,
            label=label,
        )
        ax.bar(
            x - width / 2 + i * width,
            build_vals,
            width,
            edgecolor=color,
            fill=False,
            hatch="///",
            bottom=join_vals,
        )

    ax.set_xlabel("Updates:Joins Ratios")
    ax.set_ylabel("System Latency (s)")
    ax.set_xticks(x + (len(labels) - 1) * width / 2)
    ax.set_xticklabels(ratios)
    ax.legend(bbox_to_anchor=(1.05, 1), loc="upper left")
    plt.title(title)
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

    # 读取并提取数据
    times_list = []
    with open(file_path, "r") as file:
        for line in file:
            join_time_match = re.search(r"sum_join_time=([\d\.]+)", line)
            index_build_time_match = re.search(r"sum_index_build_time=([\d\.]+)", line)

            if join_time_match and index_build_time_match:
                join_time = float(join_time_match.group(1))
                index_build_time = float(index_build_time_match.group(1))
                times_list.append((join_time, index_build_time))

    # 将数据组织到user_data中
    keys = list(user_data.keys())
    for i, times in enumerate(times_list):
        key_index = i % len(keys)
        user_data[keys[key_index]].append(times)
    print(user_data)
    return user_data


plot_data(
    extract_and_organize_data("user_id.txt"), user_ratios, "User Data", "user.pdf"
)
plot_data(
    extract_and_organize_data("movie_id.txt"), fb_ratios, "Movie Data", "movie.pdf"
)
plot_data(
    extract_and_organize_data("osm_cellids_800M_uint64.txt"),
    fb_ratios,
    "OSM Data",
    "osm.pdf",
)
plot_data(
    extract_and_organize_data("fb_200M_uint64.txt"),
    fb_ratios,
    "Fb Data",
    "fb.pdf",
)
