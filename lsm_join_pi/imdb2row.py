import csv
import struct
import numpy as np
import io


def save_as_binary1(file_path, row_num, output_path):
    with open(file_path, newline="") as csvfile:
        reader = csv.reader(csvfile)
        row_count = sum(1 for _ in reader if reader.line_num <= 2e7)
    with open(file_path) as csvfile, open(output_path, "wb") as binfile:
        binfile.write(struct.pack("<Q", row_count))
        reader = csv.reader(csvfile)
        count = 0
        for row in reader:
            if len(row) < 3:
                continue  # 跳过没有足够列的行
            num = int(row[row_num])
            data = struct.pack("<Q", num)
            binfile.write(data)
            count += 1
            if count > 2e7:
                break
        print(count)


def save_as_binary_so(file_path, row_num, output_path):
    buffer = io.BytesIO()
    with open(file_path) as csvfile, open(output_path, "wb") as binfile:
        # headers = next(reader)
        # print(headers)
        count = 0
        reader = csv.DictReader(csvfile)
        for row in reader:
            # print(row)
            # print(row[row_num])
            if len(row) < 3:
                continue  # 跳过没有足够列的行
            num = int(row[row_num])
            # print(num)
            try:
                buffer.write(struct.pack("<Q", num))
                count += 1
                if count > 2e7:
                    break
            except struct.error:
                # print(num)
                continue
        print(count)
    with open(output_path, "wb") as binfile:
        # 首先写入行数
        binfile.write(struct.pack("<Q", count))
        # 接着写入缓冲区中的数据
        buffer.seek(0)  # 重置缓冲区的读取指针
        binfile.write(buffer.read())


# save_as_binary1(
#     "/home/weiping/data/imdb_data_csv/cast_info.csv",
#     2,
#     "/home/weiping/code/lsm_join_data/cast_info_movie_id",
# )
# save_as_binary1(
#     "/home/weiping/data/imdb_data_csv/movie_info.csv",
#     1,
#     "/home/weiping/code/lsm_join_data/movie_info_movie_id",
# )

# save_as_binary_so(
#     "/home/weiping/data/so_data_csv/answer.csv",
#     "owner_user_id",
#     "/home/weiping/code/lsm_join_data/answer_user_id",
# )
# save_as_binary_so(
#     "/home/weiping/data/so_data_csv/so_user.csv",
#     "id",
#     "/home/weiping/code/lsm_join_data/so_user_user_id",
# )
# save_as_binary_so(
#     "/home/weiping/data/so_data_csv/question.csv",
#     "owner_user_id",
#     "/home/weiping/code/lsm_join_data/question_user_id",
# )
