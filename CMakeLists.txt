cmake_minimum_required(VERSION 3.25)
project("media_agent" VERSION 0.0 LANGUAGES CXX)

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

FETCHCONTENT_DECLARE(
        boost
        GIT_REPOSITORY https://github.com/boostorg/boost.git
        GIT_TAG boost-1.82.0
)

FETCHCONTENT_MAKEAVAILABLE(httplib argparse boost)


set(FFMPEG_HOME "${CMAKE_CURRENT_LIST_DIR}/thirdparty/ffmpeg_build")
set(FFMPEG_INCLUDE_DIRS "${FFMPEG_HOME}/include")
set(FFMPEG_LIBRARY_DIRS "${FFMPEG_HOME}/lib")
set(FFMPEG_LIBRARIES avcodec avdevice avfilter avformat avutil postproc swresample swscale)

set(TL_EXPECTED_HOME "${CMAKE_CURRENT_LIST_DIR}/thirdparty/expected")
set(TL_EXPECTED_INCLUDE_DIRS "${TL_EXPECTED_HOME}/include")

add_subdirectory(playground)
add_subdirectory(media_agent)