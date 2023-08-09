//
// Created by underthere on 2023/7/17.
//

#ifndef MEDIA_AGENT_SAFE_QUEUE_H
#define MEDIA_AGENT_SAFE_QUEUE_H

#include <queue>
#include <mutex>
#include <condition_variable>

template <typename T>
class safe_queue {
 private:

  constexpr static auto max_size = 100;
  std::queue<T> queue_;
  std::mutex mtx_;
  std::condition_variable some_cond_;
  std::condition_variable full_cond_;

 public:
  safe_queue() = default;
  ~safe_queue() = default;

  void push(T &&t) {
    std::unique_lock<std::mutex> lk(mtx_);
    full_cond_.wait(lk, [this] { return queue_.size() < max_size; });
    queue_.emplace(t);
    some_cond_.notify_one();
  }

  void push(const T &t) {
    std::unique_lock<std::mutex> lk(mtx_);
    full_cond_.wait(lk, [this] { return queue_.size() < max_size; });
    queue_.emplace(t);
    some_cond_.notify_one();
  }

  auto pop() -> T {
    std::unique_lock<std::mutex> lk(mtx_);
    some_cond_.wait(lk, [this] { return !queue_.empty(); });
    auto t = std::move(queue_.front());
    queue_.pop();
    full_cond_.notify_one();
    return t;
  }

  /*     auto size() const -> std::size_t {
          std::lock_guard<std::mutex> lk(mtx_);
          return queue_.size();
      }

      auto empty() const -> bool {
          std::lock_guard<std::mutex> lk(mtx_);
          return queue_.empty();
      } */

};

#endif  // MEDIA_AGENT_SAFE_QUEUE_H
