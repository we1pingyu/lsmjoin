import pandas as pd
import matplotlib.pyplot as plt
import re
import sci_palettes
import matplotlib.gridspec as gridspec

sci_palettes.register_cmap()
style = "jama"
plt.set_cmap(style)
cmap = plt.get_cmap(style)
colors = cmap.colors
darkening_factor = 0.8
colors = [
    (r * darkening_factor, g * darkening_factor, b * darkening_factor)
    for r, g, b in colors
]

style = "npg_nrc"
plt.set_cmap(style)
cmap = plt.get_cmap(style)
colors2 = cmap.colors
darkening_factor = 0.8
colors += [
    (r * darkening_factor, g * darkening_factor, b * darkening_factor)
    for r, g, b in colors2
]
# Define the lookup dictionary for mapping combinations to labels
lookup_dict = {
    ("Regular", "Primary", "INLJ"): "P-INLJ",
    ("Regular", "Primary", "SJ"): "P-ISJ",
    ("Eager", "Primary", "SJ"): "P-Eager-ISJ",
    ("Lazy", "Primary", "SJ"): "P-Lazy-ISJ",
    ("Comp", "Primary", "SJ"): "P-Comp-ISJ",
    ("CEager", "Primary", "SJ"): "P-CEager-ISJ",
    ("CLazy", "Primary", "SJ"): "P-CLazy-ISJ",
    # ("CComp", "Primary", "SJ"): "P-CComp-ISJ",
    # ("Regular", "Primary", "HJ"): "P-HJ",
    ("Regular", "Eager", "INLJ"): "Eager-INLJ",
    # ("Regular", "Lazy", "INLJ"): "Lazy-INLJ",
    ("Regular", "Comp", "INLJ"): "Comp-INLJ",
    ("Regular", "CEager", "INLJ"): "CEager-INLJ",
    # ("Regular", "CLazy", "INLJ"): "CLazy-INLJ",
    ("Regular", "CComp", "INLJ"): "CComp-INLJ",
    ("Regular", "Regular", "SJ"): "NISJ",
    # ("Regular", "Eager", "SJ"): "1Eager-ISJ",
    # ("Regular", "Lazy", "SJ"): "1Lazy-ISJ",
    ("Regular", "Comp", "SJ"): "1Comp-ISJ",
    # ("Eager", "Eager", "SJ"): "2Eager-ISJ",
    # ("Comp", "Comp", "SJ"): "2Comp-ISJ",
    # ("Lazy", "Lazy", "SJ"): "2Lazy-ISJ",
    # ("Regular", "CEager", "SJ"): "1CEager-ISJ",
    # ("Regular", "CLazy", "SJ"): "1CLazy-ISJ",
    # ("Regular", "CComp", "SJ"): "1CComp-ISJ",
    ("CEager", "CEager", "SJ"): "2CEager-ISJ",
    ("CLazy", "CLazy", "SJ"): "2CLazy-ISJ",
    # ("CComp", "CComp", "SJ"): "2CComp-ISJ",
    ("Regular", "Regular", "HJ"): "HJ",
}

# Regex patterns to capture each field from the text
regex_patterns = {
    "r_index": r"r_index=(\S+)",
    "s_index": r"s_index=(\S+)",
    "join_algorithm": r"join_algorithm=(\S+)",
    "sum_sync_time": r"sum_sync_time=([\d.]+)",
    "sum_update_time": r"sum_update_time=([\d.]+)",
    "sum_eager_time": r"sum_eager_time=([\d.]+)",
    "sum_get_index_time": r"sum_get_index_time=([\d.]+)",
    "sum_get_data_time": r"sum_get_data_time=([\d.]+)",
    "sum_sort_io_time": r"sum_sort_io_time=([\d.]+)",
    "sum_hash_io_time": r"sum_hash_io_time=([\d.]+)",
    "sum_post_list_time": r"sum_post_list_time=([\d.]+)",
    "sum_sort_cpu_time": r"sum_sort_cpu_time=([\d.]+)",
    "sum_hash_cpu_time": r"sum_hash_cpu_time=([\d.]+)",
    "sum_cpu_time": r"sum_cpu_time=([\d.]+)",
}

data = []

with open("lsm_join/test_breakdown.txt", "r") as file:
    for line in file:
        if "-------------------------" in line:
            continue

        extracted_data = {}
        for field, pattern in regex_patterns.items():
            match = re.search(pattern, line)
            if match:
                # Handle non-numeric fields separately
                if ("index" in field or "join_algorithm" in field) and (
                    "sum_get_index_time" not in field
                ):
                    extracted_data[field] = match.group(1)
                else:
                    # Ensure numeric fields are converted to float
                    extracted_data[field] = float(match.group(1))

        if extracted_data:
            label = lookup_dict.get(
                (
                    extracted_data.get("r_index"),
                    extracted_data.get("s_index"),
                    extracted_data.get("join_algorithm"),
                ),
                "Unknown",
            )
            if label == "Unknown":
                continue
            extracted_data["label"] = label
            # Calculate other CPU time
            sum_cpu_time = extracted_data.get("sum_cpu_time", 0.0)
            sum_post_list_time = extracted_data.get("sum_post_list_time", 0.0)
            sum_hash_cpu_time = extracted_data.get("sum_hash_cpu_time", 0.0)
            sum_sort_cpu_time = extracted_data.get("sum_sort_cpu_time", 0.0)
            other_cpu_time = sum_cpu_time - (
                sum_post_list_time + sum_hash_cpu_time + sum_sort_cpu_time
            )
            extracted_data["other_cpu_time"] = other_cpu_time
            data.append(extracted_data)

df = pd.DataFrame(data)

# Specify columns to save
column_save = [
    "sum_sync_time",
    "sum_update_time",
    "sum_eager_time",
    "sum_get_index_time",
    "sum_get_data_time",
    "sum_sort_io_time",
    "sum_hash_io_time",
    "sum_post_list_time",
    "sum_sort_cpu_time",
    "sum_hash_cpu_time",
    "other_cpu_time",
]

# Adjust DataFrame to contain only necessary columns
df = df[["label"] + column_save]
fig = plt.figure(figsize=(20, 4))
gs = gridspec.GridSpec(2, 9)  # 创建一个包含2行9列的网格

# axes = axes.flatten()
labels = [
    "Synchronism",
    "Index Table Update",
    "Eager Get",
    "Index Table Get",
    "Data Table Get",
    "Sort I/O",
    "Hash I/O",
    "Posting List",
    "Sort CPU",
    "Hash CPU",
    "Other CPU",
]

# Print the DataFrame rows formatted for LaTeX
for idx, row in df.iterrows():
    if idx < 7:
        ax = fig.add_subplot(gs[0, idx])  # 第一行的子图
    else:
        ax = fig.add_subplot(gs[1, idx - 7])  # 第二行的子图
    values = [row[col] for col in column_save if isinstance(row[col], (int, float))]
    wedges, texts = ax.pie(
        values,
        colors=colors,
        wedgeprops={"edgecolor": "black", "linewidth": 1, "linestyle": "solid"},
    )
    # ax.set_title(row["label"], loc="bottom")
    ax.text(
        0.5,
        0.05,
        row["label"],
        ha="center",
        va="top",
        transform=ax.transAxes,
        fontsize=12,
    )


# 设置图例
fig.legend(
    wedges,
    labels,
    ncol=2,
    loc="upper right",
    columnspacing=0.5,
    fontsize=10,
    bbox_to_anchor=(0.89, 0.86),
    edgecolor="black",
)

plt.subplots_adjust(wspace=0.001, hspace=0.01)
plt.savefig("lsm_join/plot/breakdown.pdf", bbox_inches="tight", pad_inches=0.02)
