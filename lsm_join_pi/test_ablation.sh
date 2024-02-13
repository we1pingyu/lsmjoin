#!/bin/bash
make exp_runner
db_r_path="/tmp/db_r_ablation"
db_s_path="/tmp/db_s_ablation"
index_r_path="/tmp/index_r_ablation"
index_s_path="/tmp/index_s_ablation"

# Use Cache
output="test_7_cache_size.txt"

# clear output file
rm -f $output

dataset="movie_id"
data_path="/home/weiping/code/lsm_join_data/"
public_r="${data_path}movie_info_movie_id"
public_s="${data_path}cast_info_movie_id"

nums=(0 16 64)
for num in "${nums[@]}"; do
    # Table 4
    # INLJ, Regular, Primary
    ./exp_runner --M=64 --B=128 --ingestion --J="INLJ" --r_index="Regular" --s_index="Primary" --num_loop=1 --output_file=$output $use_cache_flag --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --cache_size=$num
    # SJ, Regular, Primary
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Regular" --s_index="Primary" --num_loop=1 --output_file=$output $use_cache_flag --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --cache_size=$num
    # SJ, Comp, Primary
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Comp" --s_index="Primary" --num_loop=1 --output_file=$output $use_cache_flag --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --cache_size=$num
    # SJ, CComp, Primary
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="CComp" --s_index="Primary" --num_loop=1 --output_file=$output $use_cache_flag --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --cache_size=$num
    # HJ, Regular, Primary
    ./exp_runner --M=64 --B=128 --ingestion --J="HJ" --r_index="Regular" --s_index="Primary" --num_loop=1 --output_file=$output $use_cache_flag --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --cache_size=$num

    # Table 5
    # INLJ, Regular, Lazy
    ./exp_runner --M=64 --B=128 --ingestion --J="INLJ" --r_index="Regular" --s_index="Lazy" --num_loop=1 --output_file=$output $use_cache_flag --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --cache_size=$num
    # INLJ, Regular, CLazy
    ./exp_runner --M=64 --B=128 --ingestion --J="INLJ" --r_index="Regular" --s_index="CLazy" --num_loop=1 --output_file=$output $use_cache_flag --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --cache_size=$num
    # SJ, Regular, Regular
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Regular" --s_index="Regular" --num_loop=1 --output_file=$output $use_cache_flag --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --cache_size=$num
    # SJ, Regular, Lazy
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Regular" --s_index="Lazy" --num_loop=1 --output_file=$output $use_cache_flag --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --cache_size=$num
    # SJ, Lazy, Lazy
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Lazy" --s_index="Lazy" --num_loop=1 --output_file=$output $use_cache_flag --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --cache_size=$num
    # SJ, CComp, CComp
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="CComp" --s_index="CComp" --num_loop=1 --output_file=$output $use_cache_flag --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --cache_size=$num
    # SJ, Regular, CLazy
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Regular" --s_index="CLazy" --num_loop=1 --output_file=$output $use_cache_flag --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --cache_size=$num
    # HJ, Regular, Regular
    ./exp_runner --M=64 --B=128 --ingestion --J="HJ" --r_index="Regular" --s_index="Regular" --num_loop=1 --output_file=$output $use_cache_flag --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --cache_size=$num

done

# Uniform
output="test_7_skewness.txt"

# clear output file
rm -f $output

# 1M: 1000000
Million=1000000

r_tuples=$((10 * Million))
s_tuples=$((10 * Million))

nums=(2 4 6)
for k in "${nums[@]}"; do
    uniform_flag=""
    k_flag="--k=$k"
    # Table 4
    # INLJ, Regular, Primary
    ./exp_runner --M=64 --B=128 --ingestion --J="INLJ" --r_index="Regular" --s_index="Primary" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples $uniform_flag $k_flag --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
    # SJ, Regular, Primary
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Regular" --s_index="Primary" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples $uniform_flag $k_flag --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
    # SJ, Comp, Primary
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Comp" --s_index="Primary" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples $uniform_flag $k_flag --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
    # SJ, CComp, Primary
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="CComp" --s_index="Primary" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples $uniform_flag $k_flag --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
    # HJ, Regular, Primary
    ./exp_runner --M=64 --B=128 --ingestion --J="HJ" --r_index="Regular" --s_index="Primary" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples $uniform_flag $k_flag --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path

    # Table 5
    # INLJ, Regular, Lazy
    ./exp_runner --M=64 --B=128 --ingestion --J="INLJ" --r_index="Regular" --s_index="Lazy" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples $uniform_flag $k_flag --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
    # INLJ, Regular, CLazy
    ./exp_runner --M=64 --B=128 --ingestion --J="INLJ" --r_index="Regular" --s_index="CLazy" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples $uniform_flag $k_flag --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
    # SJ, Regular, Regular
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Regular" --s_index="Regular" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples $uniform_flag $k_flag --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
    # SJ, Regular, Lazy
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Regular" --s_index="Lazy" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples $uniform_flag $k_flag --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
    # SJ, Lazy, Lazy
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Lazy" --s_index="Lazy" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples $uniform_flag $k_flag --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
    # SJ, CComp, CComp
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="CComp" --s_index="CComp" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples $uniform_flag $k_flag --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
    # SJ, Regular, CLazy
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Regular" --s_index="CLazy" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples $uniform_flag $k_flag --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
    # HJ, Regular, Regular
    ./exp_runner --M=64 --B=128 --ingestion --J="HJ" --r_index="Regular" --s_index="Regular" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples $uniform_flag $k_flag --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
done

# Tuple Size
output="test_7_B.txt"

# clear output file
rm -f $output

# Tuple Size
dataset="user_id"
data_path="/home/weiping/code/lsm_join_data/"
public_r="${data_path}question_user_id"
public_s="${data_path}so_user_user_id"

nums=(128 32 8)

for B in "${nums[@]}"; do
    # Table 4
    # INLJ, Regular, Primary
    ./exp_runner --M=64 --B=$B --ingestion --J="INLJ" --r_index="Regular" --s_index="Primary" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
    # SJ, Regular, Primary
    ./exp_runner --M=64 --B=$B --ingestion --J="SJ" --r_index="Regular" --s_index="Primary" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
    # SJ, Comp, Primary
    ./exp_runner --M=64 --B=$B --ingestion --J="SJ" --r_index="Comp" --s_index="Primary" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
    # SJ, CComp, Primary
    ./exp_runner --M=64 --B=$B --ingestion --J="SJ" --r_index="CComp" --s_index="Primary" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
    # HJ, Regular, Primary
    ./exp_runner --M=64 --B=$B --ingestion --J="HJ" --r_index="Regular" --s_index="Primary" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path

    # Table 5
    # INLJ, Regular, Lazy
    ./exp_runner --M=64 --B=$B --ingestion --J="INLJ" --r_index="Regular" --s_index="Lazy" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
    # INLJ, Regular, CLazy
    ./exp_runner --M=64 --B=$B --ingestion --J="INLJ" --r_index="Regular" --s_index="CLazy" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
    # SJ, Regular, Regular
    ./exp_runner --M=64 --B=$B --ingestion --J="SJ" --r_index="Regular" --s_index="Regular" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
    # SJ, Regular, Lazy
    ./exp_runner --M=64 --B=$B --ingestion --J="SJ" --r_index="Regular" --s_index="Lazy" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
    # SJ, Lazy, Lazy
    ./exp_runner --M=64 --B=$B --ingestion --J="SJ" --r_index="Lazy" --s_index="Lazy" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
    # SJ, CComp, CComp
    ./exp_runner --M=64 --B=$B --ingestion --J="SJ" --r_index="CComp" --s_index="CComp" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
    # SJ, Regular, CLazy
    ./exp_runner --M=64 --B=$B --ingestion --J="SJ" --r_index="Regular" --s_index="CLazy" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
    # HJ, Regular, Regular
    ./exp_runner --M=64 --B=$B --ingestion --J="HJ" --r_index="Regular" --s_index="Regular" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
done

# # Page Size
# output="test_7_page_size.txt"

# # clear output file
# rm -f $output

# base_size=4096

# dataset="fb_200M_uint64"
# data_path="/home/weiping/code/lsm_join_data/"
# public_r="${data_path}${dataset}"
# public_s="${data_path}${dataset}"

# nums=(1 2 4 8)
# for num in "${nums[@]}"; do
#     page_size=$((num * base_size))
#     # Table 4
#     # INLJ, Regular, Primary
#     ./exp_runner --M=64 --B=128 --ingestion --J="INLJ" --r_index="Regular" --s_index="Primary" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --page_size=$page_size --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
#     # SJ, Regular, Primary
#     ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Regular" --s_index="Primary" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --page_size=$page_size --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
#     # SJ, Comp, Primary
#     ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Comp" --s_index="Primary" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --page_size=$page_size --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
#     # SJ, CComp, Primary
#     ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="CComp" --s_index="Primary" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --page_size=$page_size --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
#     # HJ, Regular, Primary
#     ./exp_runner --M=64 --B=128 --ingestion --J="HJ" --r_index="Regular" --s_index="Primary" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --page_size=$page_size --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path

#     # Table 5
#     # INLJ, Regular, Lazy
#     ./exp_runner --M=64 --B=128 --ingestion --J="INLJ" --r_index="Regular" --s_index="Lazy" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --page_size=$page_size --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
#     # INLJ, Regular, CLazy
#     ./exp_runner --M=64 --B=128 --ingestion --J="INLJ" --r_index="Regular" --s_index="CLazy" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --page_size=$page_size --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
#     # SJ, Regular, Regular
#     ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Regular" --s_index="Regular" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --page_size=$page_size --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
#     # SJ, Regular, Lazy
#     ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Regular" --s_index="Lazy" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --page_size=$page_size --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
#     # SJ, Lazy, Lazy
#     ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Lazy" --s_index="Lazy" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --page_size=$page_size --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
#     # SJ, CComp, CComp
#     ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="CComp" --s_index="CComp" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --page_size=$page_size --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
#     # SJ, Regular, CLazy
#     ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Regular" --s_index="CLazy" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --page_size=$page_size --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
#     # HJ, Regular, Regular
#     ./exp_runner --M=64 --B=128 --ingestion --J="HJ" --r_index="Regular" --s_index="Regular" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --page_size=$page_size --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
# done

# Num Loop
output="test_7_num_loop.txt"

# clear output file
rm -f $output

dataset="user_id"
data_path="/home/weiping/code/lsm_join_data/"
public_r="${data_path}question_user_id"
public_s="${data_path}so_user_user_id"

nums=(1 4 8)
for num_loop in "${nums[@]}"; do
    # Table 4
    # INLJ, Regular, Primary
    ./exp_runner --M=64 --B=128 --ingestion --J="INLJ" --r_index="Regular" --s_index="Primary" --num_loop=$num_loop --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
    # SJ, Regular, Primary
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Regular" --s_index="Primary" --num_loop=$num_loop --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
    # SJ, Comp, Primary
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Comp" --s_index="Primary" --num_loop=$num_loop --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
    # SJ, CComp, Primary
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="CComp" --s_index="Primary" --num_loop=$num_loop --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
    # HJ, Regular, Primary
    ./exp_runner --M=64 --B=128 --ingestion --J="HJ" --r_index="Regular" --s_index="Primary" --num_loop=$num_loop --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path

    # Table 5
    # INLJ, Regular, Lazy
    ./exp_runner --M=64 --B=128 --ingestion --J="INLJ" --r_index="Regular" --s_index="Lazy" --num_loop=$num_loop --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
    # INLJ, Regular, CLazy
    ./exp_runner --M=64 --B=128 --ingestion --J="INLJ" --r_index="Regular" --s_index="CLazy" --num_loop=$num_loop --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
    # SJ, Regular, Regular
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Regular" --s_index="Regular" --num_loop=$num_loop --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
    # SJ, Regular, Lazy
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Regular" --s_index="Lazy" --num_loop=$num_loop --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
    # SJ, Lazy, Lazy
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Lazy" --s_index="Lazy" --num_loop=$num_loop --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
    # SJ, CComp, CComp
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="CComp" --s_index="CComp" --num_loop=$num_loop --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
    # SJ, Regular, CLazy
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Regular" --s_index="CLazy" --num_loop=$num_loop --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
    # HJ, Regular, Regular
    ./exp_runner --M=64 --B=128 --ingestion --J="HJ" --r_index="Regular" --s_index="Regular" --num_loop=$num_loop --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
done

# K value
output="test_7_k.txt"

# clear output file
rm -f $output

s_tuples=$((10 * Million))
r_tuples=$((10 * Million))

nums=(2 4 8)
for k in "${nums[@]}"; do
    # Table 4
    # INLJ, Regular, Primary
    ./exp_runner --M=64 --B=128 --ingestion --J="INLJ" --r_index="Regular" --s_index="Primary" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --uniform --k=$k --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
    # SJ, Regular, Primary
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Regular" --s_index="Primary" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --uniform --k=$k --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
    # SJ, Comp, Primary
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Comp" --s_index="Primary" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --uniform --k=$k --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
    # SJ, CComp, Primary
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="CComp" --s_index="Primary" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --uniform --k=$k --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
    # HJ, Regular, Primary
    ./exp_runner --M=64 --B=128 --ingestion --J="HJ" --r_index="Regular" --s_index="Primary" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --uniform --k=$k --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path

    # Table 5
    # INLJ, Regular, Lazy
    ./exp_runner --M=64 --B=128 --ingestion --J="INLJ" --r_index="Regular" --s_index="Lazy" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --uniform --k=$k --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
    # INLJ, Regular, CLazy
    ./exp_runner --M=64 --B=128 --ingestion --J="INLJ" --r_index="Regular" --s_index="CLazy" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --uniform --k=$k --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
    # SJ, Regular, Regular
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Regular" --s_index="Regular" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --uniform --k=$k --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
    # SJ, Regular, Lazy
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Regular" --s_index="Lazy" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --uniform --k=$k --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
    # SJ, Lazy, Lazy
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Lazy" --s_index="Lazy" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --uniform --k=$k --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
    # SJ, CComp, CComp
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="CComp" --s_index="CComp" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --uniform --k=$k --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
    # SJ, Regular, CLazy
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Regular" --s_index="CLazy" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --uniform --k=$k --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
    # HJ, Regular, Regular
    ./exp_runner --M=64 --B=128 --ingestion --J="HJ" --r_index="Regular" --s_index="Regular" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --uniform --k=$k --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
done

# Dataset Size
output="test_7_dataset_size.txt"

# clear output file
rm -f $output

# Dataset Size
# r_tuples and s_tuples are 1M, 16M, 64M, 128M
nums=(1 16 64)
for num in "${nums[@]}"; do
    r_tuples=$((num * Million))
    s_tuples=$((num * Million))
    # Table 4
    # INLJ, Regular, Primary
    ./exp_runner --M=64 --B=128 --ingestion --J="INLJ" --r_index="Regular" --s_index="Primary" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --uniform --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
    # SJ, Regular, Primary
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Regular" --s_index="Primary" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --uniform --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
    # SJ, Comp, Primary
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Comp" --s_index="Primary" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --uniform --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
    # SJ, CComp, Primary
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="CComp" --s_index="Primary" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --uniform --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
    # HJ, Regular, Primary
    ./exp_runner --M=64 --B=128 --ingestion --J="HJ" --r_index="Regular" --s_index="Primary" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --uniform --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path

    # Table 5
    # INLJ, Regular, Lazy
    ./exp_runner --M=64 --B=128 --ingestion --J="INLJ" --r_index="Regular" --s_index="Lazy" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --uniform --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
    # INLJ, Regular, CLazy
    ./exp_runner --M=64 --B=128 --ingestion --J="INLJ" --r_index="Regular" --s_index="CLazy" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --uniform --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
    # SJ, Regular, Regular
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Regular" --s_index="Regular" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --uniform --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
    # SJ, Regular, Lazy
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Regular" --s_index="Lazy" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --uniform --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
    # SJ, Lazy, Lazy
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Lazy" --s_index="Lazy" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --uniform --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
    # SJ, CComp, CComp
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="CComp" --s_index="CComp" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --uniform --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
    # SJ, Regular, CLazy
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Regular" --s_index="CLazy" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --uniform --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
    # HJ, Regular, Regular
    ./exp_runner --M=64 --B=128 --ingestion --J="HJ" --r_index="Regular" --s_index="Regular" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --uniform --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
done

# Data Ratio
output="test_7_dataratio.txt"

# clear output file
rm -f $output

# Dataset Size
s_tuples=$((10 * Million))
nums=(0.1 1 10)
for num in "${nums[@]}"; do
    r_tuples=$(echo "$num * $s_tuples" | bc)
    # Table 4
    # INLJ, Regular, Primary
    ./exp_runner --M=64 --B=128 --ingestion --J="INLJ" --r_index="Regular" --s_index="Primary" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --uniform --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
    # SJ, Regular, Primary
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Regular" --s_index="Primary" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --uniform --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
    # SJ, Comp, Primary
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Comp" --s_index="Primary" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --uniform --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
    # SJ, CComp, Primary
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="CComp" --s_index="Primary" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --uniform --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
    # HJ, Regular, Primary
    ./exp_runner --M=64 --B=128 --ingestion --J="HJ" --r_index="Regular" --s_index="Primary" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --uniform --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path

    # Table 5
    # INLJ, Regular, Lazy
    ./exp_runner --M=64 --B=128 --ingestion --J="INLJ" --r_index="Regular" --s_index="Lazy" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --uniform --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
    # INLJ, Regular, CLazy
    ./exp_runner --M=64 --B=128 --ingestion --J="INLJ" --r_index="Regular" --s_index="CLazy" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --uniform --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
    # SJ, Regular, Regular
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Regular" --s_index="Regular" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --uniform --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
    # SJ, Regular, Lazy
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Regular" --s_index="Lazy" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --uniform --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
    # SJ, Lazy, Lazy
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Lazy" --s_index="Lazy" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --uniform --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
    # SJ, CComp, CComp
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="CComp" --s_index="CComp" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --uniform --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
    # SJ, Regular, CLazy
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Regular" --s_index="CLazy" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --uniform --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
    # HJ, Regular, Regular
    ./exp_runner --M=64 --B=128 --ingestion --J="HJ" --r_index="Regular" --s_index="Regular" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --uniform --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
done

# c value
output="test_7_c.txt"

# clear output file
rm -f $output

# Dataset Size
s_tuples=$((10 * Million))
r_tuples=$((10 * Million))

nums=(1 4 16)
for c in "${nums[@]}"; do
    # Table 4
    # INLJ, Regular, Primary
    ./exp_runner --M=64 --B=128 --ingestion --J="INLJ" --r_index="Regular" --s_index="Primary" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --c=$c --uniform --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
    # SJ, Regular, Primary
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Regular" --s_index="Primary" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --c=$c --uniform --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
    # SJ, Comp, Primary
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Comp" --s_index="Primary" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --c=$c --uniform --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
    # SJ, CComp, Primary
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="CComp" --s_index="Primary" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --c=$c --uniform --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
    # HJ, Regular, Primary
    ./exp_runner --M=64 --B=128 --ingestion --J="HJ" --r_index="Regular" --s_index="Primary" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --c=$c --uniform --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path

    # Table 5
    # INLJ, Regular, Lazy
    ./exp_runner --M=64 --B=128 --ingestion --J="INLJ" --r_index="Regular" --s_index="Lazy" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --c=$c --uniform --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
    # INLJ, Regular, CLazy
    ./exp_runner --M=64 --B=128 --ingestion --J="INLJ" --r_index="Regular" --s_index="CLazy" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --c=$c --uniform --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
    # SJ, Regular, Regular
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Regular" --s_index="Regular" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --c=$c --uniform --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
    # SJ, Regular, Lazy
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Regular" --s_index="Lazy" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --c=$c --uniform --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
    # SJ, Lazy, Lazy
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Lazy" --s_index="Lazy" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --c=$c --uniform --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
    # SJ, CComp, CComp
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="CComp" --s_index="CComp" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --c=$c --uniform --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
    # SJ, Regular, CLazy
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Regular" --s_index="CLazy" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --c=$c --uniform --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
    # HJ, Regular, Regular
    ./exp_runner --M=64 --B=128 --ingestion --J="HJ" --r_index="Regular" --s_index="Regular" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --c=$c --uniform --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
done

# c value
output="test_7_T.txt"

# clear output file
rm -f $output

# Dataset Size
s_tuples=$((10 * Million))
r_tuples=$((10 * Million))

nums=(2 5 10)
for T in "${nums[@]}"; do
    # Table 4
    # INLJ, Regular, Primary
    ./exp_runner --M=64 --B=128 --ingestion --J="INLJ" --r_index="Regular" --s_index="Primary" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --T=$T --uniform --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
    # SJ, Regular, Primary
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Regular" --s_index="Primary" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --T=$T --uniform --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
    # SJ, Comp, Primary
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Comp" --s_index="Primary" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --T=$T --uniform --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
    # SJ, CComp, Primary
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="CComp" --s_index="Primary" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --T=$T --uniform --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
    # HJ, Regular, Primary
    ./exp_runner --M=64 --B=128 --ingestion --J="HJ" --r_index="Regular" --s_index="Primary" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --T=$T --uniform --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path

    # Table 5
    # INLJ, Regular, Lazy
    ./exp_runner --M=64 --B=128 --ingestion --J="INLJ" --r_index="Regular" --s_index="Lazy" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --T=$T --uniform --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
    # INLJ, Regular, CLazy
    ./exp_runner --M=64 --B=128 --ingestion --J="INLJ" --r_index="Regular" --s_index="CLazy" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --T=$T --uniform --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
    # SJ, Regular, Regular
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Regular" --s_index="Regular" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --T=$T --uniform --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
    # SJ, Regular, Lazy
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Regular" --s_index="Lazy" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --T=$T --uniform --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
    # SJ, Lazy, Lazy
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Lazy" --s_index="Lazy" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --T=$T --uniform --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
    # SJ, CComp, CComp
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="CComp" --s_index="CComp" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --T=$T --uniform --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
    # SJ, Regular, CLazy
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Regular" --s_index="CLazy" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --T=$T --uniform --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
    # HJ, Regular, Regular
    ./exp_runner --M=64 --B=128 --ingestion --J="HJ" --r_index="Regular" --s_index="Regular" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --T=$T --uniform --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
done

# Buffer Size
output="test_7_buffer_size.txt"

# clear output file
rm -f $output

dataset="osm_cellids_800M_uint64"
data_path="/home/weiping/code/lsm_join_data/"
public_r="${data_path}${dataset}"
public_s="${data_path}${dataset}"

nums=(4 32 128)

for M in "${nums[@]}"; do
    # Table 4
    # INLJ, Regular, Primary
    ./exp_runner --M=$M --B=128 --ingestion --J="INLJ" --r_index="Regular" --s_index="Primary" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
    # SJ, Regular, Primary
    ./exp_runner --M=$M --B=128 --ingestion --J="SJ" --r_index="Regular" --s_index="Primary" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
    # SJ, Comp, Primary
    ./exp_runner --M=$M --B=128 --ingestion --J="SJ" --r_index="Comp" --s_index="Primary" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
    # SJ, CComp, Primary
    ./exp_runner --M=$M --B=128 --ingestion --J="SJ" --r_index="CComp" --s_index="Primary" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
    # HJ, Regular, Primary
    ./exp_runner --M=$M --B=128 --ingestion --J="HJ" --r_index="Regular" --s_index="Primary" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path

    # Table 5
    # INLJ, Regular, Lazy
    ./exp_runner --M=$M --B=128 --ingestion --J="INLJ" --r_index="Regular" --s_index="Lazy" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
    # INLJ, Regular, CLazy
    ./exp_runner --M=$M --B=128 --ingestion --J="INLJ" --r_index="Regular" --s_index="CLazy" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
    # SJ, Regular, Regular
    ./exp_runner --M=$M --B=128 --ingestion --J="SJ" --r_index="Regular" --s_index="Regular" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
    # SJ, Regular, Lazy
    ./exp_runner --M=$M --B=128 --ingestion --J="SJ" --r_index="Regular" --s_index="Lazy" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
    # SJ, Lazy, Lazy
    ./exp_runner --M=$M --B=128 --ingestion --J="SJ" --r_index="Lazy" --s_index="Lazy" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
    # SJ, CComp, CComp
    ./exp_runner --M=$M --B=128 --ingestion --J="SJ" --r_index="CComp" --s_index="CComp" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
    # SJ, Regular, CLazy
    ./exp_runner --M=$M --B=128 --ingestion --J="SJ" --r_index="Regular" --s_index="CLazy" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
    # HJ, Regular, Regular
    ./exp_runner --M=$M --B=128 --ingestion --J="HJ" --r_index="Regular" --s_index="Regular" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
done
