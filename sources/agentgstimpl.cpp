//
// Created by underthere on 2023/6/1.
//

#include "agentgstimpl.h"

#include <memory>

#include "media_common.h"
#include "utils.h"

template <typename K, typename V>
auto map_get_op(const std::unordered_map<K, V> &map, const K &key) -> std::optional<V> {
  auto iter = map.find(key);
  if (iter != map.end()) {
    return std::make_optional(iter->second);
  }
  return std::nullopt;
}

void MA::AgentGstImpl::init() { gst_init(nullptr, nullptr); }

auto MA::AgentGstImpl::add_source(const MediaDescription &description, const std::optional<uuid_t> &id) -> tl::expected<uuid_t, MA::Error> {
  const auto source_id = id.value_or(generate_uuid());
  if (_context_map.find(source_id) != _context_map.end()) {
    return tl::make_unexpected(MA::Error{.code = 1, .message = "source already exists"});
  }

  return MA::gst_context_create()
      .and_then([&description](const std::shared_ptr<GstContextWrapper> &ctx) {
        return MA::gst_context_add_source(ctx, description);
      })
      .and_then([this](const std::shared_ptr<GstContextWrapper> &ctx_) {
        auto source_id = generate_uuid();
        _context_map.insert(std::pair{source_id, *ctx_});
        return tl::expected<uuid_t, MA::Error>(source_id);
      });
}

auto MA::AgentGstImpl::configure_source(const MA::uuid_t &source_id, const MA::MediaDescription &description) -> tl::expected<void, MA::Error> {
  return {};
}

auto MA::AgentGstImpl::remove_source(const MA::uuid_t &source_id) -> tl::expected<void, MA::Error> {
    return {};
}

auto MA::AgentGstImpl::add_transform(const MA::uuid_t &source_id, const MA::MediaDescription &description, const std::optional<uuid_t> &id) -> tl::expected<uuid_t, MA::Error> {
  return {};
}

auto MA::AgentGstImpl::configure_transform(const MA::uuid_t &transform_id, const MA::MediaDescription &description) -> tl::expected<void, MA::Error> {
  return {};
}

auto MA::AgentGstImpl::remove_transform(const MA::uuid_t &transform_id) -> tl::expected<void, MA::Error> {
  return {};
}

auto MA::AgentGstImpl::query(const MA::uuid_t &id) -> tl::expected<void, MA::Error> {
  return {};
}
