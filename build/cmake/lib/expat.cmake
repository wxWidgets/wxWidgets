#############################################################################
# Name:        build/cmake/lib/expat.cmake
# Purpose:     Use external or internal expat lib
# Author:      Tobias Taschner
# Created:     2016-09-21
# Copyright:   (c) 2016 wxWidgets development team
# Licence:     wxWindows licence
#############################################################################

if(wxUSE_EXPAT STREQUAL "builtin")
    # TODO: implement building expat via its CMake file, using
    # add_subdirectory or ExternalProject_Add
    wx_add_builtin_library(wxexpat
        src/expat/expat/lib/xmlparse.c
        src/expat/expat/lib/xmlrole.c
        src/expat/expat/lib/xmltok.c
    )
    set(EXPAT_LIBRARIES wxexpat)
    set(EXPAT_INCLUDE_DIRS ${wxSOURCE_DIR}/src/expat/expat/lib)
elseif(wxUSE_EXPAT)
    find_package(EXPAT REQUIRED)
endif()
