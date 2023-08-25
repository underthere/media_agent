//
// Created by underthere on 2023/7/31.
//

#include <unordered_map>

#include "common/av_misc.hpp"
#include "spdlog/spdlog.h"
#include "writers/basic_writer.hpp"

using namespace async_simple;

namespace MA {

BasicWriter::BasicWriter(const MediaDescription& desc): desc_(desc) {}

BasicWriter::~BasicWriter() {
  if (fctx_) {
    avformat_close_input(&fctx_);
  }
}

auto BasicWriter::run() -> coro::Lazy<tl::expected<void, Error>> {
  co_return tl::expected<void, Error>();
}

auto BasicWriter::slot_new_packet(MediaBuffer& buffer) -> void {
  static std::unordered_map<MediaProtocol, std::string> fmt_mapping {
      {MediaProtocol::RTMP, "flv"}
  };

  AVPacket *pkt = nullptr;
  if (buffer.type == MediaBufferType::FF_PACKET && buffer.data) {
    pkt = av_packet_clone((AVPacket*)buffer.data);
  } else {
    spdlog::warn("un-support buffer type:{}", buffer.type);
    return;
  }
  auto codec_par = (AVCodecParameters*)buffer.codec_par;
  auto codec_ctx = (AVCodecContext*)buffer.codec_ctx;
  if (!codec_par && !codec_ctx) {
    spdlog::warn("un-support codec par:{}", fmt::ptr(codec_par));
    return;
  }
  spdlog::trace("writer input packet:{}, par: {}", pkt->pts, fmt::ptr(codec_par));

  int ret {0};
  if (!output_opened_){
    std::string av_fmt{};
    if (fmt_mapping.contains(desc_.protocol)) {
      av_fmt = fmt_mapping[desc_.protocol];
    }
    if (av_fmt.empty()) {
      spdlog::warn("un-support protocol:{}", protocol_as_string(desc_.protocol));
      goto fail;
    }
    ret = avformat_alloc_output_context2(&fctx_, nullptr, av_fmt.c_str(), desc_.uri.c_str());
    if (ret < 0) {
      spdlog::warn("alloc output context failed:{}", av_err2str(ret));
      goto fail;
    }

    auto stream = avformat_new_stream(fctx_, nullptr);
    if (codec_par){
      avcodec_parameters_copy(stream->codecpar, codec_par);
    } else {
      avcodec_parameters_from_context(stream->codecpar, codec_ctx);
    }

    if (codec_ctx) {
      stream->time_base = codec_ctx->time_base;
    }

    ret = avio_open2(&fctx_->pb, desc_.uri.c_str(), AVIO_FLAG_WRITE, nullptr, nullptr);

    if (ret < 0) {
      spdlog::warn("open output failed:{}", av_err2str(ret));
      goto fail;
    }

    ret = avformat_write_header(fctx_, nullptr);
    if (ret < 0) {
      spdlog::warn("write header failed:{}", av_err2str(ret));
      goto fail;
    }

    output_opened_ = true;
  }

//  if (codec_ctx) {
//    av_packet_rescale_ts(pkt, codec_ctx->time_base, fctx_->streams[0]->time_base);
//  }
  ret = av_write_frame(fctx_, pkt);
  av_packet_free(&pkt);
  if (ret < 0) {
    spdlog::warn("write frame failed:{}", av_err2str(ret));
    goto fail;
  } else {
    return ;
  }

fail:

  avformat_free_context(fctx_); fctx_ = nullptr;
  output_opened_ = false;
}

}  // namespace MA
