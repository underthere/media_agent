#include <functional>
#include <iostream>
#include <string>

#include "expected.h"

template <class A> struct IO {
  std::function<A()> recipe;
  IO(std::function<A()> r) : recipe(r) {}
  A run() { return recipe(); }

  A operator()() { return run(); }
};

struct Unit {
} unit;

std::function<IO<Unit>(char)> put_char = [](char c) {
  return IO<Unit>([c]() -> Unit {
    std::cout << c;
    return unit;
  });
};

std::function<IO<Unit>(const std::string &)> put_str =
    [](const std::string &s) {
      return IO<Unit>([s]() -> Unit {
        std::cout << s;
        return unit;
      });
    };

std::function<IO<Unit>(const std::string &)> put_str_ln =
    [](const std::string &s) { return put_str(s + "\n"); };


tl::expected<int, std::string> add(int a, int b) {
  if (a < 0 || b < 0) {
    return tl::unexpected("negative number");
  }
  return a + b;
}

int main() {
  add(1, 1)
          .and_then([](int x) { return add(x, 2); })
          .and_then([](int x) { return add(x, 3); })
          .or_else([](const std::string &s) {
            std::cout << "error: " << s << std::endl;
          });
  add(-1, -3).or_else([](const std::string &s) {
    std::cout << "error: " << s << std::endl;
  });
  return 0;
}
