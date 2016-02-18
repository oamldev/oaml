# Locate OAML
# This module defines
# OAML_LIBRARIES
# OAML_FOUND, if false, do not try to link to OAML
# OAML_INCLUDE_DIRS, where to find the headers
#
# $VORBISDIR is an environment variable that would
# correspond to the ./configure --prefix=$VORBISDIR
# used in building Vorbis.
#
# Created by Sukender (Benoit Neil). Based on FindOpenAL.cmake module.
# TODO Add hints for linux and Mac

FIND_PATH(OAML_INCLUDE_DIRS
	oaml.h
	HINTS
	$ENV{OAMLDIR}
	$ENV{OAML_PATH}
	PATH_SUFFIXES include
	PATHS
	~/Library/Frameworks
	/Library/Frameworks
	/usr/local
	/usr
	/sw # Fink
	/opt/local # DarwinPorts
	/opt/csw # Blastwave
	/opt
)

FIND_LIBRARY(OAML_LIBRARIES
	NAMES oaml_shared
	HINTS
	$ENV{OAMLDIR}
	$ENV{OAML_PATH}
	PATHS
	~/Library/Frameworks
	/Library/Frameworks
	/usr/local
	/usr
	/sw
	/opt/local
	/opt/csw
	/opt
)

SET(OAML_FOUND "NO")
IF(OAML_LIBRARIES AND OAML_INCLUDE_DIRS)
	SET(OAML_FOUND "YES")
	MARK_AS_ADVANCED(OAML_LIBRARIES OAML_INCLUDE_DIRS)
ELSEIF(OAML_FIND_REQUIRED)
	MESSAGE(FATAL_ERROR "Required library OAML not found! Install the library (including dev packages) and try again. If the library is already installed, set the missing variables manually in cmake.")
ENDIF()
