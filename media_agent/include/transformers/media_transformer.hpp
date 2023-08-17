#ifndef __MEDIA_AGENT_MEDIA_TRANSFORMER_HPP__
#define __MEDIA_AGENT_MEDIA_TRANSFORMER_HPP__

#include "common/av_defs.hpp"
#include "utils/signals.hpp"
#include "common/media_common.hpp"

namespace MA {

class MediaTransformer {
 public:
  MediaTransformer(const MediaDescription& input_desc,
                   const MediaDescription& output_desc): input_desc_(input_desc), output_desc_(output_desc) {};
  virtual ~MediaTransformer() = default;

  signals::signal<slot_new_packet_type> signal_new_packet;
  signals::signal<slot_new_frame_type> signal_new_frame;

 protected:
  MediaDescription input_desc_;
  MediaDescription output_desc_;
};

}  // namespace MA

#endif  // __MEDIA_AGENT_MEDIA_TRANSFORMER_HPP__