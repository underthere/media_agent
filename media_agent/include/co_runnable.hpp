//
// Created by underthere on 2023/8/12.
//

#ifndef MEDIA_AGENT_CO_RUNNABLE_HPP
#define MEDIA_AGENT_CO_RUNNABLE_HPP

#include "tl/expected.hpp"
#include "async_simple/coro/Lazy.h"

using namespace async_simple;

namespace MA {

template <typename T, typename E>
class CoRunnable {
    public:
    virtual ~CoRunnable() = default;
    virtual auto run() -> coro::Lazy<tl::expected<T, E>> = 0;
};

}  // namespace MA

#endif  // MEDIA_AGENT_CO_RUNNABLE_HPP
