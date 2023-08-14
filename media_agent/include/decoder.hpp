//
// Created by underthere on 2023/8/1.
//

#ifndef MEDIA_AGENT_DECODER_HPP
#define MEDIA_AGENT_DECODER_HPP

#include "boost/signals2.hpp"
#include "tl/expected.hpp"
extern "C" {
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
}

#include "media_common.hpp"

namespace MA {
class Decoder {
 public:
  explicit Decoder(MediaDescription input, HardwareAccelerator hw = HardwareAccelerator::NONE);
  virtual ~Decoder();

  boost::signals2::signal<void(AVFrame*)> sig_new_frame;

  virtual auto start() -> tl::expected<void, Error>;

 protected:
  virtual auto on_new_packet(AVPacket*) -> void;

 private:
  MediaDescription input_desc_;
  HardwareAccelerator hw_;
  AVCodecContext* codec_ctx_;
  AVBufferRef *hw_device_ctx_;
};
}  // namespace MA

#endif  // MEDIA_AGENT_DECODER_HPP
