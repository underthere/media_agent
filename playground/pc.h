//
// Created by underthere on 2023/7/14.
//

#ifndef MEDIA_AGENT_PC_H
#define MEDIA_AGENT_PC_H

#include <string>
#include <optional>

// Parser a :: String -> Maybe (a, String)
using parser_input = std::string_view;
template <typename T>
using parser_result = std::optional<std::pair<T, parser_input>>;
template <typename T>
using parser = auto(*)(parser_input) -> parser_result<T>;

const auto make_char_parser(const char& c) {
  return [=] (parser_input s) -> parser_result<char> {
    if (s.empty() || c != s[0]) return std::nullopt;
    return std::make_pair(s[0], s.substr(1));
  };
}

const auto one_of(const std::string_view chars){
  return [=](parser_input s) -> parser_result<char> {
    if (s.empty() || chars.find(s[0]) == std::string::npos) return std::nullopt;
    return std::make_pair(s[0], s.substr(1));
  };
}

// combine :: Parser a -> Parser b -> (a -> b -> c) -> Parser c
template <typename P1, typename P2, typename F,
    typename R = std::invoke_result_t<F, parser<P1>, parser<P2>>>
constexpr auto combine(P1&& p1, P2&& p2, F&& f) {
  return [=](parser_input s) -> parser_result<R> {
    auto r1 = p1(s);
    if (!r1) return std::nullopt;
    auto r2 = p2(r1->second);
    if (!r2) return std::nullopt;
    return std::make_pair(f(r1->first, r2->first), r2->second);
  };
}

// (>) :: Parser a -> Parser b -> Parser a
template<typename P1, typename P2>
constexpr auto operator>(P1&& p1, P2&& p2) {
  return combine(std::forward<P1>(p1),
                 std::forward<P2>(p2),
                 [](auto&& l, auto) { return l; });
};

// (<) :: Parser a -> Parser b -> Parser b
template<typename P1, typename P2>
constexpr auto operator<(P1&& p1, P2&& p2) {
  return combine(std::forward<P1>(p1),
                 std::forward<P2>(p2),
                 [](auto, auto&& r) { return r; });
};

// foldl :: Parser a -> b -> (b -> a -> b) -> ParserInput -> ParserResult b
template<typename P, typename R, typename F>
constexpr auto foldl(P&& p, R acc, F&& f, parser_input in)
    -> parser_result<R> {
  while (true) {
    auto r = p(in);
    if (!r) return std::make_pair(acc, in);
    acc = f(acc, r->first);
    in = r->second;
  }
};

// many :: Parser a -> Parser monostate
template<typename P>
constexpr auto many(P&& p) {
  return [p=std::forward<P>(p)](parser_input s) -> parser_result<std::monostate> {
    return foldl(p, std::monostate{}, [](auto, auto) { return std::monostate{}; }, s);
  };
}

// at_least :: Parser a -> b -> (b -> a -> b) -> Parser b
template<typename P, typename R, typename F>
constexpr auto at_least(P&& p, R&& init, F&& f) {
  static_assert(std::is_same_v<std::invoke_result_t<F, R, parser<P>>, R>,
                "type mismatch!");
  return [p=std::forward<P>(p),
          f=std::forward<F>(f),
          init=std::forward<R>(init)](parser_input s) -> parser_result<R> {
    auto r = p(s);
    if (!r) return std::nullopt;
    return foldl(p, f(init, r->first), f, r->second);
  };
};

// zero_or_one :: Parser a -> a -> Parser a
template<typename P, typename R = parser<P>>
constexpr auto zero_or_one(P&& p, R&& init) {
  return [=](parser_input s) -> parser_result<R> {
      auto r = p(s);
      if (!r) return std::make_pair(init, s);
      return r;
    };
};

#endif  // MEDIA_AGENT_PC_H
