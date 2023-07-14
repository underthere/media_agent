//
// Created by underthere on 2023/7/14.
//
#include <iostream>

#include "pc.h"

const auto sign = zero_or_one(one_of("+-"), '+');
const auto digit = one_of("1234567890");

const auto ttt = many(digit);

int main(){
  auto ttt = many(digit);
  auto r = ttt("3010203");
  if (r.has_value()){

    std::cout << r->second << std::endl;
  }
}