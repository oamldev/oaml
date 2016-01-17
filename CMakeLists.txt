cmake_minimum_required(VERSION 2.8)

project (oaml)
set(CMAKE_BUILD_TYPE Release)

##
# Set CXX_FLAGS depending on compiler
#
if (MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
else()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -Wextra")
endif()

##
# Find VorbisFile lib
#
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules ${CMAKE_MODULE_PATH})
#set(CMAKE_FIND_LIBRARY_SUFFIXES ".lib;.a")

find_package(OggVorbis)
find_package(VorbisFile)

if (OGGVORBIS_FOUND AND VORBISFILE_FOUND)
	include_directories(${VORBISFILE_INCLUDE_DIR})
	set(OAML_LIBS ${OAML_LIBS} ${OGG_LIBRARY} ${VORBIS_LIBRARY} ${VORBISFILE_LIBRARIES})
endif()

##
# 
#
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
set(SRCS src/audioFile.cpp src/ByteBuffer.cpp src/oaml.cpp src/oamlAudio.cpp src/oamlBase.cpp src/oamlTrack.cpp src/aif.cpp src/ogg.cpp src/wav.cpp src/gettime.cpp src/tinyxml2.cpp)
add_library(oaml STATIC ${SRCS})

add_executable(oamlTest ${SRCS} src/test.cpp)
target_link_libraries(oamlTest ${OAML_LIBS})
