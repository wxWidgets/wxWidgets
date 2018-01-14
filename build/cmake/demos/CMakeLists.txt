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
    add_executable(${DEMO_NAME} WIN32 MACOSX_BUNDLE ${src_files})
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
    target_link_libraries(${DEMO_NAME} core ${DEMO_LIBRARIES})
    wx_set_common_target_properties(${DEMO_NAME})
    set_target_properties(${DEMO_NAME} PROPERTIES FOLDER "Demos")
    set_target_properties(${DEMO_NAME} PROPERTIES
        VS_DEBUGGER_WORKING_DIRECTORY "${wxOUTPUT_DIR}/${wxCOMPILER_PREFIX}${wxARCH_SUFFIX}_${lib_suffix}"
        )
endfunction()

wx_add_demo(bombs
    bombs.cpp
    bombs1.cpp
    game.cpp
    )
wx_add_demo(forty
    forty.cpp
    canvas.cpp
    card.cpp
    game.cpp
    pile.cpp
    playerdg.cpp
    scoredg.cpp
    scorefil.cpp
    DATA
        about.htm
    LIBRARIES
        html adv xml
    )
wx_add_demo(fractal fractal.cpp)
wx_add_demo(life
    life.cpp
    dialogs.cpp
    game.cpp
    reader.cpp
    DATA
        breeder.lif
    )
wx_add_demo(poem wxpoem.cpp
    DATA
        wxpoem.txt wxpoem.dat wxpoem.idx
    LIBRARIES
        html
    NAME
        wxpoem
    )
