#!/bin/bash

make exp_runner
root_path="/home/weiping/tmp/"
db_r_path="${root_path}db_r"
db_s_path="${root_path}db_s"
index_r_path="${root_path}index_r"
index_s_path="${root_path}index_s"
data_path="/home/weiping/code/lsm_join_data/"

clear_path() {
    rm -rf "$db_r_path"/*
    rm -rf "$db_s_path"/*
    rm -rf "$index_r_path"/*
    rm -rf "$index_s_path"/*
}
Million=1000000
# Dataset Size
s_tuples=$((10 * Million))
r_tuples=$((10 * Million))

output="test_5.4_movie.txt"
rm -f $output

public_r="${data_path}cast_info_movie_id"
public_s="${data_path}movie_info_movie_id"
list1=(1 2 4 8 16 32)
# list1=(2 1)
list2=(10)
for num2 in "${list2[@]}"; do
    for num1 in "${list1[@]}"; do
        clear_path
        ./exp_runner --J="SJ" --r_index="Regular" --s_index="Primary" --r_index_path=$index_r_path --s_index_path=$index_s_path --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --s_tuples=$s_tuples --r_tuples=$r_tuples --num_loop=$num1 --bpk=$num2 --public_r=$public_r --public_s=$public_s --public_data
        clear_path
        ./exp_runner --J="SJ" --r_index="CComp" --s_index="Primary" --r_index_path=$index_r_path --s_index_path=$index_s_path --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --s_tuples=$s_tuples --r_tuples=$r_tuples --num_loop=$num1  --bpk=$num2 --public_r=$public_r --public_s=$public_s --public_data
        clear_path
        ./exp_runner --J="INLJ" --r_index="Regular" --s_index="Primary" --r_index_path=$index_r_path --s_index_path=$index_s_path --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --s_tuples=$s_tuples --r_tuples=$r_tuples --num_loop=$num1 --bpk=$num2 --public_r=$public_r --public_s=$public_s --public_data  
        clear_path
        ./exp_runner --J="HJ" --r_index="Regular" --s_index="Primary" --r_index_path=$index_r_path --s_index_path=$index_s_path --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --s_tuples=$s_tuples --r_tuples=$r_tuples --num_loop=$num1 --bpk=$num2 --public_r=$public_r --public_s=$public_s --public_data
    done
done

output="test_5.4_wiki.txt"
rm -f $output
public_r="${data_path}wiki_ts_200M_uint64"
public_s="${data_path}wiki_ts_200M_uint64"
list1=(1 2 4 8 16 32)
# list1=(2 1)
list2=(10)
for num2 in "${list2[@]}"; do
    for num1 in "${list1[@]}"; do
        clear_path
        ./exp_runner --J="SJ" --r_index="Regular" --s_index="Primary" --r_index_path=$index_r_path --s_index_path=$index_s_path --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --s_tuples=$s_tuples --r_tuples=$r_tuples --num_loop=$num1 --bpk=$num2 --public_r=$public_r --public_s=$public_s --public_data
        clear_path
        ./exp_runner --J="SJ" --r_index="CComp" --s_index="Primary" --r_index_path=$index_r_path --s_index_path=$index_s_path --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --s_tuples=$s_tuples --r_tuples=$r_tuples --num_loop=$num1 --bpk=$num2 --public_r=$public_r --public_s=$public_s --public_data
        clear_path
        ./exp_runner --J="INLJ" --r_index="Regular" --s_index="Primary" --r_index_path=$index_r_path --s_index_path=$index_s_path --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --s_tuples=$s_tuples --r_tuples=$r_tuples --num_loop=$num1 --bpk=$num2 --public_r=$public_r --public_s=$public_s --public_data  
        clear_path
        ./exp_runner --J="HJ" --r_index="Regular" --s_index="Primary" --r_index_path=$index_r_path --s_index_path=$index_s_path --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --s_tuples=$s_tuples --r_tuples=$r_tuples --num_loop=$num1 --bpk=$num2 --public_r=$public_r --public_s=$public_s --public_data
    done
done
