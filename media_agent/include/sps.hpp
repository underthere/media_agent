//
// Created by underthere on 2023/8/1.
//

#ifndef MEDIA_AGENT_SPS_HPP
#define MEDIA_AGENT_SPS_HPP

#include <cstdint>

namespace MA {

class SPS {
 public:
  SPS(const std::uint8_t *);
  virtual ~SPS();
 private:
  std::uint8_t *data;
};

}  // namespace MA

#endif  // MEDIA_AGENT_SPS_HPP
