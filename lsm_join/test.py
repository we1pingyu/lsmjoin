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
R_list = [
    "movie_info_movie_id",
    "question_user_id",
    "fb_200M_uint64",
    "osm_cellids_800M_uint64",
    # "wiki_ts_200M_uint64",
]
S_list = [
    "cast_info_movie_id",
    "so_user_user_id",
    "fb_200M_uint64",
    "osm_cellids_800M_uint64",
    # "wiki_ts_200M_uint64",
]

for R_name, S_name in zip(R_list, S_list):

    R = {}
    with open(base_path + R_name, "rb") as file:
        for _ in range(10000000):
            num_data = file.read(8)
            try:
                (num,) = struct.unpack("<Q", num_data)
                num %= 10000000000
                R[num] = R.get(num, 0) + 1
            except struct.error:
                continue
    S = {}
    with open(base_path + S_name, "rb") as file:
        for _ in range(10000000):
            num_data = file.read(8)
            try:
                (num,) = struct.unpack("<Q", num_data)
                num %= 10000000000
                S[num] = S.get(num, 0) + 1
            except struct.error:
                continue
    matches = 0
    count = 0
    # qu = random.sample(qu, 10)
    for r in R:
        x = S.get(r, 0)
        # print(R)
        matches += x
        if x > 0:
            count += 1
        # if matches % 100000 == 0:
        #     print(i, matches)
    print("=" * 80)
    print(R_name, S_name)
    print(len(R), len(S))
    print(sum(R.values()) / len(R), sum(S.values()) / len(S))
    print(1 - count / len(R))
    print(r)
