import pandas as pd
import csv

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


test_names = [
    # "bpk",
    # "buffer_size_t",
    # "buffer_size",
    # "cache_size",
    # "skewness",
    # "B",
    # "num_loop",
    "K",
    # "dataset_size",
    # "dataratio",
    # "c",
    # "k",
    # "buffer_size",
    "T",
    "T_t",
]


def write_overall_csv():
    datasets = [
        "user_id",
        "movie_id",
        "fb_200M_uint64",
        "wiki_ts_200M_uint64",
        "unif",
        "skew",
    ]
    names = ["User", "Movie", "Face", "Wiki", "Unif", "Skew"]
    headers = []
    all_rows = []
    for i, dataset in enumerate(datasets):
        surfixs = ["", "_5nlj", "_5sj"]
        for surfix in surfixs:
            with open(f"lsm_join/{dataset+surfix}.txt", "r") as file:
                data = file.read()
                lines = data.strip().split("\n")
                rows = []
                for line in lines:
                    if line == "-------------------------":
                        continue
                    # Split each line into key-value pairs
                    pairs = line.split()
                    row = {}
                    for pair in pairs:
                        if "=" in pair:
                            key, value = pair.split("=", 1)
                            row[key] = value
                    row["dataset"] = names[i]
                    rows.append(row)

                if i == 0:
                    # Get the headers from the keys of the first row
                    headers = rows[0].keys()
                all_rows.extend(rows)
    # Write to CSV
    with open(f"lsm_join/csv_result/overall.csv", "w", newline="") as csvfile:
        writer = csv.DictWriter(csvfile, fieldnames=headers)
        writer.writeheader()
        for row in all_rows:
            writer.writerow(row)


def write_csv_from_txt(test_name):
    path = "lsm_join"
    # iterate through each test and write to a csv file
    with open(f"{path}/test_{test_name}.txt", "r") as file:
        # with open(f"{path}/{test_name}.txt", "r") as file:
        data = file.read()
        lines = data.strip().split("\n")
        rows = []
        for line in lines:
            if line == "-------------------------":
                continue
            # Split each line into key-value pairs
            pairs = line.split()
            row = {}
            for pair in pairs:
                if "=" in pair:
                    key, value = pair.split("=", 1)
                    row[key] = value
            rows.append(row)

        # Get the headers from the keys of the first row
        headers = rows[0].keys()

        # Write to CSV
        with open(f"lsm_join/csv_result/{test_name}.csv", "w", newline="") as csvfile:
            writer = csv.DictWriter(csvfile, fieldnames=headers)
            writer.writeheader()
            for row in rows:
                writer.writerow(row)


def process_csv(test_name):
    df = pd.read_csv(f"lsm_join/csv_result/{test_name}.csv")

    df["label"] = df.apply(
        lambda x: lookup_dict[(x["r_index"], x["s_index"], x["join_algorithm"])], axis=1
    )
    column_save = [
        "sum_join_time",
        "sum_join_read_io",
        "sum_index_build_time",
        "label",
    ]

    if test_name == "T_t":
        column_save.append("T")
        df["theory"] = 1
    elif test_name == "T":
        column_save.append("T")
        df["theory"] = 0
    elif test_name == "K":
        column_save.append("K")
    elif test_name == "buffer_size":
        column_save.append("M")
        column_save.append("cache_hit_rate")
        df["theory"] = 0
        column_save.append("theory")
    elif test_name == "buffer_size_t":
        column_save.append("M")
        column_save.append("cache_hit_rate")
        df["theory"] = 1
        column_save.append("theory")
    elif test_name == "cache_size":
        column_save.append("cache_size")
        column_save.append("cache_hit_rate")
    elif test_name == "bpk":
        column_save.append("bpk")
        column_save.append("false_positive_rate")
        column_save.append("cache_hit_rate")
    elif test_name == "overall":
        column_save.append("dataset")
    elif test_name == "c":
        column_save.append("c_r")
    elif test_name == "c_k":
        column_save.append("c_r")
        column_save.append("k_r")
        column_save.append("k_s")
    elif test_name == "c_skewness":
        column_save.append("c_r")
        column_save.append("k_r")
        column_save.append("k_s")
    elif test_name == "loops_size":
        column_save.append("num_loop")
        column_save.append("s_tuples")
        column_save.append("r_tuples")
    elif "insight" in test_name:
        column_save.append("B")
        column_save.append("k_s")
        column_save.append("bpk")
        column_save.append("false_positive_rate")
        column_save.append("num_loop")
        column_save.append("noncovering")
    elif test_name == "dataratio":
        df["dataratio"] = df["r_tuples"] / df["s_tuples"]
        # 如果是整数，不保留；如果是小数，保留1位
        df["dataratio"] = df["dataratio"].apply(
            lambda x: int(x) if x == int(x) else round(x, 1)
        )
        column_save.append("dataratio")
        column_save.append("r_tuples")
        column_save.append("s_tuples")
    elif test_name == "dataset_size":
        column_save.append("r_tuples")
        column_save.append("s_tuples")
    elif test_name == "k":
        column_save.append("k_r")
    elif "num_loop" in test_name:
        column_save.append("num_loop")
        column_save.append("join_time_list")
        column_save.append("index_build_time_list")
    elif test_name == "skewness":
        column_save.append("k_s")

    # Save to csv
    df[column_save].to_csv(f"lsm_join/csv_result/{test_name}.csv", index=False)
