#############################################################################
# Name:        build/cmake/lib/jpeg.cmake
# Purpose:     Use external or internal libjpeg
# Author:      Tobias Taschner
# Created:     2016-09-21
# Copyright:   (c) 2016 wxWidgets development team
# Licence:     wxWindows licence
#############################################################################

if(wxUSE_LIBJPEG STREQUAL "builtin")
    wx_add_builtin_library(wxjpeg
        src/jpeg/jaricom.c
        src/jpeg/jcapimin.c
        src/jpeg/jcapistd.c
        src/jpeg/jcarith.c
        src/jpeg/jccoefct.c
        src/jpeg/jccolor.c
        src/jpeg/jcdctmgr.c
        src/jpeg/jchuff.c
        src/jpeg/jcinit.c
        src/jpeg/jcmainct.c
        src/jpeg/jcmarker.c
        src/jpeg/jcmaster.c
        src/jpeg/jcomapi.c
        src/jpeg/jcparam.c
        src/jpeg/jcprepct.c
        src/jpeg/jcsample.c
        src/jpeg/jctrans.c
        src/jpeg/jdapimin.c
        src/jpeg/jdapistd.c
        src/jpeg/jdarith.c
        src/jpeg/jdatadst.c
        src/jpeg/jdatasrc.c
        src/jpeg/jdcoefct.c
        src/jpeg/jdcolor.c
        src/jpeg/jddctmgr.c
        src/jpeg/jdhuff.c
        src/jpeg/jdinput.c
        src/jpeg/jdmainct.c
        src/jpeg/jdmarker.c
        src/jpeg/jdmaster.c
        src/jpeg/jdmerge.c
        src/jpeg/jdpostct.c
        src/jpeg/jdsample.c
        src/jpeg/jdtrans.c
        src/jpeg/jerror.c
        src/jpeg/jfdctflt.c
        src/jpeg/jfdctfst.c
        src/jpeg/jfdctint.c
        src/jpeg/jidctflt.c
        src/jpeg/jidctfst.c
        src/jpeg/jidctint.c
        src/jpeg/jmemmgr.c
        src/jpeg/jmemnobs.c
        src/jpeg/jquant1.c
        src/jpeg/jquant2.c
        src/jpeg/jutils.c
    )
    target_include_directories(wxjpeg
        BEFORE PRIVATE
        ${wxSETUP_HEADER_PATH}
        )
    set(JPEG_LIBRARIES wxjpeg)
    set(JPEG_INCLUDE_DIR ${wxSOURCE_DIR}/src/jpeg)
elseif(wxUSE_LIBJPEG)
    find_package(JPEG REQUIRED)
endif()
