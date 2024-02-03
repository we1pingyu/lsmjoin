#!/bin/bash
make exp_runner

output="test_7_uniform.txt"

# clear output file
rm -f $output

# Dataset Size
# 1M: 1000000
Million=1000000

r_tuples=$((10 * Million))
s_tuples=$((10 * Million))

nums=(0 1 2 4 8)
for k in "${nums[@]}"; do
    if [ $k -eq 0 ]; then
        uniform_flag="--uniform"
        k_flag=""
    else
        uniform_flag=""
        k_flag="--k=$k"
    fi
    # Table 4
    # INLJ, Regular, Primary
    ./exp_runner --M=64 --B=128 --ingestion --J="INLJ" --r_index="Regular" --s_index="Primary" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples $uniform_flag $k_flag
    # SJ, Regular, Primary
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Regular" --s_index="Primary" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples $uniform_flag $k_flag
    # SJ, Eager, Primary
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Eager" --s_index="Primary" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples $uniform_flag $k_flag
    # SJ, CComp, Primary
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="CComp" --s_index="Primary" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples $uniform_flag $k_flag
    # HJ, Regular, Primary
    ./exp_runner --M=64 --B=128 --ingestion --J="HJ" --r_index="Regular" --s_index="Primary" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples $uniform_flag $k_flag

    # Table 5
    # INLJ, Regular, Lazy
    ./exp_runner --M=64 --B=128 --ingestion --J="INLJ" --r_index="Regular" --s_index="Lazy" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples $uniform_flag $k_flag
    # INLJ, Regular, CEager
    ./exp_runner --M=64 --B=128 --ingestion --J="INLJ" --r_index="Regular" --s_index="CEager" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples $uniform_flag $k_flag
    # SJ, Regular, Regular
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Regular" --s_index="Regular" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples $uniform_flag $k_flag
    # SJ, Regular, Comp
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Regular" --s_index="Comp" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples $uniform_flag $k_flag
    # SJ, Eager, Eager
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Eager" --s_index="Eager" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples $uniform_flag $k_flag
    # SJ, CLazy, CLazy
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="CLazy" --s_index="CLazy" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples $uniform_flag $k_flag
    # SJ, Regular, CComp
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Regular" --s_index="CComp" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples $uniform_flag $k_flag
    # HJ, Regular, Regular
    ./exp_runner --M=64 --B=128 --ingestion --J="HJ" --r_index="Regular" --s_index="Regular" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples $uniform_flag $k_flag
done
