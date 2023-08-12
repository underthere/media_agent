//
// Created by underthere on 2023/8/7.
//

#ifndef MEDIA_AGENT_MEIDA_POD_HPP
#define MEDIA_AGENT_MEIDA_POD_HPP

#include <list>
#include <memory>
#include <string>
#include <unordered_map>

#include "tl/expected.hpp"

#include "media_common.hpp"

using MEDIA_READER_T = void;
using MEDIA_WRITER_T = void;
using MEDIA_TRANSOFRMER_T = void;

namespace MA {
class MediaPod {
 private:
  std::string id_;
  MediaDescription source_desc_;

  std::shared_ptr<MEDIA_READER_T> source_reader_;

  std::unordered_map<std::string, std::shared_ptr<MEDIA_TRANSOFRMER_T>> transformers_;
  std::unordered_map<std::string, std::shared_ptr<MEDIA_WRITER_T>> writers_;

  std::unordered_map<std::string, std::list<std::string>> links_tos_;

 public:
  explicit MediaPod(const std::string& id, const MediaDescription& desc);
  virtual ~MediaPod();
  auto execute() -> tl::expected<void, Error>;
};
}  // namespace MA

#endif  // MEDIA_AGENT_MEIDA_POD_HPP
