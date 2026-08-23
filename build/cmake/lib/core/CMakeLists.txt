#############################################################################
# Name:        build/cmake/lib/core/CMakeLists.txt
# Purpose:     CMake file for core library
# Author:      Tobias Taschner
# Created:     2016-10-01
# Copyright:   (c) 2016 wxWidgets development team
# Licence:     wxWindows licence
#############################################################################

wx_append_sources(CORE_SRC GUI_CMN)
wx_append_sources(CORE_SRC BASE_AND_GUI_CMN)
if(WIN32)
    wx_append_sources(CORE_SRC BASE_AND_GUI_WIN32)
elseif(UNIX)
    wx_append_sources(CORE_SRC UNIX)
    if(wxUSE_LIBSDL)
        wx_append_sources(CORE_SRC UNIX_SOUND_SDL)
    endif()
endif()

if(WXMSW)
    wx_append_sources(CORE_SRC MSW_LOWLEVEL)
    wx_append_sources(CORE_SRC MSW)
    if(NOT wxUSE_OLE)
        wx_list_add_prefix(CORE_SRC "${wxSOURCE_DIR}/" "src/generic/dirdlgg.cpp")
    endif()
    if(WXWINUI AND wxUSE_WINUI3)
        # Keep generic/hyperlinkg.cpp: wxGenericHyperlinkCtrl remains an
        # independently selectable public control alongside the native peer.
        # Same for generic/infobar.cpp: wxInfoBarGeneric is a public class in
        # the other ports (wxGTK's native wxInfoBar even derives from it) and
        # applications do use it directly, so it must remain available here as
        # well -- see wx/winui/infobar.h.
        list(REMOVE_ITEM CORE_SRC
            "${wxSOURCE_DIR}/src/generic/activityindicator.cpp"
            "${wxSOURCE_DIR}/src/generic/bmpcboxg.cpp"
            "${wxSOURCE_DIR}/src/generic/clrpickerg.cpp"
            "${wxSOURCE_DIR}/src/generic/spinctlg.cpp"
            "${wxSOURCE_DIR}/src/generic/srchctlg.cpp"
            "${wxSOURCE_DIR}/src/generic/textdlgg.cpp"
            "${wxSOURCE_DIR}/src/msw/button.cpp"
            "${wxSOURCE_DIR}/src/msw/calctrl.cpp"
            "${wxSOURCE_DIR}/src/msw/datectrl.cpp"
            "${wxSOURCE_DIR}/src/msw/datetimectrl.cpp"
            "${wxSOURCE_DIR}/src/msw/bmpcbox.cpp"
            "${wxSOURCE_DIR}/src/msw/checkbox.cpp"
            "${wxSOURCE_DIR}/src/msw/choice.cpp"
            "${wxSOURCE_DIR}/src/msw/checklst.cpp"
            "${wxSOURCE_DIR}/src/msw/combo.cpp"
            "${wxSOURCE_DIR}/src/msw/combobox.cpp"
            "${wxSOURCE_DIR}/src/msw/gauge.cpp"
            "${wxSOURCE_DIR}/src/msw/headerctrl.cpp"
            "${wxSOURCE_DIR}/src/msw/hyperlink.cpp"
            "${wxSOURCE_DIR}/src/msw/listbox.cpp"
            "${wxSOURCE_DIR}/src/msw/listctrl.cpp"
            "${wxSOURCE_DIR}/src/msw/notebook.cpp"
            "${wxSOURCE_DIR}/src/msw/radiobox.cpp"
            "${wxSOURCE_DIR}/src/msw/radiobut.cpp"
            "${wxSOURCE_DIR}/src/msw/scrolbar.cpp"
            "${wxSOURCE_DIR}/src/msw/slider.cpp"
            "${wxSOURCE_DIR}/src/msw/spinbutt.cpp"
            "${wxSOURCE_DIR}/src/msw/spinctrl.cpp"
            "${wxSOURCE_DIR}/src/msw/statbmp.cpp"
            "${wxSOURCE_DIR}/src/msw/statbox.cpp"
            "${wxSOURCE_DIR}/src/msw/statline.cpp"
            "${wxSOURCE_DIR}/src/msw/statusbar.cpp"
            "${wxSOURCE_DIR}/src/msw/timectrl.cpp"
            "${wxSOURCE_DIR}/src/msw/stattext.cpp"
            "${wxSOURCE_DIR}/src/msw/textctrl.cpp"
            "${wxSOURCE_DIR}/src/msw/tglbtn.cpp"
            "${wxSOURCE_DIR}/src/msw/treectrl.cpp"
            "${wxSOURCE_DIR}/src/msw/commandlinkbutton.cpp"
            "${wxSOURCE_DIR}/src/msw/fdrepdlg.cpp"
            "${wxSOURCE_DIR}/src/msw/fontdlg.cpp"
            "${wxSOURCE_DIR}/src/msw/richtooltip.cpp"
            "${wxSOURCE_DIR}/src/msw/toolbar.cpp"
            )
        wx_list_add_prefix(CORE_SRC "${wxSOURCE_DIR}/"
            src/generic/fdrepdlg.cpp
            src/generic/fontdlgg.cpp
            src/generic/listctrl.cpp
            src/winui/winui.cpp
            src/winui/appearance.cpp
            src/winui/ctrlhost.cpp
            src/winui/dropbroker.cpp
            src/winui/inputrouter.cpp
            src/winui/inputstate.cpp
            src/winui/titlebar.cpp
            src/winui/tlwhost.cpp
            src/winui/xamlhost.cpp
            src/winui/activityindicator.cpp
            src/winui/bmpcbox.cpp
            src/winui/button.cpp
            src/winui/calctrl.cpp
            src/winui/checkbox.cpp
            src/winui/checklst.cpp
            src/winui/clrpicker.cpp
            src/winui/colordlg.cpp
            src/winui/datectrl.cpp
            src/winui/dlgpresenter.cpp
            src/winui/choice.cpp
            src/winui/combobox.cpp
            src/winui/gauge.cpp
            src/winui/hyperlink.cpp
            src/winui/infobar.cpp
            src/winui/listbox.cpp
            src/winui/menubar.cpp
            src/winui/msgdlg.cpp
            src/winui/notebook.cpp
            src/winui/radiobox.cpp
            src/winui/radiobut.cpp
            src/winui/renderer.cpp
            src/winui/richtooltip.cpp
            src/winui/scrolbar.cpp
            src/winui/slider.cpp
            src/winui/spinbutt.cpp
            src/winui/spinctrl.cpp
            src/winui/srchctrl.cpp
            src/winui/statbmp.cpp
            src/winui/statbox.cpp
            src/winui/statbar.cpp
            src/winui/statline.cpp
            src/winui/timectrl.cpp
            src/winui/toolbar.cpp
            src/winui/stattext.cpp
            src/winui/textctrl.cpp
            src/winui/textpeer.cpp
            src/winui/textdlg.cpp
            src/winui/tglbtn.cpp
            src/winui/treectrl.cpp
            )
    endif()
elseif(WXGTK)
    if(WXGTK2)
        set(GTK2_LOWLEVEL_HDR ${GTK_LOWLEVEL_HDR})
        wx_append_sources(CORE_SRC GTK2_LOWLEVEL)
        wx_append_sources(CORE_SRC GTK2)
    else()
        wx_append_sources(CORE_SRC GTK_LOWLEVEL)
        wx_append_sources(CORE_SRC GTK)
    endif()

    if(UNIX)
        wx_append_sources(CORE_SRC XWIN_LOWLEVEL)
    elseif(WIN32)
        wx_append_sources(CORE_SRC GTK_WIN32)
    endif()
elseif(WXOSX_COCOA)
    wx_append_sources(CORE_SRC BASE_AND_GUI_OSX_COCOA)
    wx_append_sources(CORE_SRC OSX_LOWLEVEL)
    wx_append_sources(CORE_SRC OSX_SHARED)
    wx_append_sources(CORE_SRC OSX_COCOA)
elseif(WXOSX_IPHONE)
    wx_append_sources(CORE_SRC OSX_LOWLEVEL)
    wx_append_sources(CORE_SRC OSX_SHARED)
    wx_append_sources(CORE_SRC OSX_IPHONE)
elseif(WXQT)
    wx_append_sources(CORE_SRC QT)
    if(UNIX)
        wx_append_sources(CORE_SRC QT_UNIX)
    elseif(WIN32)
        wx_append_sources(CORE_SRC QT_WIN32)
    endif()
elseif(WXX11)
    wx_append_sources(CORE_SRC X11_LOWLEVEL)
endif()

wx_add_library(wxcore ${CORE_SRC})
if(WXWINUI AND wxUSE_WINUI3)
    wx_winui3_configure_target(wxcore)
endif()
foreach(lib JPEG PNG TIFF NANOSVG WebP LUNASVG)
    if(${lib}_LIBRARIES)
        wx_lib_link_libraries(wxcore PRIVATE ${${lib}_LIBRARIES})
    endif()
    # When using built-in versions of the libraries, their include directories
    # must come before the system ones which could be used by the toolkit.
    if(${lib}_INCLUDE_DIRS)
        wx_lib_include_directories_before(wxcore ${${lib}_INCLUDE_DIRS})
    elseif(${lib}_INCLUDE_DIR)
        wx_lib_include_directories_before(wxcore ${${lib}_INCLUDE_DIR})
    endif()
endforeach()
if(wxUSE_NANOSVG STREQUAL "sys" AND wxUSE_NANOSVG_EXTERNAL_ENABLE_IMPL)
    wx_lib_compile_definitions(wxcore wxUSE_NANOSVG_EXTERNAL_ENABLE_IMPL)
endif()

if(WIN32)
    wx_lib_link_libraries(wxcore PRIVATE winmm)
endif()
if(WXOSX_COCOA)
    wx_lib_link_libraries(wxcore PUBLIC "-framework AudioToolbox")
    if(wxUSE_WEBVIEW)
        wx_lib_link_libraries(wxcore PUBLIC "-framework WebKit")
    endif()
endif()
if(WXOSX_IPHONE)
    wx_lib_link_libraries(wxcore
        PUBLIC
        "-framework AudioToolbox"
        "-framework CoreGraphics"
        "-framework CoreText"
        "-framework UIKit"
        )
endif()
if(WXGTK AND wxUSE_PRIVATE_FONTS)
    wx_lib_include_directories(wxcore ${FONTCONFIG_INCLUDE_DIRS} ${PANGOFT2_INCLUDE_DIRS})
    wx_lib_link_libraries(wxcore PUBLIC ${FONTCONFIG_LIBRARIES} ${PANGOFT2_LIBRARIES})
endif()
if(wxUSE_LIBSDL)
    if(SDL3_FOUND)
        # Upstream SDL3 ships an imported target; prefer it over raw vars.
        if(TARGET SDL3::SDL3)
            wx_lib_link_libraries(wxcore PUBLIC SDL3::SDL3)
        elseif(TARGET SDL3::SDL3-shared)
            wx_lib_link_libraries(wxcore PUBLIC SDL3::SDL3-shared)
        else()
            wx_lib_include_directories(wxcore ${SDL3_INCLUDE_DIRS})
            wx_lib_link_libraries(wxcore PUBLIC ${SDL3_LIBRARIES})
        endif()
    elseif(SDL2_FOUND)
        wx_lib_include_directories(wxcore ${SDL2_INCLUDE_DIR})
        wx_lib_link_libraries(wxcore PUBLIC ${SDL2_LIBRARY})
    elseif(SDL_FOUND)
        wx_lib_include_directories(wxcore ${SDL_INCLUDE_DIR})
        wx_lib_link_libraries(wxcore PUBLIC ${SDL_LIBRARY})
    endif()
endif()
if(wxUSE_LIBNOTIFY)
    wx_lib_include_directories(wxcore ${LIBNOTIFY_INCLUDE_DIRS})
    wx_lib_link_libraries(wxcore PUBLIC ${LIBNOTIFY_LIBRARIES})
endif()
if(wxUSE_APPINDICATOR)
    wx_lib_link_libraries(wxcore PRIVATE PkgConfig::APPINDICATOR)
endif()
if(wxUSE_CAIRO AND NOT WXGTK)
    wx_lib_include_directories(wxcore ${CAIRO_INCLUDE_DIRS})
    # no libs, cairo is loaded dynamically
endif()
