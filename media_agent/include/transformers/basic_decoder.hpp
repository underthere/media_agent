//
// Created by  on 2023/8/17.
//

#ifndef MEDIA_AGENT_BASICDECODER_H
#define MEDIA_AGENT_BASICDECODER_H

#include "media_transformer.hpp"
#include "tl/expected.hpp"

namespace MA {

class BasicDecoder : public MediaTransformer {
 public:
  BasicDecoder(const MediaDescription& input_desc, const MediaDescription& output_desc);
  virtual ~BasicDecoder();

  auto slot_new_packet(AVPacket* pkt, const AVCodecParameters* codecpar) -> void;

  auto init() -> tl::expected<void, Error>;
  auto flush() -> tl::expected<void, Error>;
  auto close() -> tl::expected<void, Error>;

 private:
  bool inited_ = false;
  const AVCodec* decoder_;
  AVCodecContext *dec_ctx_;
};
}  // namespace MA

#endif  // MEDIA_AGENT_BASICDECODER_H
