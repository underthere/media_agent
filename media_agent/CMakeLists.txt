cmake_minimum_required(VERSION 3.18)
project("media_agent" VERSION 0.0 LANGUAGES CXX)

# set cxx version to 20
set(CMAKE_CXX_STANDARD 20)

set(EXE "media_agent")

file(GLOB source_files source/*.cpp include/*.hpp)

add_executable(${EXE} ${source_files})
target_include_directories(${EXE} PRIVATE include)
target_include_directories(${EXE} PUBLIC ${TL_EXPECTED_INCLUDE_DIRS} ${FFMPEG_INCLUDE_DIRS})
target_link_directories(${EXE} PUBLIC ${FFMPEG_LIBRARY_DIRS})
target_link_libraries(${EXE} PUBLIC ${FFMPEG_LIBRARIES} Boost::signals2 Boost::asio)
