#include <memory>

#include "async_simple/coro/SyncAwait.h"
#include "cinatra.hpp"
#include "media_pod.hpp"
#include "spdlog/spdlog.h"
#include "http_facade/http_facade.hpp"
#include "transformers/basic_decoder.hpp"
#include "transformers/basic_encoder.hpp"

using namespace async_simple;

auto async_main() -> coro::Lazy<int> {
  spdlog::set_level(spdlog::level::trace);
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

  auto reader = std::make_shared<MA::MEDIA_READER_T>(desc, true);
  auto decoder = std::make_shared<MA::BasicDecoder>(desc, desc);
  auto encoder = std::make_shared<MA::BasicEncoder>(output_desc, output_desc);
  auto writer = std::make_shared<MA::MEDIA_WRITER_T>(output_desc);

  auto slot = reader->sig_new_packet_.connect([t = decoder.get()](auto &&buffer) { t->slot_new_packet(buffer); });

  auto slot0 = decoder->signal_new_frame.connect([t = encoder.get()](auto &&buffer) { t->slot_new_frame(buffer); });

  auto slot1 = encoder->signal_new_packet.connect([t = writer.get()](auto &&buffer) { t->slot_new_packet(buffer); });

  HttpFacade facade;

  std::thread t([&facade](){
    facade.init();
    facade.start();
  });

  co_await reader->run();
  t.join();

  co_return 0;
}

int main() { return async_simple::coro::syncAwait(async_main()); }
