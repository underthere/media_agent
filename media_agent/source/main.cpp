#include <memory>

#include "async_simple/coro/SyncAwait.h"
#include "cinatra.hpp"
#include "spdlog/spdlog.h"
#include "nlohmann/json.hpp"
#include "common/av_misc.hpp"

#include "media_agent_impl_ff.hpp"
#include "http_facade/http_facade.hpp"

using namespace std::chrono_literals;
using namespace async_simple;

auto async_main() -> coro::Lazy<int> {
  auto agent = std::make_shared<MA::MediaAgentImplFF>();
  agent->init();

  HttpFacade facade{agent};

  std::thread t([&facade](){
    facade.init();
    facade.start();
  });

  while (true) {
    co_await coro::sleep(100ms);
  }
}

int main() { return async_simple::coro::syncAwait(async_main()); }
