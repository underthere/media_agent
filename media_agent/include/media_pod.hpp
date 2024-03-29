//
// Created by underthere on 2023/8/7.
//

#ifndef MEDIA_AGENT_MEIDA_POD_HPP
#define MEDIA_AGENT_MEIDA_POD_HPP

#include <list>
#include <memory>
#include <string>
#include <unordered_map>

#include "async_simple/coro/Lazy.h"
#include "common/media_common.hpp"
#include "readers/basic_reader.hpp"
#include "tl/expected.hpp"
#include "utils/signals.hpp"
#include "writers/basic_writer.hpp"

using namespace async_simple;

namespace MA {

using MEDIA_READER_T = BasicReader;
using MEDIA_WRITER_T = BasicWriter;
using MEDIA_TRANSOFRMER_T = void;

class MediaPod {
  using link_type = std::pair<uuid_t, signals::slot>;
 private:
  std::string id_;
  MediaDescription source_desc_;

  std::shared_ptr<MEDIA_READER_T> source_reader_;

  std::unordered_map<std::string, std::shared_ptr<MEDIA_TRANSOFRMER_T>> transformers_;
  std::unordered_map<std::string, std::shared_ptr<MEDIA_WRITER_T>> writers_;

  std::unordered_map<std::string, std::list<link_type>> links_tos_;

 public:
  explicit MediaPod(const std::string& id, const MediaDescription& desc);
  virtual ~MediaPod();
  auto run() -> coro::Lazy<tl::expected<void, Error>>;
  auto add_output(const uuid_t& id, const MediaDescription& desc) -> tl::expected<std::string, Error>;
};
}  // namespace MA

#endif  // MEDIA_AGENT_MEIDA_POD_HPP
