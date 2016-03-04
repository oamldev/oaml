#.rst:
# FindOAML
# -------
#
# Find the native OAML includes and library
#
# It defines the following variables
#
# ``OAML_INCLUDE_DIRS``
#   where to find oaml.h
# ``OAML_LIBRARIES_SHARED``
#   the libraries to link against to use OAML.
# ``OAML_LIBRARIES_STATIC``
#   the libraries to link against to use OAML.
# ``OAML_FOUND``
#   If false, do not try to use OAML.
#

set(OAML_FOUND "NO")

find_package(OggVorbis)
find_package(VorbisFile)

if(OGGVORBIS_FOUND AND VORBISFILE_FOUND)
	find_path(OAML_INCLUDE_DIRS oaml.h)
	find_library(OAML_LIBRARY_STATIC oaml)
	find_library(OAML_LIBRARY_SHARED oaml_shared)

	if (OAML_LIBRARY_SHARED AND OAML_INCLUDE_DIRS)
		set(OAML_FOUND "YES")
		set(OAML_LIBRARIES_SHARED ${OAML_LIBRARY_SHARED} ${OGG_LIBRARY} ${VORBIS_LIBRARY} ${VORBISFILE_LIBRARIES})
	endif()

	if (OAML_LIBRARY_STATIC AND OAML_INCLUDE_DIRS)
		set(OAML_FOUND "YES")
		set(OAML_LIBRARIES_STATIC ${OAML_LIBRARY_STATIC} ${OGG_LIBRARY} ${VORBIS_LIBRARY} ${VORBISFILE_LIBRARIES})
	endif()
endif()
