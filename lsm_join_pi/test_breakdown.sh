#!/bin/bash

make exp_runner
db_r_path="/tmp/db_r_breakdown"
db_s_path="/tmp/db_s_breakdown"
index_r_path="/tmp/index_r_breakdown"
index_s_path="/tmp/index_s_breakdown"

Million=1000000
# Dataset Size
s_tuples=$((2 * Million))
r_tuples=$((2 * Million))

output="test_breakdown.txt"
rm -f $output

# Table 4

# ./exp_runner --J="INLJ" --r_index="Regular" --s_index="Primary" --r_index_path=$index_r_path --s_index_path=$index_s_path --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --s_tuples=$s_tuples --r_tuples=$r_tuples
# ./exp_runner --J="SJ" --r_index="Regular" --s_index="Primary" --r_index_path=$index_r_path --s_index_path=$index_s_path --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --s_tuples=$s_tuples --r_tuples=$r_tuples
# ./exp_runner --J="SJ" --r_index="Eager" --s_index="Primary" --r_index_path=$index_r_path --s_index_path=$index_s_path --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --s_tuples=$s_tuples --r_tuples=$r_tuples
# ./exp_runner --J="SJ" --r_index="Lazy" --s_index="Primary" --r_index_path=$index_r_path --s_index_path=$index_s_path --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --s_tuples=$s_tuples --r_tuples=$r_tuples
# ./exp_runner --J="SJ" --r_index="Comp" --s_index="Primary" --r_index_path=$index_r_path --s_index_path=$index_s_path --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --s_tuples=$s_tuples --r_tuples=$r_tuples
# ./exp_runner --J="SJ" --r_index="CEager" --s_index="Primary" --r_index_path=$index_r_path --s_index_path=$index_s_path --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --s_tuples=$s_tuples --r_tuples=$r_tuples
# ./exp_runner --J="SJ" --r_index="CLazy" --s_index="Primary" --r_index_path=$index_r_path --s_index_path=$index_s_path --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --s_tuples=$s_tuples --r_tuples=$r_tuples
# ./exp_runner --J="SJ" --r_index="CComp" --s_index="Primary" --r_index_path=$index_r_path --s_index_path=$index_s_path --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --s_tuples=$s_tuples --r_tuples=$r_tuples
# ./exp_runner --J="HJ" --r_index="Regular" --s_index="Primary" --r_index_path=$index_r_path --s_index_path=$index_s_path --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --s_tuples=$s_tuples --r_tuples=$r_tuples

# # Table 5
# ./exp_runner --J="INLJ" --r_index="Regular" --s_index="Eager" --r_index_path=$index_r_path --s_index_path=$index_s_path --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --s_tuples=$s_tuples --r_tuples=$r_tuples
./exp_runner --J="INLJ" --r_index="Regular" --s_index="Lazy" --r_index_path=$index_r_path --s_index_path=$index_s_path --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --s_tuples=$s_tuples --r_tuples=$r_tuples
./exp_runner --J="INLJ" --r_index="Regular" --s_index="Comp" --r_index_path=$index_r_path --s_index_path=$index_s_path --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --s_tuples=$s_tuples --r_tuples=$r_tuples
# ./exp_runner --J="INLJ" --r_index="Regular" --s_index="CEager" --r_index_path=$index_r_path --s_index_path=$index_s_path --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --s_tuples=$s_tuples --r_tuples=$r_tuples
# ./exp_runner --J="INLJ" --r_index="Regular" --s_index="CLazy" --r_index_path=$index_r_path --s_index_path=$index_s_path --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --s_tuples=$s_tuples --r_tuples=$r_tuples
# ./exp_runner --J="INLJ" --r_index="Regular" --s_index="CComp" --r_index_path=$index_r_path --s_index_path=$index_s_path --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --s_tuples=$s_tuples --r_tuples=$r_tuples

# ./exp_runner --J="SJ" --r_index="Regular" --s_index="Regular" --r_index_path=$index_r_path --s_index_path=$index_s_path --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --s_tuples=$s_tuples --r_tuples=$r_tuples
# ./exp_runner --J="SJ" --r_index="Regular" --s_index="Eager" --r_index_path=$index_r_path --s_index_path=$index_s_path --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --s_tuples=$s_tuples --r_tuples=$r_tuples
# ./exp_runner --J="SJ" --r_index="Regular" --s_index="Lazy" --r_index_path=$index_r_path --s_index_path=$index_s_path --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --s_tuples=$s_tuples --r_tuples=$r_tuples
# ./exp_runner --J="SJ" --r_index="Regular" --s_index="Comp" --r_index_path=$index_r_path --s_index_path=$index_s_path --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --s_tuples=$s_tuples --r_tuples=$r_tuples
# ./exp_runner --J="SJ" --r_index="Eager" --s_index="Eager" --r_index_path=$index_r_path --s_index_path=$index_s_path --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --s_tuples=$s_tuples --r_tuples=$r_tuples
# ./exp_runner --J="SJ" --r_index="Lazy" --s_index="Lazy" --r_index_path=$index_r_path --s_index_path=$index_s_path --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --s_tuples=$s_tuples --r_tuples=$r_tuples
# ./exp_runner --J="SJ" --r_index="Comp" --s_index="Comp" --r_index_path=$index_r_path --s_index_path=$index_s_path --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --s_tuples=$s_tuples --r_tuples=$r_tuples
# ./exp_runner --J="SJ" --r_index="Regular" --s_index="CEager" --r_index_path=$index_r_path --s_index_path=$index_s_path --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --s_tuples=$s_tuples --r_tuples=$r_tuples
# ./exp_runner --J="SJ" --r_index="Regular" --s_index="CLazy" --r_index_path=$index_r_path --s_index_path=$index_s_path --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --s_tuples=$s_tuples --r_tuples=$r_tuples
# ./exp_runner --J="SJ" --r_index="Regular" --s_index="CComp" --r_index_path=$index_r_path --s_index_path=$index_s_path --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --s_tuples=$s_tuples --r_tuples=$r_tuples
# ./exp_runner --J="SJ" --r_index="CEager" --s_index="CEager" --r_index_path=$index_r_path --s_index_path=$index_s_path --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --s_tuples=$s_tuples --r_tuples=$r_tuples
# ./exp_runner --J="SJ" --r_index="CLazy" --s_index="CLazy" --r_index_path=$index_r_path --s_index_path=$index_s_path --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --s_tuples=$s_tuples --r_tuples=$r_tuples
# ./exp_runner --J="SJ" --r_index="CComp" --s_index="CComp" --r_index_path=$index_r_path --s_index_path=$index_s_path --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --s_tuples=$s_tuples --r_tuples=$r_tuples
# ./exp_runner --J="HJ" --r_index="Regular" --s_index="Regular" --r_index_path=$index_r_path --s_index_path=$index_s_path --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --s_tuples=$s_tuples --r_tuples=$r_tuples
