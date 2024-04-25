#!/bin/bash
make exp_runner

db_r_path="/tmp/db_r_ywp_5"
db_s_path="/tmp/db_s_ywp_5"
data_path="/home/weiping/code/lsm_join_data/"
datasets=("user_id" "movie_id" "fb_200M_uint64" "osm_cellids_800M_uint64" "even" "skew")
# datasets=("user_id")
num_loops=(1)

for dataset in "${datasets[@]}"; do
    epsilon=0.1
    k=1
    skew_flag=""
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
        epsilon=0.9
        k=4
        skew_flag="--skew"
    fi
    output="${dataset}_5nlj.txt"
    rm -f $output
    for num_loop in "${num_loops[@]}"; do
        #index nested loop join
        echo "./exp_runner --J="INLJ" --r_index="Regular" --s_index="Eager" $public_data_flag --public_r=$public_r --public_s=$public_s --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --eps_s=$epsilon --k_s=$k --k_r=$k $skew_flag"
        ./exp_runner --J="INLJ" --r_index="Regular" --s_index="Eager" $public_data_flag --public_r=$public_r --public_s=$public_s --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --eps_s=$epsilon --k_s=$k --k_r=$k $skew_flag
        ./exp_runner --J="INLJ" --r_index="Regular" --s_index="Lazy" $public_data_flag --public_r=$public_r --public_s=$public_s --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --eps_s=$epsilon --k_s=$k --k_r=$k $skew_flag
        echo "./exp_runner --J="INLJ" --r_index="Regular" --s_index="Comp" $public_data_flag --public_r=$public_r --public_s=$public_s --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --eps_s=$epsilon --k_s=$k --k_r=$k $skew_flag"
        ./exp_runner --J="INLJ" --r_index="Regular" --s_index="Comp" $public_data_flag --public_r=$public_r --public_s=$public_s --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --eps_s=$epsilon --k_s=$k --k_r=$k $skew_flag
        ./exp_runner --J="INLJ" --r_index="Regular" --s_index="CEager" $public_data_flag --public_r=$public_r --public_s=$public_s --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --eps_s=$epsilon --k_s=$k --k_r=$k $skew_flag
        ./exp_runner --J="INLJ" --r_index="Regular" --s_index="CLazy" $public_data_flag --public_r=$public_r --public_s=$public_s --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --eps_s=$epsilon --k_s=$k --k_r=$k $skew_flag
        ./exp_runner --J="INLJ" --r_index="Regular" --s_index="CComp" $public_data_flag --public_r=$public_r --public_s=$public_s --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --eps_s=$epsilon --k_s=$k --k_r=$k $skew_flag
        # hash join
        echo "./exp_runner --J="HJ" --r_index="Regular" --s_index="Regular" $public_data_flag --public_r=$public_r --public_s=$public_s --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --eps_s=$epsilon --k_s=$k --k_r=$k $skew_flag"
        ./exp_runner --J="HJ" --r_index="Regular" --s_index="Regular" $public_data_flag --public_r=$public_r --public_s=$public_s --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --eps_s=$epsilon --k_s=$k --k_r=$k $skew_flag
    done
done
