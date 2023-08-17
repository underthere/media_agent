#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "fmt/format.h"
#include "async_simple/coro/Lazy.h"
#include "async_simple/coro/SyncAwait.h"

using namespace async_simple::coro;

using Texts = std::vector<std::string>;
#define fn auto
#define let auto

fn ReadFile(const std::string &filename) -> Lazy<Texts> {
    let resutl = Texts{};
    let infile = std::ifstream{filename};

    let line = std::string{};
    while (std::getline(infile, line)) {
        resutl.push_back(line);
    }
    co_return resutl;
}

fn CoutLineChar(const std::string &line, char c) -> Lazy<int> {
    co_return std::count(line.begin(), line.end(), c);
}

fn CoutChar(const Texts &texts, char c) -> Lazy<int> {
    let result = 0;
    for (const auto &line : texts) {
        result += co_await CoutLineChar(line, c);
    }
    co_return result;
}

fn CountFileCharNum(const std::string &filename, char c) -> Lazy<int> {
    let texts = co_await ReadFile(filename);
    co_return co_await CoutChar(texts, c);
}

int main() {
    int Num = syncAwait(CountFileCharNum("/workdir/playground/play_async.cpp", 'a'));
    std::cout << fmt::format("Num: {}", Num) << std::endl;
    return 0;
}