#############################################################################
# Name:        build/cmake/lib/regex.cmake
# Purpose:     Use external or internal regex lib
# Author:      Tobias Taschner
# Created:     2016-09-25
# Copyright:   (c) 2016 wxWidgets development team
# Licence:     wxWindows licence
#############################################################################

if(wxUSE_REGEX)
    # TODO: Forcing builtin until sys is implemented
    set(wxUSE_REGEX builtin)
    wx_add_builtin_library(wxregex
        src/regex/regcomp.c
        src/regex/regexec.c
        src/regex/regerror.c
        src/regex/regfree.c
    )
    target_include_directories(wxregex PRIVATE ${wxSETUP_HEADER_PATH} ${wxSOURCE_DIR}/include)
    set(REGEX_LIBRARIES wxregex)
    set(REGEX_INCLUDE_DIRS ${wxSOURCE_DIR}/src/regex)
endif()

#TODO: find external lib and include dir
