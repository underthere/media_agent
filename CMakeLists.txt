cmake_minimum_required(VERSION 3.18)
project("media_agent" VERSION 0.0 LANGUAGES CXX)

# set cxx version to 20
set(CMAKE_CXX_STANDARD 20)

set(CMAKE_THREAD_LIBS_INIT "-lpthread")

include_directories(include)

add_subdirectory(media_agent)
add_subdirectory(playground)