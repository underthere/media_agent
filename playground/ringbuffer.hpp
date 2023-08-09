#ifndef RINGBUFFER_HPP
#define RINGBUFFER_HPP

#include <condition_variable>
#include <cstddef>
#include <functional>
#include <mutex>
#include <iostream>

template <typename T, std::size_t N, T*(*constructor)(), void(*deconstructor)(T**)>
class ring_buffer {
 private:
  T* buffer[N];
  std::size_t head;
  std::size_t tail;

  std::mutex mutex;
  std::condition_variable not_full;
  std::condition_variable not_empty;

 public:
  bool all_done = false;

  ring_buffer() : head(0), tail(0) {}
  ~ring_buffer() {
    for (auto &item : buffer) {
      if (item != nullptr) {
        deconstructor(&item);
      }
    }
  }

  auto push() -> T & {
    std::unique_lock<std::mutex> lock(mutex);
    not_full.wait(lock, [this]() { return !full_inner(); });
    auto &item = buffer[head];
    if (item == nullptr) {
      item = constructor();
    }
    head = (head + 1) % N;
    not_empty.notify_one();
    return *item;
  }

  T &pop() {
    std::unique_lock<std::mutex> lock(mutex);
    not_empty.wait(lock, [this]() { return !empty_inner(); });
    auto &item = buffer[tail];
    tail = (tail + 1) % N;
    not_full.notify_one();
    return *item;
  }

  bool empty() {
    std::lock_guard<std::mutex> lock(mutex);
    return empty_inner();
  }

  bool full() {
    std::lock_guard<std::mutex> lock(mutex);
    return full_inner();
  }

 private:
  bool empty_inner() const { return head == tail; }

  bool full_inner() const { return (head + 1) % N == tail; }
};

#endif // RINGBUFFER_HPP
