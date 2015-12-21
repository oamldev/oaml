cmake_minimum_required(VERSION 2.8)

project (oaml)
set(CMAKE_BUILD_TYPE Release)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -Wextra")

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
add_library(oaml STATIC src/ByteBuffer.cpp src/oaml.cpp src/oamlAudio.cpp src/oamlTrack.cpp src/wav.cpp src/gettime.cpp)

add_executable(oamlTest src/ByteBuffer.cpp src/oaml.cpp src/oamlAudio.cpp src/oamlTrack.cpp src/wav.cpp src/gettime.cpp src/test.cpp)
