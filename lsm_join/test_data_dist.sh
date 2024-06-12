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
output="test_c_k.txt"

# clear output file
rm -f $output

s_tuples=$((10 * Million))
r_tuples=$((10 * Million))

cs=(1 3 5 7 9 11)
ks=(1 3 5 7 9 11)
for c in "${cs[@]}"; do
    for k in "${ks[@]}"; do
        clear_path
        ./exp_runner --J="INLJ" --r_index="Regular" --s_index="Primary" --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --k_s=$k --k_r=$k --c_r=$c
        clear_path
        ./exp_runner --J="SJ" --r_index="Comp" --s_index="Primary" --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --k_s=$k --k_r=$k --c_r=$c
        clear_path
        ./exp_runner --J="SJ" --r_index="Eager" --s_index="Primary" --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --k_s=$k --k_r=$k --c_r=$c
        clear_path
        ./exp_runner --J="SJ" --r_index="CLazy" --s_index="Primary" --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --k_s=$k --k_r=$k --c_r=$c
        clear_path
        ./exp_runner --J="INLJ" --r_index="Regular" --s_index="CLazy" --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --k_s=$k --k_r=$k --c_r=$c
        clear_path
        ./exp_runner --J="SJ" --r_index="Comp" --s_index="Comp" --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --k_s=$k --k_r=$k --c_r=$c
        clear_path
        ./exp_runner --J="INLJ" --r_index="Regular" --s_index="Eager" --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --k_s=$k --k_r=$k --c_r=$c
        clear_path
        ./exp_runner --J="HJ" --r_index="Regular" --s_index="Regular" --output_file=$output --r_tuples=$r_tuples --c_r=$c --s_tuples=$s_tuples --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --k_s=$k --k_r=$k --c_r=$c
    done
done

# k value
output="test_c_skewness.txt"

# clear output file
rm -f $output

cs=(1 5 10)
# cs=(1)
ks=(0.1 0.2 0.3 0.4 0.5)
# ks=(0.1 0.5) 
for c in "${cs[@]}"; do
    for k in "${ks[@]}"; do
        clear_path
        ./exp_runner --J="INLJ" --r_index="Regular" --s_index="Primary" --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --k_s=$k --c_r=$c --skew
        clear_path
        ./exp_runner --J="INLJ" --r_index="Regular" --s_index="Comp" --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --k_s=$k --c_r=$c --skew
        clear_path
        ./exp_runner --J="INLJ" --r_index="Regular" --s_index="CLazy" --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --k_s=$k --c_r=$c --skew
        clear_path
        ./exp_runner --J="SJ" --r_index="CEager" --s_index="CEager" --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --k_s=$k --c_r=$c --skew
        clear_path
        ./exp_runner --J="INLJ" --r_index="Regular" --s_index="Eager" --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --k_s=$k --c_r=$c --skew
        clear_path
        ./exp_runner --J="HJ" --r_index="Regular" --s_index="Regular" --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --k_s=$k --c_r=$c --skew
    done
done

# # k value
# output="test_loops_size.txt"

# # clear output file
# rm -f $output

# num_loops=(1 2 3 4 5 6)
# datasizes=(10 20 30 40 50 60)
# for num_loop in "${num_loops[@]}"; do
#     for datasize in "${datasizes[@]}"; do
#         echo "num_loop: $num_loop, datasize: $datasize"
#         s_tuples=$(($datasize * $Million))
#         r_tuples=$(($datasize * $Million))
#         echo "./exp_runner --J="INLJ" --r_index="Regular" --s_index="Primary" --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --num_loop=$num_loop"
#         ./exp_runner --J="INLJ" --r_index="Regular" --s_index="Primary" --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --num_loop=$num_loop
#         ./exp_runner --J="INLJ" --r_index="Regular" --s_index="Comp" --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --num_loop=$num_loop
#         # ./exp_runner --J="INLJ" --r_index="Regular" --s_index="CLazy" --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --num_loop=$num_loop
#         ./exp_runner --J="SJ" --r_index="CLazy" --s_index="CLazy" --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --num_loop=$num_loop
#         ./exp_runner --J="SJ" --r_index="Eager" --s_index="Eager" --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --num_loop=$num_loop
#         ./exp_runner --J="HJ" --r_index="Regular" --s_index="Regular" --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --num_loop=$num_loop
#     done
# done
