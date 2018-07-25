cmake_minimum_required(VERSION 2.8)

set(OAML_VERSION_MAJOR 1)
set(OAML_VERSION_MINOR 3)
set(OAML_VERSION_PATCH 4)

if (${CMAKE_MAJOR_VERSION} LESS 3)
	project (oaml)
	set (PROJECT_VERSION_MAJOR ${OAML_VERSION_MAJOR})
	set (PROJECT_VERSION ${PROJECT_VERSION_MAJOR}.${OAML_VERSION_MINOR}.${OAML_VERSION_PATCH})
else()
	cmake_policy (SET CMP0048 NEW)
	project (oaml VERSION ${OAML_VERSION_MAJOR}.${OAML_VERSION_MINOR}.${OAML_VERSION_PATCH})
endif ()

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules ${CMAKE_MODULE_PATH})

include(GetGitRevisionDescription)
get_git_head_revision(GIT_REFSPEC GIT_SHA1)


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
option(ENABLE_STATIC "Build static library" ON)
option(ENABLE_SHARED "Build shared library" ON)
option(ENABLE_UNITYPLUGIN "Build AudioPluginOAML plugin for Unity" ON)
option(ENABLE_OGG "Build with OGG support" ON)
option(ENABLE_SOXR "Build with SOXR support" ON)
option(ENABLE_RTAUDIO "Build with RtAudio support" ON)
option(ENABLE_TEST "Build oamlTest executable" OFF)

if(ENABLE_STATIC)
	message("Build static: Yes (Disable by param -DENABLE_STATIC=OFF)")
else()
	message("Build static: No  (Enable by param -DENABLE_STATIC=ON)")
endif()

if(ENABLE_SHARED)
	message("Build shared: Yes (Disable by param -DENABLE_SHARED=OFF)")
else()
	message("Build shared: No  (Enable by param -DENABLE_SHARED=ON)")
endif()

if(ENABLE_UNITYPLUGIN)
	message("Build AudioPluginOAML: Yes (Disable by param -DENABLE_UNITYPLUGIN=OFF)")
else()
	message("Build AudioPluginOAML: No  (Enable by param -DENABLE_UNITYPLUGIN=ON)")
endif()

if(ENABLE_OGG)
	message("Build OGG support: Yes (Disable by param -DENABLE_OGG=OFF)")
else()
	message("Build OGG support: No  (Enable by param -DENABLE_OGG=ON)")
endif()

if(ENABLE_SOXR)
	message("Build SOXR support: Yes (Disable by param -DENABLE_SOXR=OFF)")
else()
	message("Build SOXR support: No  (Enable by param -DENABLE_SOXR=ON)")
endif()

if(ENABLE_RTAUDIO)
	message("Build RtAudio support: Yes (Disable by param -DENABLE_RTAUDIO=OFF)")
else()
	message("Build RtAudio support: No  (Enable by param -DENABLE_RTAUDIO=ON)")
endif()

if(ENABLE_TEST)
	message("Build oamlTest executable: Yes (Disable by param -DENABLE_TEST=OFF)")
else()
	message("Build oamlTest executable: No  (Enable by param -DENABLE_TEST=ON)")
endif()


##
# Set CXX_FLAGS depending on compiler
#
if (MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
else()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -Wextra")
endif()

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
if(COMPILER_SUPPORTS_CXX11)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(COMPILER_SUPPORTS_CXX0X)
	CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
endif()

##
# Find VorbisFile lib
#
if (ENABLE_OGG)
	find_package(OggVorbis REQUIRED)
	find_package(VorbisFile REQUIRED)

	if (OGGVORBIS_FOUND AND VORBISFILE_FOUND)
		add_definitions(-D__HAVE_OGG)
		include_directories(SYSTEM ${VORBIS_INCLUDE_DIR} ${VORBISFILE_INCLUDE_DIRS})
		set(OAML_LIBS ${OAML_LIBS} ${OGG_LIBRARY} ${VORBIS_LIBRARY} ${VORBISFILE_LIBRARIES})
	endif()
endif()

##
# Find soxr lib
#
if (ENABLE_SOXR)
	find_package(Soxr)

	if (SOXR_FOUND)
		add_definitions(-D__HAVE_SOXR)
		include_directories(SYSTEM ${SOXR_INCLUDE_DIR})
		set(OAML_LIBS ${OAML_LIBS} ${SOXR_LIBRARIES})
	else()
		message(WARNING "libsoxr not found, oaml will not be able to resample")
	endif()
endif()

##
# RtAudio flags
#

if (ENABLE_RTAUDIO)
	add_definitions(-D__HAVE_RTAUDIO)

	if (CMAKE_SYSTEM_NAME MATCHES ".*BSD")
		message(STATUS "*BSD detected, using OSS")
		find_package(Threads REQUIRED CMAKE_THREAD_PREFER_PTHREAD)
		if (CMAKE_SYSTEM_NAME MATCHES "kNetBSD|NetBSD")
			list(APPEND OAML_LIBS ossaudio)
		endif ()
		list(APPEND OAML_LIBS ${CMAKE_THREAD_LIBS_INIT})
		set(AUDIO_LINUX_OSS ON)
	elseif (UNIX AND NOT APPLE)
		find_library(PULSE_LIB pulse)
		find_library(PULSESIMPLE_LIB pulse-simple)
		if (PULSE_LIB AND PULSESIMPLE_LIB)
			list(APPEND OAML_LIBS ${PULSE_LIB} ${PULSESIMPLE_LIB})
			add_definitions(-D__LINUX_PULSE__)
			message(STATUS "RtAudio: Using Linux PulseAudio")
		endif()

		find_package(ALSA)
		if (ALSA_FOUND)
			find_package(Threads REQUIRED CMAKE_THREAD_PREFER_PTHREAD)
			include_directories(SYSTEM ${ALSA_INCLUDE_DIR})
			list(APPEND OAML_LIBS ${ALSA_LIBRARY} ${CMAKE_THREAD_LIBS_INIT})
			add_definitions(-D__LINUX_ALSA__)
			message(STATUS "RtAudio: Using Linux ALSA")
		endif()
	endif ()

	if (APPLE)
		find_library(COREAUDIO_LIB CoreAudio)
		find_library(COREFOUNDATION_LIB CoreFoundation)
		if (COREAUDIO_LIB AND COREFOUNDATION_LIB)
			list(APPEND OAML_LIBS ${COREAUDIO_LIB} ${COREFOUNDATION_LIB})
			add_definitions(-D__MACOSX_CORE__)
			message(STATUS "RtAudio: Using OSX CoreAudio")
		endif()
	endif (APPLE)

	# JACK supported on many Unices
	if (UNIX)
		find_library(JACK_LIB jack)
		if (JACK_LIB)
			list(APPEND OAML_LIBS ${JACK_LIB})
			add_definitions(-D__UNIX_JACK__)
			message(STATUS "RtAudio: Using JACK")
		endif()
	endif (UNIX)

	if (WIN32)
		list(APPEND OAML_LIBS winmm ole32)

		add_definitions(-D__WINDOWS_DS__)
		message(STATUS "RtAudio: Using Windows DirectSound")
		list(APPEND OAML_LIBS dsound)

		add_definitions(-D__WINDOWS_WASAPI__)
		message(STATUS "RtAudio: Using Windows WASAPI")
		list(APPEND OAML_LIBS uuid ksuser)
	endif (WIN32)
endif()



##
# Build static and shared libraries
#
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/include/GitSHA1.h.in" "${CMAKE_CURRENT_BINARY_DIR}/include/GitSHA1.h" @ONLY)
include_directories(BEFORE ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_BINARY_DIR}/include)

include(CheckIncludeFiles)
check_include_files("${CMAKE_CURRENT_BINARY_DIR}/include/GitSha1.h" HAVE_GITSHA1_H)
if (HAVE_GITSHA1_H)
	add_definitions(-D__HAVE_GITSHA1_H)
endif()

set(SOURCES
	src/aif.cpp
	src/audioFile.cpp
	src/ByteBuffer.cpp
	src/gettime.cpp
	src/oaml.cpp
	src/oamlAudio.cpp
	src/oamlAudioFile.cpp
	src/oamlBase.cpp
	src/oamlCompressor.cpp
	src/oamlLayer.cpp
	src/oamlMusicTrack.cpp
	src/oamlSfxTrack.cpp
	src/oamlStudioApi.cpp
	src/oamlTrack.cpp
	src/oamlUtil.cpp
	src/tinyxml2.cpp
	src/wav.cpp)

if (ENABLE_OGG)
	list(APPEND SOURCES src/ogg.cpp)
endif()

if (ENABLE_RTAUDIO)
	list(APPEND SOURCES src/RtAudio.cpp)
endif()

if (ENABLE_STATIC)
	add_library(oaml STATIC ${SOURCES} src/oamlC.cpp)
endif()

if (ENABLE_SHARED)
	add_library(oaml_shared SHARED ${SOURCES} src/oamlC.cpp)
	target_link_libraries(oaml_shared ${OAML_LIBS})
	set_target_properties(oaml_shared
		PROPERTIES
			OUTPUT_NAME oaml
			SOVERSION ${PROJECT_VERSION_MAJOR}
			VERSION   ${PROJECT_VERSION}
	)
endif()

if (ENABLE_TEST)
	add_executable(oamlTest ${SOURCES} src/test.cpp)
	target_link_libraries(oamlTest ${OAML_LIBS})
endif()

##
# Build AudioPluginOAML (for Unity) if we ENABLE_UNITYPLUGIN is TRUE
#
if (ENABLE_UNITYPLUGIN)
	if (APPLE)
		add_library(AudioPluginOAML MODULE ${SOURCES} src/oamlUnityPlugin.cpp)
		set_target_properties(AudioPluginOAML PROPERTIES
			BUNDLE_EXTENSION bundle
			BUNDLE TRUE)
	else()
		add_library(AudioPluginOAML MODULE ${SOURCES} src/oamlUnityPlugin.cpp)
	endif()
	target_link_libraries(AudioPluginOAML ${OAML_LIBS})
endif()


##
# Install rules
#
if (ENABLE_STATIC)
	install(TARGETS oaml DESTINATION lib${LIB_SUFFIX})
endif()

if (ENABLE_SHARED)
	install(TARGETS oaml_shared DESTINATION lib${LIB_SUFFIX})
endif()

configure_file("oaml.cmake.in" "oaml.cmake" @ONLY)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/oaml.cmake" DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/cmake/oaml/)

configure_file("oaml.pc.in" "oaml.pc" @ONLY)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/oaml.pc" DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/pkgconfig/)

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/include/oaml.h DESTINATION include)
