#!/bin/bash
make exp_runner

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
    # SJ, Eager, Primary
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Eager" --s_index="Primary" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --page_size=$page_size
    # SJ, CComp, Primary
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="CComp" --s_index="Primary" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --page_size=$page_size
    # HJ, Regular, Primary
    ./exp_runner --M=64 --B=128 --ingestion --J="HJ" --r_index="Regular" --s_index="Primary" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --page_size=$page_size

    # Table 5
    # INLJ, Regular, Lazy
    ./exp_runner --M=64 --B=128 --ingestion --J="INLJ" --r_index="Regular" --s_index="Lazy" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --page_size=$page_size
    # INLJ, Regular, CEager
    ./exp_runner --M=64 --B=128 --ingestion --J="INLJ" --r_index="Regular" --s_index="CEager" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --page_size=$page_size
    # SJ, Regular, Regular
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Regular" --s_index="Regular" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --page_size=$page_size
    # SJ, Regular, Comp
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Regular" --s_index="Comp" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --page_size=$page_size
    # SJ, Eager, Eager
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Eager" --s_index="Eager" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --page_size=$page_size
    # SJ, CLazy, CLazy
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="CLazy" --s_index="CLazy" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --page_size=$page_size
    # SJ, Regular, CComp
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Regular" --s_index="CComp" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --page_size=$page_size
    # HJ, Regular, Regular
    ./exp_runner --M=64 --B=128 --ingestion --J="HJ" --r_index="Regular" --s_index="Regular" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --page_size=$page_size
done
