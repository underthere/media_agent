
#include <memory>


#include "async_simple/coro/SyncAwait.h"
#include "spdlog/spdlog.h"

#include "media_common.hpp"
#include "media_reader.hpp"
#include "media_writer.hpp"
#include "media_pod.hpp"

#include "boost/asio.hpp"

int main() {
  spdlog::set_level(spdlog::level::trace);
  MA::MediaDescription desc = {
      .uri = "/Users/chenlong/Documents/media_agent/test.flv",
      .protocol = MA::MediaProtocol::FILE
  };

  MA::MediaDescription output_desc = {
      .uri = "rtmp://dev.smt.dyinnovations.com/live/test",
      .protocol = MA::MediaProtocol::RTMP
  };

  auto pod = std::make_shared<MA::MediaPod>("test", desc);
  pod->add_output("testout0", output_desc);

  async_simple::coro::syncAwait(pod->run());

  return 0;
}
