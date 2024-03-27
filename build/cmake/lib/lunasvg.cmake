#############################################################################
# Name:        build/cmake/lib/lunasvg.cmake
# Purpose:     LunaSVG SVG rengering library
# Author:      Randalphwa
# Created:     2024-03-27
# Copyright:   (c) 2024 wxWidgets development team
# Licence:     wxWindows licence
#############################################################################

if(wxUSE_LUNASVG STREQUAL "builtin")
    wx_add_builtin_library(wxlunasvg
        3rdparty/lunasvg/source/canvas.cpp
        3rdparty/lunasvg/source/clippathelement.cpp
        3rdparty/lunasvg/source/defselement.cpp
        3rdparty/lunasvg/source/element.cpp
        3rdparty/lunasvg/source/gelement.cpp
        3rdparty/lunasvg/source/geometryelement.cpp
        3rdparty/lunasvg/source/graphicselement.cpp
        3rdparty/lunasvg/source/layoutcontext.cpp
        3rdparty/lunasvg/source/lunasvg.cpp
        3rdparty/lunasvg/source/markerelement.cpp
        3rdparty/lunasvg/source/maskelement.cpp
        3rdparty/lunasvg/source/paintelement.cpp
        3rdparty/lunasvg/source/parser.cpp
        3rdparty/lunasvg/source/property.cpp
        3rdparty/lunasvg/source/stopelement.cpp
        3rdparty/lunasvg/source/styledelement.cpp
        3rdparty/lunasvg/source/styleelement.cpp
        3rdparty/lunasvg/source/svgelement.cpp
        3rdparty/lunasvg/source/symbolelement.cpp
        3rdparty/lunasvg/source/useelement.cpp
    )
    target_include_directories(wxlunasvg PRIVATE
        ${wxSOURCE_DIR}/3rdparty/lunasvg/include
        ${wxSOURCE_DIR}/3rdparty/lunasvg/3rdparty/plutovg
    )
    target_link_libraries(wxlunasvg PRIVATE wxplutovg)
endif()
