cmake_minimum_required(VERSION 3.25)
project("playground" VERSION 0.0 LANGUAGES CXX)

# set cxx version to 20
set(CMAKE_CXX_STANDARD 23)

include(FetchContent)
FETCHCONTENT_DECLARE(
        httplib
        GIT_REPOSITORY https://github.com/yhirose/cpp-httplib.git
        GIT_TAG v0.13.1
)

FETCHCONTENT_DECLARE(
        argparse
        GIT_REPOSITORY https://github.com/p-ranav/argparse.git
        GIT_TAG v2.9
)

FETCHCONTENT_MAKEAVAILABLE(httplib argparse)

add_executable(playground main.cpp play_ffmpeg.cpp)
target_link_libraries(playground httplib::httplib)

add_executable(pc pc_test.cpp)

find_package(PkgConfig REQUIRED)
pkg_check_modules(ffmpeg REQUIRED IMPORTED_TARGET libavcodec libavformat libavutil libswscale libswresample)
add_executable(play_ffmpeg play_ffmpeg.cpp)
target_link_libraries(play_ffmpeg PkgConfig::ffmpeg argparse::argparse)
