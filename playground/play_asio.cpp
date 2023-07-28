//
// Created by underthere on 2023/7/26.
//

#include <boost/asio.hpp>
#include <boost/signals2.hpp>
#include <boost/core/noncopyable.hpp>
#include <chrono>
#include <iostream>


boost::signals2::signal<void(std::size_t, std::chrono::time_point<std::chrono::steady_clock>)> s;

void slot(std::size_t depth, std::chrono::time_point<std::chrono::steady_clock> st);
void slot(std::size_t depth, std::chrono::time_point<std::chrono::steady_clock> st) {
  auto time_delta = std::chrono::steady_clock::now() - st;
  std::cout << "slot: " << depth << " delay: " << std::chrono::duration_cast<std::chrono::milliseconds>(time_delta).count() << "ms"
            << std::endl;
  s(depth + 1, st);
}

class A {
 public:
  void slot(std::size_t depth, std::chrono::time_point<std::chrono::steady_clock> st) {
    auto time_delta = std::chrono::steady_clock::now() - st;
    std::cout << "A: " << depth << " delay: " << std::chrono::duration_cast<std::chrono::milliseconds>(time_delta).count() << "ms" << std::endl;
    s(depth + 1, st);
  }
};

int main() {
//  s.connect(slot);
  A a;
  s.connect([&a](std::size_t depth, std::chrono::time_point<std::chrono::steady_clock> st) {
    a.slot(depth, st);
  });
  auto now = std::chrono::steady_clock::now();
  s(0, now);
  return 0;
}
