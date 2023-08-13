//
// Created by underthere on 2023/7/28.
//

#include "media_reader.hpp"

#include <utility>

#include "async_simple/coro/Sleep.h"
#include "av_misc.hpp"
#include "spdlog/spdlog.h"

using namespace async_simple;

namespace MA {

auto MediaReader::read() -> tl::expected<AVPacket *, Error> {
  static std::unordered_map<int, ErrorType> error_map{
      {AVERROR_EOF, ErrorType::EOS},
  };

  AVPacket *pkt = av_packet_alloc();
  int ret = av_read_frame(fctx_, pkt);
  if (ret < 0) {
    if (error_map.contains(ret)) {
      return tl::unexpected<Error>({error_map[ret], std::string("av_read_frame failed:") + std::string(av_err2str(ret))});
    }
    return tl::unexpected<Error>({ErrorType::UNKNOWN, std::string("av_read_frame failed:") + std::string(av_err2str(ret))});
  }
  return pkt;
}

auto MediaReader::run() -> coro::Lazy<tl::expected<void, Error>> {
  running = true;
  int ret = 0;
  while (running) {
    if (!media_opened_) {
      bool success = true;
      ret = avformat_open_input(&fctx_, desc_.uri.c_str(), nullptr, nullptr);
      if (ret != 0) {
        spdlog::warn("open {} failed:{}", desc_.uri, av_err2str(ret));
        success &= false;
      }

      ret = avformat_find_stream_info(fctx_, nullptr);
      if (ret < 0) {
        spdlog::warn("find stream info {} failed:{}", desc_.uri, av_err2str(ret));
        success &= false;
      } else {
        best_video_index_ = av_find_best_stream(fctx_, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);
        spdlog::info("{} find video stream index:{}", desc_.uri, best_video_index_);
        if (best_video_index_ < 0) {
          avformat_close_input(&fctx_);
          spdlog::warn("{} find video stream failed", desc_.uri);
          success &= false;
        }
      }
      media_opened_ = success;
      if (!media_opened_) co_await coro::sleep(retry_interval_);
      else start_time_ = av_gettime();
      continue;
    }
    auto result = read();
    if (result.has_value()) {
      auto now = av_gettime() - start_time_;
      auto pkt = result.value();
      spdlog::debug("read {} packet {}", desc_.uri, pkt->pts);
      if (pkt->dts == 0) pkt->dts = pkt->pts;
      auto dts = timebase2us(fctx_->streams[best_video_index_]->time_base) * pkt->dts;
      if (dts > now) {
        spdlog::trace("sleep for {} ms", (dts - now) / 1000);
        co_await coro::sleep(std::chrono::microseconds(dts - now));
      }
      sig_new_packet_(pkt, get_current_codec_par());
      av_packet_free(&pkt);
    } else {
      if (result.error().code == ErrorType::EOS) {
        spdlog::info("read {} eos", desc_.uri);
        avformat_close_input(&fctx_);
        media_opened_ = false;
      }
    }
  }
  avformat_close_input(&fctx_);
  media_opened_ = false;
  co_return tl::expected<void, Error>({});
}

MediaReader::~MediaReader() {
  if (fctx_ != nullptr) {
    avformat_close_input(&fctx_);
  }
}

MediaReader::MediaReader(MediaDescription desc) : desc_(std::move(desc)), fctx_(nullptr), best_video_index_(-1), start_time_(0) {}

auto MediaReader::get_current_codec_par() -> const AVCodecParameters * {
    if (fctx_ == nullptr || best_video_index_ < 0) return nullptr;
    return fctx_->streams[best_video_index_]->codecpar;
}

}  // namespace MA
