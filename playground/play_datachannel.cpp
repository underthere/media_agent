//
// Created by  on 2023/8/18.
//

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

extern "C" {
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavutil/avutil.h"
}

#include <async_simple/coro/Sleep.h>
#include <async_simple/coro/SyncAwait.h>

#include <memory>
#include <optional>

#include "cinatra.hpp"
#include "nlohmann/json.hpp"
#include "rtc/rtc.hpp"
#include "spdlog/spdlog.h"

#define fn auto

fn async_main()->async_simple::coro::Lazy<void> {
  spdlog::info("start");
  cinatra::coro_http_client client{};

  std::optional<rtc::Description> opt_sdp = std::nullopt;

  auto pc = std::make_shared<rtc::PeerConnection>();

  pc->onStateChange([](auto state) { spdlog::info("state: {}", state); });
  pc->onGatheringStateChange([pc, &opt_sdp](rtc::PeerConnection::GatheringState state) {
    std::cout << "gathering state: " << state << std::endl;
    if (state == rtc::PeerConnection::GatheringState::Complete) {
      auto desc = pc->localDescription();
      nlohmann::json msg = {
          {"type", desc->typeString()},
          {"sdp", std::string(desc.value())},
      };
      opt_sdp = desc;
    }
  });

  int sock = socket(AF_INET, SOCK_DGRAM, 0);
  struct sockaddr_in addr {};
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = inet_addr("127.0.0.1");
  addr.sin_port = htons(6000);

  if (bind(sock, reinterpret_cast<struct sockaddr *>(&addr), sizeof(addr)) < 0) {
    spdlog::error("bind failed");
    co_return;
  }

  int recv_buf_size = 1024 * 1024 * 10;
  setsockopt(sock, SOL_SOCKET, SO_RCVBUF, reinterpret_cast<const char *>(&recv_buf_size), sizeof(recv_buf_size));

  const std::string stream_path {"cllive/rtctest"};
  const rtc::SSRC ssrc{45};
  rtc::Description::Video media("video", rtc::Description::Direction::SendOnly);
  media.addH264Codec(96);
  media.addSSRC(ssrc, stream_path, stream_path);
  auto track = pc->addTrack(media);
  pc->setLocalDescription();

  while (!opt_sdp.has_value()) {
    spdlog::info("wait for sdp");
    co_await async_simple::coro::sleep(std::chrono::seconds(1));
  }

  client.add_header("Content-Type", "application/sdp");

  auto app_sdp_obj = opt_sdp.value();
  auto app_sdp = app_sdp_obj.generateSdp();
  spdlog::info("local sdp:\n{}", app_sdp);

  auto resp = co_await client.async_post("http://192.168.10.201:1985/rtc/v1/whip/?app=cllive&stream=rtctest", app_sdp, cinatra::TEXT);
  if (resp.status < 200 || resp.status >= 300) {
    spdlog::warn("req failed!");
    co_return;
  }

  spdlog::info("resp: {}, remote sdp:\n{}", resp.status, resp.resp_body);
  std::string remote_sdp {resp.resp_body};
  rtc::Description answer(remote_sdp, "answer");
  pc->setRemoteDescription(answer);

  char buffer[2048];
  int len;
  while ((len = recv(sock, buffer, 2048, 0)) >= 0) {
    if (len < sizeof(rtc::RtpHeader) || !track->isOpen())
      continue;
    auto rtp = reinterpret_cast<rtc::RtpHeader*>(buffer);
    rtp->setSsrc(ssrc);
    spdlog::info("send rtp");
    track->send(reinterpret_cast<const std::byte*>(buffer), len);
  }

  spdlog::info("end");
  co_return;
}

int main() {
  async_simple::coro::syncAwait(async_main());
  return 0;
}