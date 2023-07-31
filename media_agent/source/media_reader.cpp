//
// Created by underthere on 2023/7/28.
//


#include <utility>

#include "av_misc.hpp"
#include "media_reader.hpp"
#include <iostream>

namespace MA {

auto MediaReader::read() -> tl::expected<AVPacket *, Error> {
  AVPacket *pkt = av_packet_alloc();
  int ret = av_read_frame(fctx_, pkt);
  if (ret < 0) {
    av_packet_free(&pkt);
    return tl::unexpected<Error>({0, std::string("av_read_frame failed:") + std::string(av_err2str(ret))});
  }
  return pkt;
}

auto MediaReader::read_handler(AVPacket *delayed_pkt) -> void {
  if (delayed_pkt != nullptr) {
    sig_new_packet_(delayed_pkt);
  }
  auto pkt_result = read();
  if (!pkt_result.has_value()) {
    std::cout << "read failed: " << pkt_result.error().message << std::endl;
    return;
  }
  auto pkt = pkt_result.value();
  if (pkt->stream_index != best_video_index_) {
    av_packet_free(&pkt);
    read_handler();
  }
  auto now = av_gettime() - start_time_;
  if (pkt->dts == 0) pkt->dts = pkt->pts;
  auto dts = timebase2us(fctx_->streams[best_video_index_]->time_base) * pkt->dts;
  if (dts > now) {
    std::cout << "sleep for " << (dts - now) / 1000 << "ms" << std::endl;
    timer_.expires_after(std::chrono::microseconds(dts - now));
    timer_.async_wait([this, pkt](const boost::system::error_code &ec) {
      if (ec) {
        std::cout << "timer error: " << ec.message() << std::endl;
        return;
      }
      read_handler(pkt);
    });
  } else {
    read_handler(pkt);
  }
}

auto MediaReader::start() -> tl::expected<void, Error> {
  int ret = avformat_open_input(&fctx_, desc_.uri.c_str(), nullptr, nullptr);
  if (ret < 0) {
    return tl::unexpected<Error>({0, std::string("avformat_open_input failed:") + std::string(av_err2str(ret))});
  }
  ret = avformat_find_stream_info(fctx_, nullptr);
  if (ret < 0) {
    return tl::unexpected<Error>({0, std::string("avformat_find_stream_info failed:") + std::string(av_err2str(ret))});
  }
  best_video_index_ = av_find_best_stream(fctx_, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);
  if (best_video_index_ < 0) {
    return tl::unexpected<Error>({0, std::string("av_find_best_stream failed:") + std::string(av_err2str(ret))});
  }
  start_time_ = av_gettime();
  read_handler();
  return {};
}

MediaReader::~MediaReader() {
  if (fctx_ != nullptr) {
    avformat_close_input(&fctx_);
  }
}

MediaReader::MediaReader(boost::asio::io_context &ioc, MediaDescription desc)
    : ioc_(ioc), desc_(std::move(desc)), fctx_(nullptr), best_video_index_(-1), start_time_(0), timer_(ioc) {
}

}  // namespace MA
