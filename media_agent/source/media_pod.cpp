//
// Created by underthere on 2023/8/7.
//

#include "media_pod.hpp"

using namespace async_simple;

namespace MA {

MediaPod::MediaPod(const std::string &id, const MA::MediaDescription &desc) : id_(id), source_desc_(desc) {
  this->source_reader_ = std::make_shared<MEDIA_READER_T>(desc);
}

MediaPod::~MediaPod() {}

auto MediaPod::run() -> coro::Lazy<tl::expected<void, Error>> { co_return co_await this->source_reader_->run(); }

auto MediaPod::add_output(const uuid_t &id, const MediaDescription &desc) -> tl::expected<std::string, Error> {
  auto writer = std::make_shared<MEDIA_WRITER_T>(desc);
  auto conn = source_reader_->sig_new_packet_.connect([writer_raw = writer.get()](auto &&PH1, auto &&PH2) {
    writer_raw->slot_new_packet(std::forward<decltype(PH1)>(PH1), std::forward<decltype(PH2)>(PH2));
  });
  this->writers_.emplace(id, writer);
  this->links_tos_[id_].push_back({id, conn});
  return id;
}

}  // namespace MA