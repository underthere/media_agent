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

class AgentGstImpl : public MediaAgent {
 public:
  AgentGstImpl() = default;
  ~AgentGstImpl() override = default;

  void init() override;

  auto add_source(const MediaDescription &description, const std::optional<uuid_t> &id = std::nullopt)
      -> tl::expected<uuid_t, Error> override;

  auto configure_source(const uuid_t &source_id, const MediaDescription &description) -> tl::expected<void, Error> override;

  auto remove_source(const uuid_t &source_id) -> tl::expected<void, Error> override;

  auto add_transform(const uuid_t &source_id, const MediaDescription &description, const std::optional<uuid_t> &transform_id)
      -> tl::expected<uuid_t, Error> override {
    return tl::expected<uuid_t, Error>();
  }
  auto configure_transform(const uuid_t &transform_id, const MediaDescription &description) -> tl::expected<void, Error> override {
    return tl::expected<void, Error>();
  }
  auto remove_transform(const uuid_t &transform_id) -> tl::expected<void, Error> override { return tl::expected<void, Error>(); }
  auto query(const uuid_t &id) -> tl::expected<void, Error> override { return tl::expected<void, Error>(); }

 private:

 private:
  std::unordered_map<uuid_t, GstContextWrapper> _context_map;
};
}  // namespace MA
#endif  // MEDIA_AGENT_AGENTGSTIMPL_H
