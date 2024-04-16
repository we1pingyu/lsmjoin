#include <algorithm>
#include <fstream>
#include <iostream>
#include <random>
#include <string>
#include <typeinfo>
#include <unordered_map>
#include <vector>

#include "rocksdb/compaction_filter.h"
#include "rocksdb/db.h"
#include "rocksdb/filter_policy.h"
#include "rocksdb/iostats_context.h"
#include "rocksdb/merge_operator.h"
#include "rocksdb/perf_context.h"
#include "rocksdb/slice_transform.h"
#include "rocksdb/statistics.h"
#include "rocksdb/table.h"

using namespace std;

namespace MERGE {
struct MinHeapNode {
  string secondary_key;
  string* primary_key;
  int i;
};

void swap(MinHeapNode* x, MinHeapNode* y);

class MinHeap {
  MinHeapNode* harr;
  int heap_size;

 public:
  MinHeap(MinHeapNode a[], int size);
  void MinHeapify(int);
  int left(int i) { return (2 * i + 1); }
  int right(int i) { return (2 * i + 2); }
  MinHeapNode getMin() { return harr[0]; }
  void replaceMin(MinHeapNode x) {
    harr[0] = x;
    MinHeapify(0);
  }
};

MinHeap::MinHeap(MinHeapNode a[], int size) {
  heap_size = size;
  harr = a;  // store address of array
  int i = (heap_size - 1) / 2;
  while (i >= 0) {
    MinHeapify(i);
    i--;
  }
}

void MinHeap::MinHeapify(int i) {
  int l = left(i);
  int r = right(i);
  int smallest = i;
  if (l < heap_size && harr[l].secondary_key < harr[i].secondary_key)
    smallest = l;
  if (r < heap_size && harr[r].secondary_key < harr[smallest].secondary_key)
    smallest = r;
  if (smallest != i) {
    swap(&harr[i], &harr[smallest]);
    MinHeapify(smallest);
  }
}

void swap(MinHeapNode* x, MinHeapNode* y) {
  MinHeapNode temp = *x;
  *x = *y;
  *y = temp;
}

void merge(MinHeapNode arr[], int l, int m, int r) {
  int i, j, k;
  int n1 = m - l + 1;
  int n2 = r - m;
  MinHeapNode* L = new MinHeapNode[n1];
  MinHeapNode* R = new MinHeapNode[n2];
  for (i = 0; i < n1; i++) L[i] = arr[l + i];
  for (j = 0; j < n2; j++) R[j] = arr[m + 1 + j];
  i = 0;
  j = 0;
  k = l;
  while (i < n1 && j < n2) {
    if (L[i].secondary_key <= R[j].secondary_key)
      arr[k++] = L[i++];
    else
      arr[k++] = R[j++];
  }
  while (i < n1) arr[k++] = L[i++];
  while (j < n2) arr[k++] = R[j++];
  delete[] L;
  delete[] R;
}

void mergeSort(MinHeapNode arr[], int l, int r) {
  if (l < r) {
    int m = l + (r - l) / 2;
    mergeSort(arr, l, m);
    mergeSort(arr, m + 1, r);
    merge(arr, l, m, r);
  }
}

void mergeFiles(string output_file, int n, int k, RunResult& result,
                string prefix = "/tmp/") {
  vector<ifstream> in(k);
  double sort_time = 0.0;
  Timer timer = Timer();
  Timer sort_timer = Timer();
  for (int i = 0; i < k; i++) {
    string fileName;
    fileName = prefix + to_string(i);
    char* buf = new char[4096];
    in[i].open(fileName);
    in[i].rdbuf()->pubsetbuf(buf, 4096);
    delete[] buf;
  }
  sort_time += timer.elapsed();
  ofstream out;
  timer = Timer();
  out.open(output_file);
  sort_time += timer.elapsed();
  MinHeapNode* harr = new MinHeapNode[k];
  int i;
  string line;
  timespec t1, t2, t3;
  for (i = 0; i < k; i++) {
    timer = Timer();
    if (!getline(in[i], line)) {
      break;
    }
    sort_time += timer.elapsed();
    std::istringstream iss(line);
    std::string first, second;
    if (getline(iss, first, ',') && getline(iss, second)) {
      harr[i].secondary_key = first;
      string tmp = second;
      harr[i].primary_key = new string(tmp);
    }
    harr[i].i = i;
  }
  MinHeap hp(harr, i);
  int count = 0;
  uint64_t heapify_count = 0;

  while (count != i) {
    MinHeapNode root = hp.getMin();
    heapify_count++;
    out << root.secondary_key << "," << *root.primary_key << "\n";
    timer = Timer();
    bool is_success = getline(in[root.i], line) || false;
    sort_time += timer.elapsed();

    if (!is_success) {
      root.secondary_key = "999999999999";
      count++;
    } else {
      std::istringstream iss(line);
      std::string first, second;
      if (getline(iss, first, ',') && getline(iss, second)) {
        root.secondary_key = first;
        string tmp = second;
        root.primary_key = new string(tmp);
      }
    }
    hp.replaceMin(root);
  }
  // cout << "file read io time: " << time << endl;
  // cout << "pass1 sort time: " << time2 << endl;
  // cout << "heapify_count: " << heapify_count << endl;
  timer = Timer();
  for (int i = 0; i < k; i++) in[i].close();
  out.close();
  sort_time += timer.elapsed();
  result.sort_cpu_time += sort_timer.elapsed() - sort_time;
  result.sort_io_time += sort_time;
  // cout << "file write finished" << endl;
  delete[] harr;
}

void createInitialRuns(DB* db, int run_size, int num_ways, int VALUE_SIZE,
                       int SECONDARY_SIZE, RunResult& run_result,
                       string prefix = "/tmp/") {
  // cout << "num_ways: " << num_ways << endl;
  double data_time = 0.0, sort_time = 0.0, string_process_time = 0.0;
  Timer sort_timer = Timer();
  Timer timer = Timer();
  ofstream* out = new ofstream[num_ways];
  string fileName;
  for (int i = 0; i < num_ways; i++) {
    fileName = prefix + to_string(i);
    out[i].open(fileName);
  }
  sort_time += timer.elapsed();
  bool more_input = true;
  int next_output_file = 0;
  int i;

  rocksdb::Iterator* it = db->NewIterator(ReadOptions());
  it->SeekToFirst();
  int count = 0;
  uint64_t write_count = 0;
  // check if it valid
  if (!it->Valid()) {
    cout << "it is not valid" << endl;
    exit(1);
  }
  while (it->Valid()) {
    MinHeapNode* arr = new MinHeapNode[run_size];
    for (i = 0; it->Valid() && i < run_size;) {
      // Timer string_timer = Timer();

      string val_it = it->value().ToString();
      arr[i].secondary_key = val_it.substr(0, SECONDARY_SIZE);
      string tmp = it->key().ToString() +
                   val_it.substr(SECONDARY_SIZE, VALUE_SIZE - SECONDARY_SIZE);
      arr[i].primary_key = new string(tmp);
      // string_process_time += string_timer.elapsed();
      Timer timer = Timer();
      it->Next();
      i++;
      data_time += timer.elapsed();
      if (!it->Valid() || i > run_size) break;
    }
    mergeSort(arr, 0, i - 1);
    timer = Timer();
    for (int j = 0; j < i; j++) {
      write_count++;
      out[next_output_file] << arr[j].secondary_key << ","
                            << *arr[j].primary_key << "\n";
    }
    sort_time += timer.elapsed();
    // cout << "write to file finished" << endl;
    next_output_file++;

    // release memory
    for (int j = 0; j < i; j++) delete arr[j].primary_key;
    delete[] arr;
  }
  // cout << "lsm read io time: " << time << endl;
  // cout << "file write count: " << write_count << endl;
  // cout << "file write io time: " << time2 << endl;
  // cout << "pass0 sort time: " << time3 << endl;
  // close input and output files
  timer = Timer();
  for (int i = 0; i < num_ways; i++) out[i].close();
  sort_time += timer.elapsed();

  run_result.sort_cpu_time +=
      sort_timer.elapsed() - sort_time - data_time - string_process_time;
  run_result.sort_io_time += sort_time;
  run_result.get_data_time += data_time;
  run_result.string_process_time += string_process_time;
  delete it;
  delete[] out;
}

void externalSort(DB* db, string output_file, int num_ways, int run_size,
                  int VALUE_SIZE, int SECONDARY_SIZE, RunResult& run_result,
                  string prefix) {
  createInitialRuns(db, run_size, num_ways, VALUE_SIZE, SECONDARY_SIZE,
                    run_result, prefix);
  mergeFiles(output_file, run_size, num_ways, run_result, prefix);
}
}  // namespace MERGE