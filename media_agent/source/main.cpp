
#include <memory>
#include <iostream>

#include "media_common.hpp"
#include "media_reader.hpp"
#include "media_writer.hpp"

#include "boost/asio.hpp"

int main() {
  boost::asio::io_context ioc;
  MA::MediaDescription desc = {
      .uri = "/Users/chenlong/Documents/media_agent/test.flv",
  };
  MA::MediaDescription out_desc = {
      .protocol = MA::MediaProtocol::RTMP,
      .uri = "rtmp://dev.smt.dyinnovations.com/live/underthere",
      .video_description = MA::VideoDescription{
          .width = 1920,
          .height = 1080,
          .fps = 25,
          .pixel_format = MA::PixelFormat::YUV420P,
          .codec_format = MA::CodecFormat::H264,
      },
  };

  auto reader = std::make_shared<MA::MediaReader>(ioc, desc);
  auto writer = std::make_shared<MA::MediaWriter>(ioc, out_desc);

  reader->sig_codec_par_.connect([reader, writer](AVCodecParameters *par) {
    writer->set_codec_par(par);
    writer->start(reader->sig_new_packet_);
  });

  auto ret = reader->start();

  if (ret.has_value()) {
    std::cout << "start success" << std::endl;
    ioc.run();
  } else {
    std::cout << "start failed: " << ret.error().message << std::endl;
  }
  return 0;
}
