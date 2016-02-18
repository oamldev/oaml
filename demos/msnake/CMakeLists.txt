cmake_minimum_required(VERSION 2.8.4)
project(msnake)

set(CMAKE_C_FLAGS "-Wall")

set(CURSES_NEED_NCURSES TRUE)
find_package(Curses)

##
# Find SDL
#
set(SDL_BUILDING_LIBRARY true)
find_package(SDL REQUIRED)
set(LIBS ${LIBS} ${SDL_LIBRARY} SDLmain)

##
# Find VorbisFile lib
#
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules ${CMAKE_MODULE_PATH})

find_package(OggVorbis REQUIRED)
find_package(VorbisFile REQUIRED)

if (OGGVORBIS_FOUND AND VORBISFILE_FOUND)
	include_directories(${VORBISFILE_INCLUDE_DIR})
	set(LIBS ${LIBS} ${OGG_LIBRARY} ${VORBIS_LIBRARY} ${VORBISFILE_LIBRARIES})
endif()

find_package(OAMLshared REQUIRED)
set(LIBS ${LIBS} ${OAML_LIBRARIES})

include_directories(${CURSES_INCLUDE_DIRS})

set(SNAKE_SOURCE_PATH ${PROJECT_SOURCE_DIR}/src)
file(GLOB_RECURSE SNAKE_SOURCE_FILES ${SNAKE_SOURCE_PATH}/*.c)
add_executable(msnake ${SNAKE_SOURCE_FILES})
target_link_libraries(msnake ${CURSES_LIBRARIES} ${LIBS})

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    target_link_libraries(msnake rt)
endif()

install(TARGETS msnake DESTINATION /usr/local/bin)

