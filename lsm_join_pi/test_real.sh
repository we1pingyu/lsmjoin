#!/bin/bash
make exp_runner

db_r_path="/tmp/db_r_ywp"
db_s_path="/tmp/db_s_ywp"
data_path="/home/weiping/code/learned-joins/data/"
datasets=("fb_200M_uint64" "osm_cellids_800M_uint64" "wiki_ts_200M_uint64")
num_loops=(1 2 8)

for dataset in "${datasets[@]}"; do
    public_r="${data_path}${dataset}"
    public_s="${data_path}${dataset}"
    output="${dataset}.txt"
    rm -f $output
    num_loop = 1
    for num_loop in "${num_loops[@]}"; do
        if [ "$num_loop" -eq 1 ]; then
            ingestion_flag=""
        else
            ingestion_flag="--ingestion"
        fi
        #index nested loop join
        echo "./exp_runner --M=64 --B=128 --ingestion --J="INTJ" --r_index="Regular" --s_index="Primary" --public_data --public_r=$public_r --public_s=$public_s --num_loop=$num_loop --output_file=$output --db_r=$db_r_path --db_s=$db_s_path"
        ./exp_runner --M=64 --B=128 --ingestion --J="INTJ" --r_index="Regular" --s_index="Primary" --public_data --public_r=$public_r --public_s=$public_s --num_loop=$num_loop --output_file=$output --db_r=$db_r_path --db_s=$db_s_path
        #sort mege join
        echo "./exp_runner --M=64 --B=128 --J="SJ" --r_index="Regular" --s_index="Primary" --public_data --public_r=$public_r --public_s=$public_s --num_loop=$num_loop --output_file=$output --db_r=$db_r_path --db_s=$db_s_path"
        ./exp_runner --M=64 --B=128 $ingestion_flag --J="SJ" --r_index="Regular" --s_index="Primary" --public_data --public_r=$public_r --public_s=$public_s --num_loop=$num_loop --output_file=$output --db_r=$db_r_path --db_s=$db_s_path
        ./exp_runner --M=64 --B=128 $ingestion_flag --J="SJ" --r_index="Eager" --s_index="Primary" --public_data --public_r=$public_r --public_s=$public_s --num_loop=$num_loop --output_file=$output --db_r=$db_r_path --db_s=$db_s_path
        ./exp_runner --M=64 --B=128 $ingestion_flag --J="SJ" --r_index="Lazy" --s_index="Primary" --public_data --public_r=$public_r --public_s=$public_s --num_loop=$num_loop --output_file=$output --db_r=$db_r_path --db_s=$db_s_path
        ./exp_runner --M=64 --B=128 $ingestion_flag --J="SJ" --r_index="Comp" --s_index="Primary" --public_data --public_r=$public_r --public_s=$public_s --num_loop=$num_loop --output_file=$output --db_r=$db_r_path --db_s=$db_s_path
        #covering sort mege join
        echo "./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="CEager" --s_index="Primary" --public_data --public_r=$public_r --public_s=$public_s --num_loop=$num_loop --output_file=$output --db_r=$db_r_path --db_s=$db_s_path"
        ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="CEager" --s_index="Primary" --public_data --public_r=$public_r --public_s=$public_s --num_loop=$num_loop --output_file=$output --db_r=$db_r_path --db_s=$db_s_path
        ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="CLazy" --s_index="Primary" --public_data --public_r=$public_r --public_s=$public_s --num_loop=$num_loop --output_file=$output --db_r=$db_r_path --db_s=$db_s_path
        ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="CComp" --s_index="Primary" --public_data --public_r=$public_r --public_s=$public_s --num_loop=$num_loop --output_file=$output --db_r=$db_r_path --db_s=$db_s_path
        # hash join
        echo "./exp_runner --M=64 --B=128 --J="HJ" --r_index="Regular" --s_index="Primary" --public_data --public_r=$public_r --public_s=$public_s --num_loop=$num_loop --output_file=$output --db_r=$db_r_path --db_s=$db_s_path"
        ./exp_runner --M=64 --B=128 $ingestion_flag --J="HJ" --r_index="Regular" --s_index="Primary" --public_data --public_r=$public_r --public_s=$public_s --num_loop=$num_loop --output_file=$output --db_r=$db_r_path --db_s=$db_s_path
    done
done
