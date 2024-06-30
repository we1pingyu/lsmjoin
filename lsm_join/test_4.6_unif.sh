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

# 1M: 1000000
Million=1000000

# Dataset Size
s_tuples=$((10 * Million))
r_tuples=$((10 * Million))

# k value
output="test_5.8_unif_ck.txt"

# clear output file
rm -f $output

s_tuples=$((10 * Million))
r_tuples=$((10 * Million))

cs=(2 4 6 8 10)
ks=(2 4 6 8 10)
for c in "${cs[@]}"; do
    for k in "${ks[@]}"; do
        clear_path
        ./exp_runner --J="INLJ" --r_index="Regular" --s_index="Lazy" --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --c_s=$c --c_r=$c --k_s=$k --k_r=$k
        clear_path
        ./exp_runner --J="INLJ" --r_index="Regular" --s_index="CLazy" --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --c_s=$c --c_r=$c --k_s=$k --k_r=$k
        clear_path
        ./exp_runner --J="INLJ" --r_index="Regular" --s_index="Eager" --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --c_s=$c --c_r=$c --k_s=$k --k_r=$k
        clear_path
        ./exp_runner --J="INLJ" --r_index="Regular" --s_index="CEager" --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --c_s=$c --c_r=$c --k_s=$k --k_r=$k
        clear_path
        ./exp_runner --J="INLJ" --r_index="Regular" --s_index="Comp" --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --c_s=$c --c_r=$c --k_s=$k --k_r=$k
        clear_path
        ./exp_runner --J="INLJ" --r_index="Regular" --s_index="CComp" --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --c_s=$c --c_r=$c --k_s=$k --k_r=$k
    done
done


