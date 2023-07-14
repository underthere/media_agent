#include <filesystem>
#include <format>
#include <iostream>

int main() {
  auto p = std::filesystem::current_path();

  auto it = std::filesystem::directory_iterator(p);
  while (it != std::filesystem::directory_iterator()) {
    std::cout << it->path().filename() << std::endl;
    ++it;
  }

  return 0;
}