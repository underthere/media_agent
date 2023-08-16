//
// Created by underthere on 2023/6/1.
//

#ifndef MEDIA_AGENT_MEDIAAGENT_HPP
#define MEDIA_AGENT_MEDIAAGENT_HPP

#include "common/media_common.hpp"
#include "tl/expected.hpp"

namespace MA {
class MediaAgent {
 public:
  virtual ~MediaAgent() = default;

  virtual void init() = 0;

  /**
   * @brief register a media source
   * @param description
   * @return
   */
  virtual auto add_source(const MediaDescription &description, const std::optional<uuid_t> &id = std::nullopt)
      -> tl::expected<uuid_t, Error> = 0;

  /**
   * @brief configure a media source
   * @param source_id
   * @param description
   * @return
   */
  virtual auto configure_source(const uuid_t &source_id, const MediaDescription &description) -> tl::expected<void, Error> = 0;

  /**
   * @brief remove a media source
   * @param source_id
   * @return
   */
  virtual auto remove_source(const uuid_t &source_id) -> tl::expected<void, Error> = 0;

  /**
   * @brief add a transform
   * @param source_id
   * @param description
   * @param transform_id
   * @return
   */
  virtual auto add_transform(const uuid_t &source_id, const MediaDescription &description,
                             const std::optional<uuid_t> &transform_id = std::nullopt) -> tl::expected<uuid_t, Error> = 0;

  /**
   * @brief configure a transform
   * @param transform_id
   * @param description
   * @return
   */
  virtual auto configure_transform(const uuid_t &transform_id, const MediaDescription &description) -> tl::expected<void, Error> = 0;

  /**
   * @brief remove a transform
   * @param transform_id
   * @return
   */
  virtual auto remove_transform(const uuid_t &transform_id) -> tl::expected<void, Error> = 0;

  /**
   * @brief query a media flow graph
   * @param id
   * @return
   */
  virtual auto query(const uuid_t &id) -> tl::expected<void, Error> = 0;  // should return a media flow graph

 protected:

};
}  // namespace MA

#endif  // MEDIA_AGENT_MEDIAAGENT_HPP
