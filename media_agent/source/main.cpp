
#include <memory>

#include "async_simple/coro/SyncAwait.h"
#include "common/media_common.hpp"
#include "media_pod.hpp"
#include "spdlog/spdlog.h"

int main() {
  spdlog::set_level(spdlog::level::trace);
  MA::MediaDescription desc = {.protocol = MA::MediaProtocol::FILE, .uri = "/workdir/test.flv"};

  MA::MediaDescription output_desc = {
      .protocol = MA::MediaProtocol::RTMP,
      .uri = "rtmp://dev.smt.dyinnovations.com/live/test",
  };

  auto pod = std::make_shared<MA::MediaPod>("test", desc);
  pod->add_output("testout0", output_desc);

  async_simple::coro::syncAwait(pod->run());

  return 0;
}
