
#include <memory>
#include <iostream>

#include "media_common.hpp"
#include "media_reader.hpp"

#include "boost/asio.hpp"

int main() {
  boost::asio::io_context ioc;
  MA::MediaDescription desc = {
      .uri = "/Users/chenlong/Documents/media_agent/test.flv",
  };
  auto reader = std::make_shared<MA::MediaReader>(ioc, desc);
  auto ret = reader->start();
  if (ret.has_value()) {
    std::cout << "start success" << std::endl;
    ioc.run();
  } else {
    std::cout << "start failed: " << ret.error().message << std::endl;
  }
  return 0;
}
