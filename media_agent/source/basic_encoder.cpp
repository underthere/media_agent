//
// Created by  on 2023/8/17.
//

#include "transformers/basic_encoder.hpp"

#include "common/av_misc.hpp"
#include "spdlog/spdlog.h"

namespace MA {
BasicEncoder::BasicEncoder(const MediaDescription& input_desc, const MediaDescription& output_desc)
    : MediaTransformer(input_desc, output_desc) {}

auto BasicEncoder::slot_new_frame(MediaBuffer& in_buffer) -> void {
  int ret;
  if (!in_buffer.data) {
    spdlog::warn("no frame data");
    return;
  }
  auto frame = av_frame_clone((AVFrame*)in_buffer.data);
  if (!in_buffer.codec_par && !in_buffer.codec_ctx) {
    spdlog::warn("no codec data");
    return;
  }
  auto codec_ctx = (AVCodecContext*)in_buffer.codec_ctx;
  if (!inited_) {
    encoder_ = avcodec_find_encoder(codec_format2av_codec_id(output_desc_.video_description->codec_format));
    if (!encoder_) {
      spdlog::error("find encoder failed:{}", codec_format_to_string(output_desc_.video_description->codec_format));
      return;
    }

    enc_ctx_ = avcodec_alloc_context3(encoder_);
    if (!enc_ctx_) {
      avcodec_free_context(&enc_ctx_);
      spdlog::error("alloc encoder context failed");
      return;
    }
    if (frame->hw_frames_ctx) {
      spdlog::info("using frame hw_frames_ctx {}", (void*)frame->hw_frames_ctx->size);
      enc_ctx_->hw_frames_ctx = av_buffer_ref(frame->hw_frames_ctx);
    } else if (codec_ctx && codec_ctx->hw_frames_ctx) {
      spdlog::info("using codec hw_frames_ctx");
      enc_ctx_->hw_frames_ctx = av_buffer_ref(codec_ctx->hw_frames_ctx);
    }
    enc_ctx_->width = frame->width;
    enc_ctx_->height = frame->height;
    enc_ctx_->pix_fmt = static_cast<AVPixelFormat>(frame->format);
    enc_ctx_->time_base = {1, 25};
    enc_ctx_->profile = FF_PROFILE_H264_BASELINE;
    enc_ctx_->level = 31;
    enc_ctx_->bit_rate = output_desc_.video_description->bitrate;
    enc_ctx_->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

    ret = avcodec_open2(enc_ctx_, encoder_, nullptr);
    if (ret < 0) {
      spdlog::error("open encoder failed:{}", av_err2str(ret));
      return;
    }

    inited_ = true;
  }

  if (frame->pts < 0) {
    frame->pts = 0;
  }
  ret = avcodec_send_frame(enc_ctx_, frame);
  av_frame_free(&frame);
  if (ret < 0) {
    if (ret == AVERROR(EAGAIN)) return;
    spdlog::error("send frame to encoder failed:{}", av_err2str(ret));
    return;
  }

  AVPacket* pkt = av_packet_alloc();
  ret = avcodec_receive_packet(enc_ctx_, pkt);
  if (ret < 0) {
    av_packet_free(&pkt);
    if (ret == AVERROR(EAGAIN)) return;
    spdlog::error("receive packet from encoder failed:{}", av_err2str(ret));
    return;
  }

  if (enc_par_ == nullptr) {
    enc_par_ = avcodec_parameters_alloc();
    ret = avcodec_parameters_from_context(enc_par_, enc_ctx_);
    if (ret < 0) {
      spdlog::error("copy encoder context to parameters failed:{}", av_err2str(ret));
      return;
    }
  }

  spdlog::debug("encoder output packet: {}", pkt->pts);
  MediaBuffer out_buffer{
      .type = MediaBufferType::FF_PACKET,
      .data = pkt,
      .codec_par = nullptr,
      .codec_ctx = enc_ctx_,
  };
  signal_new_packet(out_buffer);
  av_packet_free(&pkt);
}
BasicEncoder::~BasicEncoder() {}
}  // namespace MA