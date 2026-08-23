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

foreach(header ${wxINSTALL_HEADERS})
    get_filename_component(path "${header}" PATH)
    install(
        FILES "${wxSOURCE_DIR}/include/${header}"
        DESTINATION "${wxINSTALL_INCLUDE_DIR}/${path}"
    )
endforeach()

if(MSVC)
    install(
        DIRECTORY "${wxSOURCE_DIR}/include/msvc"
        DESTINATION "${wxINSTALL_INCLUDE_DIR}"
    )
    install(
        FILES "${wxSOURCE_DIR}/wxwidgets.props"
        DESTINATION "."
    )
    install(
        FILES "${wxSOURCE_DIR}/build/msw/wx_setup.props"
        DESTINATION "build/msw"
    )
endif()

wx_get_install_platform_dir(library)

# setup header and wx-config
if(WIN32_MSVC_NAMING)
    # create both Debug and Release directories, so CMake doesn't complain about
    # non-existent path when only Release or Debug build has been installed
    set(lib_unicode "u")
    install(DIRECTORY
        DESTINATION "${library_dir}/${wxBUILD_TOOLKIT}${lib_unicode}")
    install(DIRECTORY
        DESTINATION "${library_dir}/${wxBUILD_TOOLKIT}${lib_unicode}d")
    install(
        DIRECTORY "${wxSETUP_HEADER_PATH}"
        DESTINATION "${library_dir}")
else()
    install(
        DIRECTORY "${wxSETUP_HEADER_PATH}"
        DESTINATION "${library_dir}/wx/include"
        PATTERN "protocols" EXCLUDE
    )

    install(
        FILES "${wxOUTPUT_DIR}/wx/config/${wxBUILD_FILE_ID}"
        DESTINATION "${library_dir}/wx/config"
        PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ
                    GROUP_EXECUTE GROUP_READ
                    WORLD_EXECUTE WORLD_READ
        )

    wx_get_install_platform_dir(runtime)
    install(DIRECTORY DESTINATION "${runtime_dir}")
    set(CONFIG_DIR "\$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}")
    set(CONFIG_SRC "${CONFIG_DIR}/${library_dir}/wx/config/${wxBUILD_FILE_ID}")
    set(CONFIG_DST "${CONFIG_DIR}/${runtime_dir}/wx-config")
    wx_install_symlink(${CONFIG_SRC} ${CONFIG_DST})

    list(APPEND WX_EXTRA_UNINSTALL_FILES "\"${CMAKE_INSTALL_PREFIX}/${runtime_dir}/wx-config\"")
endif()

wx_get_install_dir(library)
set(wx_cmake_dir "${library_dir}/cmake/wxWidgets-${wxMAJOR_VERSION}.${wxMINOR_VERSION}")

if(WXWINUI AND wxUSE_WINUI3)
    set(wx_winui_cmake_dir "${wx_cmake_dir}/${wxPLATFORM_LIB_DIR}")
    set(wx_winui_package_dir "${wx_winui_cmake_dir}/winui")
    set(wx_winui_runtime_dir "${wx_winui_package_dir}/runtime")
    set(wx_winui_config "${wxBINARY_DIR}/winui/install/wxWinUIConfig.cmake")
    configure_file(
        "${wxSOURCE_DIR}/build/cmake/wxWinUIConfig.cmake.in"
        "${wx_winui_config}"
        @ONLY)

    install(FILES "${wx_winui_config}"
        DESTINATION "${wx_winui_cmake_dir}")
    install(FILES "${wxWINUI3_INSTALL_DEPLOY_SCRIPT}"
        DESTINATION "${wx_winui_package_dir}")

    # Preserve the executable-relative layout used by the deployment script.
    list(LENGTH wxWINUI3_RUNTIME_PAYLOAD_SRC wx_winui_payload_count)
    math(EXPR wx_winui_payload_last "${wx_winui_payload_count} - 1")
    foreach(wx_winui_payload_index RANGE ${wx_winui_payload_last})
        list(GET wxWINUI3_RUNTIME_PAYLOAD_SRC
            ${wx_winui_payload_index} wx_winui_payload_src)
        list(GET wxWINUI3_RUNTIME_PAYLOAD_DST
            ${wx_winui_payload_index} wx_winui_payload_dst)
        get_filename_component(wx_winui_payload_dst_dir
            "${wx_winui_payload_dst}" DIRECTORY)
        set(wx_winui_payload_install_dir "${wx_winui_runtime_dir}")
        if(wx_winui_payload_dst_dir)
            string(APPEND wx_winui_payload_install_dir
                "/${wx_winui_payload_dst_dir}")
        endif()
        install(FILES "${wx_winui_payload_src}"
            DESTINATION "${wx_winui_payload_install_dir}")
    endforeach()

    # A shared wxWidgets DLL consumes the Windows App SDK import libraries
    # privately. Static consumers need them transitively, so package the two
    # pinned artifacts and let wxWinUIConfig.cmake expose relocatable targets.
    if(NOT wxBUILD_SHARED)
        install(FILES
            "${wxWINUI3_BOOTSTRAP_LIB}"
            "${wxWINUI3_DISPATCHING_LIB}"
            DESTINATION "${wx_winui_package_dir}/lib")
    endif()
endif()

install(EXPORT wxWidgetsTargets NAMESPACE wx:: DESTINATION "${wx_cmake_dir}/${wxPLATFORM_LIB_DIR}")

# find_package config file
include(CMakePackageConfigHelpers)
set(versionConfig "${wxOUTPUT_DIR}/wxWidgetsConfigVersion.cmake")
set(projectConfig "${wxOUTPUT_DIR}/wxWidgetsConfig.cmake")
if(CMAKE_VERSION VERSION_LESS "3.11")
    set(versionCompat SameMajorVersion)
else()
    set(versionCompat SameMinorVersion)
endif()
if(WIN32_MSVC_NAMING AND NOT CMAKE_VERSION VERSION_LESS "3.14")
    set(archCompat ARCH_INDEPENDENT)
endif()

write_basic_package_version_file(
    "${versionConfig}"
    COMPATIBILITY ${versionCompat}
    ${archCompat}
)
configure_package_config_file(
    "${wxSOURCE_DIR}/build/cmake/wxWidgetsConfig.cmake.in"
    "${projectConfig}"
    INSTALL_DESTINATION "${wx_cmake_dir}"
)
install(
    FILES "${projectConfig}" "${versionConfig}"
    DESTINATION "${wx_cmake_dir}"
)

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
