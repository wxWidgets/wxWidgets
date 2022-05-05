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

    set(NANOSVG_LIBRARIES )
    set(NANOSVG_INCLUDE_DIRS )
    set(wxUSE_NANOSVG_EXTERNAL_ENABLE_IMPL TRUE)

    find_package(NanoSVG REQUIRED)

    foreach(TARGETNAME NanoSVG::nanosvg NanoSVG::nanosvgrast unofficial::nanosvg)
        if(NOT TARGET ${TARGETNAME})
            continue()
        endif()

        list(APPEND NANOSVG_LIBRARIES ${TARGETNAME})
        get_target_property(svg_incl_dir ${TARGETNAME} INTERFACE_INCLUDE_DIRECTORIES)
        if(svg_incl_dir)
            list(APPEND NANOSVG_INCLUDE_DIRS ${svg_incl_dir})
        endif()

        get_target_property(svg_lib_d ${TARGETNAME} IMPORTED_LOCATION_DEBUG)
        get_target_property(svg_lib_r ${TARGETNAME} IMPORTED_LOCATION_RELEASE)
        get_target_property(svg_lib   ${TARGETNAME} IMPORTED_LOCATION)
        if(svg_lib_d OR svg_lib_r OR svg_lib)
            set(wxUSE_NANOSVG_EXTERNAL_ENABLE_IMPL FALSE)
        endif()
    endforeach()
endif()
