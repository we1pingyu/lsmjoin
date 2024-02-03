#!/bin/bash
make exp_runner

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
    # SJ, CEager, Primary
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="CEager" --s_index="Primary" --num_loop=$num_loop --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s
    # HJ, Regular, Primary
    ./exp_runner --M=64 --B=128 --ingestion --J="HJ" --r_index="Regular" --s_index="Primary" --num_loop=$num_loop --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s

    # Table 5
    # INLJ, Regular, Eager
    ./exp_runner --M=64 --B=128 --ingestion --J="INLJ" --r_index="Regular" --s_index="Eager" --num_loop=$num_loop --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s
    # INLJ, Regular, CLazy
    ./exp_runner --M=64 --B=128 --ingestion --J="INLJ" --r_index="Regular" --s_index="CLazy" --num_loop=$num_loop --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s
    # SJ, Regular, Regular
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Regular" --s_index="Regular" --num_loop=$num_loop --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s
    # SJ, Regular, Eager
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Regular" --s_index="Eager" --num_loop=$num_loop --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s
    # SJ, Lazy, Lazy
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Lazy" --s_index="Lazy" --num_loop=$num_loop --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s
    # SJ, CComp, CComp
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="CComp" --s_index="CComp" --num_loop=$num_loop --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s
    # SJ, Regular, CEager
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Regular" --s_index="CEager" --num_loop=$num_loop --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s
    # HJ, Regular, Regular
    ./exp_runner --M=64 --B=128 --ingestion --J="HJ" --r_index="Regular" --s_index="Regular" --num_loop=$num_loop --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s
done
