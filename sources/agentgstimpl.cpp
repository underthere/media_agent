//
// Created by 陈龙 on 2023/6/1.
//

#include "agentgstimpl.h"

auto MA::AgentGstImpl::add_source(const MediaDescription &description,
                                  const std::optional<std::string> &id) -> tl::expected<std::string, MAError> {
  if (id.has_value()) {
    const auto &source_id = id.value();
    if (_context_map.find(source_id) != _context_map.end()) {
      return tl::make_unexpected(Error{"", ""});
    }
  }
  return tl::make_unexpected(Error{"", ""});
}

void MA::AgentGstImpl::init() {}
