//
// Created by underthere on 2023/8/13.
//

#ifndef MEDIA_AGENT_HTTP_FACADE_HPP
#define MEDIA_AGENT_HTTP_FACADE_HPP

#include "common/media_common.hpp"
#include "cinatra.hpp"

class HttpFacade {
  static constexpr unsigned int DEFAULT_THREADS=2;
 public:
  explicit HttpFacade(int port = 18080);
  ~HttpFacade() = default;

 auto start() -> int;
 auto stop() -> int ;


 auto init() -> void;
 auto deinit() -> void;

 private:
  int port_;
  cinatra::http_server server_;
};

#endif  // MEDIA_AGENT_HTTP_FACADE_HPP
