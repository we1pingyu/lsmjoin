#!/bin/bash
make exp_runner

db_r_path="/tmp/db_r_overall"
db_s_path="/tmp/db_s_overall"
data_path="/home/weiping/code/lsm_join_data/"
datasets=("user_id" "movie_id" "fb_200M_uint64" "osm_cellids_800M_uint64" "unif" "skew")
# datasets=("user_id")
num_loops=(1)

for dataset in "${datasets[@]}"; do
    epsilon=0.1
    k=1
    uniform_flag="--uniform"
    if [ "$dataset" == "movie_id" ]; then
        public_data_flag="--public_data"
        public_r="${data_path}movie_info_movie_id"
        public_s="${data_path}cast_info_movie_id"
    elif [ "$dataset" == "user_id" ]; then
        public_data_flag="--public_data"
        public_r="${data_path}question_user_id"
        public_s="${data_path}so_user_user_id"
    elif [ "$dataset" == "fb_200M_uint64" ] || [ "$dataset" == "osm_cellids_800M_uint64" ]; then
        public_data_flag="--public_data"
        public_r="${data_path}${dataset}"
        public_s="${data_path}${dataset}"
    elif [ "$dataset" == "even" ]; then
        public_data_flag=""
        epsilon=0.1
        k=1
    else
        public_data_flag=""
        uniform_flag=""
        epsilon=0.4
        k=4.5
    fi
    output="${dataset}.txt"
    rm -f $output
    for num_loop in "${num_loops[@]}"; do
        if [ "$num_loop" -eq 1 ]; then
            ingestion_flag=""
        else
            ingestion_flag="--ingestion"
        fi
        #index nested loop join
        echo "./exp_runner --M=64 --B=128 --ingestion --J="INLJ" --r_index="Regular" --s_index="Primary" --public_data --public_r=$public_r --public_s=$public_s --num_loop=$num_loop --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --epsilon=$epsilon --k=$k"
        ./exp_runner --M=64 --B=128 --ingestion --J="INLJ" --r_index="Regular" --s_index="Primary" --public_data --public_r=$public_r --public_s=$public_s --num_loop=$num_loop --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --epsilon=$epsilon --k=$k $uniform_flag
        #sort mege join
        echo "./exp_runner --M=64 --B=128 --J="SJ" --r_index="Regular" --s_index="Primary" --public_data --public_r=$public_r --public_s=$public_s --num_loop=$num_loop --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --epsilon=$epsilon --k=$k $uniform_flag"
        ./exp_runner --M=64 --B=128 $ingestion_flag --J="SJ" --r_index="Regular" --s_index="Primary" --public_data --public_r=$public_r --public_s=$public_s --num_loop=$num_loop --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --epsilon=$epsilon --k=$k $uniform_flag
        ./exp_runner --M=64 --B=128 $ingestion_flag --J="SJ" --r_index="Eager" --s_index="Primary" --public_data --public_r=$public_r --public_s=$public_s --num_loop=$num_loop --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --epsilon=$epsilon --k=$k $uniform_flag
        ./exp_runner --M=64 --B=128 $ingestion_flag --J="SJ" --r_index="Lazy" --s_index="Primary" --public_data --public_r=$public_r --public_s=$public_s --num_loop=$num_loop --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --epsilon=$epsilon --k=$k $uniform_flag
        ./exp_runner --M=64 --B=128 $ingestion_flag --J="SJ" --r_index="Comp" --s_index="Primary" --public_data --public_r=$public_r --public_s=$public_s --num_loop=$num_loop --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --epsilon=$epsilon --k=$k $uniform_flag
        #covering sort mege join
        echo "./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="CEager" --s_index="Primary" --public_data --public_r=$public_r --public_s=$public_s --num_loop=$num_loop --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --epsilon=$epsilon --k=$k $uniform_flag"
        ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="CEager" --s_index="Primary" --public_data --public_r=$public_r --public_s=$public_s --num_loop=$num_loop --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --epsilon=$epsilon --k=$k $uniform_flag
        ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="CLazy" --s_index="Primary" --public_data --public_r=$public_r --public_s=$public_s --num_loop=$num_loop --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --epsilon=$epsilon --k=$k $uniform_flag
        ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="CComp" --s_index="Primary" --public_data --public_r=$public_r --public_s=$public_s --num_loop=$num_loop --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --epsilon=$epsilon --k=$k $uniform_flag
        # hash join
        echo "./exp_runner --M=64 --B=128 --J="HJ" --r_index="Regular" --s_index="Primary" --public_data --public_r=$public_r --public_s=$public_s --num_loop=$num_loop --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --epsilon=$epsilon --k=$k $uniform_flag"
        ./exp_runner --M=64 --B=128 $ingestion_flag --J="HJ" --r_index="Regular" --s_index="Primary" --public_data --public_r=$public_r --public_s=$public_s --num_loop=$num_loop --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --epsilon=$epsilon --k=$k $uniform_flag
    done
done

for dataset in "${datasets[@]}"; do
    epsilon=0.1
    k=1
    if [ "$dataset" == "movie_id" ]; then
        public_data_flag="--public_data"
        public_r="${data_path}movie_info_movie_id"
        public_s="${data_path}cast_info_movie_id"
    elif [ "$dataset" == "user_id" ]; then
        public_data_flag="--public_data"
        public_r="${data_path}question_user_id"
        public_s="${data_path}so_user_user_id"
    elif [ "$dataset" == "fb_200M_uint64" ] || [ "$dataset" == "osm_cellids_800M_uint64" ]; then
        public_data_flag="--public_data"
        public_r="${data_path}${dataset}"
        public_s="${data_path}${dataset}"
    elif [ "$dataset" == "even" ]; then
        public_data_flag=""
        uniform_flag="--uniform"
        epsilon=0.1
        k=1
    else
        public_data_flag=""
        uniform_flag=""
        epsilon=0.4
        k=4.5
    fi
    output="${dataset}_5nlj.txt"
    rm -f $output
    for num_loop in "${num_loops[@]}"; do
        #index nested loop join
        echo "./exp_runner --M=64 --B=128 --J="INLJ" --r_index="Regular" --s_index="Eager" $public_data_flag --public_r=$public_r --public_s=$public_s --num_loop=$num_loop --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --ingestion --epsilon=$epsilon --k=$k $uniform_flag"
        ./exp_runner --M=64 --B=128 --J="INLJ" --r_index="Regular" --s_index="Eager" $public_data_flag --public_r=$public_r --public_s=$public_s --num_loop=$num_loop --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --ingestion --epsilon=$epsilon --k=$k $uniform_flag
        ./exp_runner --M=64 --B=128 --J="INLJ" --r_index="Regular" --s_index="Lazy" $public_data_flag --public_r=$public_r --public_s=$public_s --num_loop=$num_loop --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --ingestion --epsilon=$epsilon --k=$k $uniform_flag
        echo "./exp_runner --M=64 --B=128 --J="INLJ" --r_index="Regular" --s_index="Comp" $public_data_flag --public_r=$public_r --public_s=$public_s --num_loop=$num_loop --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --ingestion --epsilon=$epsilon --k=$k  $uniform_flag"
        ./exp_runner --M=64 --B=128 --J="INLJ" --r_index="Regular" --s_index="Comp" $public_data_flag --public_r=$public_r --public_s=$public_s --num_loop=$num_loop --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --ingestion --epsilon=$epsilon --k=$k $uniform_flag
        ./exp_runner --M=64 --B=128 --J="INLJ" --r_index="Regular" --s_index="CEager" $public_data_flag --public_r=$public_r --public_s=$public_s --num_loop=$num_loop --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --ingestion --epsilon=$epsilon --k=$k $uniform_flag
        ./exp_runner --M=64 --B=128 --J="INLJ" --r_index="Regular" --s_index="CLazy" $public_data_flag --public_r=$public_r --public_s=$public_s --num_loop=$num_loop --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --ingestion --epsilon=$epsilon --k=$k $uniform_flag
        ./exp_runner --M=64 --B=128 --J="INLJ" --r_index="Regular" --s_index="CComp" $public_data_flag --public_r=$public_r --public_s=$public_s --num_loop=$num_loop --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --ingestion --epsilon=$epsilon --k=$k $uniform_flag
        # hash join
        echo "./exp_runner --M=64 --B=128 --J="HJ" --r_index="Regular" --s_index="Regular" $public_data_flag --public_r=$public_r --public_s=$public_s --num_loop=$num_loop --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --ingestion --epsilon=$epsilon --k=$k $uniform_flag"
        ./exp_runner --M=64 --B=128 --J="HJ" --r_index="Regular" --s_index="Regular" $public_data_flag --public_r=$public_r --public_s=$public_s --num_loop=$num_loop --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --ingestion --epsilon=$epsilon --k=$k $uniform_flag
    done
done

for dataset in "${datasets[@]}"; do
    epsilon=0.1
    k=1
    if [ "$dataset" == "movie_id" ]; then
        public_data_flag="--public_data"
        public_r="${data_path}movie_info_movie_id"
        public_s="${data_path}cast_info_movie_id"
    elif [ "$dataset" == "user_id" ]; then
        public_data_flag="--public_data"
        public_r="${data_path}question_user_id"
        public_s="${data_path}so_user_user_id"
    elif [ "$dataset" == "fb_200M_uint64" ] || [ "$dataset" == "osm_cellids_800M_uint64" ]; then
        public_data_flag="--public_data"
        public_r="${data_path}${dataset}"
        public_s="${data_path}${dataset}"
    elif [ "$dataset" == "even" ]; then
        public_data_flag=""
        uniform_flag="--uniform"
        epsilon=0.1
        k=1
    else
        public_data_flag=""
        uniform_flag=""
        epsilon=0.4
        k=4.5
    fi
    output="${dataset}_5sj.txt"
    rm -f $output
    for num_loop in "${num_loops[@]}"; do
        #sort mege join
        ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Regular" --s_index="Regular" $public_data_flag --public_r=$public_r --public_s=$public_s --num_loop=$num_loop --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --epsilon=$epsilon --k=$k $uniform_flag
        echo "./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Regular" --s_index="Eager" $public_data_flag --public_r=$public_r --public_s=$public_s --num_loop=$num_loop --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --epsilon=$epsilon --k=$k  $uniform_flag"
        ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Regular" --s_index="Eager" $public_data_flag --public_r=$public_r --public_s=$public_s --num_loop=$num_loop --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --epsilon=$epsilon --k=$k $uniform_flag
        echo "./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Regular" --s_index="Lazy" $public_data_flag --public_r=$public_r --public_s=$public_s --num_loop=$num_loop --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --epsilon=$epsilon --k=$k $uniform_flag"
        ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Regular" --s_index="Lazy" $public_data_flag --public_r=$public_r --public_s=$public_s --num_loop=$num_loop --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --epsilon=$epsilon --k=$k $uniform_flag
        echo "./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Regular" --s_index="Comp" $public_data_flag --public_r=$public_r --public_s=$public_s --num_loop=$num_loop --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --epsilon=$epsilon --k=$k $uniform_flag"
        ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Regular" --s_index="Comp" $public_data_flag --public_r=$public_r --public_s=$public_s --num_loop=$num_loop --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --epsilon=$epsilon --k=$k $uniform_flag
        echo "./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Eager" --s_index="Eager" $public_data_flag --public_r=$public_r --public_s=$public_s --num_loop=$num_loop --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --epsilon=$epsilon --k=$k $uniform_flag"
        ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Eager" --s_index="Eager" $public_data_flag --public_r=$public_r --public_s=$public_s --num_loop=$num_loop --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --epsilon=$epsilon --k=$k $uniform_flag
        echo "./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Lazy" --s_index="Lazy" $public_data_flag --public_r=$public_r --public_s=$public_s --num_loop=$num_loop --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --epsilon=$epsilon --k=$k $uniform_flag"
        ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Lazy" --s_index="Lazy" $public_data_flag --public_r=$public_r --public_s=$public_s --num_loop=$num_loop --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --epsilon=$epsilon --k=$k $uniform_flag
        echo "./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Comp" --s_index="Comp" $public_data_flag --public_r=$public_r --public_s=$public_s --num_loop=$num_loop --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --epsilon=$epsilon --k=$k $uniform_flag"
        ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Comp" --s_index="Comp" $public_data_flag --public_r=$public_r --public_s=$public_s --num_loop=$num_loop --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --epsilon=$epsilon --k=$k $uniform_flag
        #covering sort mege join
        echo "./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Regular" --s_index="CEager" $public_data_flag --public_r=$public_r --public_s=$public_s --num_loop=$num_loop --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --epsilon=$epsilon --k=$k $uniform_flag"
        ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Regular" --s_index="CEager" $public_data_flag --public_r=$public_r --public_s=$public_s --num_loop=$num_loop --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --epsilon=$epsilon --k=$k $uniform_flag
        echo "./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Regular" --s_index="CLazy" $public_data_flag --public_r=$public_r --public_s=$public_s --num_loop=$num_loop --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --epsilon=$epsilon --k=$k $uniform_flag"
        ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Regular" --s_index="CLazy" $public_data_flag --public_r=$public_r --public_s=$public_s --num_loop=$num_loop --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --epsilon=$epsilon --k=$k $uniform_flag
        echo "./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Regular" --s_index="CComp" $public_data_flag --public_r=$public_r --public_s=$public_s --num_loop=$num_loop --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --epsilon=$epsilon --k=$k $uniform_flag"
        ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="Regular" --s_index="CComp" $public_data_flag --public_r=$public_r --public_s=$public_s --num_loop=$num_loop --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --epsilon=$epsilon --k=$k $uniform_flag
        echo "./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="CEager" --s_index="CEager" $public_data_flag --public_r=$public_r --public_s=$public_s --num_loop=$num_loop --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --epsilon=$epsilon --k=$k $uniform_flag"
        ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="CEager" --s_index="CEager" $public_data_flag --public_r=$public_r --public_s=$public_s --num_loop=$num_loop --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --epsilon=$epsilon --k=$k $uniform_flag
        echo "./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="CLazy" --s_index="CLazy" $public_data_flag --public_r=$public_r --public_s=$public_s --num_loop=$num_loop --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --epsilon=$epsilon --k=$k $uniform_flag"
        ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="CLazy" --s_index="CLazy" $public_data_flag --public_r=$public_r --public_s=$public_s --num_loop=$num_loop --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --epsilon=$epsilon --k=$k $uniform_flag
        echo "./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="CComp" --s_index="CComp" $public_data_flag --public_r=$public_r --public_s=$public_s --num_loop=$num_loop --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --epsilon=$epsilon --k=$k $uniform_flag"
        ./exp_runner --M=64 --B=128 --ingestion --J="SJ" --r_index="CComp" --s_index="CComp" $public_data_flag --public_r=$public_r --public_s=$public_s --num_loop=$num_loop --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --epsilon=$epsilon --k=$k $uniform_flag
    done
done
