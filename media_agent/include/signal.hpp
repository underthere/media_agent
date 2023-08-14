//
// Created by underthere on 2023/8/14.
//

#ifndef MEDIA_AGENT_SIGNAL_HPP
#define MEDIA_AGENT_SIGNAL_HPP

#include <cassert>
#include <functional>
#include <memory>
#include <mutex>
#include <vector>

#include "nocopy.hpp"

namespace asignal {

namespace detail {
template <typename Callback> struct slot_impl;

template <typename Callback> struct signal_impl : nocopy {
  using slot_vec = std::vector<std::weak_ptr<slot_impl<Callback>>>;

  signal_impl() : slots_(new slot_vec) {}
  auto cow() {
    if (!slots_.unique()) {
      slots_.reset(new slot_vec(*slots_));
    }
    assert(slots_.unique());
  }

  auto clean() {
    std::lock_guard<std::mutex> lock(mutex_);
    cow();
    slot_vec &list(*slots_);
    auto it = list.begin();
    while (it != list.end()) {
      if (it->expired()) {
        it = list.erase(it);
      } else {
        ++it;
      }
    }
  }

  std::mutex mutex_;
  std::shared_ptr<slot_vec> slots_;
};

template <typename Callback> struct slot_impl : nocopy {
  using Data = signal_impl<Callback>;

  slot_impl(const std::shared_ptr<Data> &data, Callback &&callback)
      : data_(data), cb_(std::move(callback)), tie_(), tied_(false) {}

  slot_impl(const std::shared_ptr<Data> &data, Callback &&callback,
            const std::shared_ptr<void> &tie)
      : data_(data), cb_(std::move(callback)), tie_(tie), tied_(true) {}

  ~slot_impl() {
    std::shared_ptr<Data> data(data_.lock());
    if (data)
      data->clean();
  }

  std::weak_ptr<Data> data_;
  Callback cb_;
  std::weak_ptr<void> tie_;
  bool tied_;
};
} // namespace detail

using slot = std::shared_ptr<void>;

template <typename Signature> class signal;

template <typename R, typename... Args> class signal<R(Args...)> : nocopy {
public:
  using Callback = std::function<R(Args...)>;
  using slot_type = detail::slot_impl<Callback>;
  using signal_type = detail::signal_impl<Callback>;

  signal() : signal_(new signal_type) {}
  ~signal() {}

  auto connect(Callback &&func) -> slot {
    std::shared_ptr<slot_type> slot(new slot_type(signal_, std::move(func)));
    add(slot);
    return slot;
  }

  auto call(Args &&...args) {
    signal_type &signal(*signal_);
    std::shared_ptr<typename signal_type::slot_vec> slots;
    {
      std::lock_guard<std::mutex> lock(signal.mutex_);
      slots = signal.slots_;
    }
    typename signal_type::slot_vec &list(*slots);
    for (typename signal_type::slot_vec::const_iterator it = list.begin();
         it != list.end(); ++it) {
      std::shared_ptr<slot_type> slot(it->lock());
      if (slot) {
        std::shared_ptr<void> guard;
        if (slot->tied_) {
          guard = slot->tie_.lock();
          if (guard) {
            slot->cb_(std::forward<Args>(args)...);
          }
        } else {
          slot->cb_(std::forward<Args>(args)...);
        }
      }
    }
  }

private:
  auto add(const std::shared_ptr<slot_type> &slot) {
    signal_type &signal(*signal_);
    {
      std::lock_guard<std::mutex> lock(signal.mutex_);
      signal.cow();
      signal.slots_->push_back(slot);
    }
  }

  const std::shared_ptr<signal_type> signal_;
};

} // namespace asignal

#endif // MEDIA_AGENT_SIGNAL_HPP
