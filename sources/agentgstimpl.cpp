//
// Created by underthere on 2023/6/1.
//

#include "agentgstimpl.h"

#include "media_common.h"
#include "utils.h"

auto MA::AgentGstImpl::add_source(const MediaDescription &description, const std::optional<std::string> &id)
    -> tl::expected<std::string, MA::Error> {
  if (id.has_value()) {
    const auto &source_id = id.value();
    if (_context_map.find(source_id) != _context_map.end()) {
      auto err = MA::Error{.code = 1, .message = "source id already exist"};
      return tl::make_unexpected(err);
    }
  }

  const auto source_id = id.value_or(generate_uuid());

  return "";
}

void MA::AgentGstImpl::init() {}
