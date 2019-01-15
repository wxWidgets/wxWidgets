#############################################################################
# Name:        build/cmake/lib/tiff.cmake
# Purpose:     Use external or internal libtiff
# Author:      Tobias Taschner
# Created:     2016-09-21
# Copyright:   (c) 2016 wxWidgets development team
# Licence:     wxWindows licence
#############################################################################

if(wxUSE_LIBTIFF STREQUAL "builtin")
    # TODO: implement building libtiff via ExternalProject_Add()
    if(UNIX AND NOT APPLE)
        message(WARNING "Builtin libtiff on unix is currently not supported")
        wx_option_force_value(wxUSE_LIBTIFF OFF)
        return()
    endif()

    if(WIN32)
        set(TIFF_PLATFORM_SRC src/tiff/libtiff/tif_win32.c)
    elseif(UNIX)
        set(TIFF_PLATFORM_SRC src/tiff/libtiff/tif_unix.c)
    endif()

    wx_add_builtin_library(wxtiff
        ${TIFF_PLATFORM_SRC}
        src/tiff/libtiff/tif_aux.c
        src/tiff/libtiff/tif_close.c
        src/tiff/libtiff/tif_codec.c
        src/tiff/libtiff/tif_color.c
        src/tiff/libtiff/tif_compress.c
        src/tiff/libtiff/tif_dir.c
        src/tiff/libtiff/tif_dirinfo.c
        src/tiff/libtiff/tif_dirread.c
        src/tiff/libtiff/tif_dirwrite.c
        src/tiff/libtiff/tif_dumpmode.c
        src/tiff/libtiff/tif_error.c
        src/tiff/libtiff/tif_extension.c
        src/tiff/libtiff/tif_fax3.c
        src/tiff/libtiff/tif_fax3sm.c
        src/tiff/libtiff/tif_flush.c
        src/tiff/libtiff/tif_getimage.c
        src/tiff/libtiff/tif_jbig.c
        src/tiff/libtiff/tif_jpeg.c
        src/tiff/libtiff/tif_jpeg_12.c
        src/tiff/libtiff/tif_luv.c
        src/tiff/libtiff/tif_lzma.c
        src/tiff/libtiff/tif_lzw.c
        src/tiff/libtiff/tif_next.c
        src/tiff/libtiff/tif_ojpeg.c
        src/tiff/libtiff/tif_open.c
        src/tiff/libtiff/tif_packbits.c
        src/tiff/libtiff/tif_pixarlog.c
        src/tiff/libtiff/tif_predict.c
        src/tiff/libtiff/tif_print.c
        src/tiff/libtiff/tif_read.c
        src/tiff/libtiff/tif_strip.c
        src/tiff/libtiff/tif_swab.c
        src/tiff/libtiff/tif_thunder.c
        src/tiff/libtiff/tif_tile.c
        src/tiff/libtiff/tif_version.c
        src/tiff/libtiff/tif_warning.c
        src/tiff/libtiff/tif_webp.c
        src/tiff/libtiff/tif_write.c
        src/tiff/libtiff/tif_zip.c
        src/tiff/libtiff/tif_zstd.c
    )
    if(WIN32)
        # define this to get rid of a warning about using POSIX lfind():
        # confusingly enough, we do define lfind as _lfind for MSVC but
        # doing this results in a just more confusing warning, see:
        # http://connect.microsoft.com/VisualStudio/feedback/ViewFeedback.aspx?FeedbackID=101278
        target_compile_definitions(wxtiff PRIVATE _CRT_NONSTDC_NO_WARNINGS)
    endif()
    target_include_directories(wxtiff PRIVATE
        ${wxSOURCE_DIR}/src/tiff/libtiff
        ${ZLIB_INCLUDE_DIRS}
        ${JPEG_INCLUDE_DIR}
        )
    target_link_libraries(wxtiff PRIVATE ${ZLIB_LIBRARIES} ${JPEG_LIBRARIES})
    set(TIFF_LIBRARIES wxtiff)
    set(TIFF_INCLUDE_DIRS ${wxSOURCE_DIR}/src/tiff/libtiff)
elseif(wxUSE_LIBTIFF)
    find_package(TIFF REQUIRED)
endif()
