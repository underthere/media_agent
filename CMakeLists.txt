cmake_minimum_required(VERSION 3.25)
project("media_agent" VERSION 0.0 LANGUAGES CXX)

# set cxx version to 20
set(CMAKE_CXX_STANDARD 20)

add_executable(media_agent sources/main.cpp)
