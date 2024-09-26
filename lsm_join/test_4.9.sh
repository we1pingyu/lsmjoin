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
Million=1000000
# Dataset Size
s_tuples=$((10 * Million))
r_tuples=$((10 * Million))

# output="test_T_t.txt"
# rm -f $output
# nums=(2 5 20 100)
# for T in "${nums[@]}"; do
#     ./exp_runner --J="SJ" --r_index="Comp" --s_index="Comp" --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --T=$T --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --theory 
#     ./exp_runner --J="INLJ" --r_index="Regular" --s_index="CComp" --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --T=$T --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --theory 
#     ./exp_runner --J="INLJ" --r_index="Regular" --s_index="CEager" --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --T=$T --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --theory 
#     ./exp_runner --J="SJ" --r_index="CLazy" --s_index="CLazy" --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --T=$T --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --theory 
#     ./exp_runner --J="SJ" --r_index="Regular" --s_index="CLazy" --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --T=$T --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --theory 
# done

output="test_T.txt"
rm -f $output
nums=(2 5 10 20 50 100)
for T in "${nums[@]}"; do
    clear_path
    ./exp_runner --J="SJ" --r_index="Comp" --s_index="Comp" --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --T=$T --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path 
    clear_path
    ./exp_runner --J="INLJ" --r_index="Regular" --s_index="CComp" --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --T=$T --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path 
    clear_path
    ./exp_runner --J="INLJ" --r_index="Regular" --s_index="CEager" --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --T=$T --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path 
    clear_path
    ./exp_runner --J="INLJ" --r_index="Regular" --s_index="Lazy" --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --T=$T --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path 
    clear_path
    ./exp_runner --J="SJ" --r_index="Regular" --s_index="CLazy" --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --T=$T --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path 
done

# output="test_K.txt"
# rm -f $output
# nums=(2 3 4 5)
# for K in "${nums[@]}"; do
#     ./exp_runner --J="SJ" --r_index="Comp" --s_index="Comp" --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --K=$K --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --theory 
#     ./exp_runner --J="INLJ" --r_index="Regular" --s_index="CComp" --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --K=$K --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --theory 
#     ./exp_runner --J="INLJ" --r_index="Regular" --s_index="CEager" --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --K=$K --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --theory 
#     ./exp_runner --J="SJ" --r_index="CLazy" --s_index="CLazy" --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --K=$K --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --theory 
#     ./exp_runner --J="SJ" --r_index="Regular" --s_index="CLazy" --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --K=$K --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --theory 
# done

# output="test_buffer_size_t.txt"
# rm -f $output
# nums=(4 16 64 128)
# for M in "${nums[@]}"; do
#     ./exp_runner --M=$M --J="SJ" --r_index="Comp" --s_index="Comp" --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --theory 
#     ./exp_runner --M=$M --J="INLJ" --r_index="Regular" --s_index="CComp" --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --theory 
#     ./exp_runner --M=$M --J="INLJ" --r_index="Regular" --s_index="CEager" --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --theory 
#     ./exp_runner --M=$M --J="SJ" --r_index="CLazy" --s_index="CLazy" --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --theory 
#     ./exp_runner --M=$M --J="SJ" --r_index="Regular" --s_index="CLazy" --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --theory 
# done

output="test_buffer_size.txt"
rm -f $output
nums=(8 16 32 64 128 256)
for M in "${nums[@]}"; do
    clear_path
    ./exp_runner --M=$M --J="SJ" --r_index="Comp" --s_index="Comp" --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path 
    clear_path
    ./exp_runner --M=$M --J="INLJ" --r_index="Regular" --s_index="CComp" --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path 
    clear_path
    ./exp_runner --M=$M --J="INLJ" --r_index="Regular" --s_index="CEager" --output_file=$output --r_tuples=$r_tuple --s_tuples=$s_tuples --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path 
    clear_path
    ./exp_runner --M=$M --J="INLJ" --r_index="Regular" --s_index="Lazy" --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path 
    clear_path
    ./exp_runner --M=$M --J="SJ" --r_index="Regular" --s_index="CLazy" --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
done

# output="test_cache_size.txt"
# rm -f $output
# nums=(0 64 128 256)
# for num in "${nums[@]}"; do
#     echo "./exp_runner --J="INLJ" --r_index="Regular" --s_index="Primary" --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --cache_size=$num --r_tuples=$r_tuples --s_tuples=$s_tuples  "
#     ./exp_runner --J="INLJ" --r_index="Regular" --s_index="CEager" --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --cache_size=$num --r_tuples=$r_tuples --s_tuples=$s_tuples 
#     ./exp_runner --J="INLJ" --r_index="Regular" --s_index="CLazy" --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --cache_size=$num --r_tuples=$r_tuples --s_tuples=$s_tuples 
#     ./exp_runner --J="INLJ" --r_index="Regular" --s_index="Lazy" --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --cache_size=$num --r_tuples=$r_tuples --s_tuples=$s_tuples 
#     ./exp_runner --J="SJ" --r_index="CComp" --s_index="CComp" --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --cache_size=$num --r_tuples=$r_tuples --s_tuples=$s_tuples
#     ./exp_runner --J="SJ" --r_index="Eager" --s_index="Eager" --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --cache_size=$num --r_tuples=$r_tuples --s_tuples=$s_tuples
# done

# output="test_bpk.txt"
# rm -f $output
# nums=(2 5 10 20)
# for num in "${nums[@]}"; do
#     ./exp_runner --J="INLJ" --r_index="Regular" --s_index="CEager" --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --bpk=$num --r_tuples=$r_tuples --s_tuples=$s_tuples 
#     ./exp_runner --J="INLJ" --r_index="Regular" --s_index="CLazy" --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --bpk=$num --r_tuples=$r_tuples --s_tuples=$s_tuples 
#     ./exp_runner --J="INLJ" --r_index="Regular" --s_index="CComp" --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --bpk=$num --r_tuples=$r_tuples --s_tuples=$s_tuples
#     ./exp_runner --J="INLJ" --r_index="Regular" --s_index="Lazy" --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --bpk=$num --r_tuples=$r_tuples --s_tuples=$s_tuples
#     ./exp_runner --J="SJ" --r_index="CComp" --s_index="CComp" --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --bpk=$num --r_tuples=$r_tuples --s_tuples=$s_tuples 
# done