#############################################################################
# Name:        build/cmake/samples/webview.cmake
# Purpose:     CMake script to build webview samples
# Author:      Tobias Taschner
# Created:     2018-02-17
# Copyright:   (c) 2018 wxWidgets development team
# Licence:     wxWindows licence
#############################################################################

wx_add_sample(webview LIBRARIES webview stc adv NAME webviewsample)

if(wxUSE_WEBVIEW_CHROMIUM)
    wx_add_sample(webview LIBRARIES webview_chromium stc adv NAME webviewsample_chromium)
    target_compile_definitions(webviewsample_chromium PRIVATE -DwxWEBVIEW_SAMPLE_CHROMIUM)
    ExternalProject_Get_property(cef SOURCE_DIR)
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
                ${CMAKE_COMMAND} -E copy_directory ${SOURCE_DIR}/$<CONFIG> $<TARGET_FILE_DIR:webviewsample_chromium>
            # Copy CEF resources
            COMMAND
                ${CMAKE_COMMAND} -E copy_directory ${SOURCE_DIR}/Resources $<TARGET_FILE_DIR:webviewsample_chromium>
            COMMENT "Prepare executable for runtime..." 
        )
    elseif(APPLE)
        # TODO: define and build helper bundle
    endif()
endif()
