#!/bin/bash

make exp_runner
db_r_path="/tmp/db_r_ablation"
db_s_path="/tmp/db_s_ablation"
index_r_path="/tmp/index_r_ablation"
index_s_path="/tmp/index_s_ablation"

Million=1000000
# Dataset Size
s_tuples=$((10 * Million))
r_tuples=$((10 * Million))

output="test_7_T_t.txt"
rm -f $output
nums=(2 5 10 20)
for T in "${nums[@]}"; do
    ./exp_runner --J="SJ" --r_index="Comp" --s_index="Comp" --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --T=$T --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --theory
    ./exp_runner --J="INLJ" --r_index="Regular" --s_index="CComp" --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --T=$T --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --theory
    ./exp_runner --J="INLJ" --r_index="Regular" --s_index="CEager" --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --T=$T --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --theory
    ./exp_runner --J="SJ" --r_index="CLazy" --s_index="CLazy" --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --T=$T --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --theory
    ./exp_runner --J="SJ" --r_index="Regular" --s_index="CLazy" --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --T=$T --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --theory
done

# output="test_7_T.txt"
# rm -f $output
# nums=(2 5 10 20)
# for T in "${nums[@]}"; do
# ./exp_runner --J="SJ" --r_index="Comp" --s_index="Comp" --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --T=$T --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
#     ./exp_runner --J="INLJ" --r_index="Regular" --s_index="CComp" --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --T=$T --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
#     ./exp_runner --J="INLJ" --r_index="Regular" --s_index="CEager" --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --T=$T --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
# done

# output="test_7_K.txt"
# rm -f $output
# nums=(2 4 6 8)
# for K in "${nums[@]}"; do
#     echo ./exp_runner --J="SJ" --r_index="Comp" --s_index="Comp" --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --K=$K --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --theory
#     ./exp_runner --J="SJ" --r_index="Comp" --s_index="Comp" --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --K=$K --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --theory
#     ./exp_runner --J="INLJ" --r_index="Regular" --s_index="CComp" --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --K=$K --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --theory
#     ./exp_runner --J="INLJ" --r_index="Regular" --s_index="CEager" --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --K=$K --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --theory
# done

# output="test_7_buffer_size_t.txt"
# rm -f $output
# nums=(4 16 32 64)
# for M in "${nums[@]}"; do
#     ./exp_runner --M=$M --J="SJ" --r_index="Comp" --s_index="Comp" --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --T=5 --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --theory
#     ./exp_runner --M=$M --J="INLJ" --r_index="Regular" --s_index="CComp" --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --T=5 --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --theory
#     ./exp_runner --M=$M --J="INLJ" --r_index="Regular" --s_index="CEager" --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --T=5 --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --theory
# done

# output="test_7_buffer_size.txt"
# rm -f $output
# nums=(4 16 32 64)
# for M in "${nums[@]}"; do
#     ./exp_runner --M=$M --J="SJ" --r_index="Comp" --s_index="Comp" --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --T=5 --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
#     ./exp_runner --M=$M --J="INLJ" --r_index="Regular" --s_index="CComp" --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --T=5 --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
#     ./exp_runner --M=$M --J="INLJ" --r_index="Regular" --s_index="CEager" --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --T=5 --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
# done

# output="test_7_cache_size.txt"
# rm -f $output
# nums=(0 16 32 64)
# for num in "${nums[@]}"; do
#     ./exp_runner --J="INLJ" --r_index="Regular" --s_index="Primary" --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --cache_size=$num --r_tuples=$r_tuples --s_tuples=$s_tuples
#     ./exp_runner --J="SJ" --r_index="CComp" --s_index="CComp" --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --cache_size=$num --r_tuples=$r_tuples --s_tuples=$s_tuples
# done

# output="test_7_bpk.txt"
# rm -f $output
# nums=(2 5 10 20)
# for num in "${nums[@]}"; do
#     ./exp_runner --J="INLJ" --r_index="Regular" --s_index="CEager" --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --bpk=$num --r_tuples=$r_tuples --s_tuples=$s_tuples
#     ./exp_runner --J="INLJ" --r_index="Regular" --s_index="CLazy" --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --bpk=$num --r_tuples=$r_tuples --s_tuples=$s_tuples
#     ./exp_runner --J="INLJ" --r_index="Regular" --s_index="CComp" --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --bpk=$num --r_tuples=$r_tuples --s_tuples=$s_tuples
# done
