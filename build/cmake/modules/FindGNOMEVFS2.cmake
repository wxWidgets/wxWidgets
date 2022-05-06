# - Try to find GnomeVFS2
# Once done this will define
#
#  GNOMEVFS2_FOUND - system has GnomeVFS2
#  GNOMEVFS2_INCLUDE_DIRS - the GnomeVFS2 include directory
#  GNOMEVFS2_LIBRARIES - Link these to use GnomeVFS2
#  GNOMEVFS2_DEFINITIONS - Compiler switches required for using GnomeVFS2
#
#  Copyright (c) 2008 Joshua L. Blocher <verbalshadow@gmail.com>
#
#  Redistribution and use is allowed according to the terms of the New
#  BSD license.
#  For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#


if (GNOMEVFS2_LIBRARIES AND GNOMEVFS2_INCLUDE_DIRS)
  # in cache already
  set(GNOMEVFS2_FOUND TRUE)
else (GNOMEVFS2_LIBRARIES AND GNOMEVFS2_INCLUDE_DIRS)
  # use pkg-config to get the directories and then use these values
  # in the FIND_PATH() and FIND_LIBRARY() calls
  if (${CMAKE_MAJOR_VERSION} EQUAL 2 AND ${CMAKE_MINOR_VERSION} EQUAL 4)
    include(UsePkgConfig)
    pkgconfig(gnome-vfs-2.0 _GNOMEVFS2_INCLUDEDIR _GNOMEVFS2_LIBDIR _GNOMEVFS2_LDFLAGS _GNOMEVFS2_CFLAGS)
  else (${CMAKE_MAJOR_VERSION} EQUAL 2 AND ${CMAKE_MINOR_VERSION} EQUAL 4)
    find_package(PkgConfig)
    if (PKG_CONFIG_FOUND)
      pkg_check_modules(_GNOMEVFS2 gnome-vfs-2.0)
    endif (PKG_CONFIG_FOUND)
  endif (${CMAKE_MAJOR_VERSION} EQUAL 2 AND ${CMAKE_MINOR_VERSION} EQUAL 4)
  find_path(GNOMEVFS2_INCLUDE_DIR
    NAMES
      libgnomevfs/gnome-vfs.h
    PATHS
      ${_GNOMEVFS2_INCLUDEDIR}
      /usr/include
      /usr/local/include
      /opt/local/include
      /sw/include
      $ENV{DEVLIBS_PATH}//include//
    PATH_SUFFIXES
      gnome-vfs-2.0
  )

  find_library(GNOMEVFS-2_LIBRARY
    NAMES
      gnomevfs-2
    PATHS
      ${_GNOMEVFS2_LIBDIR}
      /usr/lib
      /usr/local/lib
      /opt/local/lib
      /sw/lib
  )

  if (GNOMEVFS-2_LIBRARY)
    set(GNOMEVFS-2_FOUND TRUE)
  endif (GNOMEVFS-2_LIBRARY)

  set(GNOMEVFS2_INCLUDE_DIRS
    ${GNOMEVFS2_INCLUDE_DIR}
  )

  if (GNOMEVFS-2_FOUND)
    set(GNOMEVFS2_LIBRARIES
      ${GNOMEVFS2_LIBRARIES}
      ${GNOMEVFS-2_LIBRARY}
    )
  endif (GNOMEVFS-2_FOUND)

  if (GNOMEVFS2_INCLUDE_DIRS AND GNOMEVFS2_LIBRARIES)
     set(GNOMEVFS2_FOUND TRUE)
  endif (GNOMEVFS2_INCLUDE_DIRS AND GNOMEVFS2_LIBRARIES)

  if (GNOMEVFS2_FOUND)
    if (NOT GNOMEVFS2_FIND_QUIETLY)
      message(STATUS "Found GnomeVFS2: ${GNOMEVFS2_LIBRARIES}")
    endif (NOT GNOMEVFS2_FIND_QUIETLY)
  else (GNOMEVFS2_FOUND)
    if (GNOMEVFS2_FIND_REQUIRED)
      message(FATAL_ERROR "Could not find GnomeVFS2")
    endif (GNOMEVFS2_FIND_REQUIRED)
  endif (GNOMEVFS2_FOUND)

  # show the GNOMEVFS2_INCLUDE_DIRS and GNOMEVFS2_LIBRARIES variables only in the advanced view
  mark_as_advanced(GNOMEVFS2_INCLUDE_DIRS GNOMEVFS2_LIBRARIES GNOMEVFS2_INCLUDE_DIR GNOMEVFS-2_LIBRARY)

endif (GNOMEVFS2_LIBRARIES AND GNOMEVFS2_INCLUDE_DIRS)
