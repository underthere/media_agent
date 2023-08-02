//
// Created by underthere on 2023/8/2.
//

#include <iostream>

extern "C" {
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavutil/avutil.h"
}

int main() {
  AVFormatContext *in_fctx;
  AVFormatContext *out_fctx;

  const auto encoder = avcodec_find_encoder_by_name("h264_rkmpp");
  const auto decoder = avcodec_find_decoder_by_name("h264_rkmpp");

  int ret;

  ret = avformat_open_input(&in_fctx, "../../test.flv", nullptr, nullptr);

  if (ret < 0) {
    std::cout << "open input failed: " << av_err2str(ret) << std::endl;
    return 1;
  }

  ret = avformat_find_stream_info(in_fctx, nullptr);
  if (ret < 0) {
    std::cout << "find stream info failed: " << av_err2str(ret) << std::endl;
    return 1;
  }

  auto v_index = av_find_best_stream(in_fctx, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);
  if (v_index < 0) {
    std::cout << "find video stream failed: " << av_err2str(v_index) << std::endl;
    return 1;
  }

  auto in_stream = in_fctx->streams[v_index];

  auto enc_ctx = avcodec_alloc_context3(encoder);
  if (!enc_ctx) {
    std::cout << "alloc encoder context failed" << std::endl;
    return 1;
  }
  auto dec_ctx = avcodec_alloc_context3(decoder);
  avcodec_parameters_to_context(dec_ctx, in_stream->codecpar);
  if (!dec_ctx) {
    std::cout << "alloc decoder context failed" << std::endl;
    return 1;
  }

  ret = avformat_alloc_output_context2(&out_fctx, nullptr, "flv", "../../test_out.flv");
  if (ret < 0) {
    std::cout << "alloc output context failed: " << av_err2str(ret) << std::endl;
    return 1;
  }

  auto out_stream = avformat_new_stream(out_fctx, nullptr);
  if (!out_stream) {
    std::cout << "alloc output stream failed" << std::endl;
    return 1;
  }

  enc_ctx->width = in_stream->codecpar->width;
  enc_ctx->height = in_stream->codecpar->height;
  enc_ctx->pix_fmt = AV_PIX_FMT_YUV420P;
  enc_ctx->framerate = in_stream->avg_frame_rate;
  enc_ctx->profile = FF_PROFILE_H264_CONSTRAINED_BASELINE;
  enc_ctx->bit_rate = 2 * 1000 * 1000;
  enc_ctx->time_base = in_stream->time_base;
  if (out_fctx->oformat->flags & AVFMT_GLOBALHEADER) {
    enc_ctx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
  }

  ret = avcodec_open2(dec_ctx, decoder, nullptr);
  if (ret < 0) {
    std::cout << "open decoder failed: " << av_err2str(ret) << std::endl;
    return 1;
  }

  ret = avcodec_open2(enc_ctx, encoder, nullptr);
  if (ret < 0) {
    std::cout << "open encoder failed: " << av_err2str(ret) << std::endl;
    return 1;
  }

  ret = avcodec_parameters_from_context(out_stream->codecpar, enc_ctx);
  if (ret < 0) {
    std::cout << "copy codec par failed: " << av_err2str(ret) << std::endl;
    return 1;
  }

  AVPacket in_pkt, out_pkt;
  AVFrame frame;

  bool output_opened = false;

  while (true) {
    ret = av_read_frame(in_fctx, &in_pkt);
    if (ret < 0) {
      std::cout << "read frame failed: " << av_err2str(ret) << std::endl;
      break;
    }
    if (in_pkt.stream_index != v_index) continue;
    ret = avcodec_send_packet(dec_ctx, &in_pkt);
    if (ret < 0) {
      if (ret == AVERROR(EAGAIN))
        continue;
      else {
        std::cout << "send packet failed: " << av_err2str(ret) << std::endl;
        break;
      }
    }
    ret = avcodec_receive_frame(dec_ctx, &frame);
    if (ret < 0) {
      if (ret == AVERROR(EAGAIN))
        continue;
      else {
        std::cout << "receive frame failed: " << av_err2str(ret) << std::endl;
        break;
      }
    }
    frame.pts = av_rescale_q(frame.pts, in_stream->time_base, enc_ctx->time_base);
    frame.pkt_dts = av_rescale_q(frame.pkt_dts, in_stream->time_base, enc_ctx->time_base);
    frame.pkt_duration = av_rescale_q(frame.pkt_duration, in_stream->time_base, enc_ctx->time_base);
    frame.pkt_pos = -1;

    ret = avcodec_send_frame(enc_ctx, &frame);
    if (ret < 0) {
      if (ret == AVERROR(EAGAIN))
        continue;
      else {
        std::cout << "send frame failed: " << av_err2str(ret) << std::endl;
        break;
      }
    }
    ret = avcodec_receive_packet(enc_ctx, &out_pkt);
    if (ret < 0) {
      if (ret == AVERROR(EAGAIN))
        continue;
      else {
        std::cout << "receive packet failed: " << av_err2str(ret) << std::endl;
        break;
      }
    }
    out_pkt.stream_index = 0;
    out_pkt.pts = av_rescale_q(out_pkt.pts, enc_ctx->time_base, out_stream->time_base);
    out_pkt.dts = av_rescale_q(out_pkt.dts, enc_ctx->time_base, out_stream->time_base);
    out_pkt.duration = av_rescale_q(out_pkt.duration, enc_ctx->time_base, out_stream->time_base);

    if (!output_opened) {
      avcodec_parameters_from_context(out_stream->codecpar, enc_ctx);
      out_stream->codecpar->codec_tag = 0;
      ret = avio_open2(&out_fctx->pb, "../../test_out.flv", AVIO_FLAG_WRITE, nullptr, nullptr);
      if (ret < 0) {
        std::cout << "open output file failed: " << av_err2str(ret) << std::endl;
        break;
      }
      ret = avformat_write_header(out_fctx, nullptr);
      if (ret < 0) {
        std::cout << "write header failed: " << av_err2str(ret) << std::endl;
        break;
      }
      output_opened = true;
    }
    av_interleaved_write_frame(out_fctx, &out_pkt);
  }
  std::cout << "all done!" << std::endl;
}