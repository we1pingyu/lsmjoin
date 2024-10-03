// ThreadPool.h
#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <atomic>
#include <condition_variable>
#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

class ThreadPool {
 public:
  ThreadPool(size_t num_threads);
  ~ThreadPool();

  // 禁止拷贝和赋值
  ThreadPool(const ThreadPool&) = delete;
  ThreadPool& operator=(const ThreadPool&) = delete;

  // 提交任务到线程池
  template <class F, class... Args>
  auto enqueue(F&& f, Args&&... args)
      -> std::future<typename std::result_of<F(Args...)>::type>;

  // 获取正在运行的线程数量
  size_t getRunningThreadCount();

 private:
  // 工作线程函数
  void worker();

  // 线程池
  std::vector<std::thread> workers;

  // 任务队列
  std::queue<std::function<void()>> tasks;

  // 同步机制
  std::mutex queue_mutex;
  std::condition_variable condition;
  bool stop;

  // 正在运行的线程计数
  std::atomic<size_t> running_threads;
};

// 构造函数
ThreadPool::ThreadPool(size_t num_threads) : stop(false), running_threads(0) {
  for (size_t i = 0; i < num_threads; ++i) {
    workers.emplace_back(&ThreadPool::worker, this);
  }
}

// 工作线程函数
void ThreadPool::worker() {
  while (true) {
    std::function<void()> task;

    // 从任务队列中获取任务
    {
      std::unique_lock<std::mutex> lock(this->queue_mutex);
      condition.wait(lock,
                     [this]() { return this->stop || !this->tasks.empty(); });
      if (this->stop && this->tasks.empty()) return;
      task = std::move(this->tasks.front());
      this->tasks.pop();
      running_threads++;
    }

    // 执行任务
    task();

    // 任务完成，减少计数
    running_threads--;
  }
}

// 提交任务到线程池
template <class F, class... Args>
auto ThreadPool::enqueue(F&& f, Args&&... args)
    -> std::future<typename std::result_of<F(Args...)>::type> {
  using return_type = typename std::result_of<F(Args...)>::type;

  auto task_ptr = std::make_shared<std::packaged_task<return_type()>>(
      std::bind(std::forward<F>(f), std::forward<Args>(args)...));

  std::future<return_type> res = task_ptr->get_future();
  {
    std::unique_lock<std::mutex> lock(queue_mutex);

    // 禁止在停止的线程池中添加任务
    if (stop) {
      throw std::runtime_error("enqueue on stopped ThreadPool");
    }

    tasks.emplace([task_ptr]() { (*task_ptr)(); });
  }
  condition.notify_one();
  return res;
}

// 获取正在运行的线程数量
size_t ThreadPool::getRunningThreadCount() { return running_threads.load(); }

// 析构函数
ThreadPool::~ThreadPool() {
  {
    std::unique_lock<std::mutex> lock(queue_mutex);
    stop = true;
  }
  condition.notify_all();
  for (auto& worker : workers) {
    worker.join();
  }
}

#endif  // THREADPOOL_H
