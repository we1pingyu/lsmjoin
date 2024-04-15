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
    ("Comp", "Comp", "SJ"): "2Lazy-ISJ",
    ("Lazy", "Lazy", "SJ"): "2Comp-ISJ",
    ("Regular", "CEager", "SJ"): "1CEager-ISJ",
    ("Regular", "CLazy", "SJ"): "1CLazy-ISJ",
    ("Regular", "CComp", "SJ"): "1CComp-ISJ",
    ("CEager", "CEager", "SJ"): "2CEager-ISJ",
    ("CLazy", "CLazy", "SJ"): "2CLazy-ISJ",
    ("CComp", "CComp", "SJ"): "2CComp-ISJ",
    ("Regular", "Regular", "HJ"): "Grace-HJ"
}


df = pd.read_csv('lsm_join_pi/test_breakdown.csv')

# new column
df['label'] = df.apply(lambda x: lookup_dict[(x['r_index'], x['s_index'], x['join_algorithm'])], axis=1)

df['other_cpu_time'] = df['sum_cpu_time'] - (df['sum_post_list_time'] + df['string_process_time'] + df['sum_hash_cpu_time'] + df['sum_sort_cpu_time'])

column_save = [
    "sum_sync_time", 
    "sum_update_time", 
    "sum_eager_time", 
    "sum_get_index_time", 
    "sum_get_data_time", 
    "sum_sort_io_time", 
    "sum_hash_io_time", 
    "string_process_time", 
    "sum_post_list_time",
    "sum_sort_cpu_time", 
    "sum_hash_cpu_time", 
    "other_cpu_time"
]
# only keep the columns we need
df = df[['label'] + column_save]

# for every row, print like 'P-INLJ& 0& 0& 0& 0& 4.5& 0& 0& 0.1& 0& 0.7 
for index, row in df.iterrows():
    print(row['label'], end='')
    for col in column_save:
        val = row[col]
        # save 1 decimal
        val = round(val, 1)
        # if 0.0 -> 0
        if val == 0.0:
            val = 0
        print('&', val, end='')
    print(r' \\\hline')