#include <memory>

#include "async_simple/coro/SyncAwait.h"
#include "cinatra.hpp"
#include "spdlog/spdlog.h"

#include "media_agent_impl_ff.hpp"
#include "http_facade/http_facade.hpp"

using namespace std::chrono_literals;
using namespace async_simple;

auto async_main() -> coro::Lazy<int> {
  spdlog::set_level(spdlog::level::debug);
  MA::MediaDescription desc = {.protocol = MA::MediaProtocol::FILE, .uri = "/workdir/test.flv"};
  MA::VideoDescription vdesc = {
      .width = 1920,
      .height = 1080,
      .fps = 25,
      .pixel_format = MA::PixelFormat::YUV420P,
      .codec_format = MA::CodecFormat::H264,
      .profile = MA::Profile::H264_BASELINE,
      .level = 0,
      .bitrate = 2000000,
  };
  MA::MediaDescription output_desc = {
      .protocol = MA::MediaProtocol::RTMP,
      .uri = "rtmp://dev.smt.dyinnovations.com/live/cltest",
      .video_description = vdesc,

  };

  auto agent = std::make_shared<MA::MediaAgentImplFF>();
  agent->init();

  MA::uuid_t src_id {"src001"}, out_id {"out001"};
  auto src_res = agent->add_source(desc, src_id);
  if (!src_res.has_value()) {
    spdlog::error("add source error: {}", src_res.error().message);
    co_return -1;
  }
  auto rest = agent->add_transform(src_id, output_desc, out_id);
  if (!rest.has_value()) {
    spdlog::error("add transform error: {}", rest.error().message);
    co_return -1;
  }

  HttpFacade facade;

  std::thread t([&facade](){
    facade.init();
    facade.start();
  });

  while (true) {
    co_await coro::sleep(100ms);
  }

  t.join();

  co_return 0;
}

int main() { return async_simple::coro::syncAwait(async_main()); }
