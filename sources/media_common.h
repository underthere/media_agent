//
// Created by underthere on 2023/6/1.
//

#ifndef MEDIA_AGENT_MEDIA_COMMON_H
#define MEDIA_AGENT_MEDIA_COMMON_H

#include <string>

namespace MA {

struct Error {
  int code;
  std::string message;
};

enum class MAError {

};

enum class MediaProtocol {
  FILE,
  RTSP,
  RTMP,
  V4l2,
  CUSTOM,
};

enum class PixelFormat {
  YUV420P,
  RGB24,
};

enum class CodecFormat {

};

struct VideoDescription {
  uint32_t width;
  uint32_t height;
  uint32_t fps;

  PixelFormat pixel_format;
  CodecFormat codec_format;
};

struct AudioDescription {};

struct MediaDescription {
  MediaProtocol protocol;
  std::string uri;
  std::optional<VideoDescription> video_description;
  std::optional<AudioDescription> audio_description;
  std::optional<void *> custom_description;
};
}  // namespace MA

#endif  // MEDIA_AGENT_MEDIA_COMMON_H
