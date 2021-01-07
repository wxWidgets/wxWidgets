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
if(MSVC)
    wx_install(
        DIRECTORY "${wxSOURCE_DIR}/include/wx"
        DESTINATION "include")
    wx_install(
        DIRECTORY "${wxSOURCE_DIR}/include/msvc"
        DESTINATION "include")
else()
    wx_install(
        DIRECTORY "${wxSOURCE_DIR}/include/wx"
        DESTINATION "include/wx-${wxMAJOR_VERSION}.${wxMINOR_VERSION}")
endif()

# setup header and wx-config
if(MSVC)
    wx_install(
        DIRECTORY "${wxSETUP_HEADER_PATH}"
        DESTINATION "lib${wxPLATFORM_LIB_DIR}")
else()
    wx_install(
        DIRECTORY "${wxSETUP_HEADER_PATH}"
        DESTINATION "lib/wx/include")

    wx_install(
        FILES "${wxOUTPUT_DIR}/wx/config/${wxBUILD_FILE_ID}"
        DESTINATION "lib/wx/config"
        PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ
                    GROUP_EXECUTE GROUP_READ
                    WORLD_EXECUTE WORLD_READ
        )

    install(DIRECTORY DESTINATION "bin")
    install(CODE "execute_process( \
        COMMAND ${CMAKE_COMMAND} -E create_symlink \
        ${CMAKE_INSTALL_PREFIX}/lib/wx/config/${wxBUILD_FILE_ID} \
        ${CMAKE_INSTALL_PREFIX}/bin/wx-config \
        )"
    )
endif()

# uninstall target
if(MSVC_IDE)
    set(UNINST_NAME UNINSTALL)
else()
    set(UNINST_NAME uninstall)
endif()

if(NOT TARGET ${UNINST_NAME})
    # these files are not added to the install manifest
    set(WX_EXTRA_UNINSTALL_FILES
        "${CMAKE_INSTALL_PREFIX}/bin/wx-config"
        "${CMAKE_INSTALL_PREFIX}/bin/wxrc-${wxMAJOR_VERSION}.${wxMINOR_VERSION}"
        )

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
