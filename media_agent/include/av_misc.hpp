//
// Created by underthere on 2023/7/28.
//

#ifndef MEDIA_AGENT_AV_MISC_HPP
#define MEDIA_AGENT_AV_MISC_HPP

#include <string>
extern "C" {
#include "libavutil/avutil.h"
#include "libavutil/time.h"
}

#include "media_common.hpp"
#include "nlohmann/json.hpp"

using json = nlohmann::json;
using namespace nlohmann::literals;

inline auto rotional2double(const AVRational& r) -> double {
  if (r.den == 0 || r.num == 0) return 0;
  return (1.0 * r.num) / r.den;
}

inline auto timebase2us(const AVRational& r) -> std::int64_t { return static_cast<std::int64_t>(1e6 * rotional2double(r)); }

inline auto protocol_as_string(const MA::MediaProtocol& protocol) -> std::string {
  switch (protocol) {
    case MA::MediaProtocol::FILE:
      return "file";
    case MA::MediaProtocol::RTSP:
      return "rtsp";
    case MA::MediaProtocol::RTMP:
      return "rtmp";
    case MA::MediaProtocol::V4l2:
      return "v4l2";
    case MA::MediaProtocol::CUSTOM:
      return "custom";
    default:
      return "unknown";
  }
}

inline auto profile_to_string(const MA::Profile& profile) -> std::string {
  switch (profile) {
    case MA::Profile::H264_BASELINE:
      return "baseline";
    case MA::Profile::H264_HIGH:
      return "high";
    default:
      return "unknown";
  }
}

inline auto pixel_format_to_string(const MA::PixelFormat& format) -> std::string {
  switch (format) {
    case MA::PixelFormat::YUV420P:
      return "yuv420p";
    case MA::PixelFormat::RGB24:
      return "rgb24";
    default:
      return "unknown";
  }
}

inline auto codec_format_to_string(const MA::CodecFormat& format) -> std::string {
  switch (format) {
    case MA::CodecFormat::H264:
      return "h264";
    case MA::CodecFormat::H265:
      return "h265";
    case MA::CodecFormat::AV1:
      return "av1";
    default:
      return "unknown";
  }
}

inline auto video_desc_to_json(const MA::VideoDescription& desc) {
  return json{
      {"width", desc.width},
      {"height", desc.height},
      {"fps", desc.fps},
      {"pixel_format", pixel_format_to_string(desc.pixel_format)},
      {"codec_format", codec_format_to_string(desc.codec_format)},
      {"profile", profile_to_string(desc.profile)},
      {"level", desc.level},
      {"bitrate", desc.bitrate},
  };
}

inline auto opt_video_desc_to_json(const std::optional<MA::VideoDescription>& desc) {
  if (!desc.has_value()) return json{};
  return video_desc_to_json(desc.value());
}

inline auto media_desc_to_json(const MA::MediaDescription& desc) {
  return json{
      {"uri", desc.uri},
      {"protocol", protocol_as_string(desc.protocol)},
      {"video_description", opt_video_desc_to_json(desc.video_description)},
  };
}

inline auto protocol_from_string(const std::string& p) -> MA::MediaProtocol {
  if (p == "file") return MA::MediaProtocol::FILE;
  if (p == "rtsp") return MA::MediaProtocol::RTSP;
  if (p == "rtmp") return MA::MediaProtocol::RTMP;
  if (p == "v4l2") return MA::MediaProtocol::V4l2;
  if (p == "custom") return MA::MediaProtocol::CUSTOM;
  return MA::MediaProtocol::CUSTOM;
}

inline auto profile_from_string(const std::string& p) -> MA::Profile {
  if (p == "baseline") return MA::Profile::H264_BASELINE;
  if (p == "high") return MA::Profile::H264_HIGH;
  return MA::Profile::UNKNOWN;
}

inline auto pixel_format_from_string(const std::string& p) -> MA::PixelFormat {
  if (p == "yuv420p") return MA::PixelFormat::YUV420P;
  if (p == "rgb24") return MA::PixelFormat::RGB24;
  return MA::PixelFormat::YUV420P;
}

inline auto codec_format_from_string(const std::string& p) -> MA::CodecFormat {
  if (p == "h264") return MA::CodecFormat::H264;
  if (p == "h265") return MA::CodecFormat::H265;
  if (p == "av1") return MA::CodecFormat::AV1;
  return MA::CodecFormat::H264;
}

inline auto json_to_video_desc(const json& j) -> MA::VideoDescription {
  return MA::VideoDescription{
      .width = j["width"].get<uint32_t>(),
      .height = j["height"].get<uint32_t>(),
      .fps = j["fps"].get<uint32_t>(),
      .pixel_format = pixel_format_from_string(j["pixel_format"].get<std::string>()),
      .codec_format = codec_format_from_string(j["codec_format"].get<std::string>()),
      .profile = profile_from_string(j["profile"].get<std::string>()),
      .level = j["level"].get<uint32_t>(),
      .bitrate = j["bitrate"].get<uint32_t>(),
  };
}

inline auto json_to_opt_video_desc(const json& j) -> std::optional<MA::VideoDescription> {
  if (j.empty()) return std::nullopt;
  return json_to_video_desc(j);
}

inline auto json_to_media_desc(const json& j) -> MA::MediaDescription {
  return MA::MediaDescription{
      .uri = j["uri"].get<std::string>(),
      .protocol = protocol_from_string(j["protocol"].get<std::string>()),
      .video_description = json_to_opt_video_desc(j.at("video_description")),
  };
}

#endif  // MEDIA_AGENT_AV_MISC_HPP
