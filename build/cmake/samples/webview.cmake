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

if(wxUSE_WEBVIEW_CHROMIUM)
    wx_add_sample(webview LIBRARIES webview_chromium stc adv NAME webviewsample_chromium)
    if(TARGET webviewsample_chromium)
        target_compile_definitions(webviewsample_chromium PRIVATE -DwxWEBVIEW_SAMPLE_CHROMIUM)
        if(WIN32)
            add_custom_command(
                TARGET webviewsample_chromium
                POST_BUILD
                # Add compatiblity manifest
                COMMAND
                    mt.exe /nologo -manifest ${wxSOURCE_DIR}/samples/webview/compatibility.manifest
                        "-inputresource:$<TARGET_FILE:webviewsample_chromium>;#1"
                        "-outputresource:$<TARGET_FILE:webviewsample_chromium>;#1"
                # Copy CEF libraries
                COMMAND
                    ${CMAKE_COMMAND} -E copy_directory ${CEF_ROOT}/$<CONFIG> $<TARGET_FILE_DIR:webviewsample_chromium>
                # Copy CEF resources
                COMMAND
                    ${CMAKE_COMMAND} -E copy_directory ${CEF_ROOT}/Resources $<TARGET_FILE_DIR:webviewsample_chromium>
                COMMENT "Prepare executable for runtime..."
            )
        elseif(APPLE)
            # Define helper bundle
            set(CEF_HELPER_OUTPUT_NAME "webviewsample_chromium Helper")
            add_executable(webviewsample_chromium_helper MACOSX_BUNDLE ${wxSOURCE_DIR}/samples/webview/cef_process_helper.cpp)
            target_include_directories(webviewsample_chromium_helper PRIVATE ${CEF_ROOT})
            target_link_libraries(webviewsample_chromium_helper libcef libcef_dll_wrapper)
            set_target_properties(webviewsample_chromium_helper PROPERTIES
                MACOSX_BUNDLE_INFO_PLIST ${wxSOURCE_DIR}/samples/webview/cef_helper_info.plist.in
                INSTALL_RPATH "@executable_path/../../../.."
                BUILD_WITH_INSTALL_RPATH TRUE
                OUTPUT_NAME ${CEF_HELPER_OUTPUT_NAME}
                CXX_STANDARD 11
            )

            if(NOT wxBUILD_SHARED)
                set_target_properties(webviewsample_chromium PROPERTIES
                    INSTALL_RPATH "@executable_path/.."
                    BUILD_WITH_INSTALL_RPATH TRUE
                )
            endif()

            add_dependencies(webviewsample_chromium webviewsample_chromium_helper)

            add_custom_command(
                TARGET webviewsample_chromium
                POST_BUILD
                # Copy the helper app bundle into the Frameworks directory.
                COMMAND
                    ${CMAKE_COMMAND} -E copy_directory
                    "$<TARGET_FILE_DIR:webviewsample_chromium_helper>/../../../${CEF_HELPER_OUTPUT_NAME}.app"
                    "$<TARGET_FILE_DIR:webviewsample_chromium>/../Frameworks/${CEF_HELPER_OUTPUT_NAME}.app"
                # Copy the CEF framework into the Frameworks directory.
                COMMAND ${CMAKE_COMMAND} -E copy_directory
                        "${CEF_ROOT}/$<CONFIG>/Chromium Embedded Framework.framework"
                        "$<TARGET_FILE_DIR:webviewsample_chromium>/../Frameworks/Chromium Embedded Framework.framework"
            )
        else()
            add_custom_command(
                TARGET webviewsample_chromium
                POST_BUILD
                # Copy CEF libraries
                COMMAND
                    ${CMAKE_COMMAND} -E copy_directory ${CEF_ROOT}/Release $<TARGET_FILE_DIR:webviewsample_chromium>
                # Copy CEF resources
                COMMAND
                    ${CMAKE_COMMAND} -E copy_directory ${CEF_ROOT}/Resources $<TARGET_FILE_DIR:webviewsample_chromium>
                COMMENT "Prepare executable for runtime..."
            )
        endif()
    endif()
endif()
