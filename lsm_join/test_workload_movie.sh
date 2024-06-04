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

output="test_workload_movie.txt"
rm -f $output

public_r="${data_path}movie_info_movie_id"
public_s="${data_path}cast_info_movie_id"

list1=(2 4 6 8 10 12)
list2=(1 5)
for num2 in "${list2[@]}"; do
    for num1 in "${list1[@]}"; do
        ./exp_runner --J="INLJ" --r_index="Regular" --s_index="CEager" --r_index_path=$index_r_path --s_index_path="/home/weiping/tmp/eager_index" --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --s_tuples=$s_tuples --r_tuples=$r_tuples --num_loop=$num1 --public_r=$public_r --public_s=$public_s --public_data --c_s=$num2

        ./exp_runner --J="INLJ" --r_index="Regular" --s_index="CLazy" --r_index_path=$index_r_path --s_index_path="/home/weiping/tmp/lazy_index" --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --s_tuples=$s_tuples --r_tuples=$r_tuples --num_loop=$num1 --public_r=$public_r --public_s=$public_s --public_data --c_s=$num2

        ./exp_runner --J="INLJ" --r_index="Regular" --s_index="CComp" --r_index_path=$index_r_path --s_index_path="/home/weiping/tmp/comp_index" --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --s_tuples=$s_tuples --r_tuples=$r_tuples --num_loop=$num1 --public_r=$public_r --public_s=$public_s --public_data --c_s=$num2
    done
done