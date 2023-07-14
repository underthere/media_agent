//
// Created by underthere on 2023/7/13.
//

#include <iostream>
#include "httplib.h"

constexpr std::string_view SRS_SERVER = "dev.smt.dyinnovations.com";
constexpr int SRS_PORT = 1985;

int main() {

  auto client = std::make_shared<httplib::Client>(std::string(SRS_SERVER), SRS_PORT);

  client->set_follow_location(true);
  client->Post("/rtc/v1/whip", "name=live&stream=cltest", "application/sdp");
//  auto res = client->Post("/rtc/v1/whip/?app=live&stream=cltest");



  return 0;
}