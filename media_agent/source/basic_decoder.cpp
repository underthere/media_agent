//
// Created by  on 2023/8/17.
//

#include "transformers/basic_decoder.hpp"

#include <spdlog/spdlog.h>

namespace MA {
BasicDecoder::BasicDecoder(const MediaDescription &input_desc, const MediaDescription &output_desc)
    : MediaTransformer(input_desc, output_desc) {}
BasicDecoder::~BasicDecoder() {}

auto BasicDecoder::slot_new_packet(MediaBuffer &buffer) -> void {
  int ret;
  if (!buffer.data) {
    spdlog::warn("no data");
    return;
  }
  auto pkt = av_packet_clone((AVPacket *)buffer.data);
  if (!buffer.codec_par && !buffer.codec_ctx) {
    spdlog::warn("no codec");
    return;
  }
  auto *codecpar = (AVCodecParameters *)buffer.codec_par;
  auto *codec_ctx = (AVCodecContext *)buffer.codec_ctx;
  if (!inited_) {
    decoder_ = avcodec_find_decoder(codecpar->codec_id);
    if (!decoder_) {
      spdlog::warn("find decoder failed:{}", codecpar->codec_id);
      return;
    }
    dec_ctx_ = avcodec_alloc_context3(decoder_);
    if (!dec_ctx_) {
      spdlog::warn("alloc decoder context failed");
      return;
    }
    if (avcodec_parameters_to_context(dec_ctx_, codecpar) < 0) {
      spdlog::warn("copy codecpar to decoder context failed");
      return;
    }
    if (avcodec_open2(dec_ctx_, decoder_, nullptr) < 0) {
      spdlog::warn("open decoder failed");
      return;
    }
    inited_ = true;
  }

  ret = avcodec_send_packet(dec_ctx_, pkt);
  av_packet_free(&pkt);
  if (ret < 0) {
    if (ret == AVERROR(EAGAIN)) return;
    spdlog::warn("send packet to decoder failed:{}", av_err2str(ret));
    return;
  }

  AVFrame *frame = av_frame_alloc();
  ret = avcodec_receive_frame(dec_ctx_, frame);
  if (ret < 0) {
    av_frame_free(&frame);
    if (ret == AVERROR(EAGAIN)) return;
    spdlog::warn("receive frame from decoder failed:{}", av_err2str(ret));
    return;
  }

  spdlog::trace("decoder output frame: {}", frame->pts);
  MediaBuffer step_buffer{
      .type = MediaBufferType::FF_FRAME,
      .data = frame,
      .codec_par = nullptr,
      .codec_ctx = dec_ctx_,
  };
  signal_new_frame(step_buffer);
  av_frame_free(&frame);
}

auto BasicDecoder::init() -> tl::expected<void, Error> { return tl::expected<void, Error>(); }
auto BasicDecoder::flush() -> tl::expected<void, Error> { return tl::expected<void, Error>(); }
auto BasicDecoder::close() -> tl::expected<void, Error> { return tl::expected<void, Error>(); }

}  // namespace MA