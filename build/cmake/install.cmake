#############################################################################
# Name:        build/cmake/install.cmake
# Purpose:     Install target CMake file
# Author:      Tobias Taschner
# Created:     2016-10-17
# Copyright:   (c) 2016 wxWidgets development team
# Licence:     wxWindows licence
#############################################################################

if(NOT wxBUILD_INSTALL)
    return()
endif()

install(CODE "message(STATUS \"Installing: Headers...\")")
wx_install(
    DIRECTORY "${wxSOURCE_DIR}/include/wx"
    DESTINATION "include")
if(MSVC)
    wx_install(
        DIRECTORY "${wxSOURCE_DIR}/include/msvc"
        DESTINATION "include")
endif()
if(MSVC OR MINGW)
    wx_install(
        DIRECTORY "${wxSETUP_HEADER_PATH}"
        DESTINATION "lib${wxPLATFORM_LIB_DIR}")
endif()

# uninstall target
if(MSVC_IDE)
    set(UNINST_NAME UNINSTALL)
else()
    set(UNINST_NAME uninstall)
endif()

if(NOT TARGET ${UNINST_NAME})
    configure_file(
        "${wxSOURCE_DIR}/build/cmake/uninstall.cmake.in"
        "${wxBINARY_DIR}/uninstall.cmake"
        IMMEDIATE @ONLY)

    add_custom_target(${UNINST_NAME}
        COMMAND ${CMAKE_COMMAND} -P ${wxBINARY_DIR}/uninstall.cmake)
    get_property(PREDEF_FOLDER GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER)
    if(NOT PREDEF_FOLDER)
        set(PREDEF_FOLDER "CMakePredefinedTargets")
    endif()
    set_target_properties(${UNINST_NAME} PROPERTIES FOLDER "${PREDEF_FOLDER}")
endif()
