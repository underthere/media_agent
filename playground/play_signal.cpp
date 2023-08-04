//
// Created by underthere on 2023/7/27.
//

#include <chrono>
#include <iostream>

extern "C" {
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavutil/avutil.h"
#include "libavutil/time.h"
}

#include "boost/signals2.hpp"

struct source_reader {
  AVFormatContext *fmt_ctx;
  AVStream *vstream;
  std::int8_t vs_index;

  int open(const std::string &input_file) {
    //

    int ret;
    ret = avformat_open_input(&fmt_ctx, input_file.c_str(), nullptr, nullptr);
    if (!fmt_ctx || ret < 0) {
      std::cout << "open input failed: " << av_err2str(ret) << std::endl;
      return -1;
    }
    ret = avformat_find_stream_info(fmt_ctx, nullptr);
    if (ret < 0) {
      std::cout << "find stream info failed: " << av_err2str(ret) << std::endl;
      return -1;
    }
    vs_index = av_find_best_stream(fmt_ctx, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);
    if (vs_index < 0) {
      std::cout << "find video stream failed: " << av_err2str(vs_index) << std::endl;
      return -1;
    }
    vstream = fmt_ctx->streams[vs_index];
    return 0;
  }

  const AVCodecParameters *get_codecpar() const {
    if (!vstream) return nullptr;
    return vstream->codecpar;
  }

  boost::signals2::signal<void(AVPacket *)> new_packet;
};

struct decoder {
  AVCodecContext *codec_ctx;
  AVBufferRef *hw_device_ctx;

  int init(const std::string name, AVCodecParameters *codecpar) {
    const auto decoder = avcodec_find_decoder_by_name(name.c_str());
    if (!decoder) {
      std::cout << "find decoder failed" << std::endl;
      return -1;
    }
    codec_ctx = avcodec_alloc_context3(decoder);
    if (!codec_ctx) {
      std::cout << "alloc decoder context failed" << std::endl;
      return -1;
    }
    avcodec_parameters_to_context(codec_ctx, codecpar);
    int ret = avcodec_open2(codec_ctx, decoder, nullptr);
    if (ret < 0) {
      std::cout << "open decoder failed: " << av_err2str(ret) << std::endl;
      return -1;
    }

    return 0;
  }

  auto set_codecpar(const AVCodecParameters *par) -> void {
    if (!codec_ctx) {
      return;
    }
    avcodec_parameters_to_context(codec_ctx, par);
  }

  boost::signals2::signal<void(AVFrame *)> new_frame;
  void slot_new_packet(AVPacket *pkt) {
    int ret = avcodec_send_packet(codec_ctx, pkt);
    if (ret < 0) {
      std::cout << "send packet failed: " << av_err2str(ret) << std::endl;
      return;
    }

    auto frame = av_frame_alloc();
    ret = avcodec_receive_frame(codec_ctx, frame);
    if (ret < 0) {
      av_frame_free(&frame);
      std::cout << "receive frame failed: " << av_err2str(ret) << std::endl;
      return;
    }

    new_frame(frame);
  }
};

struct encoder {
  AVCodecContext *codec_ctx;
  AVBufferRef *hw_device_ctx;

  boost::signals2::signal<void(AVPacket *)> new_packet;

  int init(const std::string &name, AVCodecParameters *codecpar) {
    const auto encoder = avcodec_find_encoder_by_name(name.c_str());
    if (!encoder) {
      std::cout << "find encoder failed" << std::endl;
      return -1;
    }
    codec_ctx = avcodec_alloc_context3(encoder);
    if (!codec_ctx) {
      std::cout << "alloc encoder context failed" << std::endl;
      return -1;
    }
    avcodec_parameters_to_context(codec_ctx, codecpar);
    return 0;
  }

  void slot_new_frame(AVFrame *frame) {
    if (avcodec_is_open(codec_ctx) <= 0) {
      codec_ctx->pix_fmt = static_cast<AVPixelFormat>(frame->format);
      int ret = avcodec_open2(codec_ctx, nullptr, nullptr);
      if (ret < 0) {
        std::cout << "open encoder failed: " << av_err2str(ret) << std::endl;
        return;
      }
    }
    int ret = avcodec_send_frame(codec_ctx, frame);
    if (ret < 0) {
      std::cout << "send frame failed: " << av_err2str(ret) << std::endl;
      return;
    }

    auto pkt = av_packet_alloc();
    ret = avcodec_receive_packet(codec_ctx, pkt);
    if (ret < 0) {
      av_packet_free(&pkt);
      std::cout << "receive packet failed: " << av_err2str(ret) << std::endl;
      return;
    }

    new_packet(pkt);
  }
};

struct output_writer {
  AVFormatContext *output_ctx;

  int init(const std::string &uri, const std::string &format) {
    int ret;
    ret = avformat_alloc_output_context2(&output_ctx, nullptr, format.c_str(), uri.c_str());
    if (ret < 0) {
      std::cout << "alloc output context failed: " << av_err2str(ret) << std::endl;
      return -1;
    }
    return 0;
  }

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

  source_reader reader;
  if (reader.open("../../test.flv") < 0) {
    return 1;
  }



  avformat_network_deinit();
}
