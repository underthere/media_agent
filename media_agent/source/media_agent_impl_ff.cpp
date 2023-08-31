//
// Created by underthere on 2023/8/9.
//

#include "media_agent_impl_ff.hpp"
#include "utils/misc.hpp"
#include "spdlog/spdlog.h"

namespace MA {
void MA::MediaAgentImplFF::init() {}
tl::expected<uuid_t, Error> MA::MediaAgentImplFF::add_source(const MA::MediaDescription& description, const std::optional<uuid_t>& id) {
  uuid_t ret_id = id.value_or(generate_uuid());
  auto pod = std::make_shared<MediaPod>(ret_id, description);
  media_pods_.emplace(ret_id, pod);
  auto plan = pod->run().via(coro_io::get_global_executor());
  plan.start([](auto &&res) {});
  return ret_id;
}
tl::expected<void, Error> MA::MediaAgentImplFF::configure_source(const MA::uuid_t& source_id, const MA::MediaDescription& description) {
  return tl::expected<void, Error>();
}
tl::expected<void, Error> MA::MediaAgentImplFF::remove_source(const MA::uuid_t& source_id) { return tl::expected<void, Error>(); }
tl::expected<uuid_t, Error> MA::MediaAgentImplFF::add_transform(const MA::uuid_t& source_id, const MA::MediaDescription& description,
                                                                const std::optional<uuid_t>& transform_id) {
  if (!media_pods_.contains(source_id)) {
    return tl::unexpected(Error{.code = ErrorType::UNKNOWN, .message = "source_id not found"});
  }
  auto pod = media_pods_.at(source_id);
  auto ret_id = transform_id.value_or(generate_uuid());
  auto rest = pod->add_output(ret_id, description);
  if (rest->empty()) {
    return tl::unexpected(rest.error());
  }
  return ret_id;
}
tl::expected<void, Error> MA::MediaAgentImplFF::configure_transform(const MA::uuid_t& transform_id, const MA::MediaDescription& description) {
  return tl::expected<void, Error>();
}
tl::expected<void, Error> MA::MediaAgentImplFF::remove_transform(const MA::uuid_t& transform_id) { return tl::expected<void, Error>(); }
tl::expected<void, Error> MA::MediaAgentImplFF::query(const MA::uuid_t& id) { return tl::expected<void, Error>(); }

}  // namespace MA