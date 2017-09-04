#
#  FindSoxr.cmake
#
#  Try to find the libsoxr resampling library
#
#  Once done this will define
#
#  SOXR_FOUND - system found libsoxr
#  SOXR_INCLUDE_DIRS - the libsoxr include directory
#  SOXR_LIBRARIES - link to this to use libsoxr
#

include (CheckLibraryExists)

find_path(SOXR_INCLUDE_DIRS soxr.h PATH_SUFFIXES include)
find_library(SOXR_LIBRARIES NAMES soxr PATH_SUFFIXES lib)

set(SOXR_FOUND "NO")
if(SOXR_LIBRARIES AND SOXR_INCLUDE_DIRS)
	set(SOXR_FOUND "YES")
	mark_as_advanced(SOXR_INCLUDE_DIRS SOXR_LIBRARIES)
endif()
