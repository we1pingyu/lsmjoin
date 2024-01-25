include ../make_config.mk

ifndef DISABLE_JEMALLOC
	ifdef JEMALLOC
		PLATFORM_CXXFLAGS += -DROCKSDB_JEMALLOC -DJEMALLOC_NO_DEMANGLE
	endif
	EXEC_LDFLAGS := $(JEMALLOC_LIB) $(EXEC_LDFLAGS) -lpthread
	PLATFORM_CXXFLAGS += $(JEMALLOC_INCLUDE)
endif

ifneq ($(USE_RTTI), 1)
CXXFLAGS += -fno-rtti
endif

# CFLAGS += -Wstrict-prototypes

.PHONY: clean librocksdb

all: index_nested_loop_runner covering_index_nested_loop_runner external_sort_merge_runner index_sort_merge_runner covering_index_sort_merge_runner external_hash_join_runner

index_nested_loop_runner: librocksdb index_nested_loop_runner.cc
	$(CXX) $(CXXFLAGS) $@.cc -o$@ ../librocksdb.a -I../include -O2 -std=c++17 $(PLATFORM_LDFLAGS) $(PLATFORM_CXXFLAGS) $(EXEC_LDFLAGS)

covering_index_nested_loop_runner: librocksdb covering_index_nested_loop_runner.cc
	$(CXX) $(CXXFLAGS) $@.cc -o$@ ../librocksdb.a -I../include -O2 -std=c++17 $(PLATFORM_LDFLAGS) $(PLATFORM_CXXFLAGS) $(EXEC_LDFLAGS)

external_sort_merge_runner: librocksdb external_sort_merge_runner.cc
	$(CXX) $(CXXFLAGS) $@.cc -o$@ ../librocksdb.a -I../include -O2 -std=c++17 $(PLATFORM_LDFLAGS) $(PLATFORM_CXXFLAGS) $(EXEC_LDFLAGS)

index_sort_merge_runner: librocksdb index_sort_merge_runner.cc
	$(CXX) $(CXXFLAGS) $@.cc -o$@ ../librocksdb.a -I../include -O2 -std=c++17 $(PLATFORM_LDFLAGS) $(PLATFORM_CXXFLAGS) $(EXEC_LDFLAGS)

covering_index_sort_merge_runner: librocksdb covering_index_sort_merge_runner.cc
	$(CXX) $(CXXFLAGS) $@.cc -o$@ ../librocksdb.a -I../include -O2 -std=c++17 $(PLATFORM_LDFLAGS) $(PLATFORM_CXXFLAGS) $(EXEC_LDFLAGS)

external_hash_join_runner: librocksdb external_hash_join_runner.cc
	$(CXX) $(CXXFLAGS) $@.cc -o$@ ../librocksdb.a -I../include -O2 -std=c++17 $(PLATFORM_LDFLAGS) $(PLATFORM_CXXFLAGS) $(EXEC_LDFLAGS)

exp_runner: librocksdb exp_runner.cc
	$(CXX) $(CXXFLAGS) $@.cc -o$@ ../librocksdb.a -I../include -O2 -std=c++17 $(PLATFORM_LDFLAGS) $(PLATFORM_CXXFLAGS) $(EXEC_LDFLAGS)

clean:
	rm -rf ./output.txt ./covering_index_sort_merge_runner ./exp_runner ./index_nested_loop_runner ./external_sort_merge_runner ./index_sort_merge_runner ./external_hash_join_runner ./test

librocksdb:
	cd .. && $(MAKE) static_lib USE_RTTI=1 
