# Run index_nested_loop
make index_nested_loop_runner
./index_nested_loop_runner --M=32 --ingestion

# RUN index_sort_merge
make index_sort_merge_runner
./index_sort_merge_runner --M=32 --ingestion --index=comp
./index_sort_merge_runner --M=32 --ingestion --index=lazy
./index_sort_merge_runner --M=32 --ingestion --index=eager