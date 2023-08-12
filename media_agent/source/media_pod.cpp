//
// Created by underthere on 2023/8/7.
//

#include "media_pod.hpp"

namespace MA {

MediaPod::MediaPod(const std::string &id, const MA::MediaDescription &desc): id_(id), source_desc_(desc) {

}

MediaPod::~MediaPod() {}

auto MediaPod::execute() -> tl::expected<void, Error> { return tl::expected<void, Error>(); }


}  // namespace MA