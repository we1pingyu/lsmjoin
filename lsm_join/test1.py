def find_first_difference(file1_path, file2_path):
    with open(file1_path, "r") as file1, open(file2_path, "r") as file2:
        line_number = 1
        for line1, line2 in zip(file1, file2):
            if line1 != line2:
                print(line1, line2)
                return line_number
            line_number += 1
    return None  # 所有行都相同


# 使用示例
file1_path = "comp_join.txt"
file2_path = "lazy_join.txt"
diff_line = find_first_difference(file1_path, file2_path)

if diff_line:
    print(f"Files start to differ from line {diff_line}.")
else:
    print("Files are identical.")
