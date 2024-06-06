#!/bin/bash

make exp_runner
root_path="/home/weiping/tmp/"
db_r_path="${root_path}db_r"
db_s_path="${root_path}db_s"
index_r_path="${root_path}index_r"
index_s_path="${root_path}index_s"
clear_path() {
    rm -rf "$db_r_path"/*
    rm -rf "$db_s_path"/*
    rm -rf "$index_r_path"/*
    rm -rf "$index_s_path"/*
}
data_path="/home/weiping/code/lsm_join_data/"
public_r="${data_path}movie_info_movie_id"
public_s="${data_path}cast_info_movie_id"

Million=1000000
# Dataset Size
s_tuples=$((10 * Million))
r_tuples=$((10 * Million))

output="test_inght_consist.txt"
rm -f $output

list1=(128 64 32 4 2 1) # (32 64 128 1024 4096)
list2=(1 16)
# list=(1 2)
for num2 in "${list2[@]}"; do
    for num1 in "${list1[@]}"; do
        ./exp_runner --J="INLJ" --r_index="Regular" --s_index="CLazy" --r_index_path=$index_r_path --s_index_path="/home/weiping/tmp/eager_index" --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --s_tuples=$s_tuples --r_tuples=$r_tuples --num_loop=$num2 --B=$num1 --noncovering --public_data --public_r=$public_r --public_s=$public_s

        # ./exp_runner --J="SJ" --r_index="Regular" --s_index="CLazy" --r_index_path=$index_r_path --s_index_path="/home/weiping/tmp/lazy_index" --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --s_tuples=$s_tuples --r_tuples=$r_tuples --num_loop=$num2 --B=$num1 --noncovering

        ./exp_runner --J="INLJ" --r_index="Regular" --s_index="Lazy" --r_index_path=$index_r_path --s_index_path="/home/weiping/tmp/eager_index" --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --s_tuples=$s_tuples --r_tuples=$r_tuples --num_loop=$num2 --B=$num1 

        # ./exp_runner --J="SJ" --r_index="Regular" --s_index="Lazy" --r_index_path=$index_r_path --s_index_path="/home/weiping/tmp/lazy_index" --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --s_tuples=$s_tuples --r_tuples=$r_tuples --num_loop=$num2 --B=$num1 

        ./exp_runner --J="INLJ" --r_index="Regular" --s_index="CLazy" --r_index_path=$index_r_path --s_index_path="/home/weiping/tmp/eager_index" --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --s_tuples=$s_tuples --r_tuples=$r_tuples --num_loop=$num2 --B=$num1 

        # ./exp_runner --J="SJ" --r_index="Regular" --s_index="CLazy" --r_index_path=$index_r_path --s_index_path="/home/weiping/tmp/lazy_index" --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --s_tuples=$s_tuples --r_tuples=$r_tuples --num_loop=$num2 --B=$num1
    done  
done  
