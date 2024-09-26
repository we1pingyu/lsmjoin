#!/bin/bash
make exp_runner

root_path="/home/weiping/tmp/"
db_r_path="${root_path}db_r"
db_s_path="${root_path}db_s"
index_r_path="${root_path}index_r"
index_s_path="${root_path}index_s"
clear_path() {
    rm -rf "$db_r_path"/*
    rm -rf "$db_s_path"/*
    rm -rf "$index_r_path"/*
    rm -rf "$index_s_path"/*
}
data_path="/home/weiping/code/lsm_join_data/"
datasets=("user_id" "movie_id" "fb_200M_uint64" "wiki_ts_200M_uint64" "unif" "skew")
# datasets=("skew")

for dataset in "${datasets[@]}"; do
    epsilon=0.2
    k=4
    skew_flag=""
    if [ "$dataset" == "movie_id" ]; then
        public_data_flag="--public_data"
        public_r="${data_path}movie_info_movie_id"
        public_s="${data_path}cast_info_movie_id"
    elif [ "$dataset" == "user_id" ]; then
        public_data_flag="--public_data"
        public_r="${data_path}question_user_id"
        public_s="${data_path}so_user_user_id"
    elif [ "$dataset" == "fb_200M_uint64" ] || [ "$dataset" == "wiki_ts_200M_uint64" ]; then
        public_data_flag="--public_data"
        public_r="${data_path}${dataset}"
        public_s="${data_path}${dataset}"
    elif [ "$dataset" == "unif" ]; then
        public_data_flag=""
        epsilon=0.2
        k=4
    else
        public_data_flag=""
        epsilon=0.2
        k=0.5
        skew_flag="--skew"
    fi
    output="${dataset}.txt"
    rm -f $output
    #index nested loop join
    # echo "./exp_runner --J="INLJ" --r_index="Regular" --s_index="Primary" $public_data_flag --public_r=$public_r --public_s=$public_s  --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --eps_s=$epsilon --k_s=$k --k_r=$k $skew_flag"
    # clear_path
    # ./exp_runner --J="INLJ" --r_index="Regular" --s_index="Primary" $public_data_flag --public_r=$public_r --public_s=$public_s --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --eps_s=$epsilon --k_s=$k --k_r=$k $skew_flag
    # #sort mege join
    # echo "./exp_runner --J="SJ" --r_index="Regular" --s_index="Primary" $public_data_flag --public_r=$public_r --public_s=$public_s  --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --eps_s=$epsilon --k_s=$k --k_r=$k $skew_flag"
    # clear_path
    # ./exp_runner --J="SJ" --r_index="Regular" --s_index="Primary" $public_data_flag --public_r=$public_r --public_s=$public_s --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --eps_s=$epsilon --k_s=$k --k_r=$k $skew_flag
    # clear_path
    # ./exp_runner --J="SJ" --r_index="Eager" --s_index="Primary" $public_data_flag --public_r=$public_r --public_s=$public_s  --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --eps_s=$epsilon --k_s=$k --k_r=$k $skew_flag
    # clear_path
    # ./exp_runner --J="SJ" --r_index="Lazy" --s_index="Primary" $public_data_flag --public_r=$public_r --public_s=$public_s --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --eps_s=$epsilon --k_s=$k --k_r=$k $skew_flag
    # clear_path
    # ./exp_runner --J="SJ" --r_index="Comp" --s_index="Primary" $public_data_flag --public_r=$public_r --public_s=$public_s --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --eps_s=$epsilon --k_s=$k --k_r=$k $skew_flag
    # #covering sort mege join
    # echo "./exp_runner --J="SJ" --r_index="CEager" --s_index="Primary" $public_data_flag --public_r=$public_r --public_s=$public_s --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --eps_s=$epsilon --k_s=$k --k_r=$k $skew_flag"
    # clear_path
    # ./exp_runner --J="SJ" --r_index="CEager" --s_index="Primary" $public_data_flag --public_r=$public_r --public_s=$public_s --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --eps_s=$epsilon --k_s=$k --k_r=$k $skew_flag
    # clear_path
    # ./exp_runner --J="SJ" --r_index="CLazy" --s_index="Primary" $public_data_flag --public_r=$public_r --public_s=$public_s --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --eps_s=$epsilon --k_s=$k --k_r=$k $skew_flag
    # clear_path
    # ./exp_runner --J="SJ" --r_index="CComp" --s_index="Primary" $public_data_flag --public_r=$public_r --public_s=$public_s --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --eps_s=$epsilon --k_s=$k --k_r=$k $skew_flag
    # hash join
    echo "./exp_runner --J="HJ" --r_index="Regular" --s_index="Primary" $public_data_flag --public_r=$public_r --public_s=$public_s --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --eps_s=$epsilon --k_s=$k --k_r=$k $skew_flag"
    clear_path
    ./exp_runner --J="HJ" --r_index="Regular" --s_index="Primary" $public_data_flag --public_r=$public_r --public_s=$public_s --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --eps_s=$epsilon --k_s=$k --k_r=$k $skew_flag
    ./exp_runner --J="RJ" --r_index="Regular" --s_index="Primary" $public_data_flag --public_r=$public_r --public_s=$public_s --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --eps_s=$epsilon --k_s=$k --k_r=$k $skew_flag
done

# for dataset in "${datasets[@]}"; do
#     epsilon=0.2
#     k=4
#     skew_flag=""
#     if [ "$dataset" == "movie_id" ]; then
#         public_data_flag="--public_data"
#         public_r="${data_path}movie_info_movie_id"
#         public_s="${data_path}cast_info_movie_id"
#     elif [ "$dataset" == "user_id" ]; then
#         public_data_flag="--public_data"
#         public_r="${data_path}question_user_id"
#         public_s="${data_path}so_user_user_id"
#     elif [ "$dataset" == "fb_200M_uint64" ] || [ "$dataset" == "wiki_ts_200M_uint64" ]; then
#         public_data_flag="--public_data"
#         public_r="${data_path}${dataset}"
#         public_s="${data_path}${dataset}"
#     elif [ "$dataset" == "unif" ]; then
#         public_data_flag=""
#         epsilon=0.2
#         k=4
#     else
#         public_data_flag=""
#         epsilon=0.2
#         k=0.5
#         skew_flag="--skew"
#     fi
#     output="${dataset}_5nlj.txt"
#     rm -f $output
#     #index nested loop join
#     echo "./exp_runner --J="INLJ" --r_index="Regular" --s_index="Eager" $public_data_flag --public_r=$public_r --public_s=$public_s --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --eps_s=$epsilon --k_s=$k --k_r=$k $skew_flag"
#     clear_path
#     ./exp_runner --J="INLJ" --r_index="Regular" --s_index="Eager" $public_data_flag --public_r=$public_r --public_s=$public_s --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --eps_s=$epsilon --k_s=$k --k_r=$k $skew_flag
#     clear_path
#     ./exp_runner --J="INLJ" --r_index="Regular" --s_index="Lazy" $public_data_flag --public_r=$public_r --public_s=$public_s --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --eps_s=$epsilon --k_s=$k --k_r=$k $skew_flag
#     echo "./exp_runner --J="INLJ" --r_index="Regular" --s_index="Comp" $public_data_flag --public_r=$public_r --public_s=$public_s --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --eps_s=$epsilon --k_s=$k --k_r=$k $skew_flag"
#     clear_path
#     ./exp_runner --J="INLJ" --r_index="Regular" --s_index="Comp" $public_data_flag --public_r=$public_r --public_s=$public_s --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --eps_s=$epsilon --k_s=$k --k_r=$k $skew_flag
#     clear_path
#     ./exp_runner --J="INLJ" --r_index="Regular" --s_index="CEager" $public_data_flag --public_r=$public_r --public_s=$public_s --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --eps_s=$epsilon --k_s=$k --k_r=$k $skew_flag
#     clear_path
#     ./exp_runner --J="INLJ" --r_index="Regular" --s_index="CLazy" $public_data_flag --public_r=$public_r --public_s=$public_s --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --eps_s=$epsilon --k_s=$k --k_r=$k $skew_flag
#     clear_path
#     ./exp_runner --J="INLJ" --r_index="Regular" --s_index="CComp" $public_data_flag --public_r=$public_r --public_s=$public_s --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --eps_s=$epsilon --k_s=$k --k_r=$k $skew_flag
#     # hash join
#     echo "./exp_runner --J="HJ" --r_index="Regular" --s_index="Regular" $public_data_flag --public_r=$public_r --public_s=$public_s --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --eps_s=$epsilon --k_s=$k --k_r=$k $skew_flag"
#     clear_path
#     ./exp_runner --J="HJ" --r_index="Regular" --s_index="Regular" $public_data_flag --public_r=$public_r --public_s=$public_s --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --eps_s=$epsilon --k_s=$k --k_r=$k $skew_flag
# done

# for dataset in "${datasets[@]}"; do
#     epsilon=0.2
#     k=4
#     skew_flag=""
#     if [ "$dataset" == "movie_id" ]; then
#         public_data_flag="--public_data"
#         public_r="${data_path}movie_info_movie_id"
#         public_s="${data_path}cast_info_movie_id"
#     elif [ "$dataset" == "user_id" ]; then
#         public_data_flag="--public_data"
#         public_r="${data_path}question_user_id"
#         public_s="${data_path}so_user_user_id"
#     elif [ "$dataset" == "fb_200M_uint64" ] || [ "$dataset" == "wiki_ts_200M_uint64" ]; then
#         public_data_flag="--public_data"
#         public_r="${data_path}${dataset}"
#         public_s="${data_path}${dataset}"
#     elif [ "$dataset" == "unif" ]; then
#         public_data_flag=""
#         epsilon=0.2
#         k=4
#     else
#         public_data_flag=""
#         epsilon=0.2
#         k=0.5
#         skew_flag="--skew"
#     fi
#     output="${dataset}_5sj.txt"
#     rm -f $output
#     #sort mege join
#     clear_path
#     ./exp_runner --J="SJ" --r_index="Regular" --s_index="Regular" $public_data_flag --public_r=$public_r --public_s=$public_s --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --eps_s=$epsilon --k_s=$k --k_r=$k $skew_flag
#     echo "./exp_runner --J="SJ" --r_index="Regular" --s_index="Eager" $public_data_flag --public_r=$public_r --public_s=$public_s --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --eps_s=$epsilon --k_s=$k --k_r=$k $skew_flag"
#     clear_path
#     ./exp_runner --J="SJ" --r_index="Regular" --s_index="Eager" $public_data_flag --public_r=$public_r --public_s=$public_s --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --eps_s=$epsilon --k_s=$k --k_r=$k $skew_flag
#     echo "./exp_runner --J="SJ" --r_index="Regular" --s_index="Lazy" $public_data_flag --public_r=$public_r --public_s=$public_s --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --eps_s=$epsilon --k_s=$k --k_r=$k $skew_flag"
#     clear_path
#     ./exp_runner --J="SJ" --r_index="Regular" --s_index="Lazy" $public_data_flag --public_r=$public_r --public_s=$public_s --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --eps_s=$epsilon --k_s=$k --k_r=$k $skew_flag
#     echo "./exp_runner --J="SJ" --r_index="Regular" --s_index="Comp" $public_data_flag --public_r=$public_r --public_s=$public_s --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --eps_s=$epsilon --k_s=$k --k_r=$k $skew_flag"
#     clear_path
#     ./exp_runner --J="SJ" --r_index="Regular" --s_index="Comp" $public_data_flag --public_r=$public_r --public_s=$public_s --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --eps_s=$epsilon --k_s=$k --k_r=$k $skew_flag
#     echo "./exp_runner --J="SJ" --r_index="Eager" --s_index="Eager" $public_data_flag --public_r=$public_r --public_s=$public_s --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --eps_s=$epsilon --k_s=$k --k_r=$k $skew_flag"
#     clear_path
#     ./exp_runner --J="SJ" --r_index="Eager" --s_index="Eager" $public_data_flag --public_r=$public_r --public_s=$public_s --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --eps_s=$epsilon --k_s=$k --k_r=$k $skew_flag
#     echo "./exp_runner --J="SJ" --r_index="Lazy" --s_index="Lazy" $public_data_flag --public_r=$public_r --public_s=$public_s --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --eps_s=$epsilon --k_s=$k --k_r=$k $skew_flag"
#     clear_path
#     ./exp_runner --J="SJ" --r_index="Lazy" --s_index="Lazy" $public_data_flag --public_r=$public_r --public_s=$public_s --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --eps_s=$epsilon --k_s=$k --k_r=$k $skew_flag
#     echo "./exp_runner --J="SJ" --r_index="Comp" --s_index="Comp" $public_data_flag --public_r=$public_r --public_s=$public_s --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --eps_s=$epsilon --k_s=$k --k_r=$k $skew_flag"
#     clear_path
#     ./exp_runner --J="SJ" --r_index="Comp" --s_index="Comp" $public_data_flag --public_r=$public_r --public_s=$public_s --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --eps_s=$epsilon --k_s=$k --k_r=$k $skew_flag
#     #covering sort mege join
#     echo "./exp_runner --J="SJ" --r_index="Regular" --s_index="CEager" $public_data_flag --public_r=$public_r --public_s=$public_s --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --eps_s=$epsilon --k_s=$k --k_r=$k $skew_flag"
#     clear_path
#     ./exp_runner --J="SJ" --r_index="Regular" --s_index="CEager" $public_data_flag --public_r=$public_r --public_s=$public_s --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --eps_s=$epsilon --k_s=$k --k_r=$k $skew_flag
#     echo "./exp_runner --J="SJ" --r_index="Regular" --s_index="CLazy" $public_data_flag --public_r=$public_r --public_s=$public_s --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --eps_s=$epsilon --k_s=$k --k_r=$k $skew_flag"
#     clear_path
#     ./exp_runner --J="SJ" --r_index="Regular" --s_index="CLazy" $public_data_flag --public_r=$public_r --public_s=$public_s --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --eps_s=$epsilon --k_s=$k --k_r=$k $skew_flag
#     echo "./exp_runner --J="SJ" --r_index="Regular" --s_index="CComp" $public_data_flag --public_r=$public_r --public_s=$public_s --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --eps_s=$epsilon --k_s=$k --k_r=$k $skew_flag"
#     clear_path
#     ./exp_runner --J="SJ" --r_index="Regular" --s_index="CComp" $public_data_flag --public_r=$public_r --public_s=$public_s --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --eps_s=$epsilon --k_s=$k --k_r=$k $skew_flag
#     echo "./exp_runner --J="SJ" --r_index="CEager" --s_index="CEager" $public_data_flag --public_r=$public_r --public_s=$public_s --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --eps_s=$epsilon --k_s=$k --k_r=$k $skew_flag"
#     clear_path
#     ./exp_runner --J="SJ" --r_index="CEager" --s_index="CEager" $public_data_flag --public_r=$public_r --public_s=$public_s --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --eps_s=$epsilon --k_s=$k --k_r=$k $skew_flag
#     echo "./exp_runner --J="SJ" --r_index="CLazy" --s_index="CLazy" $public_data_flag --public_r=$public_r --public_s=$public_s --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --eps_s=$epsilon --k_s=$k --k_r=$k $skew_flag"
#     clear_path
#     ./exp_runner --J="SJ" --r_index="CLazy" --s_index="CLazy" $public_data_flag --public_r=$public_r --public_s=$public_s --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --eps_s=$epsilon --k_s=$k --k_r=$k $skew_flag
#     echo "./exp_runner --J="SJ" --r_index="CComp" --s_index="CComp" $public_data_flag --public_r=$public_r --public_s=$public_s --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --eps_s=$epsilon --k_s=$k --k_r=$k $skew_flag"
#     clear_path
#     ./exp_runner --J="SJ" --r_index="CComp" --s_index="CComp" $public_data_flag --public_r=$public_r --public_s=$public_s --output_file=$output --db_r=$db_r_path --db_s=$db_s_path --eps_s=$epsilon --k_s=$k --k_r=$k $skew_flag
# done
