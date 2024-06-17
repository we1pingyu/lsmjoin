import pandas as pd
import re

# Define the lookup dictionary for mapping combinations to labels
lookup_dict = {
    ("Regular", "Primary", "INLJ"): "NL-P",  # P-INLJ
    ("Regular", "Primary", "SJ"): "SJ-P",  # P-ISJ
    ("Eager", "Primary", "SJ"): "SJ-PS/V-EI",  # P-Eager-ISJ
    ("Lazy", "Primary", "SJ"): "SJ-PS/V-LI",  # P-Lazy-ISJ
    ("Comp", "Primary", "SJ"): "SJ-PS/V-CI",  # P-Comp-ISJ
    ("CEager", "Primary", "SJ"): "SJ-PS/S-EI",  # P-CEager-ISJ
    ("CLazy", "Primary", "SJ"): "SJ-PS/S-LI",  # P-CLazy-ISJ
    ("CComp", "Primary", "SJ"): "SJ-PS/S-CI",  # P-CComp-ISJ
    ("Regular", "Primary", "HJ"): "HJ-P",  # P-HJ
    ("Regular", "Eager", "INLJ"): "NL-NS/V-EI",  # Eager-INLJ
    ("Regular", "Lazy", "INLJ"): "NL-NS/V-LI",  # Lazy-INLJ
    ("Regular", "Comp", "INLJ"): "NL-NS/V-CI",  # Comp-INLJ
    ("Regular", "CEager", "INLJ"): "NL-NS/S-EI",  # CEager-INLJ
    ("Regular", "CLazy", "INLJ"): "NL-NS/S-LI",  # CLazy-INLJ
    ("Regular", "CComp", "INLJ"): "NL-NS/S-CI",  # CComp-INLJ
    ("Regular", "Regular", "SJ"): "SJ-N",  # NISJ
    ("Regular", "Eager", "SJ"): "SJ-NS/V-EI",  # 1Eager-ISJ
    ("Regular", "Lazy", "SJ"): "SJ-NS/V-LI",  # 1Lazy-ISJ
    ("Regular", "Comp", "SJ"): "SJ-NS/V-CI",  # 1Comp-ISJ
    ("Eager", "Eager", "SJ"): "SJ-SS/V-EI",  # 2Eager-ISJ
    ("Comp", "Comp", "SJ"): "SJ-SS/V-CI",  # 2Comp-ISJ
    ("Lazy", "Lazy", "SJ"): "SJ-SS/V-LI",  # 2Lazy-ISJ
    ("Regular", "CEager", "SJ"): "SJ-NS/S-EI",  # 1CEager-ISJ
    ("Regular", "CLazy", "SJ"): "SJ-NS/S-LI",  # 1CLazy-ISJ
    ("Regular", "CComp", "SJ"): "SJ-NS/S-CI",  # 1CComp-ISJ
    ("CEager", "CEager", "SJ"): "SJ-SS/S-EI",  # 2CEager-ISJ
    ("CLazy", "CLazy", "SJ"): "SJ-SS/S-LI",  # 2CLazy-ISJ
    ("CComp", "CComp", "SJ"): "SJ-SS/S-CI",  # 2CComp-ISJ
    ("Regular", "Regular", "HJ"): "HJ-N",  # HJ
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

# Print the DataFrame rows formatted for LaTeX
for index, row in df.iterrows():
    print(row["label"], end="")
    for col in column_save:
        val = row[col]
        # Ensure the value is numeric before rounding
        if isinstance(val, (int, float)):
            val = round(val, 1)
            if val == 0.0:
                val = 0
            print(f"& {val}", end="")
    print(r" \\\hline")
