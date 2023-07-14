cmake_minimum_required(VERSION 3.25)
project("media_agent" VERSION 0.0 LANGUAGES CXX)

# set cxx version to 20
set(CMAKE_CXX_STANDARD 23)
find_package(PkgConfig)

pkg_check_modules(GST REQUIRED gstreamer-1.0)

file (GLOB srcs sources/*.cpp)

add_executable(media_agent ${srcs} playground/main.cpp) # sources/mediaagent.cpp sources/mediaagent.h sources/media_common.h sources/agentgstimpl.cpp sources/agentgstimpl.h sources/expected.h)

message(STATUS "GST_INCLUDE_DIRS: ${GST_INCLUDE_DIRS}")

target_include_directories(media_agent PRIVATE ${GST_INCLUDE_DIRS})
target_link_directories(media_agent PRIVATE ${GST_LIBRARY_DIRS})

target_link_libraries(media_agent PRIVATE ${GST_LIBRARIES})


add_subdirectory(playground)