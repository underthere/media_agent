//
// Created by underthere on 2023/8/7.
//

#ifndef MEDIA_AGENT_MEIDA_POD_HPP
#define MEDIA_AGENT_MEIDA_POD_HPP

#include <memory>
#include <list>
#include <unordered_map>
#include <string>

using MEDIA_READER_T = void;
using MEDIA_WRITER_T = void;
using MEDIA_TRANSOFRMER_T = void;

class MediaPod {
 private:
  std::string id_;
  std::shared_ptr<MEDIA_READER_T> source_reader_;

  std::unordered_map<std::string, std::shared_ptr<MEDIA_TRANSOFRMER_T>> transformers_;
  std::unordered_map<std::string, std::shared_ptr<MEDIA_WRITER_T>> writers_;

  std::unordered_map<std::string, std::list<std::string>> links_tos_;

 public:
  `\

};

#endif  // MEDIA_AGENT_MEIDA_POD_HPP
