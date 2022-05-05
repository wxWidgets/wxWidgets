#############################################################################
# Name:        build/cmake/lib/nanosvg.cmake
# Purpose:     Use external or internal nanosvg lib
# Author:      Tamas Meszaros, Maarten Bent
# Created:     2022-05-05
# Copyright:   (c) 2022 wxWidgets development team
# Licence:     wxWindows licence
#############################################################################

if(wxUSE_NANOSVG STREQUAL "builtin")
    set(wxUSE_NANOSVG_EXTERNAL 0 PARENT_SCOPE)
elseif(wxUSE_NANOSVG)
    set(wxUSE_NANOSVG_EXTERNAL 1 PARENT_SCOPE)

    find_package(NanoSVG REQUIRED)

    set(NANOSVG_LIBRARIES NanoSVG::nanosvgrast)
    get_target_property(svg_incl_dir NanoSVG::nanosvg INTERFACE_INCLUDE_DIRECTORIES)
    set(NANOSVG_INCLUDE_DIRS ${svg_incl_dir})

    get_target_property(svg_lib_d NanoSVG::nanosvgrast IMPORTED_LOCATION_DEBUG)
    get_target_property(svg_lib_r NanoSVG::nanosvgrast IMPORTED_LOCATION_RELEASE)
    get_target_property(svg_lib   NanoSVG::nanosvgrast IMPORTED_LOCATION)
    set(wxUSE_NANOSVG_EXTERNAL_ENABLE_IMPL TRUE)
    if(svg_lib_d OR svg_lib_r OR svg_lib)
        set(wxUSE_NANOSVG_EXTERNAL_ENABLE_IMPL FALSE)
    endif()
endif()
