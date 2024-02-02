#!/bin/bash
make exp_runner

output="test_7_dataset_size.txt"

# clear output file
rm -f $output

# Dataset Size
# 1M: 1000000
Million=1000000
# r_tuples and s_tuples are 1M, 16M, 64M, 128M
nums=(1 8 16)
# nums=(8 32)
for num in "${nums[@]}"; do
    r_tuples=$((num * Million))
    s_tuples=$((num * Million))
    # Table 4
    # # INLJ, Regular, Primary
    # ./exp_runner --M=64 --B=128 --ingestion --J="INLJ" --r_index="Regular" --s_index="Primary" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples
    # # SJ, Regular, Primary
    # ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Regular" --s_index="Primary" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples
    # # SJ, Comp, Primary
    # ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Comp" --s_index="Primary" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples
    # # SJ, CEager, Primary
    # ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="CEager" --s_index="Primary" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples
    # # HJ, Regular, Primary
    # ./exp_runner --M=64 --B=128 --ingestion --J="HJ" --r_index="Regular" --s_index="Primary" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples

    # Table 5
    # INLJ, Regular, Eager
    ./exp_runner --M=64 --B=128 --ingestion --J="INLJ" --r_index="Regular" --s_index="Eager" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --uniform
    # INLJ, Regular, CLazy
    ./exp_runner --M=64 --B=128 --ingestion --J="INLJ" --r_index="Regular" --s_index="CLazy" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --uniform
    # SJ, Regular, Regular
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Regular" --s_index="Regular" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --uniform
    # SJ, Regular, Eager
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Regular" --s_index="Eager" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --uniform
    # SJ, Lazy, Lazy
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Lazy" --s_index="Lazy" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --uniform
    # SJ, CComp, CComp
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="CComp" --s_index="CComp" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --uniform
    # SJ, Regular, CEager
    ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Regular" --s_index="CEager" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --uniform
    # HJ, Regular, Regular
    ./exp_runner --M=64 --B=128 --ingestion --J="HJ" --r_index="Regular" --s_index="Regular" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --uniform
done
