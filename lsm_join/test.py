import struct
import numpy as np
import random
from collections import Counter
from scipy.stats import skew
import collections

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
    # "osm_cellids_800M_uint64",
    "wiki_ts_200M_uint64",
]
S_list = [
    "cast_info_movie_id",
    "so_user_user_id",
    "fb_200M_uint64",
    # "osm_cellids_800M_uint64",
    "wiki_ts_200M_uint64",
]

for R_name, S_name in zip(R_list, S_list):

    R = {}
    r_ = []
    with open(base_path + R_name, "rb") as file:
        for _ in range(10000000):
            num_data = file.read(8)
            try:
                (num,) = struct.unpack("<Q", num_data)
                num %= 10000000000
                # if num == 1014648191 or num == 1014648675:
                #     continue
                R[num] = R.get(num, 0) + 1
                r_.append(num)
            except struct.error:
                continue
    S = {}
    with open(base_path + S_name, "rb") as file:
        for _ in range(10000000):
            num_data = file.read(8)
            try:
                (num,) = struct.unpack("<Q", num_data)
                num %= 10000000000
                # if num == 1014648191 or num == 1014648675:
                #     continue
                S[num] = S.get(num, 0) + 1
            except struct.error:
                continue
    # print(S[1014648191])
    # qu = random.sample(qu, 10)
    matches_r = 0
    count_r = 0
    for r in R:
        x = S.get(r, 0)
        matches_r += x
        if x > 0:
            count_r += 1

    matches_s = 0
    count_s = 0
    for s in S:
        x = R.get(s, 0)
        matches_s += x
        if x > 0:
            count_s += 1

    print("=" * 80)
    print(R_name, S_name)
    print(f"R length: {len(R)}, S length: {len(S)}")
    print(f"c_r:{sum(R.values()) / len(R)}, s_r:{sum(S.values()) / len(S)}")
    # print(f"r_skewness:{r_skewness}, s_skewness:{s_skewness}")
    slope, intercept = np.polyfit(
        np.log(np.arange(1, len(R) + 1)), np.log(sorted(R.values(), reverse=True)), 1
    )
    print(f"R Slope: {slope}, Intercept: {intercept}")

    slope, intercept = np.polyfit(
        np.log(np.arange(1, len(S) + 1)), np.log(sorted(S.values(), reverse=True)), 1
    )
    print(f"S Slope: {slope}, Intercept: {intercept}")
    print(f"r_eps:{1 - count_r / len(R)}, s_eps:{1 - count_s / len(S)}")
    # print(r)
    # counter = collections.Counter(S)

    # # Get the top 5 most common elements
    # top_five = counter.most_common(5)

    # # Print the top 5 elements
    # print("Top 5 elements and their counts:")
    # for num, count in top_five:
    #     print(f"Number: {num}, Count: {count}")
    # count = 0
    # with open(base_path + "fb_200M_uint64", "wb") as output_file:
    #     for num in r_:
    #         count += 1
    #         output_file.write(struct.pack("<Q", num))
    # print(count)
