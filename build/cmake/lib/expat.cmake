#############################################################################
# Name:        build/cmake/lib/expat.cmake
# Purpose:     Use external or internal expat lib
# Author:      Tobias Taschner
# Created:     2016-09-21
# Copyright:   (c) 2016 wxWidgets development team
# Licence:     wxWindows licence
#############################################################################

if(wxUSE_EXPAT STREQUAL "sys")
    find_package(EXPAT)
    if(NOT EXPAT_FOUND)
        # If the sys library can not be found use builtin
        wx_option_force_value(wxUSE_EXPAT builtin)
    endif()
endif()

if(wxUSE_EXPAT STREQUAL "builtin")
    set(wxEXPAT_DIR ${wxSOURCE_DIR}/src/expat/expat)

    # TODO: implement building expat via its CMake file, using
    # add_subdirectory or ExternalProject_Add
    #
    # Until this is done, at least use Expat's own CMake file to check for the
    # features it needs, so that we can produce the appropriate expat_config.h.
    #
    # This only makes sense under Unix, however, as almost all checks there are
    # Unix-specific and just always fail under Windows.
    if(WIN32)
        include(TestBigEndian)
        test_big_endian(WORDS_BIGENDIAN)
        if(WORDS_BIGENDIAN)
            set(BYTEORDER 4321)
        else(WORDS_BIGENDIAN)
            set(BYTEORDER 1234)
        endif(WORDS_BIGENDIAN)
    else()
        include(${wxEXPAT_DIR}/ConfigureChecks.cmake)
    endif()

    if(RETRO68)
        # newlib provides arc4random() and getentropy(), so the checks above
        # succeed, but the Retro68 runtime getentropy() always fails with
        # ENOSYS and newlib arc4random() then kills the application with
        # raise(SIGKILL), i.e. it silently quits as soon as an XML parser is
        # created. There are no other entropy sources under classic Mac OS,
        # so use Expat fallback based on the time and process ID.
        foreach(var HAVE_ARC4RANDOM HAVE_ARC4RANDOM_BUF HAVE_GETENTROPY
                    HAVE_GETRANDOM HAVE_SYSCALL_GETRANDOM)
            set(${var} 0)
        endforeach()
        set(wxEXPAT_POOR_ENTROPY ON)
    endif()

    # Also define some options normally set by Expat's CMakeLists.txt.
    set(XML_DTD 1)
    set(XML_GE 1)
    set(XML_NS 1)
    set(XML_CONTEXT_BYTES 1024)

    configure_file(${wxEXPAT_DIR}/expat_config.h.cmake
        ${wxBINARY_DIR}/libs/expat/expat_config.h
    )

    wx_add_builtin_library(wxexpat
        src/expat/expat/lib/random_arc4random.c
        src/expat/expat/lib/random_arc4random_buf.c
        src/expat/expat/lib/random_dev_urandom.c
        src/expat/expat/lib/random_getentropy.c
        src/expat/expat/lib/random_getrandom.c
        src/expat/expat/lib/random_rand_s.c
        src/expat/expat/lib/xcs.c
        src/expat/expat/lib/xmlparse.c
        src/expat/expat/lib/xmlrole.c
        src/expat/expat/lib/xmltok.c
    )

    target_include_directories(wxexpat PRIVATE ${wxBINARY_DIR}/libs/expat)
    target_compile_definitions(wxexpat PRIVATE HAVE_EXPAT_CONFIG_H)
    if(wxEXPAT_POOR_ENTROPY)
        target_compile_definitions(wxexpat PRIVATE XML_POOR_ENTROPY)
    endif()

    set(EXPAT_LIBRARIES wxexpat)
    set(EXPAT_INCLUDE_DIRS ${wxEXPAT_DIR}/lib)
endif()
