//
// Created by underthere on 2023/7/27.
//

#include <iostream>
#include <chrono>

extern "C" {
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavutil/avutil.h"
#include "libavutil/time.h"
}

#include "boost/signals2.hpp"

struct source_reader {
  AVFormatContext *fmt_ctx;
  AVCodecContext *codec_ctx;
  std::int8_t vs_index;

  boost::signals2::signal<void(AVPacket *)> new_packet;
};

struct output {
  AVFormatContext *output_ctx;

  auto on_new_packet(AVPacket *pkt) -> void {
//    std::cout << "new packet " << pkt->pts << std::endl;
    av_packet_unref(pkt);
  }
};

auto timebase2us(AVRational timebase) -> std::int64_t {
  if (timebase.den == 0 || timebase.num == 0) {
    return 0;
  }
  return timebase.num * 1e6 / timebase.den;
}

auto main() -> int {
  avformat_network_init();

  auto reader = source_reader{};
  int ret = avformat_open_input(&reader.fmt_ctx, "../../test.flv", nullptr, nullptr);
  if (ret < 0) {
    std::cout << "open input failed: " << av_err2str(ret) << std::endl;
    return 1;
  }

  ret = avformat_find_stream_info(reader.fmt_ctx, nullptr);
  if (ret < 0) {
    std::cout << "find stream info failed: " << av_err2str(ret) << std::endl;
    return 1;
  }

  auto video_index = av_find_best_stream(reader.fmt_ctx, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);
  if (video_index < 0) {
    std::cout << "find video stream failed: " << av_err2str(video_index) << std::endl;
    return 1;
  }

  reader.vs_index = video_index;

  output out;

//  ret = avformat_alloc_output_context2(&out.output_ctx, nullptr, "flv", "ttt.flv");
//  if (ret < 0) {
//    std::cout << "alloc output context failed: " << av_err2str(ret) << std::endl;
//    return 1;
//  }

  reader.new_packet.connect([&out](auto pkt){
    out.on_new_packet(pkt);
  });

  auto st = av_gettime();

  auto tb = reader.fmt_ctx->streams[reader.vs_index]->time_base;
  auto tb_unit = timebase2us(tb);

  do {
    auto pkt = av_packet_alloc();
    ret = av_read_frame(reader.fmt_ctx, pkt);
    if (ret < 0) {
      std::cout << "read frame failed: " << av_err2str(ret) << std::endl;
      av_packet_unref(pkt);
      break;
    }

    if (pkt->stream_index == reader.vs_index) {
      auto now = av_gettime() - st;
      auto dts = pkt->dts * tb_unit;
      if (dts > now) {
        std::cout << "sleep for " << (dts - now) / 1000 << "ms" << std::endl;
        av_usleep(dts - now);
      }
      reader.new_packet(pkt);
    } else {
      av_packet_free(&pkt);
    }
  } while (ret >= 0);

  avformat_network_deinit();
}
