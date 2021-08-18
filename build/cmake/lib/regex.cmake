#############################################################################
# Name:        build/cmake/lib/regex.cmake
# Purpose:     Use external or internal regex lib
# Author:      Tobias Taschner
# Created:     2016-09-25
# Copyright:   (c) 2016 wxWidgets development team
# Licence:     wxWindows licence
#############################################################################

if(wxUSE_REGEX STREQUAL "builtin")
    # TODO: implement building PCRE2 via its CMake file, using
    # add_subdirectory or ExternalProject_Add
    wx_add_builtin_library(wxregex
        3rdparty/pcre/src/pcre2_auto_possess.c
        3rdparty/pcre/src/pcre2_compile.c
        3rdparty/pcre/src/pcre2_config.c
        3rdparty/pcre/src/pcre2_context.c
        3rdparty/pcre/src/pcre2_convert.c
        3rdparty/pcre/src/pcre2_dfa_match.c
        3rdparty/pcre/src/pcre2_error.c
        3rdparty/pcre/src/pcre2_extuni.c
        3rdparty/pcre/src/pcre2_find_bracket.c
        3rdparty/pcre/src/pcre2_jit_compile.c
        3rdparty/pcre/src/pcre2_maketables.c
        3rdparty/pcre/src/pcre2_match.c
        3rdparty/pcre/src/pcre2_match_data.c
        3rdparty/pcre/src/pcre2_newline.c
        3rdparty/pcre/src/pcre2_ord2utf.c
        3rdparty/pcre/src/pcre2_pattern_info.c
        3rdparty/pcre/src/pcre2_script_run.c
        3rdparty/pcre/src/pcre2_serialize.c
        3rdparty/pcre/src/pcre2_string_utils.c
        3rdparty/pcre/src/pcre2_study.c
        3rdparty/pcre/src/pcre2_substitute.c
        3rdparty/pcre/src/pcre2_substring.c
        3rdparty/pcre/src/pcre2_tables.c
        3rdparty/pcre/src/pcre2_ucd.c
        3rdparty/pcre/src/pcre2_valid_utf.c
        3rdparty/pcre/src/pcre2_xclass.c
        3rdparty/pcre/src/pcre2_chartables.c
    )
    set(REGEX_LIBRARIES wxregex)
    set(REGEX_INCLUDE_DIRS ${wxSOURCE_DIR}/3rdparty/pcre/src/wx)
    target_compile_definitions(wxregex PRIVATE __WX__ HAVE_CONFIG_H)
    target_include_directories(wxregex PRIVATE ${wxSETUP_HEADER_PATH} ${wxSOURCE_DIR}/include ${REGEX_INCLUDE_DIRS})
elseif(wxUSE_REGEX)
    find_package(PCRE2 REQUIRED)
    set(REGEX_LIBRARIES ${PCRE2_LIBRARIES})
    set(REGEX_INCLUDE_DIRS ${PCRE2_INCLUDE_DIRS})
endif()
