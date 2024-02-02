#!/bin/bash
make exp_runner

output="test_7_k.txt"

# clear output file
rm -f $output

# Dataset Size
# 1M: 1000000
Million=1000000

s_tuples=$((10 * Million))
r_tuples=$((10 * Million))

nums=(1 2 4 8 16)
for k in "${nums[@]}"; do
    # Table 4
    # INLJ, Regular, Primary
    ./exp_runner --M=64 --B=128 --ingestion --J="INLJ" --r_index="Regular" --s_index="Primary" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --k=$k --uniform
    # SJ, Regular, Primary
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Regular" --s_index="Primary" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --k=$k --uniform
    # SJ, Eager, Primary
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Eager" --s_index="Primary" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --k=$k --uniform
    # SJ, CComp, Primary
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="CComp" --s_index="Primary" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --k=$k --uniform
    # HJ, Regular, Primary
    ./exp_runner --M=64 --B=128 --ingestion --J="HJ" --r_index="Regular" --s_index="Primary" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --k=$k --uniform

    # Table 5
    # INLJ, Regular, Lazy
    ./exp_runner --M=64 --B=128 --ingestion --J="INLJ" --r_index="Regular" --s_index="Lazy" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --k=$k --uniform
    # INLJ, Regular, CEager
    ./exp_runner --M=64 --B=128 --ingestion --J="INLJ" --r_index="Regular" --s_index="CEager" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --k=$k --uniform
    # SJ, Regular, Regular
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Regular" --s_index="Regular" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --k=$k --uniform
    # SJ, Regular, Comp
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Regular" --s_index="Comp" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --k=$k --uniform
    # SJ, Eager, Eager
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Eager" --s_index="Eager" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --k=$k --uniform
    # SJ, CLazy, CLazy
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="CLazy" --s_index="CLazy" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --k=$k --uniform
    # SJ, Regular, CComp
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Regular" --s_index="CComp" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --k=$k --uniform
    # HJ, Regular, Regular
    ./exp_runner --M=64 --B=128 --ingestion --J="HJ" --r_index="Regular" --s_index="Regular" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --k=$k --uniform
done
