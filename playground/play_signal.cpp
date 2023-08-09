//
// Created by underthere on 2023/7/27.
//

#include <iostream>
#include <thread>

extern "C" {
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavutil/avutil.h"
#include "libavutil/time.h"
}

#include "boost/signals2.hpp"

struct source_reader {
  AVFormatContext *fmt_ctx = nullptr;
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

  auto get_codecpar() const -> const AVCodecParameters * {
    if (!vstream) return nullptr;
    return vstream->codecpar;
  }

  auto read_all() -> void {
    while (true) {
      auto pkt = read_once();
      if (!pkt) {
        break;
      }
      new_packet(pkt);
    }
  }

  auto read_once() -> AVPacket * {
    auto pkt = av_packet_alloc();
    int ret = av_read_frame(fmt_ctx, pkt);
    if (ret < 0) {
      av_packet_free(&pkt);
      std::cout << "read frame failed: " << av_err2str(ret) << std::endl;
      return nullptr;
    }
    return pkt;
  }

  boost::signals2::signal<void(AVPacket *)> new_packet;
};

struct decoder {
  AVCodecContext *codec_ctx = nullptr;
  AVBufferRef *hw_device_ctx;

  int init(const std::string name, const AVCodecParameters *codecpar) {
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
    std::cout << "new packet" << std::endl;
    if (!avcodec_is_open(codec_ctx)) {
      int ret = avcodec_open2(codec_ctx, avcodec_find_decoder_by_name("h264_rkmpp"), nullptr);
      if (ret < 0) {
        std::cout << "open decoder failed: " << av_err2str(ret) << std::endl;
        return;
      }
    }
    int ret = avcodec_send_packet(codec_ctx, pkt);
    if (ret == AVERROR(EAGAIN)) return;
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
  AVCodecContext *codec_ctx = nullptr;
  AVBufferRef *hw_device_ctx;
  std::optional<AVCodecContext *> decoder;

  boost::signals2::signal<void(AVPacket *)> new_packet;

  int init(const std::string &name, const AVCodecParameters *codecpar) {
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
    if (codecpar) avcodec_parameters_to_context(codec_ctx, codecpar);
    codec_ctx->width = 1920;
    codec_ctx->height = 1080;
    codec_ctx->level = 41;
    codec_ctx->profile = FF_PROFILE_H264_BASELINE;
    codec_ctx->pix_fmt = AV_PIX_FMT_DRM_PRIME;
    codec_ctx->time_base = {1, 24};

    return 0;
  }

  void slot_new_frame(AVFrame *frame) {
    std::cout << "new frame" << std::endl;
    if (avcodec_is_open(codec_ctx) <= 0) {
      if (decoder.has_value()) {
        auto dec_ctx = decoder.value();
        if (dec_ctx->hw_frames_ctx) {
          codec_ctx->hw_frames_ctx = av_buffer_ref(dec_ctx->hw_frames_ctx);
        }
      }
      codec_ctx->pix_fmt = static_cast<AVPixelFormat>(frame->format);
      codec_ctx->width = frame->width;
      codec_ctx->height = frame->height;
      const auto encoder = avcodec_find_encoder_by_name("h264_rkmpp");
      int ret = avcodec_open2(codec_ctx, encoder, nullptr);
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
  AVFormatContext *output_ctx = nullptr;
  bool initialized = false;
  std::string uri_;
  AVCodecContext *codec_ctx = nullptr;
  AVCodecParameters *par = nullptr;

  int init(const std::string &uri, const std::string &format) {
    int ret;
    uri_ = uri;
    ret = avformat_alloc_output_context2(&output_ctx, nullptr, format.c_str(), uri.c_str());
    if (ret < 0) {
      std::cout << "alloc output context failed: " << av_err2str(ret) << std::endl;
      return -1;
    }
    return 0;
  }

  auto on_new_packet(AVPacket *pkt) -> void {
    int ret;
    if (!initialized) {
      if (!codec_ctx && !par) {
        std::cout << "no codec context" << std::endl;
        return;
      }

      auto stream = avformat_new_stream(output_ctx, nullptr);
      stream->time_base = {1, 24};
      if (codec_ctx) {
        avcodec_parameters_from_context(stream->codecpar, codec_ctx);
      } else {
        avcodec_parameters_copy(stream->codecpar, par);
      }
      stream->codecpar->codec_tag = 0;

      ret = avio_open(&output_ctx->pb, uri_.c_str(), AVIO_FLAG_WRITE);
      if (ret < 0) {
        std::cout << "open output failed: " << av_err2str(ret) << std::endl;
        return;
      }
      ret = avformat_write_header(output_ctx, nullptr);
      if (ret < 0) {
        std::cout << "write header failed: " << av_err2str(ret) << std::endl;
        return;
      }
    }
    pkt->stream_index = 0;
    ret = av_interleaved_write_frame(output_ctx, pkt);
    if (ret < 0) {
      std::cout << "write pkt failed:" << av_err2str(ret) << std::endl;
    }
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
  if (reader.open("/home/pi/workdir/media_agent/test.flv") < 0) {
    return 1;
  }

  decoder decoder;
  decoder.init("h264_rkmpp", reader.get_codecpar());

  encoder encoder;
  encoder.decoder = decoder.codec_ctx;
  encoder.init("h264_rkmpp", nullptr);

  output_writer writer;
  writer.init("test_out.flv", "flv");

  if (writer.output_ctx->oformat->flags & AVFMT_GLOBALHEADER) {
    encoder.codec_ctx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
  }
  writer.codec_ctx = encoder.codec_ctx;

  reader.new_packet.connect([&decoder](AVPacket *pkt) { decoder.slot_new_packet(pkt); });
  decoder.new_frame.connect([&encoder](AVFrame *frame) { encoder.slot_new_frame(frame); });
  encoder.new_packet.connect([&writer](AVPacket *pkt) { writer.on_new_packet(pkt); });

  std::thread t([&reader]() {
    while (true) {
      auto pkt = reader.read_once();
      if (pkt) {
        reader.new_packet(pkt);
      } else {
        break;
      }
    }
  });

  t.join();

  avformat_network_deinit();
}
