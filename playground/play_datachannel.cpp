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

#include <async_simple/coro/SyncAwait.h>

#include <memory>

#include "nlohmann/json.hpp"
#include "rtc/rtc.hpp"
#include "spdlog/spdlog.h"
#include "ylt/coro_http/coro_http_client.hpp"

#define fn auto

fn async_main()->async_simple::coro::Lazy<void> {
  spdlog::info("start");
  coro_http::coro_http_client client{};

  std::optional<rtc::Description> opt_sdp = nullopt;

  auto pc = std::make_shared<rtc::PeerConnection>();

  pc->onStateChange([](auto state) { spdlog::info("state: {}", state); });
  pc->onGatheringStateChange([pc, &opt_sdp](rtc::PeerConnection::GatheringState state){
      std::cout << "gathering state: " << state << std::endl;
      if (state == rtc::PeerConnection::GatheringState::Complete) {
        auto desc = pc->localDescription();
        nlohmann::json msg = {
            {"type", desc->typeString()},
            {"sdp", std::string(desc.value())},
        };
        opt_sdp = desc;
        std::cout << msg << std::endl;
      }
  });

  int sock = socket(AF_INET, SOCK_DGRAM, 0);
  struct sockaddr_in addr {};
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = inet_addr("127.0.0.1");
  addr.sin_port = htons(8888);

  if (bind(sock, reinterpret_cast<struct sockaddr *>(&addr), sizeof(addr)) < 0) {
    spdlog::error("bind failed");
    co_return;
  }

  int recv_buf_size = 1024 * 1024 * 10;
  setsockopt(sock, SOL_SOCKET, SO_RCVBUF, reinterpret_cast<const char*>(&recv_buf_size), sizeof(recv_buf_size));

  const rtc::SSRC ssrc {45};
  rtc::Description::Video media("video", rtc::Description::Direction::SendOnly);
  media.addH264Codec(96);
  media.addSSRC(ssrc, "video-send");
  auto track = pc->addTrack(media);
  pc->setLocalDescription();

  while (!opt_sdp.has_value(0)) {
    spdlog::info("wait for sdp");
    co_await async_simple::coro::sleep_for(std::chrono::seconds(1));
  }

  client.add_header("Content-Type", "application/sdp");
  auto resp = co_await client.async_post(cinatra)

  spdlog::info(">> {}", opt_sdp.value().generateSdp());

  spdlog::info("end");
  co_return;
}

int main() {
  async_simple::coro::syncAwait(async_main());
  return 0;
}