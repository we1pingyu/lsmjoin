#!/bin/bash
make exp_runner
root_path="/home/weiping/tmp/"
db_r_path="${root_path}db_r"
db_s_path="${root_path}db_s"
index_r_path="${root_path}index_r"
index_s_path="${root_path}index_s"

# 1M: 1000000
Million=1000000

output="test_num_loop1.txt"

# clear output file
rm -f $output

num_loops=(10)
datasize=10
for num_loop in "${num_loops[@]}"; do
    echo "num_loop: $num_loop, datasize: $datasize"
    s_tuples=$(($datasize * $Million))
    r_tuples=$(($datasize * $Million))
    echo "./exp_runner --J="INLJ" --r_index="Regular" --s_index="Primary" --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --num_loop=$num_loop"
    ./exp_runner --J="INLJ" --r_index="Regular" --s_index="Primary" --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --num_loop=$num_loop
    ./exp_runner --J="INLJ" --r_index="Regular" --s_index="CComp" --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --num_loop=$num_loop
    # ./exp_runner --J="INLJ" --r_index="Regular" --s_index="CLazy" --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --num_loop=$num_loop
    ./exp_runner --J="SJ" --r_index="CLazy" --s_index="CLazy" --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --num_loop=$num_loop
    ./exp_runner --J="SJ" --r_index="Eager" --s_index="Eager" --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --num_loop=$num_loop
    ./exp_runner --J="HJ" --r_index="Regular" --s_index="Regular" --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --num_loop=$num_loop
done


output="test_num_loop2.txt"

# clear output file
rm -f $output

num_loops=(10)
datasize=10
for num_loop in "${num_loops[@]}"; do
    echo "num_loop: $num_loop, datasize: $datasize"
    s_tuples=$(($datasize * $Million))
    r_tuples=$(($datasize * $Million))
    echo "./exp_runner --J="INLJ" --r_index="Regular" --s_index="Primary" --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --num_loop=$num_loop"
    ./exp_runner --J="INLJ" --r_index="Regular" --s_index="Primary" --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --num_loop=$num_loop --skew --k_s=0.5
    ./exp_runner --J="INLJ" --r_index="Regular" --s_index="CComp" --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --num_loop=$num_loop --skew --k_s=0.5
    # ./exp_runner --J="INLJ" --r_index="Regular" --s_index="CLazy" --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --num_loop=$num_loop --skew
    ./exp_runner --J="SJ" --r_index="CLazy" --s_index="CLazy" --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples  --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --num_loop=$num_loop --skew --k_s=0.5
    ./exp_runner --J="SJ" --r_index="Eager" --s_index="Eager" --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --num_loop=$num_loop --skew --k_s=0.5
    ./exp_runner --J="HJ" --r_index="Regular" --s_index="Regular" --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --num_loop=$num_loop --skew --k_s=0.5
done