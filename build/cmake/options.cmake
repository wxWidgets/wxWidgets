#############################################################################
# Name:        build/cmake/options.cmake
# Purpose:     User selectable build options
# Author:      Tobias Taschner
# Created:     2016-09-24
# Copyright:   (c) 2016 wxWidgets development team
# Licence:     wxWindows licence
#############################################################################

# Global build options
wx_option(wxBUILD_SHARED "Build wx libraries as shared libs" ${BUILD_SHARED_LIBS})
wx_option(wxBUILD_MONOLITHIC "build wxWidgets as single library" OFF)
wx_option(wxBUILD_SAMPLES "Build only important samples (SOME) or ALL" OFF
    STRINGS SOME ALL OFF)
wx_option(wxBUILD_TESTS "Build console tests (CONSOLE_ONLY) or ALL" OFF
    STRINGS CONSOLE_ONLY ALL OFF)
wx_option(wxBUILD_DEMOS "Build demos" OFF)
wx_option(wxBUILD_BENCHMARKS "Build benchmarks" OFF)
wx_option(wxBUILD_PRECOMP "Use precompiled headers" ON STRINGS ON OFF COTIRE)
mark_as_advanced(wxBUILD_PRECOMP)
wx_option(wxBUILD_INSTALL "Create install/uninstall target for wxWidgets")
wx_option(wxBUILD_COMPATIBILITY
    "enable compatibilty with earlier wxWidgets versions" 3.0 STRINGS 2.8 3.0 3.1)
mark_as_advanced(wxBUILD_COMPATIBILITY)
# Allow user specified setup.h folder
set(wxBUILD_CUSTOM_SETUP_HEADER_PATH "" CACHE PATH "Include path containing custom wx/setup.h")
mark_as_advanced(wxBUILD_CUSTOM_SETUP_HEADER_PATH)

if(WIN32)
    wx_option(wxUSE_DPI_AWARE_MANIFEST "DPI Awareness" "per-monitor" STRINGS "none" "system" "per-monitor")
endif()

wx_option(wxBUILD_DEBUG_LEVEL "Debug Level" Default STRINGS Default 0 1 2)
mark_as_advanced(wxBUILD_DEBUG_LEVEL)

if(NOT APPLE)
    wx_option(wxBUILD_USE_STATIC_RUNTIME "Link using the static runtime library" OFF)
    mark_as_advanced(wxBUILD_USE_STATIC_RUNTIME)
endif()

if(MSVC)
    wx_option(wxBUILD_MSVC_MULTIPROC "Enable multi-processor compilation for MSVC")
    mark_as_advanced(wxBUILD_MSVC_MULTIPROC)
endif()

if(NOT MSVC OR MSVC_VERSION GREATER 1800)
    # support setting the C++ standard, present it an option to the user
    if(DEFINED CMAKE_CXX_STANDARD)
        set(wxCXX_STANDARD_DEFAULT ${CMAKE_CXX_STANDARD})
    elseif(APPLE)
        set(wxCXX_STANDARD_DEFAULT 11)
    else()
        set(wxCXX_STANDARD_DEFAULT COMPILER_DEFAULT)
    endif()
    wx_option(wxBUILD_CXX_STANDARD "C++ standard used to build wxWidgets targets"
              ${wxCXX_STANDARD_DEFAULT} STRINGS COMPILER_DEFAULT 98 11 14 17 20)
endif()

if(WIN32)
    set(wxBUILD_VENDOR "custom" CACHE STRING "Short string identifying your company (used in DLL name)")
endif()
set(wxBUILD_FLAVOUR "" CACHE STRING "Specify a name to identify the build")
mark_as_advanced(wxBUILD_FLAVOUR)

wx_option(wxBUILD_OPTIMISE "use speed-optimised C/C++ compiler flags for release build" OFF)
mark_as_advanced(wxBUILD_OPTIMISE)
if(MSVC)
    set(wxBUILD_STRIPPED_RELEASE_DEFAULT OFF)
else()
    set(wxBUILD_STRIPPED_RELEASE_DEFAULT ON)
endif()
wx_option(wxBUILD_STRIPPED_RELEASE "remove debug symbols in release build" ${wxBUILD_STRIPPED_RELEASE_DEFAULT})
mark_as_advanced(wxBUILD_STRIPPED_RELEASE)
wx_option(wxBUILD_PIC "Enable position independent code (PIC)." ON)
mark_as_advanced(wxBUILD_PIC)
wx_option(wxUSE_NO_RTTI "disable RTTI support" OFF)

# STL options
wx_option(wxUSE_STL "use standard C++ classes for everything" OFF)
set(wxTHIRD_PARTY_LIBRARIES ${wxTHIRD_PARTY_LIBRARIES} wxUSE_STL "use C++ STL classes")
wx_dependent_option(wxUSE_STD_CONTAINERS "use standard C++ container classes" ON "wxUSE_STL" OFF)

wx_option(wxUSE_UNICODE "compile with Unicode support (NOT RECOMMENDED to be turned off)")
if(NOT WIN32)
    wx_option(wxUSE_UNICODE_UTF8 "use UTF-8 representation for strings (Unix only)" OFF)
    wx_dependent_option(wxUSE_UTF8_LOCALE_ONLY "only support UTF-8 locales in UTF-8 build (Unix only)" ON "wxUSE_UNICODE_UTF8" OFF)
endif()

wx_option(wxUSE_COMPILER_TLS "enable use of compiler TLS support")
if(NOT WIN32)
    wx_option(wxUSE_VISIBILITY "use of ELF symbols visibility")
endif()
wx_option(wxUSE_UNSAFE_WXSTRING_CONV "provide unsafe implicit conversions in wxString to const char* or std::string")
wx_option(wxUSE_REPRODUCIBLE_BUILD "enable reproducable build" OFF)

# ---------------------------------------------------------------------------
# external libraries
# ---------------------------------------------------------------------------
set(PCRE2_CODE_UNIT_WIDTH 8)
if(wxUSE_UNICODE AND (NOT DEFINED wxUSE_UNICODE_UTF8 OR NOT wxUSE_UNICODE_UTF8))
    # This is also checked in setup.cmake, but setup.cmake will run after options.cmake.
    include(CheckTypeSize)
    check_type_size(wchar_t SIZEOF_WCHAR_T)
    if(HAVE_SIZEOF_WCHAR_T AND SIZEOF_WCHAR_T EQUAL 2)
        set(PCRE2_CODE_UNIT_WIDTH 16)
    elseif(HAVE_SIZEOF_WCHAR_T AND SIZEOF_WCHAR_T EQUAL 4)
        set(PCRE2_CODE_UNIT_WIDTH 32)
    endif()
endif()

wx_add_thirdparty_library(wxUSE_REGEX PCRE2 "enable support for wxRegEx class")
wx_add_thirdparty_library(wxUSE_ZLIB ZLIB "use zlib for LZW compression" DEFAULT_APPLE sys)
wx_add_thirdparty_library(wxUSE_EXPAT EXPAT "use expat for XML parsing" DEFAULT_APPLE sys)
wx_add_thirdparty_library(wxUSE_LIBJPEG JPEG "use libjpeg (JPEG file format)")
wx_add_thirdparty_library(wxUSE_LIBPNG PNG "use libpng (PNG image format)")
wx_add_thirdparty_library(wxUSE_LIBTIFF TIFF "use libtiff (TIFF file format)")
wx_add_thirdparty_library(wxUSE_NANOSVG NanoSVG "use NanoSVG for rasterizing SVG" DEFAULT builtin)

wx_option(wxUSE_LIBLZMA "use LZMA compression" OFF)
set(wxTHIRD_PARTY_LIBRARIES ${wxTHIRD_PARTY_LIBRARIES} wxUSE_LIBLZMA "use liblzma for LZMA compression")

wx_option(wxUSE_OPENGL "use OpenGL (or Mesa)")

if(UNIX)
    wx_option(wxUSE_LIBSDL "use SDL for audio on Unix")
    wx_option(wxUSE_LIBICONV "use libiconv (character conversion)")
    wx_option(wxUSE_LIBNOTIFY "use libnotify for notifications")
    wx_option(wxUSE_XTEST "use XTest extension")
    wx_option(wxUSE_LIBMSPACK "use libmspack (CHM help files loading)")
    wx_option(wxUSE_GTKPRINT "use GTK printing support")
    wx_option(wxUSE_LIBGNOMEVFS "use GNOME VFS for associating MIME types")
    wx_option(wxUSE_GLCANVAS_EGL "use EGL backend for wxGLCanvas")

    set(wxTHIRD_PARTY_LIBRARIES ${wxTHIRD_PARTY_LIBRARIES} wxUSE_LIBSDL "use SDL for audio on Unix")
    set(wxTHIRD_PARTY_LIBRARIES ${wxTHIRD_PARTY_LIBRARIES} wxUSE_LIBMSPACK "use libmspack (CHM help files loading)")
endif()

# ---------------------------------------------------------------------------
# optional non GUI features
# ---------------------------------------------------------------------------
wx_option(wxUSE_INTL "use internationalization system")
wx_option(wxUSE_XLOCALE "use x-locale support (requires wxLocale)")
wx_option(wxUSE_CONFIG "use wxConfig (and derived) classes")

wx_option(wxUSE_SOCKETS "use socket/network classes")
wx_option(wxUSE_IPV6 "enable IPv6 support in wxSocket")
if(WIN32)
    wx_option(wxUSE_OLE "use OLE classes")
endif()
wx_option(wxUSE_DATAOBJ "use data object classes")

wx_option(wxUSE_IPC "use interprocess communication (wxSocket etc.)")

wx_option(wxUSE_CONSOLE_EVENTLOOP "use event loop in console programs too")

# please keep the settings below in alphabetical order
wx_option(wxUSE_ANY "use wxAny class")
wx_option(wxUSE_APPLE_IEEE "use the Apple IEEE codec")
wx_option(wxUSE_ARCHIVE_STREAMS "use wxArchive streams")
wx_option(wxUSE_BASE64 "use base64 encoding/decoding functions")
wx_option(wxUSE_STACKWALKER "use wxStackWalker class for getting backtraces")
wx_option(wxUSE_ON_FATAL_EXCEPTION "catch signals in wxApp::OnFatalException")
wx_option(wxUSE_CMDLINE_PARSER "use wxCmdLineParser class")
wx_option(wxUSE_DATETIME "use wxDateTime class")
wx_option(wxUSE_DEBUGREPORT "use wxDebugReport class")
if(APPLE)
    set(wxUSE_DIALUP_MANAGER_DEFAULT OFF)
else()
    set(wxUSE_DIALUP_MANAGER_DEFAULT ON)
endif()
wx_option(wxUSE_DIALUP_MANAGER "use dialup network classes" ${wxUSE_DIALUP_MANAGER_DEFAULT})
wx_option(wxUSE_DYNLIB_CLASS "use wxLibrary class for DLL loading")
wx_option(wxUSE_DYNAMIC_LOADER "use (new) wxDynamicLibrary class")
wx_option(wxUSE_EXCEPTIONS "build exception-safe library")
wx_option(wxUSE_EXTENDED_RTTI "use extended RTTI (XTI)" OFF)
wx_option(wxUSE_FFILE "use wxFFile class")
wx_option(wxUSE_FILE "use wxFile class")
wx_option(wxUSE_FILE_HISTORY "use wxFileHistory class")
wx_option(wxUSE_FILESYSTEM "use virtual file systems classes")
wx_option(wxUSE_FONTENUM "use wxFontEnumerator class")
wx_option(wxUSE_FONTMAP "use font encodings conversion classes")
wx_option(wxUSE_FS_ARCHIVE "use virtual archive filesystems")
wx_option(wxUSE_FS_INET "use virtual HTTP/FTP filesystems")
wx_option(wxUSE_FS_ZIP "now replaced by fs_archive")
wx_option(wxUSE_FSVOLUME "use wxFSVolume class")
wx_option(wxUSE_FSWATCHER "use wxFileSystemWatcher class")
wx_option(wxUSE_GEOMETRY "use geometry class")
wx_option(wxUSE_LOG "use logging system")
wx_option(wxUSE_LONGLONG "use wxLongLong class")
wx_option(wxUSE_MIMETYPE "use wxMimeTypesManager")
wx_option(wxUSE_PRINTF_POS_PARAMS "use wxVsnprintf() which supports positional parameters")
wx_option(wxUSE_SECRETSTORE "use wxSecretStore class")
wx_option(wxUSE_SNGLINST_CHECKER "use wxSingleInstanceChecker class")
wx_option(wxUSE_SOUND "use wxSound class")
wx_option(wxUSE_SPELLCHECK "enable spell checking in wxTextCtrl")
wx_option(wxUSE_STDPATHS "use wxStandardPaths class")
wx_option(wxUSE_STOPWATCH "use wxStopWatch class")
wx_option(wxUSE_STREAMS "use wxStream etc classes")
wx_option(wxUSE_SYSTEM_OPTIONS "use wxSystemOptions")
wx_option(wxUSE_TARSTREAM "use wxTar streams")
wx_option(wxUSE_TEXTBUFFER "use wxTextBuffer class")
wx_option(wxUSE_TEXTFILE "use wxTextFile class")
wx_option(wxUSE_TIMER "use wxTimer class")
wx_option(wxUSE_VARIANT "use wxVariant class")

# WebRequest options
wx_option(wxUSE_WEBREQUEST "use wxWebRequest class")
if(WIN32)
    wx_option(wxUSE_WEBREQUEST_WINHTTP "use wxWebRequest WinHTTP backend")
endif()
if(APPLE)
    wx_option(wxUSE_WEBREQUEST_URLSESSION "use wxWebRequest URLSession backend")
endif()
if(APPLE OR WIN32)
    set(wxUSE_WEBREQUEST_CURL_DEFAULT OFF)
else()
    set(wxUSE_WEBREQUEST_CURL_DEFAULT ON)
endif()
wx_option(wxUSE_WEBREQUEST_CURL "use wxWebRequest libcurl backend" ${wxUSE_WEBREQUEST_CURL_DEFAULT})

wx_option(wxUSE_ZIPSTREAM "use wxZip streams")

# URL-related classes
wx_option(wxUSE_URL "use wxURL class")
wx_option(wxUSE_PROTOCOL "use wxProtocol class")
wx_option(wxUSE_PROTOCOL_HTTP "HTTP support in wxProtocol")
wx_option(wxUSE_PROTOCOL_FTP "FTP support in wxProtocol")
wx_option(wxUSE_PROTOCOL_FILE "FILE support in wxProtocol")

wx_option(wxUSE_THREADS "use threads")

if(WIN32)
    if(MINGW)
        #TODO: version check, as newer versions have no problem enabling this
        set(wxUSE_DBGHELP_DEFAULT OFF)
    else()
        set(wxUSE_DBGHELP_DEFAULT ON)
    endif()
    wx_option(wxUSE_DBGHELP "use dbghelp.dll API" ${wxUSE_DBGHELP_DEFAULT})
    wx_option(wxUSE_INICONF "use wxIniConfig")
    wx_option(wxUSE_WINSOCK2 "include <winsock2.h> rather than <winsock.h>" OFF)
    wx_option(wxUSE_REGKEY "use wxRegKey class")
endif()

if(wxUSE_GUI)

# ---------------------------------------------------------------------------
# optional "big" GUI features
# ---------------------------------------------------------------------------

wx_option(wxUSE_DOC_VIEW_ARCHITECTURE "use document view architecture")
wx_option(wxUSE_HELP "use help subsystem")
wx_option(wxUSE_MS_HTML_HELP "use MS HTML Help (win32)")
wx_option(wxUSE_HTML "use wxHTML sub-library")
wx_option(wxUSE_WXHTML_HELP "use wxHTML-based help")
wx_option(wxUSE_XRC "use XRC resources sub-library")
wx_option(wxUSE_XML "use the xml library (overruled by wxUSE_XRC)")
wx_option(wxUSE_AUI "use AUI docking library")
wx_option(wxUSE_PROPGRID "use wxPropertyGrid library")
wx_option(wxUSE_RIBBON "use wxRibbon library")
wx_option(wxUSE_STC "use wxStyledTextCtrl library")
wx_option(wxUSE_CONSTRAINTS "use layout-constraints system")
wx_option(wxUSE_LOGGUI "use standard GUI logger")
wx_option(wxUSE_LOGWINDOW "use wxLogWindow")
wx_option(wxUSE_LOG_DIALOG "use wxLogDialog")
wx_option(wxUSE_MDI "use multiple document interface architecture")
wx_option(wxUSE_MDI_ARCHITECTURE "use docview architecture with MDI")
wx_option(wxUSE_MEDIACTRL "use wxMediaCtrl class")
wx_option(wxUSE_RICHTEXT "use wxRichTextCtrl")
wx_option(wxUSE_POSTSCRIPT "use wxPostscriptDC device context (default for gtk+)")
wx_option(wxUSE_AFM_FOR_POSTSCRIPT "in wxPostScriptDC class use AFM (adobe font metrics) file for character widths")
wx_option(wxUSE_PRINTING_ARCHITECTURE "use printing architecture")
wx_option(wxUSE_SVG "use wxSVGFileDC device context")
wx_option(wxUSE_WEBVIEW "use wxWebView library")

# wxDC is implemented in terms of wxGraphicsContext in wxOSX so the latter
# can't be disabled, don't even provide an option to do it
if(APPLE)
    set(wxUSE_GRAPHICS_CONTEXT ON)
else()
    wx_option(wxUSE_GRAPHICS_CONTEXT "use graphics context 2D drawing API")
    if(WIN32)
        wx_option(wxUSE_GRAPHICS_DIRECT2D "enable Direct2D graphics context")
    endif()
endif()

if(WXGTK)
    set(wxUSE_CAIRO_DEFAULT ON)
else()
    set(wxUSE_CAIRO_DEFAULT OFF)
endif()
wx_option(wxUSE_CAIRO "enable Cairo graphics context" ${wxUSE_CAIRO_DEFAULT})

# ---------------------------------------------------------------------------
# IPC &c
# ---------------------------------------------------------------------------

wx_option(wxUSE_CLIPBOARD "use wxClipboard class")
wx_option(wxUSE_DRAG_AND_DROP "use Drag'n'Drop classes")

# ---------------------------------------------------------------------------
# optional GUI controls (in alphabetical order except the first one)
# ---------------------------------------------------------------------------

# don't set DEFAULT_wxUSE_XXX below if the option is not specified
wx_option(wxUSE_CONTROLS "disable compilation of all standard controls")

# features affecting multiple controls
wx_option(wxUSE_MARKUP "support wxControl::SetLabelMarkup")

# please keep the settings below in alphabetical order
wx_option(wxUSE_ACCEL "use accelerators")
wx_option(wxUSE_ACTIVITYINDICATOR "use wxActivityIndicator class")
wx_option(wxUSE_ADDREMOVECTRL "use wxAddRemoveCtrl")
wx_option(wxUSE_ANIMATIONCTRL "use wxAnimationCtrl class")
wx_option(wxUSE_BANNERWINDOW "use wxBannerWindow class")
wx_option(wxUSE_ARTPROVIDER_STD "use standard XPM icons in wxArtProvider")
wx_option(wxUSE_ARTPROVIDER_TANGO "use Tango icons in wxArtProvider")
wx_option(wxUSE_BMPBUTTON "use wxBitmapButton class")
wx_option(wxUSE_BITMAPCOMBOBOX "use wxBitmapComboBox class")
wx_option(wxUSE_BUTTON "use wxButton class")
wx_option(wxUSE_CALENDARCTRL "use wxCalendarCtrl class")
wx_option(wxUSE_CARET "use wxCaret class")
wx_option(wxUSE_CHECKBOX "use wxCheckBox class")
wx_option(wxUSE_CHECKLISTBOX "use wxCheckListBox (listbox with checkboxes) class")
wx_option(wxUSE_CHOICE "use wxChoice class")
wx_option(wxUSE_CHOICEBOOK "use wxChoicebook class")
wx_option(wxUSE_COLLPANE "use wxCollapsiblePane class")
wx_option(wxUSE_COLOURPICKERCTRL "use wxColourPickerCtrl class")
wx_option(wxUSE_COMBOBOX "use wxComboBox class")
wx_option(wxUSE_COMBOCTRL "use wxComboCtrl class")
wx_option(wxUSE_COMMANDLINKBUTTON "use wxCommmandLinkButton class")
wx_option(wxUSE_DATAVIEWCTRL "use wxDataViewCtrl class")
wx_option(wxUSE_NATIVE_DATAVIEWCTRL "use the native wxDataViewCtrl if available")
wx_option(wxUSE_DATEPICKCTRL "use wxDatePickerCtrl class")
wx_option(wxUSE_DETECT_SM "use code to detect X11 session manager" OFF)
wx_option(wxUSE_DIRPICKERCTRL "use wxDirPickerCtrl class")
wx_option(wxUSE_DISPLAY "use wxDisplay class")
wx_option(wxUSE_EDITABLELISTBOX "use wxEditableListBox class")
wx_option(wxUSE_FILECTRL "use wxFileCtrl class")
wx_option(wxUSE_FILEPICKERCTRL "use wxFilePickerCtrl class")
wx_option(wxUSE_FONTPICKERCTRL "use wxFontPickerCtrl class")
wx_option(wxUSE_GAUGE "use wxGauge class")
wx_option(wxUSE_GRID "use wxGrid class")
wx_option(wxUSE_HEADERCTRL "use wxHeaderCtrl class")
wx_option(wxUSE_HYPERLINKCTRL "use wxHyperlinkCtrl class")
wx_option(wxUSE_IMAGLIST "use wxImageList class")
wx_option(wxUSE_INFOBAR "use wxInfoBar class")
wx_option(wxUSE_LISTBOOK "use wxListbook class")
wx_option(wxUSE_LISTBOX "use wxListBox class")
wx_option(wxUSE_LISTCTRL "use wxListCtrl class")
wx_option(wxUSE_NOTEBOOK "use wxNotebook class")
wx_option(wxUSE_NOTIFICATION_MESSAGE "use wxNotificationMessage class")
wx_option(wxUSE_ODCOMBOBOX "use wxOwnerDrawnComboBox class")
wx_option(wxUSE_POPUPWIN "use wxPopUpWindow class")
wx_option(wxUSE_PREFERENCES_EDITOR "use wxPreferencesEditor class")
wx_option(wxUSE_RADIOBOX "use wxRadioBox class")
wx_option(wxUSE_RADIOBTN "use wxRadioButton class")
wx_option(wxUSE_RICHMSGDLG "use wxRichMessageDialog class")
wx_option(wxUSE_RICHTOOLTIP "use wxRichToolTip class")
wx_option(wxUSE_REARRANGECTRL "use wxRearrangeList/Ctrl/Dialog")
wx_option(wxUSE_SASH "use wxSashWindow class")
wx_option(wxUSE_SCROLLBAR "use wxScrollBar class and scrollable windows")
wx_option(wxUSE_SEARCHCTRL "use wxSearchCtrl class")
wx_option(wxUSE_SLIDER "use wxSlider class")
wx_option(wxUSE_SPINBTN "use wxSpinButton class")
wx_option(wxUSE_SPINCTRL "use wxSpinCtrl class")
wx_option(wxUSE_SPLITTER "use wxSplitterWindow class")
wx_option(wxUSE_STATBMP "use wxStaticBitmap class")
wx_option(wxUSE_STATBOX "use wxStaticBox class")
wx_option(wxUSE_STATLINE "use wxStaticLine class")
wx_option(wxUSE_STATTEXT "use wxStaticText class")
wx_option(wxUSE_STATUSBAR "use wxStatusBar class")
wx_option(wxUSE_TASKBARBUTTON "use wxTaskBarButton class")
wx_option(wxUSE_TASKBARICON "use wxTaskBarIcon class")
wx_option(wxUSE_TOOLBAR_NATIVE "use native wxToolBar class")
wx_option(wxUSE_TEXTCTRL "use wxTextCtrl class")
if(wxUSE_TEXTCTRL)
    # we don't have special switches to disable wxUSE_RICHEDIT[2], it doesn't
    # seem useful to allow disabling them
    set(wxUSE_RICHEDIT ON)
    set(wxUSE_RICHEDIT2 ON)
endif()
wx_option(wxUSE_TIMEPICKCTRL "use wxTimePickerCtrl class")
wx_option(wxUSE_TIPWINDOW "use wxTipWindow class")
wx_option(wxUSE_TOGGLEBTN "use wxToggleButton class")
wx_option(wxUSE_TOOLBAR "use wxToolBar class")
wx_option(wxUSE_TOOLBOOK "use wxToolbook class")
wx_option(wxUSE_TREEBOOK "use wxTreebook class")
wx_option(wxUSE_TREECTRL "use wxTreeCtrl class")
wx_option(wxUSE_TREELISTCTRL "use wxTreeListCtrl class")

# ---------------------------------------------------------------------------
# common dialogs
# ---------------------------------------------------------------------------

wx_option(wxUSE_COMMON_DIALOGS "use all common dialogs")
wx_option(wxUSE_ABOUTDLG "use wxAboutBox")
wx_option(wxUSE_CHOICEDLG "use wxChoiceDialog")
wx_option(wxUSE_COLOURDLG "use wxColourDialog")
wx_option(wxUSE_CREDENTIALDLG "use wxCredentialEntryDialog")
wx_option(wxUSE_FILEDLG "use wxFileDialog")
wx_option(wxUSE_FINDREPLDLG "use wxFindReplaceDialog")
wx_option(wxUSE_FONTDLG "use wxFontDialog")
wx_option(wxUSE_DIRDLG "use wxDirDialog")
wx_option(wxUSE_MSGDLG "use wxMessageDialog")
wx_option(wxUSE_NUMBERDLG "use wxNumberEntryDialog")
wx_option(wxUSE_SPLASH "use wxSplashScreen")
wx_option(wxUSE_TEXTDLG "use wxTextDialog")
wx_option(wxUSE_STARTUP_TIPS "use startup tips")
wx_option(wxUSE_PROGRESSDLG "use wxProgressDialog")
wx_option(wxUSE_WIZARDDLG "use wxWizard")

# ---------------------------------------------------------------------------
# misc GUI options
# ---------------------------------------------------------------------------

wx_option(wxUSE_MENUS "use wxMenu and wxMenuItem classes")
wx_option(wxUSE_MENUBAR "use wxMenuBar class")
wx_option(wxUSE_MINIFRAME "use wxMiniFrame class")
wx_option(wxUSE_TOOLTIPS "use wxToolTip class")
wx_option(wxUSE_SPLINES "use spline drawing code")
wx_option(wxUSE_MOUSEWHEEL "use mousewheel")
wx_option(wxUSE_VALIDATORS "use wxValidator and derived classes")
wx_option(wxUSE_BUSYINFO "use wxBusyInfo")
wx_option(wxUSE_HOTKEY "use wxWindow::RegisterHotKey()")
if(UNIX AND NOT APPLE AND NOT CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(wxUSE_JOYSTICK_DEFAULT OFF)
else()
    set(wxUSE_JOYSTICK_DEFAULT ON)
endif()
wx_option(wxUSE_JOYSTICK "use wxJoystick" ${wxUSE_JOYSTICK_DEFAULT})
wx_option(wxUSE_METAFILE "use wxMetaFile")
wx_option(wxUSE_DRAGIMAGE "use wxDragImage")
wx_option(wxUSE_UIACTIONSIMULATOR "use wxUIActionSimulator (experimental)")
wx_option(wxUSE_DC_TRANSFORM_MATRIX "use wxDC::SetTransformMatrix and related")
wx_option(wxUSE_WEBVIEW_WEBKIT "use wxWebView WebKit backend")
if(WIN32 OR APPLE)
    set(wxUSE_PRIVATE_FONTS_DEFAULT ON)
else()
    set(wxUSE_PRIVATE_FONTS_DEFAULT OFF)
endif()
wx_option(wxUSE_PRIVATE_FONTS "use fonts not installed on the system" ${wxUSE_PRIVATE_FONTS_DEFAULT})

# ---------------------------------------------------------------------------
# support for image formats that do not rely on external library
# ---------------------------------------------------------------------------

wx_option(wxUSE_PALETTE "use wxPalette class")
wx_option(wxUSE_IMAGE "use wxImage class")
wx_option(wxUSE_GIF "use gif images (GIF file format)")
wx_option(wxUSE_PCX "use pcx images (PCX file format)")
wx_option(wxUSE_TGA "use tga images (TGA file format)")
wx_option(wxUSE_IFF "use iff images (IFF file format)")
wx_option(wxUSE_PNM "use pnm images (PNM file format)")
wx_option(wxUSE_XPM "use xpm images (XPM file format)")
wx_option(wxUSE_ICO_CUR "use Windows ICO and CUR formats")

# ---------------------------------------------------------------------------
# wxMSW-only options
# ---------------------------------------------------------------------------

if(WIN32)
    if(MSVC_VERSION GREATER 1600 AND NOT CMAKE_VS_PLATFORM_TOOLSET MATCHES "_xp$")
        set(wxUSE_WINRT_DEFAULT ON)
    else()
        set(wxUSE_WINRT_DEFAULT OFF)
    endif()
    if(MSVC_VERSION GREATER 1800 AND NOT CMAKE_VS_PLATFORM_TOOLSET MATCHES "_xp$" AND
        EXISTS "${wxSOURCE_DIR}/3rdparty/webview2")
        set(wxUSE_WEBVIEW_EDGE_DEFAULT ON)
    else()
        set(wxUSE_WEBVIEW_EDGE_DEFAULT OFF)
    endif()

    wx_option(wxUSE_ACCESSIBILITY "enable accessibility support")
    wx_option(wxUSE_ACTIVEX " enable wxActiveXContainer class (Win32 only)")
    wx_option(wxUSE_CRASHREPORT "enable wxCrashReport::Generate() to create mini dumps (Win32 only)")
    wx_option(wxUSE_DC_CACHEING "cache temporary wxDC objects (Win32 only)")
    wx_option(wxUSE_NATIVE_PROGRESSDLG "use native progress dialog implementation")
    wx_option(wxUSE_NATIVE_STATUSBAR "use native statusbar implementation)")
    wx_option(wxUSE_OWNER_DRAWN "use owner drawn controls (Win32)")
    wx_option(wxUSE_POSTSCRIPT_ARCHITECTURE_IN_MSW "use PS printing in wxMSW (Win32 only)")
    wx_option(wxUSE_TASKBARICON_BALLOONS "enable wxTaskBarIcon::ShowBalloon() method (Win32 only)")
    wx_option(wxUSE_UXTHEME "enable support for Windows XP themed look (Win32 only)")
    wx_option(wxUSE_WEBVIEW_EDGE "use wxWebView Edge (Chromium) backend (Windows 7+ only)" ${wxUSE_WEBVIEW_EDGE_DEFAULT})
    wx_option(wxUSE_WEBVIEW_EDGE_STATIC "use wxWebView Edge with static loader" OFF)
    wx_option(wxUSE_WEBVIEW_IE "use wxWebView IE backend (Win32 only)")
    wx_option(wxUSE_WINRT "enable WinRT support" ${wxUSE_WINRT_DEFAULT})
    wx_option(wxUSE_WXDIB "use wxDIB class (Win32 only)")
endif()

# this one is not really MSW-specific but it exists mainly to be turned off
# under MSW, it should be off by default on the other platforms
if(WIN32)
    set(wxDEFAULT_wxUSE_AUTOID_MANAGEMENT ON)
else()
    set(wxDEFAULT_wxUSE_AUTOID_MANAGEMENT OFF)
endif()

wx_option(wxUSE_AUTOID_MANAGEMENT "use automatic ids management" ${wxDEFAULT_wxUSE_AUTOID_MANAGEMENT})

endif() # wxUSE_GUI
