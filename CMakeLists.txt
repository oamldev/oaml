cmake_minimum_required(VERSION 2.8)

project (oaml)
set(CMAKE_BUILD_TYPE Release)

if (MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
else()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -Wextra")
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
set(SRCS src/ByteBuffer.cpp src/oaml.cpp src/oamlAudio.cpp src/oamlBase.cpp src/oamlTrack.cpp src/wav.cpp src/gettime.cpp src/tinyxml2.cpp)
add_library(oaml STATIC ${SRCS})

add_executable(oamlTest ${SRCS} src/test.cpp)
