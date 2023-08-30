//
// Created by underthere on 2023/8/13.
//


#include "nlohmann/json.hpp"
#include "http_facade/http_facade.hpp"
#include "common/av_misc.hpp"
using namespace cinatra;

HttpFacade::HttpFacade(std::shared_ptr<MA::MediaAgent> ma, int port) : port_(port), ma_(ma), server_(HttpFacade::DEFAULT_THREADS) {}

auto HttpFacade::start() -> int {
  server_.listen("0.0.0.0", std::to_string(port_));
  server_.run();
  return 0;
}

auto HttpFacade::init() -> void {
  // restful methods
  server_.set_http_handler<GET, POST>("/", [](auto &&req, auto &&res) { res.set_status_and_content(status_type::ok, "hello world"); });

  server_.set_http_handler<POST>("/add_source", [](request &req, response &res) {
      auto body = req.body();
      try{
        auto json = nlohmann::json::parse(body);
        std::string id {json.at("id").get<std::string>()};
        auto desc = json_to_media_desc(json.at("media_description"));
      } catch (std::exception& e) {
        res.set_status_and_content(status_type::bad_request, e.what());
        return;
      }
      res.set_status_and_content(status_type::ok, "ok");
  });
}

auto HttpFacade::deinit() -> void {}
