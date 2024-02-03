import csv

# Your data goes between the triple quotes
file_name = "test_7_k.txt"
# open the file and read all
with open(file_name, 'r') as file:
    data = file.read()

# Split the data into lines and process each line
lines = data.strip().split('\n')
rows = []
for line in lines:
    if "-" in line:
        continue
    # delete the space after "s_index="
    if "s_index= " in line:
        line = line.replace("s_index= ", "s_index=")
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
with open('output_1.csv', 'w', newline='') as csvfile:
    writer = csv.DictWriter(csvfile, fieldnames=headers)
    writer.writeheader()
    for row in rows:
        writer.writerow(row)

print("Data has been written to output.csv")
