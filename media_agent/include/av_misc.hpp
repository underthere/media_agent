//
// Created by underthere on 2023/7/28.
//

#ifndef MEDIA_AGENT_AV_MISC_HPP
#define MEDIA_AGENT_AV_MISC_HPP

extern "C" {
#include "libavutil/avutil.h"
#include "libavutil/time.h"
};


inline auto rotional2double(const AVRational& r) -> double {
  if (r.den == 0 || r.num == 0) return 0;
  return (1.0 * r.num) / r.den;
}

inline auto timebase2us(const AVRational& r) -> std::int64_t {
  return static_cast<std::int64_t>(1e6 * rotional2double(r));
}


#endif  // MEDIA_AGENT_AV_MISC_HPP
