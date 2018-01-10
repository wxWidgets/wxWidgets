#############################################################################
# Name:        build/cmake/lib/xml/CMakeLists.txt
# Purpose:     CMake file for xml library
# Author:      Tobias Taschner
# Created:     2016-09-20
# Copyright:   (c) 2016 wxWidgets development team
# Licence:     wxWindows licence
#############################################################################

include(../../source_groups.cmake)

if(wxUSE_EXPAT STREQUAL "builtin")
    wx_add_builtin_library(wxexpat
        src/expat/expat/lib/xmlparse.c
        src/expat/expat/lib/xmlrole.c
        src/expat/expat/lib/xmltok.c
    )
    set(EXPAT_LIBRARIES wxexpat)
    set(EXPAT_INCLUDE_DIRS ${wxSOURCE_DIR}/src/expat/expat/lib)
elseif(wxUSE_EXPAT)
    find_package(EXPAT)
endif()

wx_append_sources(XML_FILES XML)
wx_add_library(xml IS_BASE ${XML_FILES})
wx_lib_link_libraries(xml
    PRIVATE ${EXPAT_LIBRARIES}
    )
wx_lib_include_directories(xml PRIVATE ${EXPAT_INCLUDE_DIRS})

wx_finalize_lib(xml)
