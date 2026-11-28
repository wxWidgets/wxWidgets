#############################################################################
# Name:        build/cmake/lib/nanosvg.cmake
# Purpose:     Use external or internal nanosvg lib
# Author:      Tamas Meszaros, Maarten Bent
# Created:     2022-05-05
# Copyright:   (c) 2022 wxWidgets development team
# Licence:     wxWindows licence
#############################################################################

if(wxUSE_NANOSVG STREQUAL "sys")
    find_package(NanoSVG)
    if(NOT NanoSVG_FOUND)
        # If the sys library can not be found use builtin
        wx_option_force_value(wxUSE_NANOSVG builtin)
    endif()
endif()

if(wxUSE_NANOSVG STREQUAL "builtin")
    set(wxUSE_NANOSVG_EXTERNAL 0 PARENT_SCOPE)
elseif(wxUSE_NANOSVG)
    set(wxUSE_NANOSVG_EXTERNAL 1 PARENT_SCOPE)

    set(NANOSVG_LIBRARIES )
    set(NANOSVG_INCLUDE_DIRS )
    set(wxUSE_NANOSVG_EXTERNAL_ENABLE_IMPL TRUE)

    foreach(TARGETNAME NanoSVG::nanosvg NanoSVG::nanosvgrast unofficial::nanosvg)
        if(NOT TARGET ${TARGETNAME})
            continue()
        endif()

        list(APPEND NANOSVG_LIBRARIES ${TARGETNAME})
        get_target_property(svg_incl_dir ${TARGETNAME} INTERFACE_INCLUDE_DIRECTORIES)
        if(svg_incl_dir)
            list(APPEND NANOSVG_INCLUDE_DIRS ${svg_incl_dir})

            # The headers are included as <nanosvg/nanosvg.h>, matching their
            # installed location, so the parent directory has to be on the
            # search path as well (it usually, but not always, already is).
            get_filename_component(svg_incl_parent "${svg_incl_dir}" DIRECTORY)
            if(svg_incl_parent)
                list(APPEND NANOSVG_INCLUDE_DIRS ${svg_incl_parent})
            endif()
        endif()

        # If the package provides a compiled library (rather than just an
        # INTERFACE target carrying the headers), link with it instead of
        # building the NanoSVG implementation into wxWidgets ourselves.
        get_target_property(svg_target_type ${TARGETNAME} TYPE)
        if(NOT svg_target_type STREQUAL "INTERFACE_LIBRARY")
            set(wxUSE_NANOSVG_EXTERNAL_ENABLE_IMPL FALSE)
        endif()
    endforeach()
endif()
