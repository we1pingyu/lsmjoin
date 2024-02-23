#!/bin/bash
make exp_runner
db_r_path="/tmp/db_r_ablation"
db_s_path="/tmp/db_s_ablation"
index_r_path="/tmp/index_r_ablation"
index_s_path="/tmp/index_s_ablation"

# 1M: 1000000
Million=100000

# Tuple Size
output="test_7_B.txt"

# clear output file
rm -f $output

# data
data_path="/home/weiping/code/lsm_join_data/"
public_r="${data_path}question_user_id"
public_s="${data_path}so_user_user_id"
# Dataset Size
s_tuples=$((10 * Million))
r_tuples=$((10 * Million))
nums=(128 32 8)

for B in "${nums[@]}"; do
    ./exp_runner --B=$B --J="INLJ" --r_index="Regular" --s_index="Primary" --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --r_tuples=$r_tuples --s_tuples=$s_tuples
    ./exp_runner --B=$B --J="INLJ" --r_index="Regular" --s_index="Comp" --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --r_tuples=$r_tuples --s_tuples=$s_tuples
    ./exp_runner --B=$B --J="INLJ" --r_index="Regular" --s_index="CLazy" --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --r_tuples=$r_tuples --s_tuples=$s_tuples
    ./exp_runner --B=$B --J="SJ" --r_index="CLazy" --s_index="CLazy" --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --r_tuples=$r_tuples --s_tuples=$s_tuples
    ./exp_runner --B=$B --J="SJ" --r_index="Eager" --s_index="Eager" --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --r_tuples=$r_tuples --s_tuples=$s_tuples
    ./exp_runner --B=$B --J="HJ" --r_index="Regular" --s_index="Regular" --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --r_tuples=$r_tuples --s_tuples=$s_tuples
done

# Num Loop
output="test_7_num_loop.txt"

# clear output file
rm -f $output
data_path="/home/weiping/code/lsm_join_data/"
public_r="${data_path}question_user_id"
public_s="${data_path}so_user_user_id"
# Dataset Size
s_tuples=$((10 * Million))
r_tuples=$((10 * Million))
nums=(1 4 8)
for num_loop in "${nums[@]}"; do
    ./exp_runner --J="INLJ" --r_index="Regular" --s_index="Primary" --num_loop=$num_loop --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --r_tuples=$r_tuples --s_tuples=$s_tuples
    ./exp_runner --J="INLJ" --r_index="Regular" --s_index="Comp" --num_loop=$num_loop --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --r_tuples=$r_tuples --s_tuples=$s_tuples
    ./exp_runner --J="INLJ" --r_index="Regular" --s_index="CLazy" --num_loop=$num_loop --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --r_tuples=$r_tuples --s_tuples=$s_tuples
    ./exp_runner --J="SJ" --r_index="CLazy" --s_index="CLazy" --num_loop=$num_loop --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --r_tuples=$r_tuples --s_tuples=$s_tuples
    ./exp_runner --J="SJ" --r_index="Eager" --s_index="Eager" --num_loop=$num_loop --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --r_tuples=$r_tuples --s_tuples=$s_tuples
    ./exp_runner --J="HJ" --r_index="Regular" --s_index="Regular" --num_loop=$num_loop --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --r_tuples=$r_tuples --s_tuples=$s_tuples
done

# Dataset Size
output="test_7_dataset_size.txt"

# clear output file
rm -f $output

# Dataset Size
# r_tuples and s_tuples are 1M, 16M, 64M, 128M
nums=(1 10 50)
for num in "${nums[@]}"; do
    r_tuples=$((num * Million))
    s_tuples=$((num * Million))
    ./exp_runner --J="INLJ" --r_index="Regular" --s_index="Primary" --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples $uniform_flag --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
    ./exp_runner --J="INLJ" --r_index="Regular" --s_index="Comp" --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples $uniform_flag --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
    ./exp_runner --J="INLJ" --r_index="Regular" --s_index="CLazy" --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples $uniform_flag --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
    ./exp_runner --J="SJ" --r_index="CLazy" --s_index="CLazy" --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples $uniform_flag --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
    ./exp_runner --J="SJ" --r_index="Eager" --s_index="Eager" --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples $uniform_flag --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
    ./exp_runner --J="HJ" --r_index="Regular" --s_index="Regular" --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples $uniform_flag --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
done

# Data Ratio
output="test_7_dataratio.txt"

# clear output file
rm -f $output

# Dataset Size
s_tuples=$((10 * Million))
nums=(0.1 1 10)
for num in "${nums[@]}"; do
    r_tuples=$(echo "$num * $s_tuples" | bc)
    ./exp_runner --J="INLJ" --r_index="Regular" --s_index="Primary" --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples $uniform_flag --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
    ./exp_runner --J="INLJ" --r_index="Regular" --s_index="Comp" --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples $uniform_flag --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
    ./exp_runner --J="INLJ" --r_index="Regular" --s_index="CLazy" --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples $uniform_flag --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
    ./exp_runner --J="SJ" --r_index="CLazy" --s_index="CLazy" --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples $uniform_flag --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
    ./exp_runner --J="SJ" --r_index="Eager" --s_index="Eager" --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples $uniform_flag --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
    ./exp_runner --J="HJ" --r_index="Regular" --s_index="Regular" --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples $uniform_flag --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
done

# K value
output="test_7_k.txt"

# clear output file
rm -f $output

s_tuples=$((10 * Million))
r_tuples=$((10 * Million))

nums=(2 4 8)
for k in "${nums[@]}"; do
    ./exp_runner --J="INLJ" --r_index="Regular" --s_index="Primary" --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples $uniform_flag --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path    --k=$k
    ./exp_runner --J="INLJ" --r_index="Regular" --s_index="Comp" --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples $uniform_flag --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path    --k=$k
    ./exp_runner --J="INLJ" --r_index="Regular" --s_index="CLazy" --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples $uniform_flag --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path    --k=$k
    ./exp_runner --J="SJ" --r_index="CLazy" --s_index="CLazy" --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples $uniform_flag --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path    --k=$k
    ./exp_runner --J="SJ" --r_index="Eager" --s_index="Eager" --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples $uniform_flag --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path    --k=$k
    ./exp_runner --J="HJ" --r_index="Regular" --s_index="Regular" --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples $uniform_flag --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path    --k=$k
done

# skewness
output="test_7_skewness.txt"

# clear output file
rm -f $output

# Dataset Size
s_tuples=$((10 * Million))
r_tuples=$((10 * Million))

nums=(2 4 6)
for k in "${nums[@]}"; do
    ./exp_runner --J="INLJ" --r_index="Regular" --s_index="Primary" --output_file=$output --k=$k --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --r_tuples=$r_tuples --s_tuples=$s_tuples
    ./exp_runner --J="INLJ" --r_index="Regular" --s_index="Comp" --output_file=$output --k=$k --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --r_tuples=$r_tuples --s_tuples=$s_tuples
    ./exp_runner --J="INLJ" --r_index="Regular" --s_index="CLazy" --output_file=$output --k=$k--db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --r_tuples=$r_tuples --s_tuples=$s_tuples
    ./exp_runner --J="SJ" --r_index="CLazy" --s_index="CLazy" --output_file=$output --k=$k --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --r_tuples=$r_tuples --s_tuples=$s_tuples
    ./exp_runner --J="SJ" --r_index="Eager" --s_index="Eager" --output_file=$output --k=$k --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --r_tuples=$r_tuples --s_tuples=$s_tuples
    ./exp_runner --J="HJ" --r_index="Regular" --s_index="Regular" --output_file=$output --k=$k --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --r_tuples=$r_tuples --s_tuples=$s_tuples
done

# c value
output="test_7_c.txt"

# clear output file
rm -f $output

# Dataset Size
s_tuples=$((10 * Million))
r_tuples=$((10 * Million))

nums=(1 4 16)
for c in "${nums[@]}"; do
    ./exp_runner --J="INLJ" --r_index="Regular" --s_index="Primary" --output_file=$output --c=$c --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --r_tuples=$r_tuples --s_tuples=$s_tuples
    ./exp_runner --J="INLJ" --r_index="Regular" --s_index="Comp" --output_file=$output --c=$c --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --r_tuples=$r_tuples --s_tuples=$s_tuples
    ./exp_runner --J="INLJ" --r_index="Regular" --s_index="CLazy" --output_file=$output --c=$c--db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --r_tuples=$r_tuples --s_tuples=$s_tuples
    ./exp_runner --J="SJ" --r_index="CLazy" --s_index="CLazy" --output_file=$output --c=$c --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --r_tuples=$r_tuples --s_tuples=$s_tuples
    ./exp_runner --J="SJ" --r_index="Eager" --s_index="Eager" --output_file=$output --c=$c --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --r_tuples=$r_tuples --s_tuples=$s_tuples
    ./exp_runner --J="HJ" --r_index="Regular" --s_index="Regular" --output_file=$output --c=$c --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --r_tuples=$r_tuples --s_tuples=$s_tuples
done
