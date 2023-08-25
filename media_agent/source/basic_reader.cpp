//
// Created by underthere on 2023/7/28.
//

#include "readers/basic_reader.hpp"

#include <utility>

#include "async_simple/coro/Sleep.h"
#include "common/av_misc.hpp"
#include "spdlog/spdlog.h"

using namespace async_simple;

namespace MA {

auto BasicReader::read() -> tl::expected<void, Error> {
  static std::unordered_map<int, ErrorType> error_map{
      {AVERROR_EOF, ErrorType::EOS},
  };

  int ret = av_read_frame(fctx_, &readed_pkt_);
  if (ret < 0) {
    if (error_map.contains(ret)) {
      return tl::unexpected<Error>({error_map[ret], std::string("av_read_frame failed:") + std::string(av_err2str(ret))});
    }
    return tl::unexpected<Error>({ErrorType::UNKNOWN, std::string("av_read_frame failed:") + std::string(av_err2str(ret))});
  }
  return {};
}

auto BasicReader::run() -> coro::Lazy<tl::expected<void, Error>> {
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
      if (!media_opened_)
        co_await coro::sleep(retry_interval_);
      else
        start_time_ = av_gettime();
      continue;
    }
    auto result = read();
    if (result.has_value()) {
      auto now = av_gettime() - start_time_;
      auto pkt = &readed_pkt_;
      if (pkt->dts == 0) pkt->dts = pkt->pts;
      if (realtime_) {
        auto dts = timebase2us(fctx_->streams[best_video_index_]->time_base) * pkt->dts;
        if (dts > now) {
          spdlog::trace("sleep {} ms", (dts - now) / 1000);
          co_await coro::sleep(std::chrono::microseconds(dts - now));
        }
      }
      MediaBuffer buffer{
          .type = MediaBufferType::FF_PACKET,
          .data = pkt,
          .codec_par = fctx_->streams[best_video_index_]->codecpar,
          .codec_ctx = nullptr,
      };
      sig_new_packet_(buffer);
      av_packet_unref(pkt);
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

BasicReader::~BasicReader() {
  if (fctx_ != nullptr) {
    avformat_close_input(&fctx_);
  }
}

BasicReader::BasicReader(MediaDescription desc, bool realtime)
    : desc_(std::move(desc)), realtime_(realtime), fctx_(nullptr), best_video_index_(-1), start_time_(0) {}

auto BasicReader::get_current_codec_par() -> const AVCodecParameters* {
  if (fctx_ == nullptr || best_video_index_ < 0) return nullptr;
  return fctx_->streams[best_video_index_]->codecpar;
}

}  // namespace MA
