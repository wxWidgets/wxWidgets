#############################################################################
# Name:        build/cmake/utils/CMakeLists.txt
# Purpose:     CMake script for utilities
# Author:      Tobias Taschner
# Created:     2016-10-21
# Copyright:   (c) 2016 wxWidgets development team
# Licence:     wxWindows licence
#############################################################################

if(wxUSE_XRC)
    add_executable(wxrc "${wxSOURCE_DIR}/utils/wxrc/wxrc.cpp")
    wx_set_common_target_properties(wxrc)
    if(wxUSE_XML)
        wx_exe_link_libraries(wxrc wxxml)
    endif()
    wx_exe_link_libraries(wxrc wxbase)

    set(wxrc_output_name "wxrc")
    if(NOT WIN32_MSVC_NAMING)
        wx_get_flavour(lib_flavour "-")
        set(wxrc_output_name "wxrc-${wxMAJOR_VERSION}.${wxMINOR_VERSION}${lib_flavour}")
    endif()

    set_target_properties(wxrc PROPERTIES OUTPUT_NAME ${wxrc_output_name})

    set_target_properties(wxrc PROPERTIES FOLDER "Utilities")

    wx_install(TARGETS wxrc
        RUNTIME DESTINATION "bin"
        BUNDLE DESTINATION "bin"
        )

    if(NOT WIN32_MSVC_NAMING)
        if(IPHONE)
            set(EXE_SUFFIX ".app")
        else()
            set(EXE_SUFFIX ${CMAKE_EXECUTABLE_SUFFIX})
        endif()

        install(CODE "execute_process( \
            COMMAND ${CMAKE_COMMAND} -E create_symlink \
            ${CMAKE_INSTALL_PREFIX}/bin/${wxrc_output_name}${EXE_SUFFIX} \
            ${CMAKE_INSTALL_PREFIX}/bin/wxrc${EXE_SUFFIX} \
            )"
        )
    endif()
endif()

# TODO: build targets for other utils
