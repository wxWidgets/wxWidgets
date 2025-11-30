#############################################################################
# Name:        build/cmake/lib/lunasvg.cmake
# Purpose:     LunaSVG SVG rendering library
# Author:      Randalphwa
# Created:     2024-03-27
# Copyright:   (c) 2024 wxWidgets development team
# Licence:     wxWindows licence
#############################################################################

if(wxUSE_LUNASVG STREQUAL "sys")
    message(FATAL_ERROR "Not supported")
endif()

if(wxUSE_LUNASVG STREQUAL "ON" OR wxUSE_LUNASVG STREQUAL "builtin")
    wx_add_builtin_library(wxlunasvg
        3rdparty/lunasvg/source/lunasvg.cpp
        3rdparty/lunasvg/source/graphics.cpp
        3rdparty/lunasvg/source/svgelement.cpp
        3rdparty/lunasvg/source/svggeometryelement.cpp
        3rdparty/lunasvg/source/svglayoutstate.cpp
        3rdparty/lunasvg/source/svgpaintelement.cpp
        3rdparty/lunasvg/source/svgparser.cpp
        3rdparty/lunasvg/source/svgproperty.cpp
        3rdparty/lunasvg/source/svgrenderstate.cpp
        3rdparty/lunasvg/source/svgtextelement.cpp

        3rdparty/lunasvg/plutovg/source/plutovg-blend.c
        3rdparty/lunasvg/plutovg/source/plutovg-canvas.c
        3rdparty/lunasvg/plutovg/source/plutovg-font.c
        3rdparty/lunasvg/plutovg/source/plutovg-matrix.c
        3rdparty/lunasvg/plutovg/source/plutovg-paint.c
        3rdparty/lunasvg/plutovg/source/plutovg-path.c
        3rdparty/lunasvg/plutovg/source/plutovg-rasterize.c
        3rdparty/lunasvg/plutovg/source/plutovg-surface.c
        3rdparty/lunasvg/plutovg/source/plutovg-ft-math.c
        3rdparty/lunasvg/plutovg/source/plutovg-ft-raster.c
        3rdparty/lunasvg/plutovg/source/plutovg-ft-stroker.c
    )
    target_include_directories(wxlunasvg PRIVATE
        ${wxSOURCE_DIR}/3rdparty/lunasvg/include
        ${wxSOURCE_DIR}/3rdparty/lunasvg/plutovg/include
    )

    set(LUNASVG_LIBRARIES wxlunasvg)
    set(LUNASVG_INCLUDE_DIRS ${wxSOURCE_DIR}/3rdparty/lunasvg/include)
endif()
