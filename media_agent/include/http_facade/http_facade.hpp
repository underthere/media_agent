//
// Created by underthere on 2023/8/13.
//

#ifndef MEDIA_AGENT_HTTP_FACADE_HPP
#define MEDIA_AGENT_HTTP_FACADE_HPP

#include "cinatra.hpp"
#include "common/media_common.hpp"
#include "mediaagent.hpp"


class HttpFacade {
  static constexpr unsigned int DEFAULT_THREADS=2;
 public:
  HttpFacade(std::shared_ptr<MA::MediaAgent> ma, int port = 18080);
  ~HttpFacade() = default;

 auto start() -> int;
 auto stop() -> int ;


 auto init() -> void;
 auto deinit() -> void;

 private:
  int port_;
  std::shared_ptr<MA::MediaAgent> ma_;
  cinatra::http_server server_;
};

#endif  // MEDIA_AGENT_HTTP_FACADE_HPP
