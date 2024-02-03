#!/bin/bash
make exp_runner

# Use Cache
output="test_7_use_cache.txt"

# clear output file
rm -f $output

dataset="movie_id"
data_path="/home/weiping/code/lsm_join_data/"
public_r="${data_path}movie_info_movie_id"
public_s="${data_path}cast_info_movie_id"

nums=(0 1)
for cache in "${nums[@]}"; do
    if [ $cache -eq 0 ]; then
        use_cache_flag=""
    else
        use_cache_flag="--use_cache"
    fi
    # Table 4
    # INLJ, Regular, Primary
    ./exp_runner --M=64 --B=128 --ingestion --J="INLJ" --r_index="Regular" --s_index="Primary" --num_loop=1 --output_file=$output $use_cache_flag --public_data --public_r=$public_r --public_s=$public_s
    # SJ, Regular, Primary
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Regular" --s_index="Primary" --num_loop=1 --output_file=$output $use_cache_flag --public_data --public_r=$public_r --public_s=$public_s
    # SJ, Comp, Primary
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Comp" --s_index="Primary" --num_loop=1 --output_file=$output $use_cache_flag --public_data --public_r=$public_r --public_s=$public_s
    # SJ, CComp, Primary
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="CComp" --s_index="Primary" --num_loop=1 --output_file=$output $use_cache_flag --public_data --public_r=$public_r --public_s=$public_s
    # HJ, Regular, Primary
    ./exp_runner --M=64 --B=128 --ingestion --J="HJ" --r_index="Regular" --s_index="Primary" --num_loop=1 --output_file=$output $use_cache_flag --public_data --public_r=$public_r --public_s=$public_s

    # Table 5
    # INLJ, Regular, Lazy
    ./exp_runner --M=64 --B=128 --ingestion --J="INLJ" --r_index="Regular" --s_index="Lazy" --num_loop=1 --output_file=$output $use_cache_flag --public_data --public_r=$public_r --public_s=$public_s
    # INLJ, Regular, CLazy
    ./exp_runner --M=64 --B=128 --ingestion --J="INLJ" --r_index="Regular" --s_index="CLazy" --num_loop=1 --output_file=$output $use_cache_flag --public_data --public_r=$public_r --public_s=$public_s
    # SJ, Regular, Regular
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Regular" --s_index="Regular" --num_loop=1 --output_file=$output $use_cache_flag --public_data --public_r=$public_r --public_s=$public_s
    # SJ, Regular, Lazy
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Regular" --s_index="Lazy" --num_loop=1 --output_file=$output $use_cache_flag --public_data --public_r=$public_r --public_s=$public_s
    # SJ, Lazy, Lazy
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Lazy" --s_index="Lazy" --num_loop=1 --output_file=$output $use_cache_flag --public_data --public_r=$public_r --public_s=$public_s
    # SJ, CComp, CComp
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="CComp" --s_index="CComp" --num_loop=1 --output_file=$output $use_cache_flag --public_data --public_r=$public_r --public_s=$public_s
    # SJ, Regular, CLazy
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Regular" --s_index="CLazy" --num_loop=1 --output_file=$output $use_cache_flag --public_data --public_r=$public_r --public_s=$public_s
    # HJ, Regular, Regular
    ./exp_runner --M=64 --B=128 --ingestion --J="HJ" --r_index="Regular" --s_index="Regular" --num_loop=1 --output_file=$output $use_cache_flag --public_data --public_r=$public_r --public_s=$public_s

done

# Uniform
output="test_7_uniform.txt"

# clear output file
rm -f $output

# 1M: 1000000
Million=1000

r_tuples=$((10 * Million))
s_tuples=$((10 * Million))

nums=(0 1 2 4 8)
for k in "${nums[@]}"; do
    if [ $k -eq 0 ]; then
        uniform_flag="--uniform"
        k_flag=""
    else
        uniform_flag=""
        k_flag="--k=$k"
    fi
    # Table 4
    # INLJ, Regular, Primary
    ./exp_runner --M=64 --B=128 --ingestion --J="INLJ" --r_index="Regular" --s_index="Primary" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples $uniform_flag $k_flag
    # SJ, Regular, Primary
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Regular" --s_index="Primary" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples $uniform_flag $k_flag
    # SJ, Comp, Primary
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Comp" --s_index="Primary" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples $uniform_flag $k_flag
    # SJ, CComp, Primary
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="CComp" --s_index="Primary" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples $uniform_flag $k_flag
    # HJ, Regular, Primary
    ./exp_runner --M=64 --B=128 --ingestion --J="HJ" --r_index="Regular" --s_index="Primary" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples $uniform_flag $k_flag

    # Table 5
    # INLJ, Regular, Lazy
    ./exp_runner --M=64 --B=128 --ingestion --J="INLJ" --r_index="Regular" --s_index="Lazy" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples $uniform_flag $k_flag
    # INLJ, Regular, CLazy
    ./exp_runner --M=64 --B=128 --ingestion --J="INLJ" --r_index="Regular" --s_index="CLazy" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples $uniform_flag $k_flag
    # SJ, Regular, Regular
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Regular" --s_index="Regular" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples $uniform_flag $k_flag
    # SJ, Regular, Lazy
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Regular" --s_index="Lazy" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples $uniform_flag $k_flag
    # SJ, Lazy, Lazy
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Lazy" --s_index="Lazy" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples $uniform_flag $k_flag
    # SJ, CComp, CComp
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="CComp" --s_index="CComp" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples $uniform_flag $k_flag
    # SJ, Regular, CLazy
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Regular" --s_index="CLazy" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples $uniform_flag $k_flag
    # HJ, Regular, Regular
    ./exp_runner --M=64 --B=128 --ingestion --J="HJ" --r_index="Regular" --s_index="Regular" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples $uniform_flag $k_flag
done

# Tuple Size
output="test_7_tuple_size.txt"

# clear output file
rm -f $output

# Tuple Size
dataset="user_id"
data_path="/home/weiping/code/lsm_join_data/"
public_r="${data_path}question_user_id"
public_s="${data_path}so_user_user_id"

nums=(128 64 32 16)

for B in "${nums[@]}"; do
    # Table 4
    # INLJ, Regular, Primary
    ./exp_runner --M=64 --B=$B --ingestion --J="INLJ" --r_index="Regular" --s_index="Primary" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s
    # SJ, Regular, Primary
    ./exp_runner --M=64 --B=$B --ingestion --J="SJ" --r_index="Regular" --s_index="Primary" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s
    # SJ, Comp, Primary
    ./exp_runner --M=64 --B=$B --ingestion --J="SJ" --r_index="Comp" --s_index="Primary" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s
    # SJ, CComp, Primary
    ./exp_runner --M=64 --B=$B --ingestion --J="SJ" --r_index="CComp" --s_index="Primary" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s
    # HJ, Regular, Primary
    ./exp_runner --M=64 --B=$B --ingestion --J="HJ" --r_index="Regular" --s_index="Primary" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s

    # Table 5
    # INLJ, Regular, Lazy
    ./exp_runner --M=64 --B=$B --ingestion --J="INLJ" --r_index="Regular" --s_index="Lazy" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s
    # INLJ, Regular, CLazy
    ./exp_runner --M=64 --B=$B --ingestion --J="INLJ" --r_index="Regular" --s_index="CLazy" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s
    # SJ, Regular, Regular
    ./exp_runner --M=64 --B=$B --ingestion --J="SJ" --r_index="Regular" --s_index="Regular" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s
    # SJ, Regular, Lazy
    ./exp_runner --M=64 --B=$B --ingestion --J="SJ" --r_index="Regular" --s_index="Lazy" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s
    # SJ, Lazy, Lazy
    ./exp_runner --M=64 --B=$B --ingestion --J="SJ" --r_index="Lazy" --s_index="Lazy" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s
    # SJ, CComp, CComp
    ./exp_runner --M=64 --B=$B --ingestion --J="SJ" --r_index="CComp" --s_index="CComp" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s
    # SJ, Regular, CLazy
    ./exp_runner --M=64 --B=$B --ingestion --J="SJ" --r_index="Regular" --s_index="CLazy" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s
    # HJ, Regular, Regular
    ./exp_runner --M=64 --B=$B --ingestion --J="HJ" --r_index="Regular" --s_index="Regular" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s
done

# Page Size
output="test_7_page_size.txt"

# clear output file
rm -f $output

base_size=4096

dataset="fb_200M_uint64"
data_path="/home/weiping/code/lsm_join_data/"
public_r="${data_path}${dataset}"
public_s="${data_path}${dataset}"

nums=(1 2 4 8)
for num in "${nums[@]}"; do
    page_size=$((num * base_size))
    # Table 4
    # INLJ, Regular, Primary
    ./exp_runner --M=64 --B=128 --ingestion --J="INLJ" --r_index="Regular" --s_index="Primary" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --page_size=$page_size
    # SJ, Regular, Primary
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Regular" --s_index="Primary" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --page_size=$page_size
    # SJ, Comp, Primary
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Comp" --s_index="Primary" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --page_size=$page_size
    # SJ, CComp, Primary
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="CComp" --s_index="Primary" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --page_size=$page_size
    # HJ, Regular, Primary
    ./exp_runner --M=64 --B=128 --ingestion --J="HJ" --r_index="Regular" --s_index="Primary" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --page_size=$page_size

    # Table 5
    # INLJ, Regular, Lazy
    ./exp_runner --M=64 --B=128 --ingestion --J="INLJ" --r_index="Regular" --s_index="Lazy" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --page_size=$page_size
    # INLJ, Regular, CLazy
    ./exp_runner --M=64 --B=128 --ingestion --J="INLJ" --r_index="Regular" --s_index="CLazy" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --page_size=$page_size
    # SJ, Regular, Regular
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Regular" --s_index="Regular" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --page_size=$page_size
    # SJ, Regular, Lazy
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Regular" --s_index="Lazy" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --page_size=$page_size
    # SJ, Lazy, Lazy
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Lazy" --s_index="Lazy" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --page_size=$page_size
    # SJ, CComp, CComp
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="CComp" --s_index="CComp" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --page_size=$page_size
    # SJ, Regular, CLazy
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Regular" --s_index="CLazy" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --page_size=$page_size
    # HJ, Regular, Regular
    ./exp_runner --M=64 --B=128 --ingestion --J="HJ" --r_index="Regular" --s_index="Regular" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --page_size=$page_size
done

# Num Loop
output="test_7_num_loop.txt"

# clear output file
rm -f $output

dataset="user_id"
data_path="/home/weiping/code/lsm_join_data/"
public_r="${data_path}question_user_id"
public_s="${data_path}so_user_user_id"

nums=(1 4 8 16)
for num_loop in "${nums[@]}"; do
    # Table 4
    # INLJ, Regular, Primary
    ./exp_runner --M=64 --B=128 --ingestion --J="INLJ" --r_index="Regular" --s_index="Primary" --num_loop=$num_loop --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s
    # SJ, Regular, Primary
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Regular" --s_index="Primary" --num_loop=$num_loop --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s
    # SJ, Comp, Primary
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Comp" --s_index="Primary" --num_loop=$num_loop --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s
    # SJ, CComp, Primary
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="CComp" --s_index="Primary" --num_loop=$num_loop --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s
    # HJ, Regular, Primary
    ./exp_runner --M=64 --B=128 --ingestion --J="HJ" --r_index="Regular" --s_index="Primary" --num_loop=$num_loop --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s

    # Table 5
    # INLJ, Regular, Lazy
    ./exp_runner --M=64 --B=128 --ingestion --J="INLJ" --r_index="Regular" --s_index="Lazy" --num_loop=$num_loop --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s
    # INLJ, Regular, CLazy
    ./exp_runner --M=64 --B=128 --ingestion --J="INLJ" --r_index="Regular" --s_index="CLazy" --num_loop=$num_loop --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s
    # SJ, Regular, Regular
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Regular" --s_index="Regular" --num_loop=$num_loop --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s
    # SJ, Regular, Lazy
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Regular" --s_index="Lazy" --num_loop=$num_loop --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s
    # SJ, Lazy, Lazy
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Lazy" --s_index="Lazy" --num_loop=$num_loop --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s
    # SJ, CComp, CComp
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="CComp" --s_index="CComp" --num_loop=$num_loop --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s
    # SJ, Regular, CLazy
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Regular" --s_index="CLazy" --num_loop=$num_loop --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s
    # HJ, Regular, Regular
    ./exp_runner --M=64 --B=128 --ingestion --J="HJ" --r_index="Regular" --s_index="Regular" --num_loop=$num_loop --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s
done

# K value
output="test_7_k.txt"

# clear output file
rm -f $output

s_tuples=$((10 * Million))
r_tuples=$((10 * Million))

nums=(1 2 4 8 16)
for k in "${nums[@]}"; do
    # Table 4
    # INLJ, Regular, Primary
    ./exp_runner --M=64 --B=128 --ingestion --J="INLJ" --r_index="Regular" --s_index="Primary" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --uniform --k=$k
    # SJ, Regular, Primary
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Regular" --s_index="Primary" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --uniform --k=$k
    # SJ, Comp, Primary
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Comp" --s_index="Primary" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --uniform --k=$k
    # SJ, CComp, Primary
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="CComp" --s_index="Primary" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --uniform --k=$k
    # HJ, Regular, Primary
    ./exp_runner --M=64 --B=128 --ingestion --J="HJ" --r_index="Regular" --s_index="Primary" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --uniform --k=$k

    # Table 5
    # INLJ, Regular, Lazy
    ./exp_runner --M=64 --B=128 --ingestion --J="INLJ" --r_index="Regular" --s_index="Lazy" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --uniform --k=$k
    # INLJ, Regular, CLazy
    ./exp_runner --M=64 --B=128 --ingestion --J="INLJ" --r_index="Regular" --s_index="CLazy" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --uniform --k=$k
    # SJ, Regular, Regular
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Regular" --s_index="Regular" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --uniform --k=$k
    # SJ, Regular, Lazy
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Regular" --s_index="Lazy" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --uniform --k=$k
    # SJ, Lazy, Lazy
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Lazy" --s_index="Lazy" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --uniform --k=$k
    # SJ, CComp, CComp
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="CComp" --s_index="CComp" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --uniform --k=$k
    # SJ, Regular, CLazy
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Regular" --s_index="CLazy" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --uniform --k=$k
    # HJ, Regular, Regular
    ./exp_runner --M=64 --B=128 --ingestion --J="HJ" --r_index="Regular" --s_index="Regular" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --uniform --k=$k
done

# Dataset Size
output="test_7_dataset_size.txt"

# clear output file
rm -f $output

# Dataset Size
# r_tuples and s_tuples are 1M, 16M, 64M, 128M
nums=(1 8 16 32 64)
for num in "${nums[@]}"; do
    r_tuples=$((num * Million))
    s_tuples=$((num * Million))
    # Table 4
    # INLJ, Regular, Primary
    ./exp_runner --M=64 --B=128 --ingestion --J="INLJ" --r_index="Regular" --s_index="Primary" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --uniform
    # SJ, Regular, Primary
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Regular" --s_index="Primary" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --uniform
    # SJ, Comp, Primary
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Comp" --s_index="Primary" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --uniform
    # SJ, CComp, Primary
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="CComp" --s_index="Primary" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --uniform
    # HJ, Regular, Primary
    ./exp_runner --M=64 --B=128 --ingestion --J="HJ" --r_index="Regular" --s_index="Primary" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --uniform

    # Table 5
    # INLJ, Regular, Lazy
    ./exp_runner --M=64 --B=128 --ingestion --J="INLJ" --r_index="Regular" --s_index="Lazy" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --uniform
    # INLJ, Regular, CLazy
    ./exp_runner --M=64 --B=128 --ingestion --J="INLJ" --r_index="Regular" --s_index="CLazy" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --uniform
    # SJ, Regular, Regular
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Regular" --s_index="Regular" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --uniform
    # SJ, Regular, Lazy
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Regular" --s_index="Lazy" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --uniform
    # SJ, Lazy, Lazy
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Lazy" --s_index="Lazy" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --uniform
    # SJ, CComp, CComp
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="CComp" --s_index="CComp" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --uniform
    # SJ, Regular, CLazy
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Regular" --s_index="CLazy" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --uniform
    # HJ, Regular, Regular
    ./exp_runner --M=64 --B=128 --ingestion --J="HJ" --r_index="Regular" --s_index="Regular" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --uniform
done

# Data Ratio
output="test_7_dataratio.txt"

# clear output file
rm -f $output

# Dataset Size
s_tuples=$((10 * Million))

nums=(1 10 100)
for num in "${nums[@]}"; do
    r_tuples=$((num * Million))
    # Table 4
    # INLJ, Regular, Primary
    ./exp_runner --M=64 --B=128 --ingestion --J="INLJ" --r_index="Regular" --s_index="Primary" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --uniform
    # SJ, Regular, Primary
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Regular" --s_index="Primary" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --uniform
    # SJ, Comp, Primary
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Comp" --s_index="Primary" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --uniform
    # SJ, CComp, Primary
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="CComp" --s_index="Primary" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --uniform
    # HJ, Regular, Primary
    ./exp_runner --M=64 --B=128 --ingestion --J="HJ" --r_index="Regular" --s_index="Primary" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --uniform

    # Table 5
    # INLJ, Regular, Lazy
    ./exp_runner --M=64 --B=128 --ingestion --J="INLJ" --r_index="Regular" --s_index="Lazy" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --uniform
    # INLJ, Regular, CLazy
    ./exp_runner --M=64 --B=128 --ingestion --J="INLJ" --r_index="Regular" --s_index="CLazy" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --uniform
    # SJ, Regular, Regular
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Regular" --s_index="Regular" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --uniform
    # SJ, Regular, Lazy
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Regular" --s_index="Lazy" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --uniform
    # SJ, Lazy, Lazy
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Lazy" --s_index="Lazy" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --uniform
    # SJ, CComp, CComp
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="CComp" --s_index="CComp" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --uniform
    # SJ, Regular, CLazy
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Regular" --s_index="CLazy" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --uniform
    # HJ, Regular, Regular
    ./exp_runner --M=64 --B=128 --ingestion --J="HJ" --r_index="Regular" --s_index="Regular" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --uniform
done

# c value
output="test_7_c.txt"

# clear output file
rm -f $output

# Dataset Size
s_tuples=$((10 * Million))
r_tuples=$((10 * Million))

nums=(1 2 4 8 16)
for c in "${nums[@]}"; do
    # Table 4
    # INLJ, Regular, Primary
    ./exp_runner --M=64 --B=128 --ingestion --J="INLJ" --r_index="Regular" --s_index="Primary" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --c=$c --uniform
    # SJ, Regular, Primary
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Regular" --s_index="Primary" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --c=$c --uniform
    # SJ, Comp, Primary
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Comp" --s_index="Primary" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --c=$c --uniform
    # SJ, CComp, Primary
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="CComp" --s_index="Primary" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --c=$c --uniform
    # HJ, Regular, Primary
    ./exp_runner --M=64 --B=128 --ingestion --J="HJ" --r_index="Regular" --s_index="Primary" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --c=$c --uniform

    # Table 5
    # INLJ, Regular, Lazy
    ./exp_runner --M=64 --B=128 --ingestion --J="INLJ" --r_index="Regular" --s_index="Lazy" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --c=$c --uniform
    # INLJ, Regular, CLazy
    ./exp_runner --M=64 --B=128 --ingestion --J="INLJ" --r_index="Regular" --s_index="CLazy" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --c=$c --uniform
    # SJ, Regular, Regular
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Regular" --s_index="Regular" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --c=$c --uniform
    # SJ, Regular, Lazy
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Regular" --s_index="Lazy" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --c=$c --uniform
    # SJ, Lazy, Lazy
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Lazy" --s_index="Lazy" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --c=$c --uniform
    # SJ, CComp, CComp
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="CComp" --s_index="CComp" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --c=$c --uniform
    # SJ, Regular, CLazy
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Regular" --s_index="CLazy" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --c=$c --uniform
    # HJ, Regular, Regular
    ./exp_runner --M=64 --B=128 --ingestion --J="HJ" --r_index="Regular" --s_index="Regular" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --c=$c --uniform
done

# Buffer Size
output="test_7_buffer_size.txt"

# clear output file
rm -f $output

dataset="osm_cellids_800M_uint64"
data_path="/home/weiping/code/lsm_join_data/"
public_r="${data_path}${dataset}"
public_s="${data_path}${dataset}"

nums=(32 64 128 256)

for M in "${nums[@]}"; do
    # Table 4
    # INLJ, Regular, Primary
    ./exp_runner --M=$M --B=128 --ingestion --J="INLJ" --r_index="Regular" --s_index="Primary" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s
    # SJ, Regular, Primary
    ./exp_runner --M=$M --B=128 --ingestion --J="SJ" --r_index="Regular" --s_index="Primary" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s
    # SJ, Comp, Primary
    ./exp_runner --M=$M --B=128 --ingestion --J="SJ" --r_index="Comp" --s_index="Primary" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s
    # SJ, CComp, Primary
    ./exp_runner --M=$M --B=128 --ingestion --J="SJ" --r_index="CComp" --s_index="Primary" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s
    # HJ, Regular, Primary
    ./exp_runner --M=$M --B=128 --ingestion --J="HJ" --r_index="Regular" --s_index="Primary" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s

    # Table 5
    # INLJ, Regular, Lazy
    ./exp_runner --M=$M --B=128 --ingestion --J="INLJ" --r_index="Regular" --s_index="Lazy" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s
    # INLJ, Regular, CLazy
    ./exp_runner --M=$M --B=128 --ingestion --J="INLJ" --r_index="Regular" --s_index="CLazy" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s
    # SJ, Regular, Regular
    ./exp_runner --M=$M --B=128 --ingestion --J="SJ" --r_index="Regular" --s_index="Regular" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s
    # SJ, Regular, Lazy
    ./exp_runner --M=$M --B=128 --ingestion --J="SJ" --r_index="Regular" --s_index="Lazy" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s
    # SJ, Lazy, Lazy
    ./exp_runner --M=$M --B=128 --ingestion --J="SJ" --r_index="Lazy" --s_index="Lazy" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s
    # SJ, CComp, CComp
    ./exp_runner --M=$M --B=128 --ingestion --J="SJ" --r_index="CComp" --s_index="CComp" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s
    # SJ, Regular, CLazy
    ./exp_runner --M=$M --B=128 --ingestion --J="SJ" --r_index="Regular" --s_index="CLazy" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s
    # HJ, Regular, Regular
    ./exp_runner --M=$M --B=128 --ingestion --J="HJ" --r_index="Regular" --s_index="Regular" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s
done
