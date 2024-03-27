#############################################################################
# Name:        build/cmake/lib/plutovg.cmake
# Purpose:     Required library for lunasvg
# Author:      Randalphwa
# Created:     2024-03-27
# Copyright:   (c) 2024 wxWidgets development team
# Licence:     wxWindows licence
#############################################################################

if(wxUSE_LUNASVG STREQUAL "builtin")
    wx_add_builtin_library(wxplutovg
        3rdparty/lunasvg/3rdparty/plutovg/plutovg-blend.c
        3rdparty/lunasvg/3rdparty/plutovg/plutovg-dash.c
        3rdparty/lunasvg/3rdparty/plutovg/plutovg-ft-math.c
        3rdparty/lunasvg/3rdparty/plutovg/plutovg-ft-raster.c
        3rdparty/lunasvg/3rdparty/plutovg/plutovg-ft-stroker.c
        3rdparty/lunasvg/3rdparty/plutovg/plutovg-geometry.c
        3rdparty/lunasvg/3rdparty/plutovg/plutovg-paint.c
        3rdparty/lunasvg/3rdparty/plutovg/plutovg-rle.c
        3rdparty/lunasvg/3rdparty/plutovg/plutovg.c
    )
    target_include_directories(wxplutovg PRIVATE
        ${wxSOURCE_DIR}/3rdparty/lunasvg/3rdparty/plutovg
    )
endif()
