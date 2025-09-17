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
        DESTINATION "${library_dir}/wx/include")

    install(
        FILES "${wxOUTPUT_DIR}/wx/config/${wxBUILD_FILE_ID}"
        DESTINATION "${library_dir}/wx/config"
        PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ
                    GROUP_EXECUTE GROUP_READ
                    WORLD_EXECUTE WORLD_READ
        )

    wx_get_install_platform_dir(runtime)
    install(DIRECTORY DESTINATION "${runtime_dir}")
    install(CODE "execute_process( \
        COMMAND ${CMAKE_COMMAND} -E create_symlink \
        \"${CMAKE_INSTALL_PREFIX}/${library_dir}/wx/config/${wxBUILD_FILE_ID}\" \
        \"\$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/${runtime_dir}/wx-config\" \
        )"
    )
    list(APPEND WX_EXTRA_UNINSTALL_FILES "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/${runtime_dir}/wx-config")
endif()

wx_get_install_dir(library)
set(wx_cmake_dir "${library_dir}/cmake/wxWidgets-${wxMAJOR_VERSION}.${wxMINOR_VERSION}")

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
