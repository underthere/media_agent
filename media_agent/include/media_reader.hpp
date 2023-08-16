//
// Created by underthere on 2023/7/28.
//

#ifndef MEDIA_AGENT_MEDIA_READER_HPP
#define MEDIA_AGENT_MEDIA_READER_HPP


#include <chrono>

extern "C" {
#include "libavformat/avformat.h"
#include "libavutil/avutil.h"
#include "libavutil/time.h"
#include <libavcodec/codec_par.h>
}
#include "async_simple/coro/Lazy.h"
#include "utils/signals.hpp"
#include "tl/expected.hpp"

#include "common/media_common.hpp"
#include "common/av_defs.hpp"

using namespace std::chrono_literals;
using namespace async_simple;

namespace MA {

class MediaReader {
 public:
  explicit MediaReader(MediaDescription desc);
  virtual ~MediaReader();

  auto run() -> coro::Lazy<tl::expected<void, Error>>;

  auto read() -> tl::expected<AVPacket *, Error>;

  signals::signal<slot_new_packet_type> sig_new_packet_;
  signals::signal<slot_new_frame_type> sig_new_frame_;

 private:
  auto get_current_codec_par() -> const AVCodecParameters*;

 private:
  bool running = false;
  bool media_opened_ = false;
  std::chrono::duration<uint64_t, std::micro> retry_interval_ = 1000ms;
  MediaDescription desc_;
  AVFormatContext *fctx_;
  int best_video_index_;

  std::int64_t start_time_{};
};

}  // namespace MA

#endif  // MEDIA_AGENT_MEDIA_READER_HPP
