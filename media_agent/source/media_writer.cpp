//
// Created by underthere on 2023/7/31.
//

#include "media_writer.hpp"

#include <iostream>
#include <utility>



namespace MA {

MediaWriter::MediaWriter(boost::asio::io_context& ioc, MediaDescription desc) : ioc_(ioc), desc_(std::move(desc)) {}
MediaWriter::~MediaWriter() {}

auto MediaWriter::start(boost::signals2::signal<void(AVPacket*)> &sig) -> tl::expected<void, Error> {
  int ret;
  avformat_alloc_output_context2(&fctx_, nullptr, "flv", desc_.uri.c_str());
  if (!fctx_) {
    return tl::make_unexpected(Error{.code = -1, .message = "failed to alloc output context"});
  }
  avformat_new_stream(fctx_, nullptr);

  ret = avio_open2(&fctx_->pb, desc_.uri.c_str(), AVIO_FLAG_WRITE, nullptr, nullptr);
  if (ret < 0) {
    return tl::make_unexpected(Error{.code = -1, .message = "failed to open output file"});
  }

  ret = avformat_write_header(fctx_, nullptr);
  if (ret < 0) {
    return tl::make_unexpected(Error{.code = -1, .message = "failed to write header"});
  }

  sig.connect([this](AVPacket* pkt) { on_new_packet(pkt); });

  return {};
}


auto MediaWriter::on_new_packet(AVPacket* pkt) -> void {
  int ret = av_interleaved_write_frame(fctx_, pkt);
  if (ret < 0) {
    std::cout << "write frame failed: " << av_err2str(ret) << std::endl;
  }
  av_packet_unref(pkt);
}

}  // namespace MA