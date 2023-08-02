//
// Created by underthere on 2023/8/1.
//

#include "decoder.hpp"

#include <utility>
namespace MA {
Decoder::Decoder(MA::MediaDescription input, HardwareAccelerator hw) : input_desc_(std::move(input)), hw_(hw) {}
Decoder::~Decoder() {}
auto Decoder::on_new_packet(AVPacket *) -> void {}
auto Decoder::start() -> tl::expected<void, Error> {
  if (hw_ == HardwareAccelerator::VAAPI) {
    auto ret = av_hwdevice_ctx_create(&hw_device_ctx_, AV_HWDEVICE_TYPE_VAAPI, nullptr, nullptr, 0);
    if (ret < 0) {
      return tl::make_unexpected(Error{.code = ret, .message = "Failed to create VAAPI device context"});
    }

    auto type = av_hwdevice_find_type_by_name("vaapi");
    const auto decoder = avcodec_find_decoder_by_name("h264_vaapi");
    const AVCodecHWConfig *cfg = avcodec_get_hw_config(decoder, i);
  }
  return {};
}
}  // namespace MA
