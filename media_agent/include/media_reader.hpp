//
// Created by underthere on 2023/7/28.
//

#ifndef MEDIA_AGENT_MEDIA_READER_HPP
#define MEDIA_AGENT_MEDIA_READER_HPP

extern "C" {
#include "libavformat/avformat.h"
}
#include "boost/asio.hpp"
#include "boost/signals2.hpp"
#include "media_common.hpp"
#include "tl/expected.hpp"

namespace MA {

class MediaReader {
 public:
  explicit MediaReader(boost::asio::io_context &ioc, MediaDescription desc);
  virtual ~MediaReader();

  virtual auto start() -> tl::expected<void, Error>;
  virtual auto read() -> tl::expected<AVPacket *, Error>;

 protected:
  virtual auto read_handler(AVPacket *delayed_pkt = nullptr) -> void;
 private:
  MediaDescription desc_;
  AVFormatContext *fctx_;
  int best_video_index_;

  std::int64_t start_time_;
  boost::asio::steady_timer timer_;
  boost::asio::io_context &ioc_;
  boost::signals2::signal<void(AVPacket *)> sig_new_packet_;
};

}  // namespace MA

#endif  // MEDIA_AGENT_MEDIA_READER_HPP
