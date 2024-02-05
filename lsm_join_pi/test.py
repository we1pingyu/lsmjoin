import struct
import numpy as np
import random
from collections import Counter

file_list = [
    # "movie_info_movie_id",
    # "cast_info_movie_id",
    "question_user_id",
    "so_user_user_id",
    # "fb_200M_uint64",
    # "osm_cellids_800M_uint64",
    # "wiki_ts_200M_uint64",
]
base_path = "/home/weiping/code/lsm_join_data/"
# for file_name in file_list:
#     data = []
#     with open(base_path + file_name, "rb") as file:
#         for _ in range(10000000):
#             num_data = file.read(8)
#             try:
#                 (num,) = struct.unpack("<Q", num_data)
#                 num %= 10000000000
#                 # print(num)
#                 data.append(num)
#             except struct.error:
#                 # print(num_data)
#                 continue
#     print(file_name)
#     print(len(data))
#     counts = Counter(data)
#     print(len(counts))
#     avg_repetition = sum(counts.values()) / len(counts)
#     print(avg_repetition)
qu = []
with open(base_path + "question_user_id", "rb") as file:
    for _ in range(10000000):
        num_data = file.read(8)
        try:
            (num,) = struct.unpack("<Q", num_data)
            num %= 10000000000
            qu.append(num)
        except struct.error:
            continue
usr = {}
with open(base_path + "so_user_user_id", "rb") as file:
    for _ in range(10000000):
        num_data = file.read(8)
        try:
            (num,) = struct.unpack("<Q", num_data)
            num %= 10000000000
            usr[num] = usr.get(num, 0) + 1
        except struct.error:
            continue
matches = 0
count = 0
# qu = random.sample(qu, 10)
for i in range(len(qu)):
    x = usr.get(qu[i], 0)
    matches += x
    if x > 0:
        count += 1
    # if matches % 100000 == 0:
    #     print(i, matches)
print(count)
print(matches)
