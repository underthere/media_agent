#include <filesystem>
#include <iostream>

#include "media_common.hpp"
#include "media_reader.hpp"
#include "mediaagent.hpp"
#include "misc.hpp"

#include "boost/signals2.hpp"
#include "boost/asio.hpp"

int asio_main();

int main() {
  boost::asio::io_context ioc;

  ioc.post([]() {
    asio_main();
  });

  ioc.run();

  return 0;
}

int asio_main() {
  return 0;
}