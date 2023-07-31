//
// Created by underthere on 2023/7/31.
//

#ifndef MEDIA_AGENT_MEDIA_WRITER_HPP
#define MEDIA_AGENT_MEDIA_WRITER_HPP

extern "C" {
#include "libavformat/avformat.h"
}

#include "boost/asio.hpp"
#include "boost/signals2.hpp"
#include "media_common.hpp"
#include "tl/expected.hpp"

namespace MA {
class MediaWriter {
  explicit MediaWriter(boost::asio::io_context &ioc, MediaDescription desc);
  virtual ~MediaWriter();

  virtual auto start(boost::signals2::signal<void(AVPacket*)> &) -> tl::expected<void, Error>;

  virtual auto on_new_packet(AVPacket*) -> void;

 private:
  MediaDescription desc_;
  AVFormatContext *fctx_;
  boost::asio::io_context &ioc_;
};
}  // namespace MA

#endif  // MEDIA_AGENT_MEDIA_WRITER_HPP
