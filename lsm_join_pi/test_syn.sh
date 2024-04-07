#!/bin/bash
make exp_runner

db_r_path="/tmp/db_r_ywp"
db_s_path="/tmp/db_s_ywp"
data_path="/home/weiping/code/lsm_join_data/"
datasets=("even" "skew")
num_loops=(1)

for dataset in "${datasets[@]}"; do
    output="${dataset}.txt"
    rm -f $output
    if [ "$dataset" == "even" ]; then
        epsilon=0.1
        k=1
    else
        epsilon=0.9
        k=9
    fi
    for num_loop in "${num_loops[@]}"; do
        if [ "$num_loop" -eq 1 ]; then
            ingestion_flag=""
        else
            ingestion_flag="--ingestion"
        fi
        #index nested loop join
        echo "./exp_runner --M=64 --B=128 --ingestion --J="INLJ" --r_index="Regular" --s_index="Primary"  --num_loop=$num_loop --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --epsilon=$epsilon --k=$k"
        ./exp_runner --M=64 --B=128 --ingestion --J="INLJ" --r_index="Regular" --s_index="Primary" --num_loop=$num_loop --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --epsilon=$epsilon --k=$k
        #sort mege join
        echo "./exp_runner --M=64 --B=128 $ingestion_flag --J="SJ" --r_index="Regular" --s_index="Primary"  --num_loop=$num_loop --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --epsilon=$epsilon --k=$k"
        ./exp_runner --M=64 --B=128 $ingestion_flag --J="SJ" --r_index="Regular" --s_index="Primary" --num_loop=$num_loop --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --epsilon=$epsilon --k=$k
        ./exp_runner --M=64 --B=128 $ingestion_flag --J="SJ" --r_index="Eager" --s_index="Primary" --num_loop=$num_loop --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --epsilon=$epsilon --k=$k
        ./exp_runner --M=64 --B=128 $ingestion_flag --J="SJ" --r_index="Lazy" --s_index="Primary" --num_loop=$num_loop --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --epsilon=$epsilon --k=$k
        ./exp_runner --M=64 --B=128 $ingestion_flag --J="SJ" --r_index="Comp" --s_index="Primary" --num_loop=$num_loop --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --epsilon=$epsilon --k=$k
        #covering sort mege join
        echo "./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="CEager" --s_index="Primary"  --num_loop=$num_loop --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --epsilon=$epsilon --k=$k"
        ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="CEager" --s_index="Primary" --num_loop=$num_loop --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --epsilon=$epsilon --k=$k
        ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="CLazy" --s_index="Primary" --num_loop=$num_loop --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --epsilon=$epsilon --k=$k
        ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="CComp" --s_index="Primary" --num_loop=$num_loop --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --epsilon=$epsilon --k=$k
        # hash join
        echo "./exp_runner --M=64 --B=128 --J="HJ" --r_index="Regular" --s_index="Primary"  --num_loop=$num_loop --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --epsilon=$epsilon --k=$k"
        ./exp_runner --M=64 --B=128 $ingestion_flag --J="HJ" --r_index="Regular" --s_index="Primary" --num_loop=$num_loop --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --epsilon=$epsilon --k=$k
    done
done
