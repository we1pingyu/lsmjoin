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

void mergeFiles(string output_file, int n, int k, string prefix = "/tmp/") {
  vector<ifstream> in(k);
  for (int i = 0; i < k; i++) {
    string fileName;
    fileName = prefix + to_string(i);
    char* buf = new char[4096];
    in[i].open(fileName);
    in[i].rdbuf()->pubsetbuf(buf, 4096);
    delete[] buf;
  }

  ofstream out;
  out.open(output_file);
  MinHeapNode* harr = new MinHeapNode[k];
  int i;
  string line;
  timespec t1, t2, t3;
  double time = 0.0, time2 = 0.0;
  for (i = 0; i < k; i++) {
    if (!getline(in[i], line)) {
      break;
    }
    std::istringstream iss(line);
    std::string first, second;
    clock_gettime(CLOCK_MONOTONIC, &t1);
    if (getline(iss, first, ',') && getline(iss, second)) {
      harr[i].secondary_key = first;
      string tmp = second;
      harr[i].primary_key = new string(tmp);
    }
    clock_gettime(CLOCK_MONOTONIC, &t2);
    time += (t2.tv_sec - t1.tv_sec) + (t2.tv_nsec - t1.tv_nsec) / 1000000000.0;
    harr[i].i = i;
  }
  clock_gettime(CLOCK_MONOTONIC, &t1);
  MinHeap hp(harr, i);
  clock_gettime(CLOCK_MONOTONIC, &t2);
  time2 += (t2.tv_sec - t1.tv_sec) + (t2.tv_nsec - t1.tv_nsec) / 1000000000.0;
  int count = 0;
  uint64_t heapify_count = 0;

  while (count != i) {
    clock_gettime(CLOCK_MONOTONIC, &t1);
    MinHeapNode root = hp.getMin();
    clock_gettime(CLOCK_MONOTONIC, &t2);
    time2 += (t2.tv_sec - t1.tv_sec) + (t2.tv_nsec - t1.tv_nsec) / 1000000000.0;
    heapify_count++;
    // cout << root.secondary_key << "," << *root.primary_key << endl;
    out << root.secondary_key << "," << *root.primary_key << "\n";
    clock_gettime(CLOCK_MONOTONIC, &t1);
    if (!getline(in[root.i], line)) {
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
    clock_gettime(CLOCK_MONOTONIC, &t2);
    time += (t2.tv_sec - t1.tv_sec) + (t2.tv_nsec - t1.tv_nsec) / 1000000000.0;
    clock_gettime(CLOCK_MONOTONIC, &t1);
    hp.replaceMin(root);
    clock_gettime(CLOCK_MONOTONIC, &t2);
    time2 += (t2.tv_sec - t1.tv_sec) + (t2.tv_nsec - t1.tv_nsec) / 1000000000.0;
  }
  cout << "file read io time: " << time << endl;
  cout << "pass1 sort time: " << time2 << endl;
  cout << "heapify_count: " << heapify_count << endl;
  for (int i = 0; i < k; i++) in[i].close();
  out.close();
  cout << "file write finished" << endl;
  delete[] harr;
}

void createInitialRuns(DB* db, int run_size, int num_ways, int VALUE_SIZE,
                       int SECONDARY_SIZE, string prefix = "/tmp/") {
  cout << "num_ways: " << num_ways << endl;
  ofstream* out = new ofstream[num_ways];
  string fileName;
  for (int i = 0; i < num_ways; i++) {
    fileName = prefix + to_string(i);
    out[i].open(fileName);
  }

  bool more_input = true;
  int next_output_file = 0;
  int i;
  rocksdb::Iterator* it = db->NewIterator(ReadOptions());
  it->SeekToFirst();
  int count = 0;
  struct timespec t1, t2;
  double time = 0.0, time2 = 0.0, time3 = 0.0;
  uint64_t write_count = 0;
  while (it->Valid()) {
    clock_gettime(CLOCK_MONOTONIC, &t1);
    MinHeapNode* arr = new MinHeapNode[run_size];
    for (i = 0; it->Valid() && i < run_size; i++, it->Next()) {
      arr[i].secondary_key = it->value().ToString().substr(0, SECONDARY_SIZE);
      string tmp = it->key().ToString() +
                   it->value().ToString().substr(SECONDARY_SIZE,
                                                 VALUE_SIZE - SECONDARY_SIZE);
      arr[i].primary_key = new string(tmp);
    }
    clock_gettime(CLOCK_MONOTONIC, &t2);
    time += (t2.tv_sec - t1.tv_sec) + (t2.tv_nsec - t1.tv_nsec) / 1000000000.0;
    clock_gettime(CLOCK_MONOTONIC, &t1);
    mergeSort(arr, 0, i - 1);
    clock_gettime(CLOCK_MONOTONIC, &t2);
    time3 += (t2.tv_sec - t1.tv_sec) + (t2.tv_nsec - t1.tv_nsec) / 1000000000.0;
    clock_gettime(CLOCK_MONOTONIC, &t1);

    for (int j = 0; j < i; j++) {
      write_count++;
      // cout << arr[j].primary_key << " " << *arr[j].primary_key
      //      << endl;

      out[next_output_file] << arr[j].secondary_key << ","
                            << *arr[j].primary_key << "\n";
    }
    clock_gettime(CLOCK_MONOTONIC, &t2);
    time2 += (t2.tv_sec - t1.tv_sec) + (t2.tv_nsec - t1.tv_nsec) / 1000000000.0;
    // cout << "write to file finished" << endl;
    next_output_file++;

    // release memory
    for (int j = 0; j < i; j++) delete arr[j].primary_key;
    delete[] arr;
  }
  cout << "lsm read io time: " << time << endl;
  cout << "file write count: " << write_count << endl;
  cout << "file write io time: " << time2 << endl;
  cout << "pass0 sort time: " << time3 << endl;
  // close input and output files
  for (int i = 0; i < num_ways; i++) out[i].close();
  delete it;
  delete[] out;
}

void externalSort(DB* db, string output_file, int num_ways, int run_size,
                  int VALUE_SIZE, int SECONDARY_SIZE) {
  createInitialRuns(db, run_size, num_ways, VALUE_SIZE, SECONDARY_SIZE);
  mergeFiles(output_file, run_size, num_ways);
}