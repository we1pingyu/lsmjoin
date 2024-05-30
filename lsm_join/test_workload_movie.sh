#!/bin/bash

make exp_runner
db_r_path="/home/weiping/tmp/db_r_breakdown"
db_s_path="/home/weiping/tmp/db_s_breakdown"
index_r_path="/home/weiping/tmp/index_r_breakdown"
index_s_path="/home/weiping/tmp/index_s_breakdown"
data_path="/home/weiping/code/lsm_join_data/"

Million=1000000
# Dataset Size
s_tuples=$((10 * Million))
r_tuples=$((10 * Million))

output="test_7_workload_movie.txt"
rm -f $output

public_r="${data_path}movie_info_movie_id"
public_s="${data_path}cast_info_movie_id"

list=(1 2 4 8 16 32)
# list=(2)
for num in "${list[@]}"; do
    ./exp_runner --J="INLJ" --r_index="Regular" --s_index="CEager" --r_index_path=$index_r_path --s_index_path="/home/weiping/tmp/eager_index" --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --s_tuples=$s_tuples --r_tuples=$r_tuples --num_loop=$num --public_r=$public_r --public_s=$public_s --public_data

    ./exp_runner --J="INLJ" --r_index="Regular" --s_index="CLazy" --r_index_path=$index_r_path --s_index_path="/home/weiping/tmp/lazy_index" --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --s_tuples=$s_tuples --r_tuples=$r_tuples --num_loop=$num --public_r=$public_r --public_s=$public_s --public_data

    ./exp_runner --J="INLJ" --r_index="Regular" --s_index="CComp" --r_index_path=$index_r_path --s_index_path="/home/weiping/tmp/comp_index" --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --s_tuples=$s_tuples --r_tuples=$r_tuples --num_loop=$num --public_r=$public_r --public_s=$public_s --public_data
done