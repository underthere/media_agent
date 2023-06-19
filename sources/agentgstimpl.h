//
// Created by 陈龙 on 2023/6/1.
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

  auto add_source(const MediaDescription &description,
                  const std::optional<std::string> &id = std::nullopt)
      -> tl::expected<std::string, MAError> override;

  auto configure_source(const std::string &source_id,
                        const MediaDescription &description) -> int override;

  auto remove_source(const std::string &source_id) -> int override;

  auto
  add_transform(const std::string &source_id,
                const MediaDescription &description,
                const std::optional<std::string> &transform_id = std::nullopt)
      -> int override;

  auto configure_transform(const std::string &transform_id,
                           const MediaDescription &description) -> int override;

  auto remove_transform(const std::string &transform_id) -> int override;

  auto query(const std::string &id)
      -> int override; // should return a data flow graph

private:
  std::unordered_map<std::string, GstContextWrapper> _context_map;
};
} // namespace MA
#endif // MEDIA_AGENT_AGENTGSTIMPL_H
