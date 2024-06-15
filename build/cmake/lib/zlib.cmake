#############################################################################
# Name:        build/cmake/lib/zlib.cmake
# Purpose:     Use external or internal zlib
# Author:      Tobias Taschner
# Created:     2016-09-21
# Copyright:   (c) 2016 wxWidgets development team
# Licence:     wxWindows licence
#############################################################################

if(wxUSE_ZLIB STREQUAL "builtin")
    # TODO: implement building zlib via its CMake file, using
    # add_subdirectory or ExternalProject_Add
    wx_add_builtin_library(wxzlib
        src/zlib/adler32.c
        src/zlib/compress.c
        src/zlib/crc32.c
        src/zlib/deflate.c
        src/zlib/gzclose.c
        src/zlib/gzlib.c
        src/zlib/gzread.c
        src/zlib/gzwrite.c
        src/zlib/infback.c
        src/zlib/inffast.c
        src/zlib/inflate.c
        src/zlib/inftrees.c
        src/zlib/trees.c
        src/zlib/uncompr.c
        src/zlib/zutil.c
    )
    if(WIN32)
        # Define this to get rid of many warnings about using open(),
        # read() and other POSIX functions in zlib code. This is much
        # more convenient than having to modify it to avoid them.
        target_compile_definitions(wxzlib PRIVATE _CRT_NONSTDC_NO_WARNINGS)
    endif()
    if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
        target_compile_options(wxzlib PRIVATE -Wno-deprecated-non-prototype)
    endif()
    set(ZLIB_LIBRARIES wxzlib)
    set(ZLIB_INCLUDE_DIRS ${wxSOURCE_DIR}/src/zlib)
elseif(wxUSE_ZLIB)
    find_package(ZLIB REQUIRED)
endif()
