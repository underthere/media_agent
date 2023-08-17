
#include <memory>

#include "async_simple/coro/SyncAwait.h"
#include "common/media_common.hpp"
#include "media_pod.hpp"
#include "spdlog/spdlog.h"
#include "transformers/basic_decoder.hpp"
#include "transformers/basic_encoder.hpp"

int main() {
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
      .uri = "rtmp://dev.smt.dyinnovations.com/live/test",
      .video_description = vdesc,

  };

  auto reader = std::make_shared<MA::MEDIA_READER_T>(desc, true);
  auto decoder = std::make_shared<MA::BasicDecoder>(desc, desc);
  auto encoder = std::make_shared<MA::BasicEncoder>(output_desc, output_desc);
  auto writer = std::make_shared<MA::MEDIA_WRITER_T>(output_desc);

  auto slot = reader->sig_new_packet_.connect([t = decoder.get()](auto &&PH1, auto &&PH2) {
    t->slot_new_packet(std::forward<decltype(PH1)>(PH1), std::forward<decltype(PH2)>(PH2));
  });

  auto slot0 = decoder->signal_new_frame.connect([t = encoder.get()](auto &&PH1, auto &&PH2) {
    t->slot_new_frame(std::forward<decltype(PH1)>(PH1), std::forward<decltype(PH2)>(PH2));
  });

  auto slot1 = encoder->signal_new_packet.connect([t = writer.get()](auto &&PH1, auto &&PH2) {
    t->slot_new_packet(std::forward<decltype(PH1)>(PH1), std::forward<decltype(PH2)>(PH2));
  });

  async_simple::coro::syncAwait(reader->run());

  // auto pod = std::make_shared<MA::MediaPod>("test", desc);
  // pod->add_output("testout0", output_desc);

  // async_simple::coro::syncAwait(pod->run());

  return 0;
}
