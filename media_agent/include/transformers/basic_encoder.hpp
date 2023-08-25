//
// Created by  on 2023/8/17.
//

#ifndef MEDIA_AGENT_BASIC_ENCODER_HPP
#define MEDIA_AGENT_BASIC_ENCODER_HPP

#include "media_transformer.hpp"

namespace MA {

class BasicEncoder : public MediaTransformer {
 public:
  BasicEncoder(const MediaDescription& input_desc, const MediaDescription& output_desc);
  ~BasicEncoder();

  auto slot_new_frame(MediaBuffer &in_buffer) -> void;

 private:
  bool inited_;
  const AVCodec* encoder_;
  AVCodecParameters* enc_par_;
  AVCodecContext* enc_ctx_;
};

}  // namespace MA

#endif  // MEDIA_AGENT_BASIC_ENCODER_HPP
