# Run index_nested_loop
make index_nested_loop_runner
./index_nested_loop_runner --M=32 --ingestion --r_tuples=100 --s_tuples=200

# RUN index_sort_merge
make index_sort_merge_runner
./index_sort_merge_runner --M=32 --ingestion --index=lazy --r_tuples=100 --s_tuples=200

./index_sort_merge_runner --M=32 --ingestion --index=comp --r_tuples=100 --s_tuples=200
./index_sort_merge_runner --M=32 --ingestion --index=lazy --r_tuples=100 --s_tuples=200
./index_sort_merge_runner --M=32 --ingestion --index=eager --r_tuples=100 --s_tuples=200

make covering_index_sort_merge_runner
./covering_index_sort_merge_runner --M=32 --ingestion --index=lazy --r_tuples=100 --s_tuples=200
./covering_index_sort_merge_runner --M=32 --ingestion --index=eager --r_tuples=100 --s_tuples=200
./covering_index_sort_merge_runner --M=32 --ingestion --index=comp --r_tuples=100 --s_tuples=200

make external_sort_merge_runner
./external_sort_merge_runner --M=32 --ingestion --r_tuples=100 --s_tuples=200

make external_hash_join_runner
./external_hash_join_runner --M=32 --ingestion --r_tuples=100 --s_tuples=200