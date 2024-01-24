# Run index_nested_loop
make index_nested_loop_runner
./index_nested_loop_runner --M=32 --ingestion --public_data --db_r="/home/weiping/code/learned-joins/data/fb_200M_uint64"
./index_nested_loop_runner --M=32 --ingestion --public_data
# RUN index_sort_merge
make index_sort_merge_runner
./index_sort_merge_runner --ingestion --index="lazy" --num_loop=10

./index_sort_merge_runner --M=32 --ingestion --index=comp --r_tuples=100 --s_tuples=200
./index_sort_merge_runner --M=32 --ingestion --index=lazy --r_tuples=100 --s_tuples=200
./index_sort_merge_runner --M=32 --ingestion --index=eager --r_tuples=100 --s_tuples=200

make covering_index_sort_merge_runner
./covering_index_sort_merge_runner --M=32 --ingestion --index="lazy"
./covering_index_sort_merge_runner --M=32 --ingestion --index=lazy --r_tuples=200 --s_tuples=100 --loop_count=10
./covering_index_sort_merge_runner --M=32 --ingestion --index=eager --r_tuples=100 --s_tuples=200
./covering_index_sort_merge_runner --M=32 --ingestion --index=comp --r_tuples=100 --s_tuples=200

make external_sort_merge_runner && ./external_sort_merge_runner --ingestion --num_loop=10

make external_hash_join_runner && ./external_hash_join_runner --ingestion --num_loop=10

# fb_200M_uint64  fb_200M_uint64.txt  osm_cellids_800M_uint64  wiki_ts_200M_uint64

make index_nested_loop_runner && ./index_nested_loop_runner --M=64 --ingestion --public_data --db_r="/home/weiping/code/learned-joins/data/fb_200M_uint64" --db_s="/home/weiping/code/learned-joins/data/fb_200M_uint64" --num_loop=100

g++ -g -fno-rtti external_sort_merge_runner.cc -oexternal_sort_merge_runner ../librocksdb.a -I../include -O2 -std=c++17 -lpthread -lrt -ldl -lsnappy -lgflags -lz -lbz2 -llz4 -lzstd -lnuma -ltbb -std=c++17  -faligned-new -DHAVE_ALIGNED_NEW -DROCKSDB_PLATFORM_POSIX -DROCKSDB_LIB_IO_POSIX  -DOS_LINUX -fno-builtin-memcmp -DROCKSDB_FALLOCATE_PRESENT -DSNAPPY -DGFLAGS=1 -DZLIB -DBZIP2 -DLZ4 -DZSTD -DNUMA -DTBB -DROCKSDB_MALLOC_USABLE_SIZE -DROCKSDB_PTHREAD_ADAPTIVE_MUTEX -DROCKSDB_BACKTRACE -DROCKSDB_RANGESYNC_PRESENT -DROCKSDB_SCHED_GETCPU_PRESENT -DROCKSDB_AUXV_GETAUXVAL_PRESENT -march=native  -DHAVE_UINT128_EXTENSION -DROCKSDB_JEMALLOC -DJEMALLOC_NO_DEMANGLE   -ldl -lpthread