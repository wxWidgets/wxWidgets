#############################################################################
# Name:        build/cmake/samples/webview.cmake
# Purpose:     CMake script to build webview samples
# Author:      Tobias Taschner
# Created:     2018-02-17
# Copyright:   (c) 2018 wxWidgets development team
# Licence:     wxWindows licence
#############################################################################

wx_add_sample(webview LIBRARIES wxwebview
    DATA ../help/doc.zip:doc.zip handler_advanced.html
    NAME webviewsample)
if(wxUSE_STC)
    wx_exe_link_libraries(webviewsample wxstc)
endif()

if(wxUSE_WEBVIEW_CHROMIUM AND TARGET webviewsample)
    if(WIN32 OR (UNIX AND NOT APPLE))
        # Copy CEF libraries and resources next to the executable.
        add_custom_command(
            TARGET webviewsample
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory ${CEF_ROOT}/Release $<TARGET_FILE_DIR:webviewsample>
            COMMAND ${CMAKE_COMMAND} -E copy_directory ${CEF_ROOT}/Resources $<TARGET_FILE_DIR:webviewsample>
            COMMENT "Copying webviewsample CEF resources..."
        )
        if(UNIX)
            # Set rpath so that libraries can be placed next to the executable.
            # Fixes error: "Invalid file descriptor to ICU data received."
            set_target_properties(webviewsample PROPERTIES
                INSTALL_RPATH "$ORIGIN"
                BUILD_WITH_INSTALL_RPATH TRUE
            )
        endif()
    elseif(APPLE)
        # Copy the CEF framework into the Frameworks directory.
        add_custom_command(
            TARGET webviewsample
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${CEF_ROOT}/Release/Chromium Embedded Framework.framework"
                "$<TARGET_FILE_DIR:webviewsample>/../Frameworks/Chromium Embedded Framework.framework"
        )

        # CEF Helper app suffixes.
        # Format is "<name suffix>:<target suffix>:<plist suffix>".
        set(CEF_HELPER_APP_SUFFIXES
            "::"
            " (Alerts):_alerts:.alerts"
            " (GPU):_gpu:.gpu"
            " (Plugin):_plugin:.plugin"
            " (Renderer):_renderer:.renderer"
        )
        set(CEF_HELPER_TARGET "webviewsample_Helper")
        set(CEF_HELPER_OUTPUT_NAME "webviewsample Helper")

        # Create the multiple Helper app bundle targets.
        foreach(_suffix_list ${CEF_HELPER_APP_SUFFIXES})
            # Convert to a list and extract the suffix values.
            string(REPLACE ":" ";" _suffix_list ${_suffix_list})
            list(GET _suffix_list 0 _name_suffix)
            list(GET _suffix_list 1 _target_suffix)
            list(GET _suffix_list 2 _plist_suffix)

            # Define Helper target and output names.
            set(_helper_target "${CEF_HELPER_TARGET}${_target_suffix}")
            set(_helper_output_name "${CEF_HELPER_OUTPUT_NAME}${_name_suffix}")

            # Create Helper-specific variants of the helper-Info.plist file. Do this
            # manually because the configure_file command (which is executed as part of
            # MACOSX_BUNDLE_INFO_PLIST) uses global env variables and would insert the
            # wrong values with multiple targets.
            set(_helper_info_plist "${CMAKE_CURRENT_BINARY_DIR}/helper-Info${_target_suffix}.plist")
            file(READ "${wxSOURCE_DIR}/samples/webview/cef_helper_info.plist.in" _plist_contents)
            string(REPLACE "\${EXECUTABLE_NAME}" "${_helper_output_name}" _plist_contents ${_plist_contents})
            string(REPLACE "\${PRODUCT_NAME}" "${_helper_output_name}" _plist_contents ${_plist_contents})
            string(REPLACE "\${BUNDLE_ID_SUFFIX}" "${_plist_suffix}" _plist_contents ${_plist_contents})
            file(WRITE ${_helper_info_plist} ${_plist_contents})

            # Create Helper executable target.
            add_executable(${_helper_target} MACOSX_BUNDLE ${wxSOURCE_DIR}/samples/webview/cef_process_helper.cpp)
            target_include_directories(${_helper_target} PRIVATE "${CEF_ROOT}")
            target_link_libraries(${_helper_target} libcef_dll_wrapper)
            set_target_properties(${_helper_target} PROPERTIES
                MACOSX_BUNDLE_INFO_PLIST ${_helper_info_plist}
                OUTPUT_NAME ${_helper_output_name}
            )

            # Add the Helper as a dependency of the main executable target.
            add_dependencies(webviewsample "${_helper_target}")

            # Copy the Helper app bundle into the Frameworks directory.
            add_custom_command(
                TARGET webviewsample
                POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_directory
                    "$<TARGET_FILE_DIR:${_helper_target}>/../../../${_helper_output_name}.app"
                    "$<TARGET_FILE_DIR:webviewsample>/../Frameworks/${_helper_output_name}.app"
                VERBATIM
            )
        endforeach()
    endif()
endif()
