//
// Created by underthere on 2023/8/1.
//

#ifndef MEDIA_AGENT_TRANSCODER_HPP
#define MEDIA_AGENT_TRANSCODER_HPP

extern "C" {
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
};
#include "tl/expected.hpp"

#include "media_common.hpp"

namespace MA {
class Transcoder {
 public:
  explicit Transcoder(const MediaDescription& input, const MediaDescription& output);
  virtual ~Transcoder();

  virtual auto start() -> tl::expected<void, Error>;

 protected:
  virtual auto slot_on_new_packet(AVPacket*) -> void;

 private:
  MediaDescription input_desc_;
  MediaDescription output_desc_;
};
}  // namespace MA

#endif  // MEDIA_AGENT_TRANSCODER_HPP
