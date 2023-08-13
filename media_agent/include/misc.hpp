//
// Created by underthere on 2023/6/20.
//

#ifndef MEDIA_AGENT_UTILS_H
#define MEDIA_AGENT_UTILS_H
#include <uuid/uuid.h>
#include <cstdint>
#include <string>

inline std::string generate_uuid() {
  uuid_t uuid;
  uuid_generate(uuid);
  char uuid_str[37];
  uuid_unparse(uuid, uuid_str);
  return {uuid_str};
}

#endif  // MEDIA_AGENT_UTILS_H
