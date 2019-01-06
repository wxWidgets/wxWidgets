#############################################################################
# Name:        build/cmake/lib/base/CMakeLists.txt
# Purpose:     CMake file for base library
# Author:      Tobias Taschner
# Created:     2016-09-20
# Copyright:   (c) 2016 wxWidgets development team
# Licence:     wxWindows licence
#############################################################################

include(../../source_groups.cmake)

wx_append_sources(BASE_FILES BASE_CMN)
wx_append_sources(BASE_FILES BASE_AND_GUI_CMN)

if(WIN32)
    wx_append_sources(BASE_FILES BASE_WIN32)
    wx_append_sources(BASE_FILES BASE_AND_GUI_WIN32)
elseif(APPLE)
    wx_append_sources(BASE_FILES BASE_OSX_SHARED)
    if(wxBUILD_TOOLKIT MATCHES "osx_iphone")
        wx_append_sources(BASE_FILES BASE_AND_GUI_OSX_IPHONE)
    else()
        wx_append_sources(BASE_FILES BASE_AND_GUI_OSX_COCOA)
    endif()
elseif(UNIX)
    wx_append_sources(BASE_FILES BASE_UNIX)
endif()

wx_add_library(base IS_BASE ${BASE_FILES})

if(NOT wxBUILD_MONOLITHIC)
    wx_lib_compile_definitions(base PRIVATE wxUSE_BASE=1)
endif()

if(wxUSE_ZLIB)
    wx_lib_include_directories(base PRIVATE ${ZLIB_INCLUDE_DIRS})
    wx_lib_link_libraries(base PRIVATE ${ZLIB_LIBRARIES})
endif()
if(wxUSE_REGEX)
    wx_lib_include_directories(base PRIVATE ${REGEX_INCLUDE_DIRS})
    wx_lib_link_libraries(base PRIVATE ${REGEX_LIBRARIES})
endif()
if(wxUSE_LIBLZMA)
    wx_lib_include_directories(base PRIVATE ${LIBLZMA_INCLUDE_DIRS})
    wx_lib_link_libraries(base PRIVATE ${LIBLZMA_LIBRARIES})
endif()
if(UNIX AND wxUSE_SECRETSTORE)
    wx_lib_include_directories(base PRIVATE ${LIBSECRET_INCLUDE_DIRS})
    wx_lib_link_libraries(base PRIVATE ${LIBSECRET_LIBRARIES})
endif()
if(wxUSE_LIBICONV)
    wx_lib_include_directories(base PRIVATE ${ICONV_INCLUDE_DIR})
    wx_lib_link_libraries(base PRIVATE ${ICONV_LIBRARIES})
endif()
if(wxUSE_THREADS AND CMAKE_THREAD_LIBS_INIT)
    wx_lib_link_libraries(base PRIVATE ${CMAKE_THREAD_LIBS_INIT})
endif()

if(APPLE)
    wx_lib_link_libraries(base
        PRIVATE
        "-framework Security"
        PUBLIC
        "-framework Carbon"
        "-framework Cocoa"
        "-framework CoreFoundation"
        "-framework IOKit"
    )
elseif(UNIX)
    wx_lib_link_libraries(base PRIVATE dl)
endif()

wx_finalize_lib(base)
