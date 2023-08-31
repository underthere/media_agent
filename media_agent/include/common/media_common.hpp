//
// Created by underthere on 2023/6/1.
//

#ifndef MEDIA_AGENT_MEDIA_COMMON_HPP
#define MEDIA_AGENT_MEDIA_COMMON_HPP

#include <optional>
#include <string>

namespace MA {

using uuid_t = std::string;

enum class HardwareAccelerator { NONE, VAAPI, RKMPP };

enum class ErrorType {
  EOS = -1,
  OK = 0,
  UNKNOWN = 9999,
};

struct Error {
  ErrorType code;
  std::string message;
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

enum class CodecFormat { H264, H265, AV1 };

enum class Profile {
  H264_BASELINE,
  H264_HIGH,
  UNKNOWN,
};

enum class Level {
  UNKNOWN,
};

struct VideoDescription {
  uint32_t width;
  uint32_t height;
  uint32_t fps;

  PixelFormat pixel_format;
  CodecFormat codec_format;
  std::optional<Profile> profile = std::nullopt;
  std::optional<uint32_t> level = std::nullopt;
  std::optional<std::uint64_t> bitrate = std::nullopt;  // in bps
};

struct AudioDescription {};

struct MediaDescription {
  MediaProtocol protocol;
  std::string uri;
  std::optional<VideoDescription> video_description = std::nullopt;
  std::optional<AudioDescription> audio_description = std::nullopt;
  std::optional<void*> custom_description = std::nullopt;
};

enum class MediaBufferType {
  FF_PACKET,
  FF_FRAME,
};

struct MediaBuffer {
  MediaBufferType type;
  void* data;
  void* codec_par;
  void* codec_ctx;
};
}  // namespace MA

#endif  // MEDIA_AGENT_MEDIA_COMMON_HPP
