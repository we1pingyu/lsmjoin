import pandas as pd

lookup_dict = {
    ("Regular", "Primary", "INLJ"): "P-INLJ",
    ("Regular", "Primary", "SJ"): "P-ISJ",
    ("Eager", "Primary", "SJ"): "P-Eager-ISJ",
    ("Lazy", "Primary", "SJ"): "P-Lazy-ISJ",
    ("Comp", "Primary", "SJ"): "P-Comp-ISJ",
    ("CEager", "Primary", "SJ"): "P-CEager-ISJ",
    ("CLazy", "Primary", "SJ"): "P-CLazy-ISJ",
    ("CComp", "Primary", "SJ"): "P-CComp-ISJ",
    ("Regular", "Primary", "HJ"): "P-Grace-HJ",
    ("Regular", "Eager", "INLJ"): "Eager-INLJ",
    ("Regular", "Lazy", "INLJ"): "Lazy-INLJ",
    ("Regular", "Comp", "INLJ"): "Comp-INLJ",
    ("Regular", "CEager", "INLJ"): "CEager-INLJ",
    ("Regular", "CLazy", "INLJ"): "CLazy-INLJ",
    ("Regular", "CComp", "INLJ"): "CComp-INLJ",
    ("Regular", "Regular", "SJ"): "NISJ",
    ("Regular", "Eager", "SJ"): "1Eager-ISJ",
    ("Regular", "Lazy", "SJ"): "1Lazy-ISJ",
    ("Regular", "Comp", "SJ"): "1Comp-ISJ",
    ("Eager", "Eager", "SJ"): "2Eager-ISJ",
    ("Comp", "Comp", "SJ"): "2Comp-ISJ",
    ("Lazy", "Lazy", "SJ"): "2Lazy-ISJ",
    ("Regular", "CEager", "SJ"): "1CEager-ISJ",
    ("Regular", "CLazy", "SJ"): "1CLazy-ISJ",
    ("Regular", "CComp", "SJ"): "1CComp-ISJ",
    ("CEager", "CEager", "SJ"): "2CEager-ISJ",
    ("CLazy", "CLazy", "SJ"): "2CLazy-ISJ",
    ("CComp", "CComp", "SJ"): "2CComp-ISJ",
    ("Regular", "Regular", "HJ"): "Grace-HJ",
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

test_name = test_names[0]
for test_name in test_names:
    df = pd.read_csv(f"lsm_join/{test_name}.csv")

    df["label"] = df.apply(
        lambda x: lookup_dict[(x["r_index"], x["s_index"], x["join_algorithm"])], axis=1
    )

    column_save = [
        "sum_join_time",
        "sum_index_build_time",
        "label",
    ]

    if test_name == "T_t":
        column_save.append("T")
        df["theory"] = 1

    # Save to csv
    df[column_save].to_csv(f"lsm_join/lsm_res/{test_name}.csv", index=False)
