#include <fstream>
#include <memory>
#include <string>

#include "async_simple/coro/SyncAwait.h"
#include "cinatra.hpp"
#include "common/av_misc.hpp"
#include "http_facade/http_facade.hpp"
#include "media_agent_impl_ff.hpp"
#include "mediaagent.hpp"
#include "nlohmann/json.hpp"
#include "spdlog/spdlog.h"
#include "utils/argparse.hpp"

using namespace std::chrono_literals;
using namespace async_simple;


auto preload_legacy(const std::shared_ptr<MA::MediaAgent>& ma, const std::string& preload_filepath) -> void {
  auto real_preload_path = preload_filepath;
  std::ifstream preload_file(real_preload_path);
  if (!preload_file.good()) {
    spdlog::info("preload file {} not found", real_preload_path);
    return;
  }
  nlohmann::json preload_config;
  preload_file >> preload_config;

  if (!preload_config.contains("sources")) return;
  auto sources = preload_config["sources"];
  for (auto& j : sources) {
    try {
      auto id = j["id"].get<std::string>();
      auto ret = ma->add_source(json_to_media_desc(j["media_description"]), id);
      if (ret.has_value()) {
        spdlog::info("preload: source {} added !", ret.value());
      } else {
        spdlog::error("preload: source {} added failed !", id);
      }
    } catch (std::exception& e) {
      spdlog::error("preload: source {} added failed !", e.what());
    }
  }

  if (!preload_config.contains("transforms")) return;
  auto transforms = preload_config["transforms"];
  for (auto& j : transforms) {
    try {
      auto source_id = j["source_id"].get<std::string>();
      auto transform_id = j["transform_id"].get<std::string>();
      auto ret = ma->add_transform(source_id, json_to_media_desc(j["media_description"]), transform_id);
      if (ret.has_value()) {
        spdlog::info("preload: transform {} added !", ret.value());
      } else {
        spdlog::error("preload: transform {} added failed !", transform_id);
      }
    } catch (std::exception& e) {
      spdlog::error("preload: transform {} added failed !", e.what());
    }
  }
}

auto async_main(int argc, const char** argv) -> coro::Lazy<int> {
  cmdline::parser parser;
  parser.add<std::string>("legacy-preload", 'l', "legacy preload filepath", false, "");
  if (!parser.parse(argc, argv)) {
    std::cerr << parser.usage();
    co_return 1;
  }

  std::shared_ptr<MA::MediaAgent> agent = std::make_shared<MA::MediaAgentImplFF>();
  agent->init();

  preload_legacy(agent, parser.get<std::string>("legacy-preload"));

  HttpFacade facade{agent};

  std::thread t([&facade]() {
    facade.init();
    facade.start();
  });

  while (true) {
    co_await coro::sleep(100ms);
  }
}

int main(int argc, const char** argv) { return async_simple::coro::syncAwait(async_main(argc, argv)); }
