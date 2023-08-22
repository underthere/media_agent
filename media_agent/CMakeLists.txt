cmake_minimum_required(VERSION 3.18)
project("media_agent" VERSION 0.0 LANGUAGES CXX)

# set cxx version to 20
set(CMAKE_CXX_STANDARD 20)

set(EXE "media_agent")

file(GLOB source_files source/*.cpp)
message(STATUS " source files: ${source_files}")

find_package(PkgConfig REQUIRED)
pkg_check_modules(ffmpeg REQUIRED IMPORTED_TARGET libavcodec libavdevice libavfilter libavformat libavutil libpostproc libswresample libswscale)

find_package(nlohmann_json REQUIRED)
find_package(fmt REQUIRED)

set(async_simple "/usr/local/lib/libasync_simple.a")
set(dep_libs pthread uuid nlohmann_json::nlohmann_json fmt::fmt ${async_simple})

add_executable(${EXE} ${source_files})
target_include_directories(${EXE} PRIVATE include)
target_link_libraries(${EXE} PUBLIC PkgConfig::ffmpeg ${dep_libs})
