#!/bin/bash
make exp_runner

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
    ./exp_runner --M=64 --B=128 --ingestion --J="INLJ" --r_index="Regular" --s_index="Primary" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s $use_cache_flag
    # SJ, Regular, Primary
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Regular" --s_index="Primary" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s $use_cache_flag
    # SJ, Comp, Primary
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Comp" --s_index="Primary" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s $use_cache_flag
    # SJ, CEager, Primary
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="CEager" --s_index="Primary" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s $use_cache_flag
    # HJ, Regular, Primary
    ./exp_runner --M=64 --B=128 --ingestion --J="HJ" --r_index="Regular" --s_index="Primary" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s $use_cache_flag

    # Table 5
    # INLJ, Regular, Eager
    ./exp_runner --M=64 --B=128 --ingestion --J="INLJ" --r_index="Regular" --s_index="Eager" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --uniform $use_cache_flag
    # INLJ, Regular, CLazy
    ./exp_runner --M=64 --B=128 --ingestion --J="INLJ" --r_index="Regular" --s_index="CLazy" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --uniform $use_cache_flag
    # SJ, Regular, Regular
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Regular" --s_index="Regular" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --uniform $use_cache_flag
    # SJ, Regular, Eager
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Regular" --s_index="Eager" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --uniform $use_cache_flag
    # SJ, Lazy, Lazy
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Lazy" --s_index="Lazy" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --uniform $use_cache_flag
    # SJ, CComp, CComp
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="CComp" --s_index="CComp" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --uniform $use_cache_flag
    # SJ, Regular, CEager
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Regular" --s_index="CEager" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --uniform $use_cache_flag
    # HJ, Regular, Regular
    ./exp_runner --M=64 --B=128 --ingestion --J="HJ" --r_index="Regular" --s_index="Regular" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --uniform $use_cache_flag
done
