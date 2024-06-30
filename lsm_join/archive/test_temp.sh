#!/bin/bash

make exp_runner
root_path="/home/weiping/tmp/"
db_r_path="${root_path}db_r"
db_s_path="${root_path}db_s"
index_r_path="${root_path}index_r"
index_s_path="${root_path}index_s"
data_path="/home/weiping/code/lsm_join_data/"
# public_r="${data_path}wiki_ts_200M_uint64"
# public_s="${data_path}wiki_ts_200M_uint64"
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

# k value
output="test_temp.txt"

# clear output file
rm -f $output

cs=(2)
ks=(0.1 0.3 0.5 0.7 0.9)
ks=(4) 
for c in "${cs[@]}"; do
    for k in "${ks[@]}"; do
        clear_path
        # ./exp_runner --J="INLJ" --r_index="Regular" --s_index="Lazy" --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --k_s=$k --skew
        # clear_path
        # ./exp_runner --J="INLJ" --r_index="Regular" --s_index="CLazy" --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --k_s=$k --skew
        # clear_path
        # ./exp_runner --J="INLJ" --r_index="Regular" --s_index="Eager" --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --k_s=$k --skew
        clear_path
        ./exp_runner --J="INLJ" --r_index="Regular" --s_index="CEager" --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --k_s=$k --c_s=$c --T=2
        clear_path
        ./exp_runner --J="INLJ" --r_index="Regular" --s_index="CLazy" --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --k_s=$k --c_s=$c --T=2
        clear_path
        ./exp_runner --J="INLJ" --r_index="Regular" --s_index="CComp" --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --k_s=$k --c_s=$c --T=2
        clear_path
        ./exp_runner --J="HJ" --r_index="Regular" --s_index="Regular" --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --k_s=$k --c_s=$c --T=2
        clear_path
        # ./exp_runner --J="SJ" --r_index="Regular" --s_index="Regular" --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --k_s=$k 
    done
done