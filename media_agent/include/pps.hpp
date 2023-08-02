//
// Created by underthere on 2023/8/1.
//

#ifndef MEDIA_AGENT_PPS_HPP
#define MEDIA_AGENT_PPS_HPP

#include <bitset>

namespace MA {

class PPS {
 public:
  static const std::size_t size_ = 20;
  auto get_nal_ref_idc() -> std::uint8_t ;
  auto set_nal_ref_idc(std::uint8_t) -> void;
  auto get_nal_unit_type() -> std::uint8_t;
  auto set_nal_unit_type(std::uint8_t) -> void;

 private:
  std::bitset<size_> data_;
};

}  // namespace MA

#endif  // MEDIA_AGENT_PPS_HPP
