#!/bin/bash

make exp_runner
db_r_path="/tmp/db_r_ablation"
db_s_path="/tmp/db_s_ablation"
index_r_path="/tmp/index_r_ablation"
index_s_path="/tmp/index_s_ablation"
output="test_7_T_t.txt"

# clear output file
rm -f $output
Million=1000000
# Dataset Size
s_tuples=$((10 * Million))
r_tuples=$((10 * Million))

data_path="/home/weiping/code/lsm_join_data/"
public_r="${data_path}question_user_id"
public_s="${data_path}so_user_user_id"

nums=(2 5 20)
# for T in "${nums[@]}"; do
#     # Table 4
# #     # INLJ, Regular, Primary
# echo "./exp_runner --M=16 --B=128 --ingestion --J="INLJ" --r_index="Regular" --s_index="Primary" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --T=$T --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --theory"
# ./exp_runner --M=16 --B=128 --ingestion --J="INLJ" --r_index="Regular" --s_index="Primary" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --T=$T --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --theory
#     # # SJ, Regular, Primary
#     # ./exp_runner --M=16 --B=128 --ingestion --J="SJ" --r_index="Regular" --s_index="Primary" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --T=$T --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --theory
#     # SJ, Comp, Primary
#     # ./exp_runner --M=16 --B=128 --ingestion --J="SJ" --r_index="Comp" --s_index="Primary" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --T=$T --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --theory
#     # # SJ, CComp, Primary
#     # ./exp_runner --M=16 --B=128 --ingestion --J="SJ" --r_index="CComp" --s_index="Primary" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --T=$T --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --theory
#     # HJ, Regular, Primary
#     # ./exp_runner --M=16 --B=128 --ingestion --J="HJ" --r_index="Regular" --s_index="Primary" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --T=$T --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --theory

#     # Table 5
#     # INLJ, Regular, Lazy
# echo "./exp_runner --M=16 --B=128 --ingestion --J="INLJ" --r_index="Regular" --s_index="Lazy" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --T=$T --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --theory"
# ./exp_runner --M=16 --B=128 --ingestion --J="INLJ" --r_index="Regular" --s_index="Lazy" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --T=$T --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --theory
#     # # INLJ, Regular, CLazy
#     # ./exp_runner --M=16 --B=128 --ingestion --J="INLJ" --r_index="Regular" --s_index="CLazy" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --T=$T --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --theory
#     # # SJ, Regular, Regular
#     # ./exp_runner --M=16 --B=128 --ingestion --J="SJ" --r_index="Regular" --s_index="Regular" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --T=$T --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --theory
#     # SJ, Regular, Lazy
#     ./exp_runner --M=16 --B=128 --ingestion --J="SJ" --r_index="Lazy" --s_index="Lazy" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --T=$T --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --theory
#     # # SJ, Lazy, Lazy
#     ./exp_runner --M=16 --B=128 --ingestion --J="SJ" --r_index="CLazy" --s_index="CLazy" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --T=$T --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --theory
#     # SJ, CComp, CComp
# echo "./exp_runner --M=16 --B=128 --ingestion --J="SJ" --r_index="Comp" --s_index="Comp" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --T=$T --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --theory"
# ./exp_runner --M=16 --B=128 --ingestion --J="SJ" --r_index="CComp" --s_index="CComp" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --T=$T --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --theory
# # SJ, Regular, CLazy
# echo "./exp_runner --M=16 --B=128 --ingestion --J="SJ" --r_index="Lazy" --s_index="Lazy" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --T=$T --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --theory"
# ./exp_runner --M=16 --B=128 --ingestion --J="SJ" --r_index="CLazy" --s_index="CLazy" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --T=$T --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --theory
#     # HJ, Regular, Regular
#     # ./exp_runner --M=16 --B=128 --ingestion --J="HJ" --r_index="Regular" --s_index="Regular" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --T=$T --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --theory
# done

# output="test_7_T.txt"

# # clear output file
# rm -f $output
# Million=1000000
# # Dataset Size
# s_tuples=$((10 * Million))
# r_tuples=$((10 * Million))

# nums=(2 5 20)
# for T in "${nums[@]}"; do
#     # Table 4
#     # INLJ, Regular, Primary
#     # ./exp_runner --M=16 --B=128 --ingestion --J="INLJ" --r_index="Regular" --s_index="Primary" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --T=$T --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
#     # # SJ, Regular, Primary
#     # ./exp_runner --M=16 --B=128 --ingestion --J="SJ" --r_index="Regular" --s_index="Primary" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --T=$T --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
#     # SJ, Comp, Primary
#     # ./exp_runner --M=16 --B=128 --ingestion --J="SJ" --r_index="Comp" --s_index="Primary" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --T=$T --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
#     # # SJ, CComp, Primary
#     # ./exp_runner --M=16 --B=128 --ingestion --J="SJ" --r_index="CComp" --s_index="Primary" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --T=$T --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
#     # HJ, Regular, Primary
#     # ./exp_runner --M=16 --B=128 --ingestion --J="HJ" --r_index="Regular" --s_index="Primary" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --T=$T --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path

#     # Table 5
#     # INLJ, Regular, Lazy
#     # ./exp_runner --M=16 --B=128 --ingestion --J="INLJ" --r_index="Regular" --s_index="Lazy" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --T=$T --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
#     # # INLJ, Regular, CLazy
#     # ./exp_runner --M=16 --B=128 --ingestion --J="INLJ" --r_index="Regular" --s_index="CLazy" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --T=$T --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
#     # # SJ, Regular, Regular
#     # ./exp_runner --M=16 --B=128 --ingestion --J="SJ" --r_index="Regular" --s_index="Regular" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --T=$T --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
#     # SJ, Regular, Lazy
#     ./exp_runner --M=16 --B=128 --ingestion --J="SJ" --r_index="Lazy" --s_index="Lazy" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --T=$T --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
#     # SJ, Lazy, Lazy
#     ./exp_runner --M=16 --B=128 --ingestion --J="SJ" --r_index="CLazy" --s_index="CLazy" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --T=$T --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
#     # SJ, CComp, CComp
#     ./exp_runner --M=16 --B=128 --ingestion --J="SJ" --r_index="CComp" --s_index="CComp" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --T=$T --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
#     # SJ, Regular, CLazy
#     # ./exp_runner --M=16 --B=128 --ingestion --J="SJ" --r_index="Regular" --s_index="CLazy" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --T=$T --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
#     # HJ, Regular, Regular
#     # ./exp_runner --M=16 --B=128 --ingestion --J="HJ" --r_index="Regular" --s_index="Regular" --num_loop=1 --output_file=$output --r_tuples=$r_tuples --s_tuples=$s_tuples --T=$T --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
# done

# output="test_7_buffer_size_t.txt"

# # clear output file
# rm -f $output

# nums=(4 16 64)

# for M in "${nums[@]}"; do
#     # Table 4
#     # INLJ, Regular, Primary
#     # ./exp_runner --M=$M --B=128 --ingestion --J="INLJ" --r_index="Regular" --s_index="Primary" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --theory --s_index_path=$index_s_path
#     # SJ, Regular, Primary
#     # ./exp_runner --M=$M --B=128 --ingestion --J="SJ" --r_index="Regular" --s_index="Primary" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --theory --s_index_path=$index_s_path
#     # SJ, Comp, Primary
#     # ./exp_runner --M=$M --B=128 --ingestion --J="SJ" --r_index="Comp" --s_index="Primary" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --theory
#     # # SJ, CComp, Primary
#     # ./exp_runner --M=$M --B=128 --ingestion --J="SJ" --r_index="CComp" --s_index="Primary" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --theory
#     # HJ, Regular, Primary
#     # ./exp_runner --M=$M --B=128 --ingestion --J="HJ" --r_index="Regular" --s_index="Primary" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --theory --s_index_path=$index_s_path

#     # Table 5
#     # INLJ, Regular, Lazy
#     # ./exp_runner --M=$M --B=128 --ingestion --J="INLJ" --r_index="Regular" --s_index="Lazy" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --theory
#     # # INLJ, Regular, CLazy
#     # ./exp_runner --M=$M --B=128 --ingestion --J="INLJ" --r_index="Regular" --s_index="CLazy" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --theory --s_index_path=$index_s_path
#     # # SJ, Regular, Regular
#     # ./exp_runner --M=$M --B=128 --ingestion --J="SJ" --r_index="Regular" --s_index="Regular" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --theory --s_index_path=$index_s_path
#     # SJ, Regular, Lazy
#     ./exp_runner --M=$M --B=128 --ingestion --J="SJ" --r_index="Lazy" --s_index="Lazy" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --theory
#     # SJ, Lazy, Lazy
#     ./exp_runner --M=$M --B=128 --ingestion --J="SJ" --r_index="CLazy" --s_index="CLazy" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --theory
#     # SJ, CComp, CComp
#     ./exp_runner --M=$M --B=128 --ingestion --J="SJ" --r_index="CComp" --s_index="CComp" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --theory
#     # SJ, Regular, CLazy
#     # ./exp_runner --M=$M --B=128 --ingestion --J="SJ" --r_index="Regular" --s_index="CLazy" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --theory
#     # HJ, Regular, Regular
#     # ./exp_runner --M=$M --B=128 --ingestion --J="HJ" --r_index="Regular" --s_index="Regular" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --theory --s_index_path=$index_s_path
# done

# output="test_7_buffer_size.txt"

# # clear output file
# rm -f $output

# nums=(4 16 64)

# for M in "${nums[@]}"; do
#     # Table 4
#     # INLJ, Regular, Primary
#     # ./exp_runner --M=$M --B=128 --ingestion --J="INLJ" --r_index="Regular" --s_index="Primary" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
#     # SJ, Regular, Primary
#     # ./exp_runner --M=$M --B=128 --ingestion --J="SJ" --r_index="Regular" --s_index="Primary" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
#     # SJ, Comp, Primary
#     # ./exp_runner --M=$M --B=128 --ingestion --J="SJ" --r_index="Comp" --s_index="Primary" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
#     # # SJ, CComp, Primary
#     # ./exp_runner --M=$M --B=128 --ingestion --J="SJ" --r_index="CComp" --s_index="Primary" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
#     # HJ, Regular, Primary
#     # ./exp_runner --M=$M --B=128 --ingestion --J="HJ" --r_index="Regular" --s_index="Primary" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path

#     # Table 5
#     # INLJ, Regular, Lazy
#     # ./exp_runner --M=$M --B=128 --ingestion --J="INLJ" --r_index="Regular" --s_index="Lazy" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
#     # # INLJ, Regular, CLazy
#     # ./exp_runner --M=$M --B=128 --ingestion --J="INLJ" --r_index="Regular" --s_index="CLazy" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
#     # # SJ, Regular, Regular
#     # ./exp_runner --M=$M --B=128 --ingestion --J="SJ" --r_index="Regular" --s_index="Regular" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
#     # SJ, Regular, Lazy
#     ./exp_runner --M=$M --B=128 --ingestion --J="SJ" --r_index="Lazy" --s_index="Lazy" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
#     # SJ, Lazy, Lazy
#     ./exp_runner --M=$M --B=128 --ingestion --J="SJ" --r_index="CLazy" --s_index="CLazy" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
#     # SJ, CComp, CComp
#     ./exp_runner --M=$M --B=128 --ingestion --J="SJ" --r_index="CComp" --s_index="CComp" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
#     # SJ, Regular, CLazy
#     # ./exp_runner --M=$M --B=128 --ingestion --J="SJ" --r_index="Regular" --s_index="CLazy" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
#     # HJ, Regular, Regular
#     # ./exp_runner --M=$M --B=128 --ingestion --J="HJ" --r_index="Regular" --s_index="Regular" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
# done

output="test_7_cache_size.txt"

# clear output file
rm -f $output

nums=(0 32 64)

# for num in "${nums[@]}"; do
# Table 4
# INLJ, Regular, Primary
# ./exp_runner --M=16 --B=128 --ingestion --J="INLJ" --r_index="Regular" --s_index="Primary" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --cache_size=$num
# SJ, Regular, Primary
# ./exp_runner --M=$M --B=128 --ingestion --J="SJ" --r_index="Regular" --s_index="Primary" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
# SJ, Comp, Primary
# ./exp_runner --M=$M --B=128 --ingestion --J="SJ" --r_index="Comp" --s_index="Primary" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
# # SJ, CComp, Primary
# ./exp_runner --M=$M --B=128 --ingestion --J="SJ" --r_index="CComp" --s_index="Primary" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
# HJ, Regular, Primary
# ./exp_runner --M=$M --B=128 --ingestion --J="HJ" --r_index="Regular" --s_index="Primary" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path

# Table 5
# INLJ, Regular, Lazy
# echo "./exp_runner --M=16 --B=128 --ingestion --J="INLJ" --r_index="Regular" --s_index="Lazy" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --cache_size=$num"
# ./exp_runner --M=16 --B=128 --ingestion --J="INLJ" --r_index="Regular" --s_index="Primary" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --cache_size=$num
# # INLJ, Regular, CLazy
# ./exp_runner --M=$M --B=128 --ingestion --J="INLJ" --r_index="Regular" --s_index="CLazy" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
# # SJ, Regular, Regular
# ./exp_runner --M=16 --B=128 --ingestion --J="SJ" --r_index="Regular" --s_index="Regular" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
# SJ, Regular, Lazy
# ./exp_runner --M=16 --B=128 --ingestion --J="SJ" --r_index="Lazy" --s_index="Lazy" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --cache_size=$num
# SJ, Lazy, Lazy
# ./exp_runner --M=16 --B=128 --ingestion --J="SJ" --r_index="CLazy" --s_index="CLazy" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --cache_size=$num
# SJ, CComp, CComp
# ./exp_runner --M=16 --B=128 --ingestion --J="SJ" --r_index="CComp" --s_index="CComp" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --cache_size=$num
# SJ, Regular, CLazy
# ./exp_runner --M=$M --B=128 --ingestion --J="SJ" --r_index="Regular" --s_index="CLazy" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
# HJ, Regular, Regular
# ./exp_runner --M=$M --B=128 --ingestion --J="HJ" --r_index="Regular" --s_index="Regular" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
# done

# output="test_7_cache_size_t.txt"

# # clear output file
# rm -f $output

# nums=(1 4 6)

# for num in "${nums[@]}"; do
#     # Table 4
#     # INLJ, Regular, Primary
#     ./exp_runner --M=16 --B=128 --ingestion --J="INLJ" --r_index="Regular" --s_index="Primary" --num_loop=1 --output_file=$output --uniform  --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --cache_size=16 --k=$num
#     # SJ, Regular, Primary
#     # ./exp_runner --M=$M --B=128 --ingestion --J="SJ" --r_index="Regular" --s_index="Primary" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
#     # SJ, Comp, Primary
#     # ./exp_runner --M=$M --B=128 --ingestion --J="SJ" --r_index="Comp" --s_index="Primary" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
#     # # SJ, CComp, Primary
#     # ./exp_runner --M=$M --B=128 --ingestion --J="SJ" --r_index="CComp" --s_index="Primary" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
#     # HJ, Regular, Primary
#     # ./exp_runner --M=$M --B=128 --ingestion --J="HJ" --r_index="Regular" --s_index="Primary" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path

#     # Table 5
#     # INLJ, Regular, Lazy
#     ./exp_runner --M=16 --B=128 --ingestion --J="INLJ" --r_index="Regular" --s_index="Lazy" --num_loop=1 --output_file=$output --uniform --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --cache_size=16 --k=$num
#     # # INLJ, Regular, CLazy
#     # ./exp_runner --M=$M --B=128 --ingestion --J="INLJ" --r_index="Regular" --s_index="CLazy" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
#     # # SJ, Regular, Regular
#     # ./exp_runner --M=$M --B=128 --ingestion --J="SJ" --r_index="Regular" --s_index="Regular" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
#     # SJ, Regular, Lazy
#     # ./exp_runner --M=16 --B=128 --ingestion --J="SJ" --r_index="Lazy" --s_index="Lazy" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --cache_size=$num
#     # SJ, Lazy, Lazy
#     # ./exp_runner --M=16 --B=128 --ingestion --J="SJ" --r_index="CLazy" --s_index="CLazy" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --cache_size=$num
#     # SJ, CComp, CComp
#     ./exp_runner --M=16 --B=128 --ingestion --J="SJ" --r_index="CComp" --s_index="CComp" --num_loop=1 --output_file=$output --uniform --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --cache_size=16 --k=$num
#     # SJ, Regular, CLazy
#     # ./exp_runner --M=$M --B=128 --ingestion --J="SJ" --r_index="Regular" --s_index="CLazy" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
#     # HJ, Regular, Regular
#     # ./exp_runner --M=$M --B=128 --ingestion --J="HJ" --r_index="Regular" --s_index="Regular" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path
# done

output="test_7_bpk.txt"

# clear output file
rm -f $output

nums=(2 10 20)

for num in "${nums[@]}"; do
    # Table 4
    # INLJ, Regular, Primary
    # ./exp_runner --M=$M --B=128 --ingestion --J="INLJ" --r_index="Regular" --s_index="Primary" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --theory --s_index_path=$index_s_path
    # SJ, Regular, Primary
    # ./exp_runner --M=$M --B=128 --ingestion --J="SJ" --r_index="Regular" --s_index="Primary" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --theory --s_index_path=$index_s_path
    # SJ, Comp, Primary
    # ./exp_runner --M=$M --B=128 --ingestion --J="SJ" --r_index="Comp" --s_index="Primary" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --theory
    # # SJ, CComp, Primary
    # ./exp_runner --M=$M --B=128 --ingestion --J="SJ" --r_index="CComp" --s_index="Primary" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --theory
    # HJ, Regular, Primary
    # ./exp_runner --M=$M --B=128 --ingestion --J="HJ" --r_index="Regular" --s_index="Primary" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --theory --s_index_path=$index_s_path

    # Table 5
    # INLJ, Regular, Lazy
    # ./exp_runner --M=$M --B=128 --ingestion --J="INLJ" --r_index="Regular" --s_index="Lazy" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --theory
    # # INLJ, Regular, CLazy
    # ./exp_runner --M=$M --B=128 --ingestion --J="INLJ" --r_index="Regular" --s_index="CLazy" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --theory --s_index_path=$index_s_path
    # # SJ, Regular, Regular
    # ./exp_runner --M=$M --B=128 --ingestion --J="SJ" --r_index="Regular" --s_index="Regular" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --theory --s_index_path=$index_s_path
    # SJ, Regular, Lazy
    # ./exp_runner --M=16 --B=128 --ingestion --J="INLJ" --r_index="Regular" --s_index="CEager" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --bpk=$num
    # SJ, Lazy, Lazy
    # ./exp_runner --M=16 --B=128 --ingestion --J="INLJ" --r_index="Regular" --s_index="CEager" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --bpk=$num --cache_size=0 --r_tuples=1000000 --s_tuples=1000000
    # SJ, CComp, CComp
    ./exp_runner --M=16 --B=128 --ingestion --J="INLJ" --r_index="Regular" --s_index="CComp" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --bpk=$num --cache_size=0 --r_tuples=5000000 --s_tuples=5000000
    # SJ, Regular, CLazy
    # ./exp_runner --M=$M --B=128 --ingestion --J="SJ" --r_index="Regular" --s_index="CLazy" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --s_index_path=$index_s_path --theory
    # HJ, Regular, Regular
    # ./exp_runner --M=$M --B=128 --ingestion --J="HJ" --r_index="Regular" --s_index="Regular" --num_loop=1 --output_file=$output --uniform --public_data --public_r=$public_r --public_s=$public_s --db_r=$db_r_path --db_s=$db_s_path --r_index_path=$index_r_path --theory --s_index_path=$index_s_path
done
