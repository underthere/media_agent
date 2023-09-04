//
// Created by  on 2023/8/30.
//

#ifndef MEDIA_AGENT_SIMPLE_IO_EXECUTOR_HPP
#define MEDIA_AGENT_SIMPLE_IO_EXECUTOR_HPP

#include <liburing.h>

#include "async_simple/IOExecutor.h"

namespace MA {
using namespace async_simple;

class SimpleIOExecutor : public IOExecutor {
  static constexpr std::uint32_t QD = 64;
  static constexpr std::uint32_t BS = 1024;
  static constexpr std::uint32_t STKSIZE = 8192;

  struct io_context {
    struct io_uring *ring;
    unsigned char* stack_buf;
    ucontext_t ctx_main, ctx_fnew;
  };

 public:
  SimpleIOExecutor() : IOExecutor() { io_uring_setup(32, nullptr); }

  void init() {
    auto ret = io_uring_queue_init(QD, &ring_, 0);
    assert(ret >= 0);

  }

  virtual ~SimpleIOExecutor() = default;

  void submitIO(int fd, iocb_cmd cmd, void* buffer, size_t length, off_t offset, AIOCallback cbfn) {
    io_uring_submit(&ring_);
  }
  void submitIOV(int fd, iocb_cmd cmd, const iovec_t* iov, size_t count, off_t offset, AIOCallback cbfn) {}

 private:
  struct io_uring ring_;
};

}  // namespace MA
#endif  // MEDIA_AGENT_SIMPLE_IO_EXECUTOR_HPP
