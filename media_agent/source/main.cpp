
#include <memory>


#include "async_simple/coro/SyncAwait.h"
#include "spdlog/spdlog.h"

#include "media_common.hpp"
#include "media_reader.hpp"
#include "media_writer.hpp"

#include "boost/asio.hpp"

int main() {
  spdlog::set_level(spdlog::level::trace);
  MA::MediaDescription desc = {
      .uri = "/Users/chenlong/Documents/media_agent/test.flv",
  };

  auto reader = std::make_shared<MA::MediaReader>(desc);

  async_simple::coro::syncAwait(reader->run());

  return 0;
}
