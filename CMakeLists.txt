cmake_minimum_required(VERSION 2.8)

project (oaml)


# Configuration types
set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	add_definitions(-DDEBUG)
	message("Debug mode: Yes (Disable by param -DCMAKE_BUILD_TYPE=Release)")
else()
	message("Debug mode: No (Enable by param -DCMAKE_BUILD_TYPE=Debug)")
endif()


##
# Options
#
option(ENABLE_UNITYPLUGIN "Build AudioPluginOAML plugin for Unity" ON)

if(ENABLE_UNITYPLUGIN)
	message("Build AudioPluginOAML: Yes (Disable by param -DENABLE_UNITYPLUGIN=OFF)")
else()
	message("Build AudioPluginOAML: No  (Enable by param -DENABLE_UNITYPLUGIN=ON)")
endif()


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

find_package(OggVorbis)
find_package(VorbisFile)

if (OGGVORBIS_FOUND AND VORBISFILE_FOUND)
	include_directories(${VORBISFILE_INCLUDE_DIR})
	set(OAML_LIBS ${OAML_LIBS} ${OGG_LIBRARY} ${VORBIS_LIBRARY} ${VORBISFILE_LIBRARIES})
endif()


##
# Build static and shared libraries
#
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
set(SRCS src/audioFile.cpp src/ByteBuffer.cpp src/oaml.cpp src/oamlAudio.cpp src/oamlBase.cpp src/oamlCompressor.cpp src/oamlTrack.cpp src/aif.cpp src/ogg.cpp src/wav.cpp src/gettime.cpp src/tinyxml2.cpp)
add_library(oaml STATIC ${SRCS})
add_library(oaml_shared SHARED ${SRCS})

target_link_libraries(oaml_shared ${OAML_LIBS})


##
# Build AudioPluginOAML (for Unity) if we ENABLE_UNITYPLUGIN is TRUE
#
if (ENABLE_UNITYPLUGIN)
	if (APPLE)
		add_library(AudioPluginOAML MODULE ${SRCS} src/oamlUnityPlugin.cpp)
		set_target_properties(AudioPluginOAML PROPERTIES
			BUNDLE_EXTENSION bundle
			BUNDLE TRUE)
	else()
		add_library(AudioPluginOAML MODULE ${SRCS} src/oamlUnityPlugin.cpp)
	endif()
	target_link_libraries(AudioPluginOAML ${OAML_LIBS})
endif()

