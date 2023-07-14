//
// Created by underthere on 2023/7/14.
//

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavdevice/avdevice.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
}

#include <condition_variable>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>

#include "argparse/argparse.hpp"

struct AvStuff {
  AVFormatContext *fmt_ctx;
  AVStream *stream;
  AVCodecContext *dec_ctx;
  AVCodecContext *enc_ctx;

  std::mutex mtx_rd_que;
  std::condition_variable cond_rd_que;
  std::queue<AVPacket *> read_pkt_queue;
  std::queue<AVFrame *> frame_queue;
  std::queue<AVPacket *> output_pkt_queue;
};

void read_func(std::shared_ptr<AvStuff> stuff) {
  while (true) {
    AVPacket *pkt = av_packet_alloc();
    auto ret = av_read_frame(stuff->fmt_ctx, pkt);
    if (ret < 0) {
      av_packet_free(&pkt);
      break;
    }
    if (!pkt->size) continue;
    {
      std::lock_guard<std::mutex> lk(stuff->mtx_rd_que);
      stuff->read_pkt_queue.push(pkt);
    }
  }
}

void decode_func(std::shared_ptr<AvStuff> stuff) {
  auto stream_index = av_find_best_stream(stuff->fmt_ctx, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);
  if (stream_index < 0) {
    std::cout << "av_find_best_stream failed" << std::endl;
    return;
  }
  AVStream *stream = stuff->fmt_ctx->streams[stream_index];
  stuff->stream = stream;
  AVCodecContext *dec_ctx = avcodec_alloc_context3(nullptr);
  if (!dec_ctx) {
    std::cout << "avcodec_alloc_context3 failed" << std::endl;
    return;
  }

  stuff->dec_ctx = dec_ctx;
  const AVCodec *decoder = avcodec_find_decoder(stream->codecpar->codec_id);

  avcodec_parameters_to_context(dec_ctx, stream->codecpar);
  avcodec_open2(dec_ctx, decoder, nullptr);

  AVPacket *pkt;
  size_t cnt = 0;
  while (true) {
    {
      std::unique_lock<std::mutex> lk(stuff->mtx_rd_que);
      stuff->cond_rd_que.wait(lk, [&] { return !stuff->read_pkt_queue.empty(); });
      pkt = stuff->read_pkt_queue.front();
      stuff->read_pkt_queue.pop();
    }
    auto frame = av_frame_alloc();
    avcodec_send_packet(dec_ctx, pkt);
    avcodec_receive_frame(dec_ctx, frame);
    av_frame_free(&frame);
    std::cout << "frame: " << ++cnt << std::endl;
  }
  av_packet_free(&pkt);
}

void encode_func(std::shared_ptr<AvStuff> stuff) {}

auto print_av_error(int err) -> void {
  static char err_buf[AV_ERROR_MAX_STRING_SIZE];
  av_strerror(err, err_buf, AV_ERROR_MAX_STRING_SIZE);
  std::cout << "error code: " << err << std::endl;
  std::cout << "error message: " << err_buf << std::endl;
}

int main(int argc, char **argv) {
  argparse::ArgumentParser program("play_ffmpeg");
  program.add_argument("file").help("file to play");
  program.add_argument("srs_addr");

  try {
    program.parse_args(argc, argv);
  } catch (const std::runtime_error &err) {
    std::cout << err.what() << std::endl;
    std::cout << program;
    std::exit(1);
  }

  auto stuff = std::make_shared<AvStuff>();

  auto av_ctx = avformat_alloc_context();
  if (!av_ctx) {
    std::cout << "avformat_alloc_context failed" << std::endl;
    std::exit(1);
  }
  stuff->fmt_ctx = av_ctx;

  auto ret = avformat_open_input(&av_ctx, program.get<std::string>("file").c_str(), nullptr, nullptr);
  if (ret < 0) {
    std::cout << "avformat_open_input failed" << std::endl;
    print_av_error(ret);
    std::exit(1);
  }

  std::thread read_thread(read_func, stuff);
  std::thread decode_thread(decode_func, stuff);
  std::thread encode_thread(encode_func, stuff);

  read_thread.join();
  decode_thread.join();
  encode_thread.join();

  return 0;
}