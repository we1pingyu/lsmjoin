# Run index_nested_loop
make index_nested_loop_runner
./index_nested_loop_runner --M=32 --ingestion --public_data --db_r="/home/weiping/code/learned-joins/data/fb_200M_uint64"
./index_nested_loop_runner --M=32 --ingestion --public_data
# RUN index_sort_merge
make index_sort_merge_runner
./index_sort_merge_runner --M=32 --ingestion --index="lazy" --r_tuples=100 --s_tuples=200

./index_sort_merge_runner --M=32 --ingestion --index=comp --r_tuples=100 --s_tuples=200
./index_sort_merge_runner --M=32 --ingestion --index=lazy --r_tuples=100 --s_tuples=200
./index_sort_merge_runner --M=32 --ingestion --index=eager --r_tuples=100 --s_tuples=200

make covering_index_sort_merge_runner
./covering_index_sort_merge_runner --M=32 --ingestion --index="lazy"
./covering_index_sort_merge_runner --M=32 --ingestion --index=lazy --r_tuples=200 --s_tuples=100 --loop_count=10
./covering_index_sort_merge_runner --M=32 --ingestion --index=eager --r_tuples=100 --s_tuples=200
./covering_index_sort_merge_runner --M=32 --ingestion --index=comp --r_tuples=100 --s_tuples=200

make external_sort_merge_runner
./external_sort_merge_runner --M=32 --ingestion --r_tuples=100 --s_tuples=200

make external_hash_join_runner
./external_hash_join_runner --M=32 --ingestion --r_tuples=200 --s_tuples=100

# fb_200M_uint64  fb_200M_uint64.txt  osm_cellids_800M_uint64  wiki_ts_200M_uint64