#############################################################################
# Name:        build/cmake/demos/CMakeLists.txt
# Purpose:     CMake script for demos
# Author:      Tobias Taschner
# Created:     2016-10-21
# Copyright:   (c) 2016 wxWidgets development team
# Licence:     wxWindows licence
#############################################################################

function(wx_add_demo name)
    cmake_parse_arguments(DEMO "" "NAME" "DATA;LIBRARIES" ${ARGN})
    if(NOT DEMO_NAME)
        set(DEMO_NAME ${name})
    endif()
    wx_list_add_prefix(src_files
        "${wxSOURCE_DIR}/demos/${name}/"
        ${DEMO_UNPARSED_ARGUMENTS})
    if(WIN32)
        list(APPEND src_files ${wxSOURCE_DIR}/demos/${name}/${DEMO_NAME}.rc)
    endif()

    if (WXMSW AND DEFINED wxUSE_DPI_AWARE_MANIFEST)
        set(wxDPI_MANIFEST_PRFIX "wx")
        if (wxARCH_SUFFIX)
            set(wxDPI_MANIFEST_PRFIX "amd64")
        endif()
        set(wxUSE_DPI_AWARE_MANIFEST_VALUE 0)
        if (${wxUSE_DPI_AWARE_MANIFEST} MATCHES "system")
            set(wxUSE_DPI_AWARE_MANIFEST_VALUE 1)
            list(APPEND src_files "${wxSOURCE_DIR}/include/wx/msw/${wxDPI_MANIFEST_PRFIX}_dpi_aware.manifest")
        elseif(${wxUSE_DPI_AWARE_MANIFEST} MATCHES "per-monitor")
            set(wxUSE_DPI_AWARE_MANIFEST_VALUE 2)
            list(APPEND src_files "${wxSOURCE_DIR}/include/wx/msw/${wxDPI_MANIFEST_PRFIX}_dpi_aware_pmv2.manifest")
        endif()
    endif()

    add_executable(${DEMO_NAME} WIN32 MACOSX_BUNDLE ${src_files})

    if (DEFINED wxUSE_DPI_AWARE_MANIFEST_VALUE)
        target_compile_definitions(${DEMO_NAME} PRIVATE wxUSE_DPI_AWARE_MANIFEST=${wxUSE_DPI_AWARE_MANIFEST_VALUE})
    endif()

    if(DEMO_DATA)
        # TODO: unify with data handling for samples
        # TODO: handle data files differently for OS X bundles
        # Copy data files to output directory
        foreach(data_file ${DEMO_DATA})
            list(APPEND cmds COMMAND ${CMAKE_COMMAND}
                -E copy ${wxSOURCE_DIR}/demos/${name}/${data_file}
                ${wxOUTPUT_DIR}/${wxPLATFORM_LIB_DIR}/${data_file})
        endforeach()
        add_custom_command(
            TARGET ${DEMO_NAME} ${cmds}
            COMMENT "Copying demo data files...")
    endif()
    if(wxBUILD_SHARED)
        target_compile_definitions(${DEMO_NAME} PRIVATE WXUSINGDLL)
    endif()
    wx_exe_link_libraries(${DEMO_NAME} wxcore ${DEMO_LIBRARIES})
    wx_set_common_target_properties(${DEMO_NAME})
    set_target_properties(${DEMO_NAME} PROPERTIES FOLDER "Demos")
    set_target_properties(${DEMO_NAME} PROPERTIES
        VS_DEBUGGER_WORKING_DIRECTORY "${wxOUTPUT_DIR}/${wxCOMPILER_PREFIX}${wxARCH_SUFFIX}_${lib_suffix}"
        )
endfunction()

wx_add_demo(bombs
    bombs.cpp
    bombs.h
    bombs1.cpp
    game.cpp
    game.h
    )

wx_add_demo(forty
    canvas.cpp
    canvas.h
    card.cpp
    card.h
    forty.cpp
    forty.h
    game.cpp
    game.h
    pile.cpp
    pile.h
    playerdg.cpp
    playerdg.h
    scoredg.cpp
    scoredg.h
    scorefil.cpp
    scorefil.h
    DATA
        about.htm
    LIBRARIES
        wxhtml wxxml
    )

wx_add_demo(fractal
    fractal.cpp
    )

wx_add_demo(life
    dialogs.cpp
    dialogs.h
    game.cpp
    game.h
    life.cpp
    life.h
    reader.cpp
    reader.h
    DATA
        breeder.lif
    )

wx_add_demo(poem
    wxpoem.cpp
    wxpoem.h
    DATA
        wxpoem.txt wxpoem.dat wxpoem.idx
    LIBRARIES
        wxhtml
    NAME
        wxpoem
    )
