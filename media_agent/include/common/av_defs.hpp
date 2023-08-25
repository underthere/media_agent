#ifndef __MEDIA_AGENT_MEDIA_AV_DEFS_HPP__
#define __MEDIA_AGENT_MEDIA_AV_DEFS_HPP__

extern "C" {
    #include "libavcodec/avcodec.h"
};

#include "media_common.hpp"

namespace MA {

using slot_new_packet_type = void(MediaBuffer);
using slot_new_frame_type = void(MediaBuffer);

} // namespace MA

#endif // __MEDIA_AGENT_MEDIA_AV_DEFS_HPP__