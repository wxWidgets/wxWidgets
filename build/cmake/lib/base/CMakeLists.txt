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
    if(WXOSX_COCOA)
        wx_append_sources(BASE_FILES BASE_AND_GUI_OSX_COCOA)
    endif()
elseif(UNIX)
    wx_append_sources(BASE_FILES BASE_UNIX)
endif()

wx_add_library(wxbase IS_BASE ${BASE_FILES})

if(NOT wxBUILD_MONOLITHIC)
    wx_lib_compile_definitions(wxbase PRIVATE wxUSE_BASE=1)
endif()

if(wxUSE_ZLIB)
    wx_lib_include_directories(wxbase PRIVATE ${ZLIB_INCLUDE_DIRS})
    wx_lib_link_libraries(wxbase PRIVATE ${ZLIB_LIBRARIES})
endif()
if(wxUSE_REGEX)
    wx_lib_include_directories(wxbase PRIVATE ${REGEX_INCLUDE_DIRS})
    wx_lib_link_libraries(wxbase PRIVATE ${REGEX_LIBRARIES})
endif()
if(wxUSE_LIBLZMA)
    wx_lib_include_directories(wxbase PRIVATE ${LIBLZMA_INCLUDE_DIRS})
    wx_lib_link_libraries(wxbase PRIVATE ${LIBLZMA_LIBRARIES})
endif()
if(UNIX AND wxUSE_SECRETSTORE)
    wx_lib_include_directories(wxbase PRIVATE ${LIBSECRET_INCLUDE_DIRS})
    wx_lib_link_libraries(wxbase PRIVATE ${LIBSECRET_LIBRARIES})
endif()
if(wxUSE_LIBICONV)
    wx_lib_include_directories(wxbase PRIVATE ${ICONV_INCLUDE_DIR})
    wx_lib_link_libraries(wxbase PRIVATE ${ICONV_LIBRARIES})
endif()
if(wxUSE_THREADS AND CMAKE_THREAD_LIBS_INIT)
    wx_lib_link_libraries(wxbase PRIVATE ${CMAKE_THREAD_LIBS_INIT})
endif()

if(APPLE)
    wx_lib_link_libraries(wxbase
        PUBLIC
        "-framework CoreFoundation"
    )
    if(WXOSX_COCOA)
        wx_lib_link_libraries(wxbase
            PRIVATE
            "-framework Security"
            PUBLIC
            "-framework Carbon"
            "-framework Cocoa"
            "-framework IOKit"
            "-framework QuartzCore"
        )
    endif()
elseif(UNIX)
    wx_lib_link_libraries(wxbase PRIVATE dl)
endif()

wx_finalize_lib(wxbase)
