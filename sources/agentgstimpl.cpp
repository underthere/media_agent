//
// Created by underthere on 2023/6/1.
//

#include "agentgstimpl.h"

#include <memory>

#include "media_common.h"
#include "utils.h"

using gst_ctx_creator_t = std::function<tl::expected<std::shared_ptr<MA::GstContextWrapper>, MA::Error>(const MA::MediaDescription &)>;

const std::unordered_map<MA::MediaProtocol, gst_ctx_creator_t> CTX_CREATION_IMPL_MAP = {
    std::pair{MA::MediaProtocol::RTMP, [](const MA::MediaDescription &desc) { return std::make_shared<MA::GstContextWrapper>(); }},
    std::pair{MA::MediaProtocol::RTSP, [](const MA::MediaDescription &desc) { return std::make_shared<MA::GstContextWrapper>(); }}};

template <typename K, typename V>
auto map_get_op(const std::unordered_map<K, V> &map, const K &key) -> std::optional<V> {
  auto iter = map.find(key);
  if (iter != map.end()) {
    return std::make_optional(iter->second);
  }
  return std::nullopt;
}

auto create_gst_context(const MA::MediaDescription &description) -> tl::expected<std::shared_ptr<MA::GstContextWrapper>, MA::Error> {
  return map_get_op<MA::MediaProtocol, gst_ctx_creator_t>(CTX_CREATION_IMPL_MAP, description.protocol)
      .value_or([](const MA::MediaDescription &desc) {
        auto err = MA::Error{.code = 1, .message = "unsupported protocol"};
        return tl::make_unexpected(err);
      })(description);
}

void MA::AgentGstImpl::init() { gst_init(nullptr, nullptr); }

auto MA::AgentGstImpl::add_source(const MediaDescription &description, const std::optional<uuid_t> &id) -> tl::expected<uuid_t, MA::Error> {
  const auto source_id = id.value_or(generate_uuid());
  if (_context_map.find(source_id) != _context_map.end()) {
    return tl::make_unexpected(MA::Error{.code = 1, .message = "source already exists"});
  }

  return create_gst_context(description).and_then([this](const std::shared_ptr<GstContextWrapper> &ctx) {
    auto source_id = generate_uuid();
    _context_map.insert(std::pair{source_id, *ctx});
    return tl::expected<uuid_t, MA::Error>(source_id);
  });
}
