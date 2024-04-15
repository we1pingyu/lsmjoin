import csv

path = "/home/weiping/xuwei/lsmjoin/lsm_join"

# read data from file
test_names = [
    # "bpk",
    # "buffer_size_t",
    # "buffer_size",
    # "cache_size",
    # "skewness",
    # "B",
    # "num_loop",
    # "K",
    # "dataset_size",
    # "dataratio",
    # "c",
    # "k",
    # "buffer_size",
    # "T",
    # "T_t"
    "test_breakdown"
]

# iterate through each test and write to a csv file
for test_name in test_names:
    # with open(f"{path}/test_7_{test_name}.txt", "r") as file:
    with open(f"{path}/{test_name}.txt", "r") as file:
        data = file.read()
        lines = data.strip().split('\n')
        rows = []
        for line in lines:
            if line == '-------------------------':
                continue
            # Split each line into key-value pairs
            pairs = line.split()
            row = {}
            for pair in pairs:
                if '=' in pair:
                    key, value = pair.split('=', 1)
                    row[key] = value
            rows.append(row)

        # Get the headers from the keys of the first row
        headers = rows[0].keys()

        # Write to CSV
        with open(f'lsm_join/{test_name}.csv', 'w', newline='') as csvfile:
            writer = csv.DictWriter(csvfile, fieldnames=headers)
            writer.writeheader()
            for row in rows:
                writer.writerow(row)

        print(f"Data has been written to {test_name}.csv")