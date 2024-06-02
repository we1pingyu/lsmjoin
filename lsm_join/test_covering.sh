#!/bin/bash

make exp_runner
db_r_path="/home/weiping/tmp/db_r_breakdown"
db_s_path="/home/weiping/tmp/db_s_breakdown"
index_r_path="/home/weiping/tmp/index_r_breakdown"
index_s_path="/home/weiping/tmp/index_s_breakdown"

Million=1000000
# Dataset Size
s_tuples=$((10 * Million))
r_tuples=$((10 * Million))

output="test_covering.txt"
rm -f $output

list1=(128 64 32 4 2 1) # (32 64 128 1024 4096)
list2=(1 16)
# list=(1 2)
for num2 in "${list2[@]}"; do
    for num1 in "${list1[@]}"; do
        ./exp_runner --J="INLJ" --r_index="Regular" --s_index="CLazy" --r_index_path=$index_r_path --s_index_path="/home/weiping/tmp/eager_index" --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --s_tuples=$s_tuples --r_tuples=$r_tuples --num_loop=$num2 --B=$num1 --noncovering

        # ./exp_runner --J="SJ" --r_index="Regular" --s_index="CLazy" --r_index_path=$index_r_path --s_index_path="/home/weiping/tmp/lazy_index" --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --s_tuples=$s_tuples --r_tuples=$r_tuples --num_loop=$num2 --B=$num1 --noncovering

        ./exp_runner --J="INLJ" --r_index="Regular" --s_index="Lazy" --r_index_path=$index_r_path --s_index_path="/home/weiping/tmp/eager_index" --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --s_tuples=$s_tuples --r_tuples=$r_tuples --num_loop=$num2 --B=$num1 

        # ./exp_runner --J="SJ" --r_index="Regular" --s_index="Lazy" --r_index_path=$index_r_path --s_index_path="/home/weiping/tmp/lazy_index" --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --s_tuples=$s_tuples --r_tuples=$r_tuples --num_loop=$num2 --B=$num1 

        ./exp_runner --J="INLJ" --r_index="Regular" --s_index="CLazy" --r_index_path=$index_r_path --s_index_path="/home/weiping/tmp/eager_index" --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --s_tuples=$s_tuples --r_tuples=$r_tuples --num_loop=$num2 --B=$num1 

        # ./exp_runner --J="SJ" --r_index="Regular" --s_index="CLazy" --r_index_path=$index_r_path --s_index_path="/home/weiping/tmp/lazy_index" --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --s_tuples=$s_tuples --r_tuples=$r_tuples --num_loop=$num2 --B=$num1
    done  
done  
