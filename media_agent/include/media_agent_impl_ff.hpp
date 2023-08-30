//
// Created by underthere on 2023/8/9.
//

#ifndef MEDIA_AGENT_MEDIA_AGENT_IMPL_FF_HPP
#define MEDIA_AGENT_MEDIA_AGENT_IMPL_FF_HPP

#include <memory>
#include "mediaagent.hpp"
#include "media_pod.hpp"

#include "cinatra/ylt/coro_io/io_context_pool.hpp"

namespace MA {

class MediaAgentImplFF: public MediaAgent {
 public:
  MediaAgentImplFF() = default;
  virtual ~MediaAgentImplFF() = default;

  void init() override;
  auto add_source(const MediaDescription& description, const std::optional<uuid_t>& id) -> tl::expected<uuid_t, Error> override;
  auto configure_source(const uuid_t& source_id, const MediaDescription& description) -> tl::expected<void, Error> override;
  auto remove_source(const uuid_t& source_id) -> tl::expected<void, Error> override;
  auto add_transform(const uuid_t& source_id, const MediaDescription& description, const std::optional<uuid_t>& transform_id)
      -> tl::expected<uuid_t, Error> override;
  auto configure_transform(const uuid_t& transform_id, const MediaDescription& description) -> tl::expected<void, Error> override;
  auto remove_transform(const uuid_t& transform_id) -> tl::expected<void, Error> override;
  auto query(const uuid_t& id) -> tl::expected<void, Error> override;

 private:
  std::unordered_map<uuid_t, std::shared_ptr<MediaPod>> media_pods_;
  // coro_io::ExecutorWrapper<> executor_;
};

}  // namespace MA

#endif  // MEDIA_AGENT_MEDIA_AGENT_IMPL_FF_HPP

