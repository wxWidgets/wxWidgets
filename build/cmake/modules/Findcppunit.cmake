# - try to find cppunit library
#
# Cache Variables: (probably not for direct use in your scripts)
#  CPPUNIT_INCLUDE_DIR
#  CPPUNIT_LIBRARY
#
# Non-cache variables you might use in your CMakeLists.txt:
#  CPPUNIT_FOUND
#  CPPUNIT_INCLUDE_DIRS
#  CPPUNIT_LIBRARIES
#
# Requires these CMake modules:
#  SelectLibraryConfigurations (included with CMake >= 2.8.0)
#  FindPackageHandleStandardArgs (known included with CMake >=2.6.2)
#
# Original Author:
# 2009-2011 Ryan Pavlik <rpavlik@iastate.edu> <abiryan@ryand.net>
# http://academic.cleardefinition.com
# Iowa State University HCI Graduate Program/VRAC
#
# Copyright Iowa State University 2009-2011.
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

set(CPPUNIT_ROOT_DIR
	"${CPPUNIT_ROOT_DIR}"
	CACHE
	PATH
	"Directory to search")

find_library(CPPUNIT_LIBRARY_RELEASE
	NAMES
	cppunit
	HINTS
	"${CPPUNIT_ROOT_DIR}"
    "${CPPUNIT_ROOT_DIR}/lib")

find_library(CPPUNIT_LIBRARY_DEBUG
	NAMES
	cppunitd
	HINTS
	"${CPPUNIT_ROOT_DIR}"
    "${CPPUNIT_ROOT_DIR}/lib")

include(SelectLibraryConfigurations)
select_library_configurations(CPPUNIT)

# Might want to look close to the library first for the includes.
get_filename_component(_libdir "${CPPUNIT_LIBRARY_RELEASE}" PATH)

find_path(CPPUNIT_INCLUDE_DIR
	NAMES
	cppunit/TestCase.h
	HINTS
	"${_libdir}/.."
	PATHS
	"${CPPUNIT_ROOT_DIR}"
	PATH_SUFFIXES
	include/)


include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(cppunit
	DEFAULT_MSG
	CPPUNIT_LIBRARY
	CPPUNIT_INCLUDE_DIR)

if(CPPUNIT_FOUND)
	set(CPPUNIT_LIBRARIES ${CPPUNIT_LIBRARY} ${CMAKE_DL_LIBS})
	set(CPPUNIT_INCLUDE_DIRS "${CPPUNIT_INCLUDE_DIR}")
	mark_as_advanced(CPPUNIT_ROOT_DIR)
endif()

mark_as_advanced(CPPUNIT_INCLUDE_DIR
	CPPUNIT_LIBRARY_RELEASE
	CPPUNIT_LIBRARY_DEBUG)
