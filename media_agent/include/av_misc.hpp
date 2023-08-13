//
// Created by underthere on 2023/7/28.
//

#ifndef MEDIA_AGENT_AV_MISC_HPP
#define MEDIA_AGENT_AV_MISC_HPP

#include <string>
extern "C" {
#include "libavutil/avutil.h"
#include "libavutil/time.h"
}
#include "media_common.hpp"




inline auto rotional2double(const AVRational& r) -> double {
  if (r.den == 0 || r.num == 0) return 0;
  return (1.0 * r.num) / r.den;
}

inline auto timebase2us(const AVRational& r) -> std::int64_t {
  return static_cast<std::int64_t>(1e6 * rotional2double(r));
}

inline auto protocol_as_string(const MA::MediaProtocol& protocol) -> std::string {
  switch (protocol) {
    case MA::MediaProtocol::FILE:
      return "file";
    case MA::MediaProtocol::RTSP:
      return "rtsp";
    case MA::MediaProtocol::RTMP:
      return "rtmp";
    case MA::MediaProtocol::V4l2:
      return "v4l2";
    case MA::MediaProtocol::CUSTOM:
      return "custom";
    default:
      return "unknown";
  }
}



#endif  // MEDIA_AGENT_AV_MISC_HPP
