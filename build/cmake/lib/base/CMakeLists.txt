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
    if(wxUSE_SECRETSTORE)
        # The required APIs are always available under MSW and OS X but we must
        # have GNOME libsecret under Unix to be able to compile this class.
        find_package(Libsecret REQUIRED)
        if(NOT LIBSECRET_FOUND)
            message(WARNING "libsecret not found, wxSecretStore won't be available")
            wx_option_force_value(wxUSE_SECRETSTORE OFF)
        endif()
    endif()
endif()

wx_add_library(base IS_BASE ${BASE_FILES})
wx_lib_link_libraries(base PRIVATE
    ${ZLIB_LIBRARIES}
    ${REGEX_LIBRARIES}
)
if(NOT wxBUILD_MONOLITHIC)
    wx_lib_compile_definitions(base PRIVATE wxUSE_BASE=1)
endif()
if(wxUSE_ZLIB)
    wx_lib_include_directories(base PRIVATE ${ZLIB_INCLUDE_DIRS})
endif()
if(wxUSE_REGEX)
    wx_lib_include_directories(base PRIVATE ${REGEX_INCLUDE_DIRS})
endif()
if(LIBSECRET_FOUND)
    wx_lib_include_directories(base PRIVATE ${LIBSECRET_INCLUDE_DIRS})
endif()
if(wxUSE_LIBICONV AND ICONV_LIBRARIES)
    wx_lib_link_libraries(base PRIVATE ${ICONV_LIBRARIES})
endif()
if(wxUSE_THREADS AND CMAKE_THREAD_LIBS_INIT)
    wx_lib_link_libraries(base PRIVATE ${CMAKE_THREAD_LIBS_INIT})
endif()
if(WIN32)
    wx_lib_link_libraries(base PUBLIC
        kernel32
        user32
        shell32
        ole32
        oleaut32
        uuid
        rpcrt4
        advapi32
        Shlwapi
        version
        uuid
    )
elseif(APPLE)
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
    wx_lib_link_libraries(base PRIVATE
        dl
        ${LIBSECRET_LIBRARIES}
    )
endif()

wx_finalize_lib(base)
