//
// Created by underthere on 2023/8/13.
//


#include "nlohmann/json.hpp"
#include "http_facade/http_facade.hpp"
#include "common/av_misc.hpp"
#include "spdlog/spdlog.h"
using namespace cinatra;

HttpFacade::HttpFacade(std::shared_ptr<MA::MediaAgent> ma, int port) : port_(port), ma_(ma), server_(HttpFacade::DEFAULT_THREADS) {}

auto HttpFacade::start() -> int {
  server_.listen("0.0.0.0", std::to_string(port_));
  server_.run();
  return 0;
}

auto HttpFacade::stop() -> int {
  server_.stop();
  return 0;
}

auto HttpFacade::init() -> void {
  // restful methods
  server_.set_http_handler<GET, POST>("/", [](auto &&req, auto &&res) { res.set_status_and_content(status_type::ok, "hello world"); });

  server_.set_http_handler<POST>("/add_source", [this](request &req, response &res) {
      auto body = req.body();
      try{
        auto j= nlohmann::json::parse(body);
        std::string id {j.at("id").get<std::string>()};
        auto desc = json_to_media_desc(j.at("media_description"));
        auto id_ret = ma_->add_source(desc, id);
        spdlog::info("source {} added !", id_ret.value_or("nothing"));
        nlohmann::json resp_body{};
        if (id_ret.has_value()) {
          resp_body["id"] = id_ret.value();
        } else {
          resp_body["error"] = id_ret.error().message;
        }
        res.set_status_and_content(status_type::ok, resp_body.dump());
        return;
      } catch (std::exception& e) {
        res.set_status_and_content(status_type::bad_request, e.what());
        return;
      }
  });

  server_.set_http_handler<POST>("/add_transform", [this](request &req, response &res) {
    auto body = req.body();
    try{
      auto j = nlohmann::json::parse(body);
      std::string src_id {j.at("source_id").get<std::string>()};
      std::string dst_id {j.at("transform_id").get<std::string>()};
      auto dst_desc = json_to_media_desc(j["media_description"]);
      auto id_ret = ma_->add_transform(src_id, dst_desc, dst_id);
      nlohmann::json resp_body{};
      if (id_ret.has_value()) resp_body["transform_id"] = id_ret.value();
      else resp_body["error"] = id_ret.error().message;
      res.set_status_and_content(status_type::ok, resp_body.dump());
    } catch (std::exception& e) {
      res.set_status_and_content(status_type::bad_request, e.what());
    }
  });
}

auto HttpFacade::deinit() -> void {}
