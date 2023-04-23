/////////////////////////////////////////////////////////////////////////////
// Name:        const_wxusedef.h
// Purpose:     wxUSE preprocessor symbols
// Author:      Tim Stahlhut
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////


/**

@page page_wxusedef wxUSE Preprocessor Symbols

@tableofcontents

This section documents the wxUSE preprocessor symbols used in the wxWidgets
source, grouped by category (and sorted by alphabetical order inside each
category). These symbols are always defined and whether the given feature is
active or not depends on their value: if defined as @c 1, feature is active,
otherwise it is disabled. Because of this these symbols should always be tested
using @if_ and not @ifdef_.



@section page_wxusedef_important Most Important Symbols

This table summarizes some of the global build features affecting the entire
library:

@beginDefList
@itemdef{wxUSE_STD_CONTAINERS, Container classes are implemented using
    standard classes (while still providing some compatibility with legacy
    wxWidgets container classes).}
@itemdef{wxUSE_UNICODE, Always defined as 1 in wxWidgets 3.3 and later, only
    exists for compatibility.}
@itemdef{wxUSE_UNICODE_WCHAR, wxString uses wchar_t buffer for internal storage
    (default).}
@itemdef{wxUSE_UNICODE_UTF8, wxString uses UTF-8 for internal storage (mostly
    useful for wxGTK where UTF-8 is used by the underlying toolkit).}
@itemdef{wxUSE_UTF8_LOCALE_ONLY, Library supports running only under UTF-8 (and
    C) locale. This eliminates the code necessary for conversions from the
    other locales and reduces the library size; useful for embedded systems.}
@itemdef{wxUSE_GUI, Use the GUI classes; if set to 0 only non-GUI classes are
    available.}
@itemdef{wxUSE_UNSAFE_WXSTRING_CONV, Defined as 1 by default for compatibility,
    but can be set to 0 to globally disable unsafe implicit conversions in
    wxString class. See @ref string_conv for more details.}
@endDefList


@section page_wxusedef_multi Generic Symbols

@beginDefList
@itemdef{wxUSE_ABOUTDLG, Use wxAboutDialogInfo class.}
@itemdef{wxUSE_ACCEL, Use wxAcceleratorTable/Entry classes and support for them in wxMenu, wxMenuBar.}
@itemdef{wxUSE_AFM_FOR_POSTSCRIPT, In wxPostScriptDC class use AFM (adobe font metrics) file for character widths.}
@itemdef{wxUSE_ACTIVITYINDICATOR, Use wxActivityIndicator class.}
@itemdef{wxUSE_ANIMATIONCTRL, Use wxAnimationCtrl class.}
@itemdef{wxUSE_ARTPROVIDER_STD, Use standard low quality icons in wxArtProvider.}
@itemdef{wxUSE_ARTPROVIDER_TANGO, Use Tango icons in wxArtProvider.}
@itemdef{wxUSE_ANY, Use wxAny class.}
@itemdef{wxUSE_APPLE_IEEE, IEEE Extended to/from double routines, see wxDataOutputStream.}
@itemdef{wxUSE_ARCHIVE_STREAMS, Enable streams for archive formats.}
@itemdef{wxUSE_AUI, Use AUI (dockable windows) library.}
@itemdef{wxUSE_BASE64, Enables Base64 support.}
@itemdef{wxUSE_BITMAPCOMBOBOX, Use wxBitmapComboBox class.}
@itemdef{wxUSE_BMPBUTTON, Use wxBitmapButton class.}
@itemdef{wxUSE_BUSYINFO, Use wxBusyInfo class.}
@itemdef{wxUSE_BUTTON, Use wxButton class.}
@itemdef{wxUSE_CALENDARCTRL, Use wxCalendarCtrl class.}
@itemdef{wxUSE_CARET, Use wxCaret class.}
@itemdef{wxUSE_CHECKBOX, Use wxCheckBox class.}
@itemdef{wxUSE_CHECKLISTBOX, Use wxCheckListBox class.}
@itemdef{wxUSE_CHOICE, Use wxChoice class.}
@itemdef{wxUSE_CHOICEBOOK, Use wxChoicebook class.}
@itemdef{wxUSE_CHOICEDLG, Use wxSingleChoiceDialog, or wxMultiChoiceDialog classes.}
@itemdef{wxUSE_CLIPBOARD, Use wxClipboard class.}
@itemdef{wxUSE_CMDLINE_PARSER, Use wxCmdLineParser class.}
@itemdef{wxUSE_COLLPANE, Use wxCollapsiblePane class.}
@itemdef{wxUSE_COLOURDLG, Use wxColourDialog class.}
@itemdef{wxUSE_COLOURPICKERCTRL, Use wxColourPickerCtrl class.}
@itemdef{wxUSE_COMBOBOX, Use wxComboBox class.}
@itemdef{wxUSE_COMBOCTRL, Use wxComboCtrl class.}
@itemdef{wxUSE_CONFIG, Use wxConfig and related classes.}
@itemdef{wxUSE_CONFIG_NATIVE, When enabled use native OS configuration instead of the wxFileConfig class.}
@itemdef{wxUSE_CONSOLE_EVENTLOOP, Enable event loop in console programs.}
@itemdef{wxUSE_CONSTRAINTS, Use wxLayoutConstraints}
@itemdef{wxUSE_CONTROLS, If set to 0, no classes deriving from wxControl can be used.}
@itemdef{wxUSE_CREDENTIALDLG, If set to 0, disabled wxCredentialEntryDialog used by wxWebRequest.}
@itemdef{wxUSE_DATAOBJ, Use wxDataObject and related classes.}
@itemdef{wxUSE_DATAVIEWCTRL, Use wxDataViewCtrl class.}
@itemdef{wxUSE_DATEPICKCTRL, Use wxDatePickerCtrl class.}
@itemdef{wxUSE_DATETIME, Use wxDateTime and related classes.}
@itemdef{wxUSE_DBGHELP, Predefine as 0 to avoid using wxDbgHelpDLL and related classes.}
@itemdef{wxUSE_DC_TRANSFORM_MATRIX, Use wxDC::SetTransformMatrix() and related methods.}
@itemdef{wxUSE_DEBUGREPORT, Use wxDebugReport class.}
@itemdef{wxUSE_DIALUP_MANAGER, Use wxDialUpManager and related classes.}
@itemdef{wxUSE_DIRDLG, Use wxDirDialog class.}
@itemdef{wxUSE_DIRPICKERCTRL, Use wxDirPickerCtrl class.}
@itemdef{wxUSE_DISPLAY, Use wxDisplay and related classes.}
@itemdef{wxUSE_DOC_VIEW_ARCHITECTURE, Use wxDocument and related classes.}
@itemdef{wxUSE_DRAG_AND_DROP, Use Drag and drop classes.}
@itemdef{wxUSE_DRAGIMAGE, Use wxDragImage class.}
@itemdef{wxUSE_DYNAMIC_LOADER, Use wxPluginManager and related classes. Requires wxDynamicLibrary}
@itemdef{wxUSE_DYNLIB_CLASS, Use wxDynamicLibrary}
@itemdef{wxUSE_EDITABLELISTBOX, Use wxEditableListBox class.}
@itemdef{wxUSE_EXCEPTIONS, Use exception handling.}
@itemdef{wxUSE_EXPAT, enable XML support using expat parser.}
@itemdef{wxUSE_EXTENDED_RTTI, Use extended RTTI, see also Runtime class information (RTTI)}
@itemdef{wxUSE_FFILE, Use wxFFile class.}
@itemdef{wxUSE_FILE, Use wxFile class.}
@itemdef{wxUSE_FILECONFIG, Use wxFileConfig class.}
@itemdef{wxUSE_FILECTRL, Use wxFileCtrl class.}
@itemdef{wxUSE_FILEDLG, Use wxFileDialog class.}
@itemdef{wxUSE_FILEPICKERCTRL, Use wxFilePickerCtrl class.}
@itemdef{wxUSE_FILESYSTEM, Use wxFileSystem and related classes.}
@itemdef{wxUSE_FINDREPLDLG, Use wxFindReplaceDialog class.}
@itemdef{wxUSE_FONTDLG, Use wxFontDialog class.}
@itemdef{wxUSE_FONTENUM, Use wxFontEnumerator class.}
@itemdef{wxUSE_FONTMAP, Use wxFontMapper class.}
@itemdef{wxUSE_FONTPICKERCTRL, Use wxFontPickerCtrl class.}
@itemdef{wxUSE_FS_ARCHIVE, Use virtual archive filesystems like wxArchiveFSHandler in wxFileSystem class.}
@itemdef{wxUSE_FS_INET, Use virtual HTTP/FTP filesystems like wxInternetFSHandler in wxFileSystem class.}
@itemdef{wxUSE_FS_ZIP, Please use wxUSE_FS_ARCHIVE instead.}
@itemdef{wxUSE_FSVOLUME, Use wxFSVolume class.}
@itemdef{wxUSE_GAUGE, Use wxGauge class.}
@itemdef{wxUSE_GENERIC_DRAGIMAGE, Used in wxDragImage sample.}
@itemdef{wxUSE_GENERIC_DRAWELLIPSE, See comment in wx/dc.h file.}
@itemdef{wxUSE_GENERIC_MDI_AS_NATIVE, This is not a user-settable symbol, it is only used internally in wx/generic/mdig.h.}
@itemdef{wxUSE_GEOMETRY, Use common geometry classes}
@itemdef{wxUSE_GIF, Use GIF wxImageHandler}
@itemdef{wxUSE_GLCANVAS, Enables OpenGL support.}
@itemdef{wxUSE_GRAPHICS_CONTEXT, Use wxGraphicsContext and related classes.}
@itemdef{wxUSE_GRID, Use wxGrid and related classes.}
@itemdef{wxUSE_HELP, Use wxHelpController and related classes.}
@itemdef{wxUSE_HTML, Use wxHtmlWindow and related classes.}
@itemdef{wxUSE_HYPERLINKCTRL, Use wxHyperlinkCtrl}
@itemdef{wxUSE_ICO_CUR, Support Windows ICO and CUR formats.}
@itemdef{wxUSE_IFF, Enables the wxImage handler for Amiga IFF images.}
@itemdef{wxUSE_IMAGE, Use wxImage and related classes.}
@itemdef{wxUSE_IMAGLIST, Use wxImageList class.}
@itemdef{wxUSE_INTL, Use wxLocale and related classes.}
@itemdef{wxUSE_IPC, Use interprocess communication classes.}
@itemdef{wxUSE_IPV6, Use experimental wxIPV6address and related classes.}
@itemdef{wxUSE_JOYSTICK, Use wxJoystick class.}
@itemdef{wxUSE_LIBJPEG, Enables JPEG format support (requires libjpeg).}
@itemdef{wxUSE_LIBLZMA, Enables LZMA compression support (see @ref page_build_liblzma).}
@itemdef{wxUSE_LIBPNG, Enables PNG format support (requires libpng). Also requires wxUSE_ZLIB.}
@itemdef{wxUSE_LIBTIFF, Enables TIFF format support (requires libtiff).}
@itemdef{wxUSE_LISTBOOK, Use wxListbook class.}
@itemdef{wxUSE_LISTBOX, Use wxListBox class.}
@itemdef{wxUSE_LISTCTRL, Use wxListCtrl class.}
@itemdef{wxUSE_LOG, Use wxLog and related classes.}
@itemdef{wxUSE_LOG_DEBUG, Enabled when wxLog used with \__WXDEBUG__ defined.}
@itemdef{wxUSE_LOG_DIALOG, Use wxLogDialog class.}
@itemdef{wxUSE_LOGGUI, Use wxLogGui class.}
@itemdef{wxUSE_LOGWINDOW, Use wxLogFrame class.}
@itemdef{wxUSE_LONGLONG, Use wxLongLong class.}
@itemdef{wxUSE_LONGLONG_NATIVE, Use native <tt>long long</tt> type in wxLongLong implementation.}
@itemdef{wxUSE_LONGLONG_WX, Use generic wxLongLong implementation.}
@itemdef{wxUSE_MARKUP, Provide wxControl::SetLabelMarkup() method.}
@itemdef{wxUSE_MDI, Use wxMDIParentFrame, and wxMDIChildFrame}
@itemdef{wxUSE_MDI_ARCHITECTURE, Use MDI-based document-view classes.}
@itemdef{wxUSE_MEDIACTRL, Use wxMediaCtrl.}
@itemdef{wxUSE_MENUS, Use wxMenu and wxMenuItem.}
@itemdef{wxUSE_MENUBAR, Use wxMenubar.}
@itemdef{wxUSE_METAFILE, Use wxMetaFile and related classes.}
@itemdef{wxUSE_MIMETYPE, Use wxFileType class.}
@itemdef{wxUSE_MINIFRAME, Use wxMiniFrame class.}
@itemdef{wxUSE_MOUSEWHEEL, Support mouse wheel events.}
@itemdef{wxUSE_MSGDLG, Use wxMessageDialog class and wxMessageBox function.}
@itemdef{wxUSE_NANOSVG, Use NanoSVG for rasterizing SVG (this option is new since wxWidgets 3.1.7).}
@itemdef{wxUSE_NANOSVG_EXTERNAL, Use an external NanoSVG library when wxUSE_NANOSVG is enabled.}
@itemdef{wxUSE_NANOSVG_EXTERNAL_ENABLE_IMPL, When the external NanoSVG library is header only, define this to enable its implementation.}
@itemdef{wxUSE_NATIVE_DATAVIEWCTRL, Use native wxDataViewCtrl class (this option is new since wxWidgets 3.1.4).}
@itemdef{wxUSE_NATIVE_STATUSBAR, Use native wxStatusBar class.}
@itemdef{wxUSE_NOTEBOOK, Use wxNotebook and related classes.}
@itemdef{wxUSE_NUMBERDLG, Use wxNumberEntryDialog class.}
@itemdef{wxUSE_ODCOMBOBOX, Use wxOwnerDrawnComboBox class.}
@itemdef{wxUSE_ON_FATAL_EXCEPTION, Catch signals in wxApp::OnFatalException method.}
@itemdef{wxUSE_OPENGL, Please use wxUSE_GLCANVAS to test for enabled OpenGL support instead.}
@itemdef{wxUSE_OWNER_DRAWN, Use interface for owner-drawn GUI elements.}
@itemdef{wxUSE_PALETTE, Use wxPalette and related classes.}
@itemdef{wxUSE_PCX, Enables wxImage PCX handler.}
@itemdef{wxUSE_PNM, Enables wxImage PNM handler.}
@itemdef{wxUSE_POPUPWIN, Use wxPopupWindow class.}
@itemdef{wxUSE_POSTSCRIPT, Use wxPostScriptPrinter class.}
@itemdef{wxUSE_PRINTF_POS_PARAMS, Use wxVsnprintf which supports positional parameters.}
@itemdef{wxUSE_PRINTING_ARCHITECTURE, Enable printer classes.}
@itemdef{wxUSE_PRIVATE_FONTS, Implement wxFont::AddPrivateFont() method.}
@itemdef{wxUSE_PROGRESSDLG, Enables progress dialog classes.}
@itemdef{wxUSE_PROPGRID, Use wxPropertyGrid library.}
@itemdef{wxUSE_PROTOCOL, Use wxProtocol and derived classes.}
@itemdef{wxUSE_PROTOCOL_FILE, Use wxFileProto class. (requires wxProtocol)}
@itemdef{wxUSE_PROTOCOL_FTP, Use wxFTP class. (requires wxProtocol)}
@itemdef{wxUSE_PROTOCOL_HTTP, Use wxHTTP class. (requireswxProtocol)}
@itemdef{wxUSE_RADIOBOX, Use wxRadioBox class.}
@itemdef{wxUSE_RADIOBTN, Use wxRadioButton class.}
@itemdef{wxUSE_REPRODUCIBLE_BUILD, Make library builds reproducible.}
@itemdef{wxUSE_REGEX, Use wxRegEx class.}
@itemdef{wxUSE_RICHTEXT, Use wxRichTextCtrl class.}
@itemdef{wxUSE_RICHTEXT_XML_HANDLER, See src/xrc/xh_richtext.cpp file.}
@itemdef{wxUSE_SASH, Use wxSashWindow class.}
@itemdef{wxUSE_SCROLLBAR, Use wxScrollBar class.}
@itemdef{wxUSE_SEARCHCTRL, Use wxSearchCtrl class.}
@itemdef{wxUSE_SELECT_DISPATCHER, Use wxSelectDispatcher class.}
@itemdef{wxUSE_SLIDER, Use wxSlider class.}
@itemdef{wxUSE_SNGLINST_CHECKER, Use wxSingleInstanceChecker class.}
@itemdef{wxUSE_SOCKETS, Enables Network address classes.}
@itemdef{wxUSE_SOUND, Use wxSound class.}
@itemdef{wxUSE_SPINBTN, Use wxSpinButton class.}
@itemdef{wxUSE_SPINCTRL, Use wxSpinCtrl class.}
@itemdef{wxUSE_SPLASH, Use wxSplashScreen class.}
@itemdef{wxUSE_SPLINES, Provide methods for spline drawing in wxDC.}
@itemdef{wxUSE_SPLITTER, Use wxSplitterWindow class.}
@itemdef{wxUSE_STACKWALKER, Enables wxStackWalker and related classes.}
@itemdef{wxUSE_STARTUP_TIPS, Use startup tips, wxTipProvider class.}
@itemdef{wxUSE_STATBMP, Use wxStaticBitmap class.}
@itemdef{wxUSE_STATBOX, Use wxStaticBox class.}
@itemdef{wxUSE_STATLINE, Use wxStaticLine class.}
@itemdef{wxUSE_STATTEXT, Use wxStaticText class.}
@itemdef{wxUSE_STATUSBAR, Use wxStatusBar class.}
@itemdef{wxUSE_STC, Use wxStyledTextCtrl.}
@itemdef{wxUSE_STDPATHS, Use wxStandardPaths class.}
@itemdef{wxUSE_STD_IOSTREAM, Use standard stream classes in addition to or, in wxDocument, instead of, wx streams.}
@itemdef{wxUSE_STOPWATCH, Use wxStopWatch class.}
@itemdef{wxUSE_STREAMS, Enable stream classes.}
@itemdef{wxUSE_SVG, Use wxSVGFileDC class.}
@itemdef{wxUSE_SYSTEM_OPTIONS, Use wxSystemOptions class.}
@itemdef{wxUSE_TAB_DIALOG, Use the obsolete wxTabControl class.}
@itemdef{wxUSE_TARSTREAM, Enable Tar files support.}
@itemdef{wxUSE_TASKBARICON, Use wxTaskBarIcon class.}
@itemdef{wxUSE_TEXTBUFFER, Use wxTextBuffer class.}
@itemdef{wxUSE_TEXTCTRL, Use wxTextCtrl class.}
@itemdef{wxUSE_TEXTDLG, Use wxTextEntryDialog class.}
@itemdef{wxUSE_TEXTFILE, Use wxTextFile class.}
@itemdef{wxUSE_TGA, Enable wxImage TGA handler.}
@itemdef{wxUSE_THREADS, Use wxThread and related classes.}
@itemdef{wxUSE_TIMER, Use wxTimer class.}
@itemdef{wxUSE_TIPWINDOW, Use wxTipWindow class.}
@itemdef{wxUSE_TOGGLEBTN, Use wxToggleButton class.}
@itemdef{wxUSE_TOOLBAR, Use wxToolBar class.}
@itemdef{wxUSE_TOOLBAR_NATIVE, Use native wxToolBar class.}
@itemdef{wxUSE_TOOLBOOK, Use wxToolbook class.}
@itemdef{wxUSE_TOOLTIPS, Use wxToolTip class.}
@itemdef{wxUSE_TREEBOOK, Use wxTreebook class.}
@itemdef{wxUSE_TREECTRL, Use wxTreeCtrl class.}
@itemdef{wxUSE_TREELISTCTRL, Use wxTreeListCtrl class.}
@itemdef{wxUSE_TTM_WINDOWFROMPOINT, Obsolete, do not use.}
@itemdef{wxUSE_URL, Use wxURL class.}
@itemdef{wxUSE_URL_NATIVE, Use native support for some operations with wxURL.}
@itemdef{wxUSE_VALIDATORS, Use wxValidator class.}
@itemdef{wxUSE_VARIANT, Use wxVariant class.}
@itemdef{wxUSE_WEBREQUEST, Use wxWebRequest class.}
@itemdef{wxUSE_WEBVIEW, Use wxWebView class.}
@itemdef{wxUSE_WIZARDDLG, Use wxWizard class.}
@itemdef{wxUSE_WXHTML_HELP, Use wxHtmlHelpController and related classes.}
@itemdef{wxUSE_XML, Use XML parsing classes.}
@itemdef{wxUSE_XPM, Enable XPM reader for wxImage and wxBitmap classes.}
@itemdef{wxUSE_XRC, Use XRC XML-based resource system.}
@itemdef{wxUSE_ZIPSTREAM, Enable streams for Zip files.}
@itemdef{wxUSE_ZLIB, Use wxZlibInput and wxZlibOutputStream classes, required by wxUSE_LIBPNG.}
@endDefList


@section page_wxusedef_unix Unix Platform Symbols

@beginDefList
@itemdef{wxUSE_EPOLL_DISPATCHER, Use wxEpollDispatcher class. See also wxUSE_SELECT_DISPATCHER.}
@itemdef{wxUSE_GSTREAMER, Use GStreamer library in wxMediaCtrl.}
@itemdef{wxUSE_LIBMSPACK, Use libmspack library.}
@itemdef{wxUSE_LIBSDL, Use SDL for wxSound implementation.}
@itemdef{wxUSE_PLUGINS, See also wxUSE_LIBSDL.}
@itemdef{wxUSE_UNIX, Enabled on Unix Platform.}
@itemdef{wxUSE_XTEST, Use XTest extension.}
@endDefList


@section page_wxusedef_x11 wxX11 Symbols

@beginDefList
@itemdef{wxUSE_NANOX, Use NanoX.}
@itemdef{wxUSE_UNIV_TEXTCTRL, Use wxUniv's implementation of wxTextCtrl class.}
@endDefList


@section page_wxusedef_gtk wxGTK Symbols

@beginDefList
@itemdef{wxUSE_DETECT_SM, Use code to detect X11 session manager.}
@itemdef{wxUSE_GTKPRINT, Use GTK+ printing support.}
@itemdef{wxUSE_LIBGNOMEVFS, Use GNOME VFS support. Currently has no effect. }
@endDefList


@section page_wxusedef_mac wxMac Symbols

@beginDefList
@itemdef{wxUSE_MAC_CRITICAL_REGION_MUTEX, See src/osx/carbon/thread.cpp file.}
@itemdef{wxUSE_MAC_PTHREADS_MUTEX, See src/osx/carbon/thread.cpp file.}
@itemdef{wxUSE_MAC_SEMAPHORE_MUTEX, See src/osx/carbon/thread.cpp file.}
@itemdef{wxUSE_WEBKIT, Use wxWebKitCtrl class.}
@endDefList


@section page_wxusedef_msw wxMSW Symbols

@beginDefList
@itemdef{wxUSE_ACCESSIBILITY, Enable accessibility support}
@itemdef{wxUSE_ACTIVEX, Use wxActiveXContainer and related classes.}
@itemdef{wxUSE_COMBOCTRL_POPUP_ANIMATION, See wx/msw/combo.h file.}
@itemdef{wxUSE_COMMON_DIALOGS, Enable use of windows common dialogs from header commdlg.h; example PRINTDLG.}
@itemdef{wxUSE_CRASHREPORT, Use wxCrashReport class.}
@itemdef{wxUSE_DATEPICKCTRL_GENERIC, Use generic wxDatePickerCtrl implementation in addition to the native one.}
@itemdef{wxUSE_DC_CACHEING, cache temporary wxDC objects.}
@itemdef{wxUSE_DDE_FOR_IPC, See wx/ipc.h file.}
@itemdef{wxUSE_DPI_AWARE_MANIFEST, Set the DPI awareness of the application
(0=none, 1=system, 2=per-monitor). Used by CMake and when wxUSE_RC_MANIFEST is enabled.}
@itemdef{wxUSE_ENH_METAFILE, Use wxEnhMetaFile.}
@itemdef{wxUSE_HOTKEY, Use wxWindow::RegisterHotKey() and wxWindow::UnregisterHotKey}
@itemdef{wxUSE_INKEDIT, Use InkEdit library. Related to Tablet PCs.}
@itemdef{wxUSE_MS_HTML_HELP, Use wxCHMHelpController class.}
@itemdef{wxUSE_NO_MANIFEST, Can be predefined to disable inclusion of the
manifest from wxWidgets RC file. See also wxUSE_RC_MANIFEST.}
@itemdef{wxUSE_OLE, Enables OLE helper routines.}
@itemdef{wxUSE_OLE_AUTOMATION, Enable OLE automation utilities.}
@itemdef{wxUSE_OLE_CLIPBOARD, Use OLE clipboard.}
@itemdef{wxUSE_POSTSCRIPT_ARCHITECTURE_IN_MSW, Use PS printing in wxMSW.}
@itemdef{wxUSE_PS_PRINTING, See src/msw/dcprint.cpp file.}
@itemdef{wxUSE_RC_MANIFEST, Include manifest for common controls library v6
from wxWidgets RC file. This is disabled by default for MSVC but enabled for
the other compilers. See also wxUSE_NO_MANIFEST.}
@itemdef{wxUSE_REGKEY, Use wxRegKey class.}
@itemdef{wxUSE_RICHEDIT, Enable use of riched32.dll in wxTextCtrl}
@itemdef{wxUSE_RICHEDIT2, Enable use of riched20.dll in wxTextCtrl}
@itemdef{wxUSE_VC_CRTDBG, See wx/msw/msvcrt.h file.}
@itemdef{wxUSE_UXTHEME, Enable support for XP themes.}
@itemdef{wxUSE_WIN_METAFILES_ALWAYS, Use wxMetaFile even when wxUSE_ENH_METAFILE=1.}
@itemdef{wxUSE_WINRT, Enable WinRT support.}
@itemdef{wxUSE_WXDIB, Use wxDIB class.}
@itemdef{wxUSE_WINSOCK2, Include @c <winsock2.h> instead of @c <winsock.h>.
Turned on automatically if wxUSE_IPV6 is on (this option is new since
wxWidgets 3.1.3).}
@endDefList


@section page_wxusedef_univ wxUniversal Symbols

@beginDefList
@itemdef{wxUSE_ALL_THEMES, Use all themes in wxUniversal; See wx/univ/theme.h file.}
@itemdef{wxUSE_THEME_GTK, Use GTK+ 1-like theme in wxUniversal}
@itemdef{wxUSE_THEME_METAL, Use GTK+ 2-like theme in wxUniversal}
@itemdef{wxUSE_THEME_MONO, Use simple monochrome theme in wxUniversal}
@itemdef{wxUSE_THEME_WIN32, Use Win32-like theme in wxUniversal}
@endDefList

*/
