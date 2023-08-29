//
// Created by  on 2023/8/23.
//

#include <coroutine>
#include <iostream>

struct promise;

struct coro : std::coroutine_handle<promise> {
  using promise_type = ::promise;
};

struct promise {
  coro get_return_object() { return {coro::from_promise(*this)}; }

  std::suspend_always initial_suspend() noexcept {
    std::cout << "initial_suspend" << std::endl;
    return {};
  }

  std::suspend_always final_suspend() noexcept {
    std::cout << "final_suspend" << std::endl;
    return {};
  }

  void return_void() noexcept { std::cout << "return_void" << std::endl; }

  void unhandled_exception() noexcept { std::cout << "unhandled_exception" << std::endl; }
};

struct S {
  int i;
  coro f() {
    std::cout << "f: " << i << std::endl;
    co_return;
  }
};

void bad1() {
  coro h = S{0}.f();
  h.resume();
  h.destroy();
}

coro bad2() {
  S s{0};
  return s.f();
}

void bad3() {
  coro h = [i = 0]() -> coro {
    std::cout << "h: " << i << std::endl;
    co_return;
  }();

  h.resume();
  h.destroy();
}

void good() {
  coro h = [](int i) -> coro {
    std::cout << "good: " << i << std::endl;
    co_return;
  }(0);
  h.resume();
  h.destroy();
}

int main() {
  good();
}