//
// Created by underthere on 2023/7/31.
//

#ifndef MEDIA_AGENT_MEDIA_WRITER_HPP
#define MEDIA_AGENT_MEDIA_WRITER_HPP

extern "C" {
#include "libavformat/avformat.h"
}

#include "async_simple/coro/Lazy.h"
#include "tl/expected.hpp"
#include "common/media_common.hpp"
#include "utils/signals.hpp"

using namespace async_simple;

namespace MA {
class MediaWriter {
 public:
  explicit MediaWriter(const MediaDescription &desc);
  virtual ~MediaWriter();

  auto run() -> coro::Lazy<tl::expected<void, Error>>;

  auto slot_new_packet(AVPacket *, const AVCodecParameters*) -> void;

 private:


 private:
  bool output_opened_ = false;
  MediaDescription desc_;
  AVFormatContext *fctx_;
};
}  // namespace MA

#endif  // MEDIA_AGENT_MEDIA_WRITER_HPP
