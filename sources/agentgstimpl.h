//
// Created by underthere on 2023/6/1.
//

#ifndef MEDIA_AGENT_AGENTGSTIMPL_H
#define MEDIA_AGENT_AGENTGSTIMPL_H

#include <gst/gst.h>

#include "mediaagent.h"

namespace MA {
struct GstContextWrapper {
  GstElement *pipeline;
  GstElement *source;
  GstElement *sink;
  GstElement *transform;
};

class AgentGstImpl : protected MediaAgent {
 public:
  AgentGstImpl() = default;
  ~AgentGstImpl() override = default;

  void init() override;

  auto add_source(const MediaDescription &description, const std::optional<std::string> &id = std::nullopt)
      -> tl::expected<std::string, Error> override;

  auto configure_source(const std::string &source_id, const MediaDescription &description) -> tl::expected<void, Error> override;

  auto remove_source(const std::string &source_id) -> tl::expected<void, Error> override;

 private:
  std::unordered_map<std::string, GstContextWrapper> _context_map;
};
}  // namespace MA
#endif  // MEDIA_AGENT_AGENTGSTIMPL_H
