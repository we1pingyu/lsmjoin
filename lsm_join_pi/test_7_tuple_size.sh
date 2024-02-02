#!/bin/bash
make exp_runner

output="test_7_tuple_size.txt"

# clear output file
rm -f $output

# Tuple Size
dataset="user_id"
public_data_flag=""
public_r="${data_path}question_user_id"
public_s="${data_path}so_user_user_id"

nums=(128 64 32 16)

for B in "${nums[@]}"; do
    # Table 4
    # INLJ, Regular, Primary
    ./exp_runner --M=64 --B=$B --ingestion --J="INLJ" --r_index="Regular" --s_index="Primary" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s
    # SJ, Regular, Primary
    ./exp_runner --M=64 --B=$B --ingestion --J="SJ" --r_index="Regular" --s_index="Primary" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s
    # SJ, Lazy, Primary
    ./exp_runner --M=64 --B=$B --ingestion --J="SJ" --r_index="Lazy" --s_index="Primary" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s
    # SJ, CComp, Primary
    ./exp_runner --M=64 --B=$B --ingestion --J="SJ" --r_index="CComp" --s_index="Primary" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s
    # HJ, Regular, Primary
    ./exp_runner --M=64 --B=$B --ingestion --J="HJ" --r_index="Regular" --s_index="Primary" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s

    # Table 5
    # INLJ, Regular, Comp
    ./exp_runner --M=64 --B=$B --ingestion --J="INLJ" --r_index="Regular" --s_index="Comp" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s
    # INLJ, Regular, CEager
    ./exp_runner --M=64 --B=$B --ingestion --J="INLJ" --r_index="Regular" --s_index="CEager" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s
    # SJ, Regular, Regular
    ./exp_runner --M=64 --B=$B --ingestion --J="SJ" --r_index="Regular" --s_index="Regular" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s
    # SJ, Regular, Lazy
    ./exp_runner --M=64 --B=$B --ingestion --J="SJ" --r_index="Regular" --s_index="Lazy" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s
    # SJ, Comp, Comp
    ./exp_runner --M=64 --B=$B --ingestion --J="SJ" --r_index="Comp" --s_index="Comp" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s
    # SJ, Regular, CEager
    ./exp_runner --M=64 --B=$B --ingestion --J="SJ" --r_index="Regular" --s_index="CEager" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s
    # SJ, CLazy, CLazy
    ./exp_runner --M=64 --B=$B --ingestion --J="SJ" --r_index="CLazy" --s_index="CLazy" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s
    # HJ, Regular, Regular
    ./exp_runner --M=64 --B=$B --ingestion --J="HJ" --r_index="Regular" --s_index="Regular" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s
done
