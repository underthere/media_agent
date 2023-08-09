//
// Created by underthere on 2023/8/9.
//

#include "media_agent_impl_ff.hpp"

namespace MA {
void MA::MediaAgentImplFF::init() {}
tl::expected<uuid_t, Error> MA::MediaAgentImplFF::add_source(const MA::MediaDescription& description, const std::optional<uuid_t>& id) {
  return tl::expected<uuid_t, Error>();
}
tl::expected<void, Error> MA::MediaAgentImplFF::configure_source(const MA::uuid_t& source_id, const MA::MediaDescription& description) {
  return tl::expected<void, Error>();
}
tl::expected<void, Error> MA::MediaAgentImplFF::remove_source(const MA::uuid_t& source_id) { return tl::expected<void, Error>(); }
tl::expected<uuid_t, Error> MA::MediaAgentImplFF::add_transform(const MA::uuid_t& source_id, const MA::MediaDescription& description,
                                                                const std::optional<uuid_t>& transform_id) {
  return tl::expected<uuid_t, Error>();
}
tl::expected<void, Error> MA::MediaAgentImplFF::configure_transform(const MA::uuid_t& transform_id, const MA::MediaDescription& description) {
  return tl::expected<void, Error>();
}
tl::expected<void, Error> MA::MediaAgentImplFF::remove_transform(const MA::uuid_t& transform_id) { return tl::expected<void, Error>(); }
tl::expected<void, Error> MA::MediaAgentImplFF::query(const MA::uuid_t& id) { return tl::expected<void, Error>(); }

}  // namespace MA