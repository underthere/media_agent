//
// Created by underthere on 2023/6/20.
//

#ifndef MEDIA_AGENT_UTILS_H
#define MEDIA_AGENT_UTILS_H
#include <uuid/uuid.h>
#include <cstdint>
#include "sps.hpp"
#include "pps.hpp"

std::string generate_uuid() {
  uuid_t uuid;
  uuid_generate(uuid);
  char uuid_str[37];
  uuid_unparse(uuid, uuid_str);
  return {uuid_str};
}

auto extract_pps(const char* extradata, std::size_t size) {
  std::size_t pps_start_index = 0;
  std::size_t pps_length = 0;
  for (int i = 0; i < size; ++i) {
    if (extradata[i] == 0x68 && extradata[i-2] == 0) {
        sps_start_index = i;
        sps_length = extradata[i-1];
    }
  }
}

auto extract_sps(const char* extradata, std::size_t size) {
  std::size_t sps_start_index = 0;
  std::size_t sps_length = 0;
  for (int i = 0; i < size; ++i) {
    if (extradata[i] == 0x67 && extradata[i-2] == 0) {
        sps_start_index = i;
        sps_length = extradata[i-1];
    }
  }
}

#endif  // MEDIA_AGENT_UTILS_H
