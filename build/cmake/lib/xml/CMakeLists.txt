#############################################################################
# Name:        build/cmake/lib/xml/CMakeLists.txt
# Purpose:     CMake file for xml library
# Author:      Tobias Taschner
# Created:     2016-09-20
# Copyright:   (c) 2016 wxWidgets development team
# Licence:     wxWindows licence
#############################################################################

include(../../source_groups.cmake)

if(WIN32)
    set(EXPAT_POSTFIX $<$<CONFIG:Debug>:d>)
endif(WIN32)

if(wxUSE_EXPAT STREQUAL "builtin")
    ExternalProject_Add(wxexpat
        DOWNLOAD_COMMAND ""
        SOURCE_DIR ${wxSOURCE_DIR}/src/expat/expat
        CMAKE_ARGS
            -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
            -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
            -DBUILD_tools=OFF
            -DBUILD_examples=OFF
            -DBUILD_tests=OFF
            -DBUILD_shared=OFF
        INSTALL_COMMAND
            ${CMAKE_COMMAND} --build <BINARY_DIR> --config $<CONFIGURATION> --target install
        COMMAND
            ${CMAKE_COMMAND} -E make_directory <INSTALL_DIR>/wxlib
        COMMAND
            ${CMAKE_COMMAND} -E rename
                <INSTALL_DIR>/lib/${CMAKE_STATIC_LIBRARY_PREFIX}expat${EXPAT_POSTFIX}${CMAKE_STATIC_LIBRARY_SUFFIX}
                <INSTALL_DIR>/wxlib/${CMAKE_STATIC_LIBRARY_PREFIX}wxexpat$<$<CONFIG:Debug>:d>${CMAKE_STATIC_LIBRARY_SUFFIX}
        )
    ExternalProject_Get_Property(wxexpat INSTALL_DIR)
    add_library(expat STATIC IMPORTED)
    set_target_properties(expat PROPERTIES
        IMPORTED_LOCATION "${INSTALL_DIR}/wxlib/${CMAKE_STATIC_LIBRARY_PREFIX}wxexpat${CMAKE_STATIC_LIBRARY_SUFFIX}"
        IMPORTED_LOCATION_DEBUG "${INSTALL_DIR}/wxlib/${CMAKE_STATIC_LIBRARY_PREFIX}wxexpatd${CMAKE_STATIC_LIBRARY_SUFFIX}"
        FOLDER "Third Party Libraries"
        )
    add_dependencies(expat wxexpat)
    set(EXPAT_INCLUDE_DIRS "${INSTALL_DIR}/include")
    set(EXPAT_LIBRARIES expat)
    if(NOT wxBUILD_SHARED)
        wx_install(
            FILES ${INSTALL_DIR}/wxlib/${CMAKE_STATIC_LIBRARY_PREFIX}wxexpat$<$<CONFIG:Debug>:d>${CMAKE_STATIC_LIBRARY_SUFFIX}
            DESTINATION "lib${wxPLATFORM_LIB_DIR}")
    endif()
elseif(wxUSE_EXPAT)
    find_package(EXPAT)
endif()

wx_append_sources(XML_FILES XML)
wx_add_library(xml IS_BASE ${XML_FILES})
wx_lib_link_libraries(xml
    PRIVATE ${EXPAT_LIBRARIES}
    )
wx_lib_include_directories(xml PRIVATE ${EXPAT_INCLUDE_DIRS})

wx_finalize_lib(xml)
