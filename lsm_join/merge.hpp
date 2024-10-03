#include <algorithm>
#include <condition_variable>
#include <fstream>
#include <iostream>
#include <queue>
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
    timer = Timer();
    out << root.secondary_key << "," << *root.primary_key << "\n";
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
  // 线程安全的队列，用于存储数据块
  struct DataChunk {
    vector<MinHeapNode> data;
    int output_file_index;
  };

  std::queue<DataChunk> data_queue;
  std::mutex queue_mutex;
  std::condition_variable data_cond;
  bool done = false;

  // 创建输出文件和对应的互斥锁
  vector<ofstream> out_files(num_ways);
  vector<mutex> out_file_mutexes(num_ways);
  for (int i = 0; i < num_ways; i++) {
    string fileName = prefix + to_string(i);
    out_files[i].open(fileName);
  }

  // 消费者线程函数
  auto worker = [&]() {
    while (true) {
      DataChunk data_chunk;
      {
        std::unique_lock<std::mutex> lock(queue_mutex);
        data_cond.wait(lock, [&] { return !data_queue.empty() || done; });
        if (data_queue.empty() && done) {
          break;
        }
        data_chunk = std::move(data_queue.front());
        data_queue.pop();
      }
      // 对数据块进行排序
      mergeSort(data_chunk.data.data(), 0, data_chunk.data.size() - 1);
      // 写入对应的输出文件
      int output_file_index = data_chunk.output_file_index;
      {
        std::lock_guard<std::mutex> lock(out_file_mutexes[output_file_index]);
        for (const auto& node : data_chunk.data) {
          out_files[output_file_index] << node.secondary_key << ","
                                       << *node.primary_key << "\n";
        }
      }
      // 释放内存
      for (auto& node : data_chunk.data) {
        delete node.primary_key;
      }
    }
  };

  // 启动消费者线程
  int num_threads = std::thread::hardware_concurrency();
  if (num_threads == 0) num_threads = 4;  // 默认使用4个线程
  std::vector<std::thread> workers;
  for (int i = 0; i < num_threads; ++i) {
    workers.emplace_back(worker);
  }

  // 主线程作为生产者，从数据库读取数据并放入队列
  rocksdb::Iterator* it = db->NewIterator(ReadOptions());
  it->SeekToFirst();
  int next_output_file = 0;
  while (it->Valid()) {
    DataChunk data_chunk;
    data_chunk.output_file_index = next_output_file;
    next_output_file = (next_output_file + 1) % num_ways;
    for (int i = 0; it->Valid() && i < run_size; ++i) {
      MinHeapNode node;
      string val_it = it->value().ToString();
      node.secondary_key = val_it.substr(0, SECONDARY_SIZE);
      string tmp = it->key().ToString() +
                   val_it.substr(SECONDARY_SIZE, VALUE_SIZE - SECONDARY_SIZE);
      node.primary_key = new string(tmp);
      data_chunk.data.push_back(node);
      it->Next();
    }
    {
      std::unique_lock<std::mutex> lock(queue_mutex);
      data_queue.push(std::move(data_chunk));
    }
    data_cond.notify_one();
  }
  delete it;

  // 通知消费者线程数据已全部生产完毕
  {
    std::unique_lock<std::mutex> lock(queue_mutex);
    done = true;
  }
  data_cond.notify_all();

  // 等待所有消费者线程完成
  for (auto& t : workers) {
    t.join();
  }

  // 关闭输出文件
  for (int i = 0; i < num_ways; i++) {
    out_files[i].close();
  }
}

void concurrent_createInitialRuns(DB* db, int run_size, int num_ways,
                                  int VALUE_SIZE, int SECONDARY_SIZE,
                                  RunResult& run_result, int nthreads,
                                  string prefix = "/tmp/") {
  const int batch_size = run_size;   // 每个批次处理的数据量
  const int num_threads = nthreads;  // 最大线程数
  cout << "nthreads: " << nthreads << endl;
  // 线程向量，用于管理创建的线程
  std::vector<std::thread> threads;
  std::mutex mutex;  // 用于保护共享资源的互斥锁

  // 输出文件流数组和对应的互斥锁
  std::vector<std::ofstream> out_files(num_ways);
  std::vector<std::mutex> out_file_mutexes(num_ways);

  // 打开输出文件
  for (int i = 0; i < num_ways; ++i) {
    std::string fileName = prefix + std::to_string(i);
    out_files[i].open(fileName);
    if (!out_files[i].is_open()) {
      std::cerr << "无法打开文件：" << fileName << std::endl;
      return;
    }
  }

  // 创建迭代器，从数据库中读取数据
  rocksdb::Iterator* it = db->NewIterator(ReadOptions());
  it->SeekToFirst();

  int next_output_file = 0;

  while (it->Valid()) {
    // 收集一个批次的数据
    std::vector<MinHeapNode> batch_data;
    batch_data.reserve(batch_size);

    for (int i = 0; i < batch_size && it->Valid(); ++i, it->Next()) {
      MinHeapNode node;
      std::string val_it = it->value().ToString();
      node.secondary_key = val_it.substr(0, SECONDARY_SIZE);
      std::string tmp =
          it->key().ToString() +
          val_it.substr(SECONDARY_SIZE, VALUE_SIZE - SECONDARY_SIZE);
      node.primary_key = new std::string(tmp);
      batch_data.push_back(std::move(node));
    }

    // 确定当前批次要写入的输出文件索引
    int output_file_index = next_output_file;
    next_output_file = (next_output_file + 1) % num_ways;

    // 创建新线程处理当前批次的数据
    threads.emplace_back([&, batch_data = std::move(batch_data),
                          output_file_index]() mutable {
      // 对批次数据进行排序
      mergeSort(batch_data.data(), 0, batch_data.size() - 1);

      // 将排序后的数据写入对应的输出文件
      {
        std::lock_guard<std::mutex> lock(out_file_mutexes[output_file_index]);
        for (const auto& node : batch_data) {
          out_files[output_file_index] << node.secondary_key << ","
                                       << *node.primary_key << "\n";
        }
      }

      // 释放内存
      for (auto& node : batch_data) {
        delete node.primary_key;
      }
    });

    // 控制活动线程的数量，避免创建过多线程
    if (threads.size() >= static_cast<size_t>(num_threads)) {
      for (auto& t : threads) {
        if (t.joinable()) t.join();
      }
      threads.clear();
    }
  }

  // 处理剩余的线程
  for (auto& t : threads) {
    if (t.joinable()) t.join();
  }

  // 关闭输出文件
  for (int i = 0; i < num_ways; ++i) {
    out_files[i].close();
  }

  // 释放迭代器
  delete it;
}

// 多线程版本的 mergeFiles 函数
void concurrent_mergeFiles(string output_file, int n, int k, RunResult& result,
                           int nthreads, string prefix = "/tmp/") {
  double sort_time = 0.0;
  Timer sort_timer;

  // 定义互斥锁和条件变量
  std::mutex mutex;
  std::condition_variable cv;
  int active_threads = 0;
  int max_threads = nthreads;
  if (max_threads == 0) max_threads = 4;  // 默认设置为4个线程

  // 打开所有输入文件
  std::vector<std::ifstream> in(k);
  for (int i = 0; i < k; i++) {
    in[i].open(prefix + to_string(i));
  }

  // 打开输出文件
  ofstream out(output_file);

  // 初始化小顶堆
  MinHeapNode* harr = new MinHeapNode[k];
  int i;
  string line;

  // 从每个输入文件读取第一行数据
  for (i = 0; i < k; i++) {
    if (!getline(in[i], line)) {
      break;
    }
    std::istringstream iss(line);
    std::string first, second;
    if (getline(iss, first, ',') && getline(iss, second)) {
      harr[i].secondary_key = first;
      harr[i].primary_key = new string(second);
    }
    harr[i].i = i;
  }
  MinHeap hp(harr, i);

  int count = 0;
  uint64_t heapify_count = 0;

  // 合并过程
  while (count != i) {
    MinHeapNode root = hp.getMin();
    heapify_count++;
    out << root.secondary_key << "," << *root.primary_key << "\n";
    delete root.primary_key;  // 释放内存

    if (!getline(in[root.i], line)) {
      root.secondary_key = "999999999999";
      count++;
    } else {
      std::istringstream iss(line);
      std::string first, second;
      if (getline(iss, first, ',') && getline(iss, second)) {
        root.secondary_key = first;
        root.primary_key = new string(second);
      }
    }
    hp.replaceMin(root);
  }

  // 关闭所有文件
  for (int i = 0; i < k; i++) in[i].close();
  out.close();

  result.sort_cpu_time += sort_timer.elapsed() - sort_time;
  result.sort_io_time += sort_time;

  delete[] harr;
}

void externalSort(DB* db, string output_file, int num_ways, int run_size,
                  int VALUE_SIZE, int SECONDARY_SIZE, RunResult& run_result,
                  string prefix) {
  createInitialRuns(db, run_size, num_ways, VALUE_SIZE, SECONDARY_SIZE,
                    run_result, prefix);
  mergeFiles(output_file, run_size, num_ways, run_result, prefix);
}

void concurrent_externalSort(DB* db, string output_file, int num_ways,
                             int run_size, int VALUE_SIZE, int SECONDARY_SIZE,
                             RunResult& run_result, int nthreads,
                             string prefix) {
  concurrent_createInitialRuns(db, run_size, num_ways, VALUE_SIZE,
                               SECONDARY_SIZE, run_result, nthreads, prefix);
  concurrent_mergeFiles(output_file, run_size, num_ways, run_result, nthreads,
                        prefix);
}
}  // namespace MERGE