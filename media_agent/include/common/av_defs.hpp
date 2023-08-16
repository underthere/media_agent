#ifndef __MEDIA_AGENT_MEDIA_AV_DEFS_HPP__
#define __MEDIA_AGENT_MEDIA_AV_DEFS_HPP__

extern "C" {
    #include "libavcodec/avcodec.h"
};

namespace MA {
using slot_new_packet_type = void(AVPacket *, const AVCodecParameters *);
using slot_new_frame_type = void(AVFrame *, const AVCodecParameters *);

} // namespace MA

#endif // __MEDIA_AGENT_MEDIA_AV_DEFS_HPP__