//
// Created by underthere on 2023/8/9.
//

#include <chrono>
#include <thread>
extern "C" {
#include "libavcodec/avcodec.h"
#include "libavcodec/codec.h"
#include "libavcodec/packet.h"
#include "libavformat/avformat.h"
#include "libavutil/avutil.h"
#include "libavutil/buffer.h"
#include "libavutil/frame.h"
#include "libavutil/hwcontext.h"
#include "libavutil/time.h"
}
#include <cstdio>

#include <memory>
#include <unordered_map>

#include "fmt/format.h"

#include "tl/expected.hpp"

#include "ringbuffer.hpp"
#include "safe_queue.hpp"

struct Stuff {
  AVFormatContext *in_fmt_ctx;
  AVFormatContext *out_fmt_ctx;
  AVOutputFormat *out_fmt;
  AVCodecContext *dec_ctx;
  AVCodecContext *enc_ctx;
  AVBufferRef *hw_device_ctx;
  AVStream *output_stream;
  std::size_t v_index;
  const AVCodec *decoder;
  const AVCodec *encoder;

  safe_queue<AVPacket *> input_packet_queue;
  // ring_buffer<AVFrame, 25, av_frame_alloc, av_frame_free>
  // internal_frame_queue; ring_buffer<AVPacket, 25, av_packet_alloc,
  // av_packet_free> output_packet_queue;
  safe_queue<AVFrame *> internal_frame_queue;
  safe_queue<AVPacket *> output_packet_queue;

  ~Stuff() {
    if (in_fmt_ctx) {
      avformat_close_input(&in_fmt_ctx);
    }
    if (out_fmt_ctx) {
      avformat_free_context(out_fmt_ctx);
    }
    if (hw_device_ctx) {
      av_buffer_unref(&hw_device_ctx);
    }
    if (dec_ctx) {
      avcodec_free_context(&dec_ctx);
    }
    if (enc_ctx) {
      avcodec_free_context(&enc_ctx);
    }
  }
};

enum class ErrorType {
  Unkown,
  EOS,
};

struct Error {
  ErrorType type;
  std::string msg;
};

auto open_input_file(std::shared_ptr<Stuff> stuff, const std::string &filepath)
    -> tl::expected<void, Error>;

auto open_output(std::shared_ptr<Stuff> stuff, const std::string &uri)
    -> tl::expected<void, Error>;

auto open_hw_device(std::shared_ptr<Stuff> stuff, const std::string &dev_name)
    -> tl::expected<void, Error>;

auto read_write_frame(std::shared_ptr<Stuff> stuff)
    -> tl::expected<void, Error>;

inline auto read_input_packet(std::shared_ptr<Stuff> stuff)
    -> tl::expected<AVPacket *, Error>;

auto just_process(std::shared_ptr<Stuff> stuff) -> void;

int main(int argc, char *argv[]) {
  avformat_network_init();

  std::string file_path{"/workdir/test.flv"};

  auto stuff = std::make_shared<Stuff>();
  auto res = open_input_file(stuff, file_path);
  if (!res.has_value()) {
    std::cout << res.error().msg << std::endl;
    exit(1);
  }

  res = open_output(stuff, "rtmp://120.24.65.63:1935/live/cltest");
  if (!res.has_value()) {
    std::cout << res.error().msg << std::endl;
    exit(1);
  }

  just_process(stuff);

  return 0;
}

auto r2d(AVRational r) -> double {
  return r.num == 0 || r.den == 0 ? 0. : static_cast<double>(r.num) / r.den;
}

static enum AVPixelFormat get_vaapi_format(AVCodecContext *ctx,
                                           const enum AVPixelFormat *pix_fmts) {
  const enum AVPixelFormat *p;

  for (p = pix_fmts; *p != AV_PIX_FMT_NONE; p++) {
    if (*p == AV_PIX_FMT_DRM_PRIME)
      return *p;
  }

  fprintf(stderr, "Unable to decode this file using VA-API.\n");
  return AV_PIX_FMT_NONE;
}

auto open_input_file(std::shared_ptr<Stuff> stuff, const std::string &filepath)
    -> tl::expected<void, Error> {
  int ret = avformat_open_input(&stuff->in_fmt_ctx, filepath.c_str(), nullptr,
                                nullptr);
  if (ret < 0) {
    std::string av_err{av_err2str(ret)};
    return tl::make_unexpected(
        Error{ErrorType::Unkown,
              fmt::format("avformat_open_input failed: {}", av_err)});
  }
  ret = avformat_find_stream_info(stuff->in_fmt_ctx, nullptr);
  if (ret < 0) {
    std::string av_err{av_err2str(ret)};
    return tl::make_unexpected(
        Error{ErrorType::Unkown,
              fmt::format("avformat_find_stream_info failed: {}", av_err)});
  }

  ret = av_find_best_stream(stuff->in_fmt_ctx, AVMEDIA_TYPE_VIDEO, -1, -1,
                            nullptr, 0);
  if (ret < 0) {
    std::string av_err{"video stream not found"};
    return tl::make_unexpected(
        Error{ErrorType::Unkown,
              fmt::format("av_find_best_stream failed: {}", av_err)});
  }
  stuff->decoder = avcodec_find_decoder_by_name("h264_rkmpp");
  stuff->v_index = ret;
  return {};
}

auto open_output(std::shared_ptr<Stuff> stuff, const std::string &url)
    -> tl::expected<void, Error> {
  int ret = avformat_alloc_output_context2(&stuff->out_fmt_ctx, nullptr, "flv",
                                           url.c_str());
  if (ret < 0) {
    std::string av_err{av_err2str(ret)};
    return tl::make_unexpected(Error{
        ErrorType::Unkown,
        fmt::format("avformat_alloc_output_context2 failed: {}", av_err)});
  }
  // stuff->output_stream = avformat_new_stream(stuff->out_fmt_ctx, nullptr);
  // if (!stuff->output_stream) {
  //   std::string av_err{"avformat_new_stream failed"};
  //   return tl::make_unexpected(
  //       Error{ErrorType::Unkown,
  //             fmt::format("avformat_new_stream failed: {}", av_err)});
  // }

  // ret = avcodec_parameters_copy(
  //     stuff->output_stream->codecpar,
  //     stuff->in_fmt_ctx->streams[stuff->v_index]->codecpar);
  // if (ret < 0) {
  //   std::string av_err{av_err2str(ret)};
  //   return tl::make_unexpected(
  //       Error{ErrorType::Unkown,
  //             fmt::format("avcodec_parameters_copy failed: {}", av_err)});
  // }
  // stuff->output_stream->codecpar->codec_tag = 0;
  // stuff->output_stream->time_base =
  //     stuff->in_fmt_ctx->streams[stuff->v_index]->time_base;
  // ret = avio_open(&stuff->out_fmt_ctx->pb, url.c_str(), AVIO_FLAG_WRITE);
  // if (ret < 0) {
  //   std::string av_err{av_err2str(ret)};
  //   return tl::make_unexpected(
  //       Error{ErrorType::Unkown, fmt::format("avio_open failed: {}",
  //       av_err)});
  // }
  // ret = avformat_write_header(stuff->out_fmt_ctx, nullptr);
  // if (ret < 0) {
  //   std::string av_err{av_err2str(ret)};
  //   return tl::make_unexpected(
  //       Error{ErrorType::Unkown,
  //             fmt::format("avformat_write_header failed: {}", av_err)});
  // }
  return {};
}

inline auto read_write_frame(std::shared_ptr<Stuff> stuff)
    -> tl::expected<void, Error> {
  AVPacket *pkt = av_packet_alloc();
  int ret = av_read_frame(stuff->in_fmt_ctx, pkt);
  if (ret < 0) {
    std::string av_err{av_err2str(ret)};
    return tl::make_unexpected(Error{
        ErrorType::Unkown, fmt::format("av_read_frame failed: {}", av_err)});
  }
  std::cout << fmt::format("read frame: pts: {}, dts: {}", pkt->pts, pkt->dts)
            << std::endl;

  pkt->stream_index = stuff->output_stream->index;
  ret = av_interleaved_write_frame(stuff->out_fmt_ctx, pkt);
  if (ret < 0) {
    std::string av_err{av_err2str(ret)};
    return tl::make_unexpected(
        Error{ErrorType::Unkown,
              fmt::format("av_interleaved_write_frame failed: {}", av_err)});
  }
  av_packet_unref(pkt);
  return {};
}

inline auto read_input_packet(std::shared_ptr<Stuff> stuff)
    -> tl::expected<AVPacket *, Error> {
  AVPacket *pkt = av_packet_alloc();
  int ret = av_read_frame(stuff->in_fmt_ctx, pkt);
  if (ret == AVERROR_EOF) {
    return tl::make_unexpected(
        Error{ErrorType::EOS, fmt::format("av_read_frame failed: {}", "EOF")});
  }
  if (ret < 0) {
    std::string av_err{av_err2str(ret)};
    return tl::make_unexpected(Error{
        ErrorType::Unkown, fmt::format("av_read_frame failed: {}", av_err)});
  }
  return pkt;
}

auto open_hw_device(std::shared_ptr<Stuff> stuff,
                    const std::string &device_name)
    -> tl::expected<void, Error> {

  static std::unordered_map<std::string, std::int64_t> device_type_map{
      {"vaapi", AV_HWDEVICE_TYPE_VAAPI},
      {"dxva2", AV_HWDEVICE_TYPE_DXVA2},
      {"d3d11va", AV_HWDEVICE_TYPE_D3D11VA},
      {"qsv", AV_HWDEVICE_TYPE_QSV},
      {"cuda", AV_HWDEVICE_TYPE_CUDA},
      {"vdpau", AV_HWDEVICE_TYPE_VDPAU},
      {"videotoolbox", AV_HWDEVICE_TYPE_VIDEOTOOLBOX},
      {"drm", AV_HWDEVICE_TYPE_DRM},
      {"opencl", AV_HWDEVICE_TYPE_OPENCL},
      {"mediacodec", AV_HWDEVICE_TYPE_MEDIACODEC},
  };

  auto it = device_type_map.find(device_name);
  if (it == device_type_map.end()) {
    return tl::make_unexpected(
        Error{ErrorType::Unkown,
              fmt::format("device type not found: {}", device_name)});
  }

  AVHWDeviceType type = static_cast<AVHWDeviceType>(it->second);
  int ret =
      av_hwdevice_ctx_create(&stuff->hw_device_ctx, type, nullptr, nullptr, 0);
  if (ret < 0) {
    std::string av_err{av_err2str(ret)};
    return tl::make_unexpected(
        Error{ErrorType::Unkown,
              fmt::format("av_hwdevice_ctx_create failed: {}", av_err)});
  }
  return {};
}

auto fn_read_input_thread(std::shared_ptr<Stuff> stuff) {
  int ret;
  std::size_t cnt = 0;
  while (true) {
    auto pkt = av_packet_alloc();
    ret = av_read_frame(stuff->in_fmt_ctx, pkt);
    std::cout << "read source source: " << ++cnt << std::endl;
    if (ret < 0) {
      std::string av_err{av_err2str(ret)};
      std::cout << fmt::format("av_read_frame failed: {}", av_err) << std::endl;
      av_packet_free(&pkt);
      break;
    }
    stuff->input_packet_queue.push(pkt);
  }
}

auto fn_consume_input_thread(std::shared_ptr<Stuff> stuff) {
  int ret;
  stuff->dec_ctx = avcodec_alloc_context3(stuff->decoder);
  if (!stuff->dec_ctx) {
    std::cout << "avcodec_alloc_context3 failed" << std::endl;
    return;
  }

  ret = avcodec_parameters_to_context(
      stuff->dec_ctx, stuff->in_fmt_ctx->streams[stuff->v_index]->codecpar);
  // stuff->dec_ctx->hw_device_ctx = av_buffer_ref(stuff->hw_device_ctx);
  if (ret < 0) {
    std::string av_err{av_err2str(ret)};
    std::cout << fmt::format("avcodec_parameters_to_context failed: {}", av_err)
              << std::endl;
    return;
  }

  // stuff->dec_ctx->get_format = ge;
  stuff->dec_ctx->pix_fmt = AV_PIX_FMT_DRM_PRIME;

  ret = avcodec_open2(stuff->dec_ctx, stuff->decoder, nullptr);
  if (ret < 0) {
    std::string av_err{av_err2str(ret)};
    std::cout << fmt::format("avcodec_open2 failed: {}", av_err) << std::endl;
    return;
  }

  stuff->encoder = avcodec_find_encoder_by_name("h264_rkmpp");
  if (!stuff->encoder) {
    std::cout << "avcodec_find_encoder_by_name failed" << std::endl;
    return;
  }

  if (!(stuff->enc_ctx = avcodec_alloc_context3(stuff->encoder))) {
    std::cout << "avcodec_alloc_context3 failed" << std::endl;
    return;
  }
  auto st = av_gettime();

  std::size_t cnt = 0;

  bool encoder_init = false;
  // AVFrame* frame = av_frame_alloc();
  while (true) {
    AVFrame *frame = av_frame_alloc();
    // auto &frame = stuff->internal_frame_queue.push();
    do {
      auto pkt = stuff->input_packet_queue.pop();
      if (pkt->stream_index != stuff->v_index) {
        continue;
      }

      std::cout << "pop input packet size: " << pkt->size << std::endl;

      if ((ret = avcodec_send_packet(stuff->dec_ctx, pkt)) < 0) {
        if (ret == AVERROR(EAGAIN)) {
          continue;
        }
        std::string av_err{av_err2str(ret)};
        std::cout << fmt::format("avcodec_send_packet failed: {}: {}", ret,
                                 av_err)
                  << std::endl;
        return;
      }
      av_packet_free(&pkt);
      if ((ret = avcodec_receive_frame(stuff->dec_ctx, frame)) < 0) {
        if (ret == AVERROR(EAGAIN)) {
          continue;
        }

        std::string av_err{av_err2str(ret)};
        std::cout << fmt::format("avcodec_receive_frame failed: {}", av_err)
                  << std::endl;
        return;
      }
      frame->duration = 40;
      if (frame->pts < 0)
        frame->pts = 0;
      std::cout << fmt::format("decode frame dec_hw_ctx: {} hw_ctx: {}, pts: {}, duration: {}", (void*)stuff->dec_ctx->hw_frames_ctx, (void*)frame->hw_frames_ctx, frame->pts, frame->duration);

      if (!encoder_init) {
        stuff->enc_ctx->hw_frames_ctx = av_buffer_ref(frame->hw_frames_ctx);
        stuff->enc_ctx->width = stuff->dec_ctx->width;
        stuff->enc_ctx->height = stuff->dec_ctx->height;
        stuff->enc_ctx->pix_fmt = (AVPixelFormat)frame->format;
        stuff->enc_ctx->profile = FF_PROFILE_H264_BASELINE;
        stuff->enc_ctx->level = 41;
        stuff->enc_ctx->bit_rate = 2 * 1000 * 1000;
        stuff->enc_ctx->time_base = {1, 25};
        // stuff->enc_ctx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
        if (stuff->out_fmt_ctx->oformat->flags & AVFMT_GLOBALHEADER) {
          stuff->enc_ctx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
        }

        if ((ret = avcodec_open2(stuff->enc_ctx, stuff->encoder, nullptr)) <
            0) {
          std::string av_err{av_err2str(ret)};
          std::cout << fmt::format("avcodec_open2 failed: {}", av_err)
                    << std::endl;
          return;
        }

        if (!(stuff->output_stream =
                  avformat_new_stream(stuff->out_fmt_ctx, nullptr))) {
          std::cout << "avformat_new_stream failed" << std::endl;
          return;
        }
        stuff->output_stream->time_base = stuff->enc_ctx->time_base;
        ret = avcodec_parameters_from_context(stuff->output_stream->codecpar,
                                              stuff->enc_ctx);
        if (ret < 0) {
          std::string av_err{av_err2str(ret)};
          std::cout << fmt::format("avcodec_parameters_from_context failed:{}",
                                   av_err)
                    << std::endl;
          return;
        }

        stuff->output_stream->codecpar->codec_tag = 0;
        ret =
            avio_open(&stuff->out_fmt_ctx->pb,
                      "rtmp://120.24.65.63:1935/live/cltest", AVIO_FLAG_WRITE);
        if (ret < 0) {
          std::string av_err{av_err2str(ret)};
          std::cout << fmt::format("avio_open failed: {}", av_err) << std::endl;
          return;
        }

        if ((ret = avformat_write_header(stuff->out_fmt_ctx, nullptr)) < 0) {
          std::string av_err{av_err2str(ret)};
          std::cout << fmt::format("avformat_write_header failed: {}", av_err)
                    << std::endl;
          return;
        }

        encoder_init = true;
      }
      // std::cout << "encoded frame pts: " << frame->pts << " duration: " <<
      // frame->duration << std::endl;
      ret = avcodec_send_frame(stuff->enc_ctx, frame);
      if (ret < 0) {
        std::string av_err{av_err2str(ret)};
        std::cout << fmt::format("avcodec_send_frame failed: {}", av_err)
                  << std::endl;
        return;
      }
      AVPacket pkt_out;
      ret = avcodec_receive_packet(stuff->enc_ctx, &pkt_out);
      if (ret == AVERROR(EAGAIN)) {
        continue;
      }
      if (ret < 0) {
        std::string av_err{av_err2str(ret)};
        std::cout << fmt::format("avcodec_receive_packet failed: {}", av_err)
                  << std::endl;
        return;
      }

      pkt_out.stream_index = stuff->output_stream->index;
      av_packet_rescale_ts(
          &pkt_out, stuff->in_fmt_ctx->streams[stuff->v_index]->time_base,
          stuff->output_stream->time_base);
      pkt_out.pos = -1;
      auto otb = stuff->output_stream->time_base;
      auto now = av_gettime() - st;
      auto dts =
          static_cast<std::int64_t>(pkt_out.dts * (1000 * 1000 * r2d(otb)));
      if (dts > now) {
        std::cout << fmt::format("sleep for {} ms", (dts - now) / 1000)
                  << std::endl;
        std::this_thread::sleep_for(std::chrono::microseconds(dts - now));
      }
      std::cout << fmt::format("receive reencode packet: pts: {}, dts: {}, "
                       "duration: {}, size: {}",
                       pkt_out.pts, pkt_out.dts, pkt_out.duration,
                       pkt_out.size)
                << std::endl;
      ret = av_interleaved_write_frame(stuff->out_fmt_ctx, &pkt_out);
      if (ret < 0) {
        std::string av_err{av_err2str(ret)};
        std::cout << fmt::format("av_interleaved_write_frame failed: {}",
                                 av_err)
                  << std::endl;
        continue;
      }

      std::cout << "receive frame: " << ++cnt << std::endl;
      break;
    } while (true);
    av_frame_unref(frame);
  }
}

auto fn_internal_encode_thread(std::shared_ptr<Stuff> stuff) {
  int ret;

  // stuff->encoder = avcodec_find_encoder_by_name("h264_vaapi");
  // if (!stuff->encoder) {
  // std::cout << "avcodec_find_encoder_by_name failed" << std::endl;
  // return;
  // }

  // if (!(stuff->enc_ctx = avcodec_alloc_context3(stuff->encoder))) {
  // std::cout << "avcodec_alloc_context3 failed" << std::endl;
  // return;
  // }

  bool encoder_init = false;

  while (true) {
    // auto &pkt = stuff->output_packet_queue.push();
    do {
      auto frame = stuff->internal_frame_queue.pop();
      // if (!encoder_init) {
      //   stuff->enc_ctx->hw_frames_ctx =
      //       av_buffer_ref(stuff->dec_ctx->hw_frames_ctx);
      //   stuff->enc_ctx->width = stuff->dec_ctx->width;
      //   stuff->enc_ctx->height = stuff->dec_ctx->height;
      //   stuff->enc_ctx->pix_fmt = AV_PIX_FMT_VAAPI;
      // stuff->enc_ctx->profile = FF_PROFILE_H264_CONSTRAINED_BASELINE;
      //   stuff->enc_ctx->bit_rate = 2 * 1000 * 1000;
      //   stuff->enc_ctx->time_base = stuff->dec_ctx->time_base;
      //   if ((ret = avcodec_open2(stuff->enc_ctx, stuff->encoder, nullptr)) <
      //       0) {
      //     std::string av_err{av_err2str(ret)};
      //     std::cout << fmt::format("avcodec_open2 failed: {}", av_err)
      //               << std::endl;
      //     return;
      //   }
      //   encoder_init = true;
      // }
      // ret = avcodec_send_frame(stuff->enc_ctx, &frame);
      // if (ret < 0) {
      //   std::string av_err{av_err2str(ret)};
      //   std::cout << fmt::format("avcodec_send_frame failed: {}", av_err)
      //             << std::endl;
      //   return;
      // }
      // ret = avcodec_receive_packet(stuff->enc_ctx, &pkt);
      // if (ret == AVERROR(EAGAIN)) {
      //   continue;
      // }
      // if (ret < 0) {
      //   std::string av_err{av_err2str(ret)};
      //   std::cout << fmt::format("avcodec_receive_packet failed: {}", av_err)
      //             << std::endl;
      //   return;
      // }
      break;
    } while (true);
  }
}

auto fn_output_thread(std::shared_ptr<Stuff> stuff) {
  // int ret;
  // auto st = av_gettime();
  // auto etb = stuff->enc_ctx->time_base;
  // auto otb = stuff->out_fmt_ctx->streams[0]->time_base;
  // while (true) {
  //   auto &pkt = stuff->output_packet_queue.pop();
  //   pkt.stream_index = stuff->output_stream->index;
  //   pkt.pts = av_rescale_q_rnd(pkt.pts, etb, otb, AV_ROUND_NEAR_INF);
  //   pkt.dts = av_rescale_q_rnd(pkt.dts, etb, otb, AV_ROUND_NEAR_INF);
  //   pkt.duration = av_rescale_q_rnd(pkt.duration, etb, otb,
  //   AV_ROUND_NEAR_INF); pkt.pos = -1;

  //   auto now = av_gettime() - st;
  //   auto dts = pkt.dts * (1000 * 1000 * r2d(etb));

  //   ret = av_interleaved_write_frame(stuff->out_fmt_ctx, &pkt);
  //   if (ret < 0) {
  //     std::string av_err{av_err2str(ret)};
  //     std::cout << fmt::format("av_interleaved_write_frame failed: {}",
  //     av_err)
  //               << std::endl;
  //     return;
  //   }
  // }
}

auto just_process(std::shared_ptr<Stuff> stuff) -> void {
  std::thread t1{fn_read_input_thread, stuff};
  std::thread t2{fn_consume_input_thread, stuff};
  // std::thread t3{fn_internal_encode_thread, stuff};
  // std::thread t4{fn_output_thread, stuff};
  t1.join();
  t2.join();
  // t3.join();
  // t4.join();
}
