cmake_minimum_required(VERSION 3.18)
project("playground" VERSION 0.0 LANGUAGES CXX)

# set cxx version to 20
set(CMAKE_CXX_STANDARD 20)

# add_executable(play_asio play_asio.cpp play_asio.cpp)
# target_link_libraries(play_asio PRIVATE argparse::argparse Boost::asio Boost::signals2)

# add_executable(play_signal play_signal.cpp)
# target_include_directories(play_signal PRIVATE ${FFMPEG_INCLUDE_DIRS})
# target_link_directories(play_signal PRIVATE ${FFMPEG_LIBRARY_DIRS})
# target_link_libraries(play_signal PRIVATE Boost::signals2 fmt::fmt ${FFMPEG_LIBRARIES})


# add_executable(play_rkmpp play_rkmpp.cpp)
# target_include_directories(play_rkmpp PRIVATE ${FFMPEG_INCLUDE_DIRS})
# target_link_directories(play_rkmpp PRIVATE ${FFMPEG_LIBRARY_DIRS})
# target_link_libraries(play_rkmpp PRIVATE ${FFMPEG_LIBRARIES})


# set(EXE play_thirds)
# add_executable(${EXE} play_thirds.cpp)
# target_link_libraries(${EXE} PRIVATE argparse::argparse fmt::fmt)

# set(EXE rtmp_publisher)
# add_executable(${EXE} rtmp_publisher.cpp)
# target_include_directories(${EXE} PRIVATE ${TL_EXPECTED_INCLUDE_DIRS} ${FFMPEG_INCLUDE_DIRS})
# target_link_directories(${EXE} PRIVATE ${FFMPEG_LIBRARY_DIRS})
# target_link_libraries(${EXE} PRIVATE ${FFMPEG_LIBRARIES} argparse::argparse fmt::fmt)

find_package(fmt CONFIG REQUIRED)
set(EXE play_async)
add_executable(${EXE} play_async.cpp)
target_link_libraries(${EXE} PRIVATE async_simple fmt::fmt)