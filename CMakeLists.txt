cmake_minimum_required(VERSION 3.18)
project("media_agent" VERSION 0.0 LANGUAGES CXX)

# set cxx version to 20
set(CMAKE_CXX_STANDARD 20)

set(CMAKE_THREAD_LIBS_INIT "-lpthread")

include(FetchContent)

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

FETCHCONTENT_DECLARE(
        fmt
        GIT_REPOSITORY https://github.com/fmtlib/fmt.git
        GIT_TAG 10.0.0
)

#FETCHCONTENT_DECLARE(
#        cpprestsdk
#        GIT_REPOSITORY https://github.com/microsoft/cpprestsdk.git
#        GIT_TAG v2.10.18
#)

FETCHCONTENT_DECLARE(
        spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_TAG v1.12.0
)

FETCHCONTENT_MAKEAVAILABLE(argparse boost fmt spdlog)

if (DEFINED ENV{ROCKCHIP})
    message(INFO "build on rockchip")
    set(FFMPEG_HOME "${CMAKE_CURRENT_LIST_DIR}" /thirdparty/ffmpeg_rk_build)
    set(FFMPEG_LIBRARIES avcodec avdevice avfilter avformat avutil postproc swresample swscale drm rockchip_mpp)
else()
    set(FFMPEG_HOME "${CMAKE_CURRENT_LIST_DIR}/thirdparty/ffmpeg_build")
    set(FFMPEG_LIBRARIES avcodec avdevice avfilter avformat avutil postproc swresample swscale)
endif()
set(FFMPEG_INCLUDE_DIRS "${FFMPEG_HOME}/include")
set(FFMPEG_LIBRARY_DIRS "${FFMPEG_HOME}/lib")

set(TL_EXPECTED_HOME "${CMAKE_CURRENT_LIST_DIR}/thirdparty/expected")
set(TL_EXPECTED_INCLUDE_DIRS "${TL_EXPECTED_HOME}/include")

add_subdirectory(playground)
add_subdirectory(media_agent)
