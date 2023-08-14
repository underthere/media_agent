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
template <typename Callback>
struct slot_impl;

template <typename Callback>
struct signal_impl : nocopy {
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
    slot_vec& list(*slots_);
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

template <typename Callback>
struct slot_impl : nocopy {
  using Data = signal_impl<Callback>;

  slot_impl(const std::shared_ptr<Data>& data, Callback&& callback) : data_(data), cb_(std::move(callback)), tie_(), tied_(false) {}

  ~slot_impl() {
    std::shared_ptr<Data> data(data_.lock());
    if (data) data->clean();
  }

  std::weak_ptr<Data> data_;
  Callback cb_;
  std::weak_ptr<void> tie_;
  bool tied_;
};
}  // namespace detail

using slot = std::shared_ptr<void>;

template <typename Signature>
class signal;

template <typename R, typename... Args>
class signal<R(Args...)> : nocopy {
 public:
  using Callback = std::function<R(Args...)>;
  using slot_type = slot_impl<Callback>;
  using signal_type = ;

  signal() : data_(new Data) {}

  auto connect(Callback&& callback) -> slot {
    std::lock_guard<std::mutex> lock(data_->mutex_);
    data_->cow();
    auto slot = std::make_shared<slot_type>(data_, std::move(callback));
    data_->slots_->push_back(slot);
    return slot;
  }

  auto operator()(Args... args) -> void {
    std::lock_guard<std::mutex> lock(data_->mutex_);
    data_->cow();
    slot_type* slot = nullptr;
    for (auto& weak_slot : *data_->slots_) {
      if (auto strong_slot = weak_slot.lock()) {
        if (slot) {
          slot->cb_(args...);
        } else {
          slot = strong_slot.get();
          slot->cb_(args...);
        }
      }
    }
  }

 private:
  using Data = signal_impl<Callback>;
  std::shared_ptr<Data> data_;
};

}  // namespace asignal

#endif  // MEDIA_AGENT_SIGNAL_HPP
